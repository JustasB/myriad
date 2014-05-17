/**
   @file    HHLeakMechanism.h
 
   @brief   Hodgkin-Huxley Leak Mechanism class definition file.
 
   @details Defines the Hodgkin-Huxley Leak Mechanism class specification for Myriad
 
   @author  Pedro Rittner
 
   @date    April 9, 2014
 */
#ifndef HHLEAKMECHANISM_H
#define HHLEAKMECHANISM_H

#include "MyriadObject.h"
#include "Mechanism.h"

#include "HHLeakMechanism_meta.h"

// Generic pointers for new/class-of purposes

extern const void* HHLEAKMECHANISM_OBJECT;
extern const void* HHLEAKMECHANISM_CLASS;

/**
   HHLeakMechanism mechanism for Hodgkin-Huxley leak channel.

   @see Mechanism
 */
struct HHLEAKMECHANISM_OBJECT
{
	struct Mechanism HHLEAKMECHANISM_OBJECT_NAME; //! HHLeakMechanism : Mechanism
	double HHLEAKMECHANISM_G_LEAK;      //! Leak Conductance - nS
	double HHLEAKMECHANISM_E_REV;       //! Leak Reversal Potential - mV
};

struct HHLEAKMECHANISM_CLASS
{
	struct MechanismClass HHLEAKMECHANISM_CLASS_NAME; //! HHLeakMechanismClass : MechanismClass
};

void initHHLeakMechanism(int cuda_init);

#endif
