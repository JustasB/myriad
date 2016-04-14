#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "MyriadObject.h"
#include "Compartment.h"
#include "Compartment.cuh"

/////////////////////////////////
// Compartment Super Overrides //
/////////////////////////////////

static void* Compartment_ctor(void* _self, va_list* app)
{
	struct Compartment* self = (struct Compartment*) super_ctor(Compartment, _self, app);

	self->id = va_arg(*app, uint64_t);
	self->num_mechs = va_arg(*app, uint64_t);

    // XXX: DEPRECATED
    // This allows us to "inherit" Mechanisms from other compartments easily.
    struct Mechanism** mechs = va_arg(*app, struct Mechanism**);
    if (mechs != NULL)
    {
        memcpy(self->my_mechs,
               mechs,
               sizeof(struct Mechanism*) * MAX_NUM_MECHS);
    }

	return self;
}

//////////////////////////////////////
// Native Functions Implementations //
//////////////////////////////////////

// Simulate function
static void Compartment_simul_fxn(void* _self,
                                  void** network,
                                  const double global_time,
                                  const uint64_t curr_step)
{
	// const struct Compartment* self = (const struct Compartment*) _self;
	// printf("My id is %u\n", self->id);
	// printf("My num_mechs is %u\n", self->num_mechs);
	return;
}

void simul_fxn(void* _self,
               void** network,
               const double global_time,
               const uint64_t curr_step)
{
	const struct CompartmentClass* m_class = 
		(const struct CompartmentClass*) myriad_class_of((void*) _self);
	assert(m_class->m_compartment_simul_fxn);
	m_class->m_compartment_simul_fxn(_self, network, global_time, curr_step);
}

void super_simul_fxn(void* _class,
                     void* _self,
                     void** network,
                     const double global_time,
                     const uint64_t curr_step)
{
	const struct CompartmentClass* s_class=(const struct CompartmentClass*) myriad_super(_class);
	assert(_self && s_class->m_compartment_simul_fxn);
	s_class->m_compartment_simul_fxn(_self, network, global_time, curr_step);
}

// Add mechanism function
static int Compartment_add_mech(void* _self, void* mechanism)
{
	if (_self == NULL || mechanism == NULL)
	{
		fprintf(stderr, "Cannot add NULL mechanism/add to NULL compartment.\n");
		return -1;
	}

	struct Compartment* self = (struct Compartment*) _self;
	struct Mechanism* mech = (struct Mechanism*) mechanism;

    if (self->num_mechs + 1 >= MAX_NUM_MECHS)
    {
        fprintf(stderr, "Cannot add mechanism to Compartment: out of room.\n");
        return -1;
    }
    
	self->my_mechs[self->num_mechs] = mech;
	self->num_mechs++;

	return 0;
}

int add_mechanism(void* _self, void* mechanism)
{
	const struct CompartmentClass* m_class = 
		(const struct CompartmentClass*) myriad_class_of((void*) _self);
	assert(m_class->m_compartment_add_mech_fxn);
	return m_class->m_compartment_add_mech_fxn(_self, mechanism);
}

int super_add_mechanism(const void* _class, void* _self, void* mechanism)
{
	const struct CompartmentClass* s_class=(const struct CompartmentClass*) myriad_super(_class);
	assert(_self && s_class->m_compartment_add_mech_fxn);
	return s_class->m_compartment_add_mech_fxn(_self, mechanism);
}

//////////////////////////////////////
// CompartmentClass Super Overrides //
//////////////////////////////////////

static void* CompartmentClass_ctor(void* _self, va_list* app)
{
	struct CompartmentClass* self = (struct CompartmentClass*) super_ctor(CompartmentClass, _self, app);

	voidf selector = NULL; selector = va_arg(*app, voidf);

	while (selector)
	{
		const voidf method = va_arg(*app, voidf);
		
		if (selector == (voidf) simul_fxn)
		{
			*(voidf *) &self->m_compartment_simul_fxn = method;
		} else if (selector == (voidf) add_mechanism) {
			*(voidf *) &self->m_compartment_add_mech_fxn = method;
		}

		selector = va_arg(*app, voidf);
	}

	return self;
}

static void* CompartmentClass_cudafy(void* _self, int clobber)
{
	#ifdef CUDA
    // We know what class we are
	struct CompartmentClass* my_class = (struct CompartmentClass*) _self;

	// Make a temporary copy-class because we need to change shit
	struct CompartmentClass copy_class = *my_class;
	struct MyriadClass* copy_class_class = (struct MyriadClass*) &copy_class;

	
	// !!!!!!!!! IMPORTANT !!!!!!!!!!!!!!
	// By default we clobber the copy_class_class' superclass with
	// the superclass' device_class' on-GPU address value. 
	// To avoid cloberring this value (e.g. if an underclass has already
	// clobbered it), the clobber flag should be 0.
	if (clobber)
	{
		// TODO: Find a better way to get function pointers for on-card functions
		compartment_simul_fxn_t my_comp_fun = NULL;
		CUDA_CHECK_RETURN(
			cudaMemcpyFromSymbol(
				(void**) &my_comp_fun,
				(const void*) &Compartment_cuda_compartment_fxn_t,
				sizeof(void*),
				0,
				cudaMemcpyDeviceToHost
				)
			);
		copy_class.m_compartment_simul_fxn = my_comp_fun;
		
		DEBUG_PRINTF("Copy Class comp fxn: %p\n", my_comp_fun);
		
		const struct MyriadClass* super_class = (const struct MyriadClass*) MyriadClass;
		memcpy((void**) &copy_class_class->super, &super_class->device_class, sizeof(void*));
    }

	// This works because super methods rely on the given class'
	// semi-static superclass definition, not it's ->super attribute.
	return super_cudafy(CompartmentClass, (void*) &copy_class, 0);

	#else

    return NULL;

	#endif

}

///////////////////////////
// Object Initialization //
///////////////////////////

const void *CompartmentClass, *Compartment;

void initCompartment(const bool init_cuda)
{
	if (!CompartmentClass)
	{
		CompartmentClass = 
			myriad_new(
				   MyriadClass,
				   MyriadClass,
				   sizeof(struct CompartmentClass),
				   myriad_ctor, CompartmentClass_ctor,
				   myriad_cudafy, CompartmentClass_cudafy,
				   0
			);
		struct MyriadObject* mech_class_obj = (struct MyriadObject*) CompartmentClass;
		memcpy( (void**) &mech_class_obj->m_class, &CompartmentClass, sizeof(void*));

		#ifdef CUDA
		if (init_cuda)
		{
			void* tmp_comp_c_t = myriad_cudafy((void*)CompartmentClass, 1);
			((struct MyriadClass*) CompartmentClass)->device_class = (struct MyriadClass*) tmp_comp_c_t;
			CUDA_CHECK_RETURN(
				cudaMemcpyToSymbol(
					(const void*) &CompartmentClass_dev_t,
					&tmp_comp_c_t,
					sizeof(struct CompartmentClass*),
					0,
					cudaMemcpyHostToDevice
					)
				);
		}
		#endif
	}
	
	if (!Compartment)
	{
		Compartment = 
			myriad_new(
				   CompartmentClass,
				   MyriadObject,
				   sizeof(struct Compartment),
				   myriad_ctor, Compartment_ctor,
				   simul_fxn, Compartment_simul_fxn,
				   add_mechanism, Compartment_add_mech,
				   0
			);

		#ifdef CUDA
		if (init_cuda)
		{
			void* tmp_mech_t = myriad_cudafy((void*)Compartment, 1);
			((struct MyriadClass*) Compartment)->device_class = (struct MyriadClass*) tmp_mech_t;
			CUDA_CHECK_RETURN(
				cudaMemcpyToSymbol(
					(const void*) &Compartment_dev_t,
					&tmp_mech_t,
					sizeof(struct Compartment*),
					0,
					cudaMemcpyHostToDevice
					)
				);

		}
		#endif
	}
	
}
