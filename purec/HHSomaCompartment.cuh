#ifndef HHSOMACOMPARTMENT_CUH
#define HHSOMACOMPARTMENT_CUH

#ifdef CUDA

#include <cuda_runtime.h>
#include <cuda_runtime_api.h>

#include "MyriadObject.h"
#include "Compartment.h"
#include "HHSomaCompartment.h"

#include "MyriadObject.cuh"
#include "Compartment.cuh"
#include "Mechanism.cuh"

extern __device__ __constant__ struct HHSomaCompartmentClass* HHSomaCompartmentClass_dev_t;
extern __device__ __constant__ struct HHSomaCompartment* HHSomaCompartment_dev_t;

extern __device__ simul_fxn_t HHSomaCompartment_simul_fxn_t;

extern __device__ void HHSomaCompartment_cuda_simul_fxn(void* _self,
                                                        void** network,
                                                        const double global_time,
                                                        const uint64_t curr_step);
#endif /* CUDA */
#endif /* HHSOMACOMPARTMENT_CUH */
