/**
 * @file    DCCurrentMech.h
 *
 * @brief   DC Current Mechanism definition file.
 *
 * @details Defines the DCCurrentMech class specification for Myriad
 *
 * @author  Pedro Rittner
 *
 * @date    May 5, 2014
 */
#ifndef DCCURRENTMECH_H
#define DCCURRENTMECH_H

#include <stdint.h>

#include "MyriadObject.h"
#include "Mechanism.h"


//! Generic pointer for new(DCCurrentMech) purposes
extern const void* DCCurrentMech;
//! Generic pointer for new(DCCurrentMechClass) purposes
extern const void* DCCurrentMechClass;

// -----------------------------------------

/**
 * DC Current Mechanism object structure definition.
 *
 * Stores DC Current Mechanism state.
 *
 * @see Mechanism
 */
struct DCCurrentMech
{
    //! DCCurrentMech : Mechanism
	const struct Mechanism _;
    //! Time step at which current starts flowing
	uint64_t t_start;
    //! Time step at which current stops flowing
	uint64_t t_stop;
    //! Current amplitude in nA
	double amplitude;
};

/**
 * DC Current Mechanism class structure definition.
 *
 * Defines DC Current Mechanism behavior.
 *
 * @see MechanismClass
 */
struct DCCurrentMechClass
{
    //! MechanismClass : MyriadClass
	struct MechanismClass _;
};

// -------------------------------------

/**
 * Initializes prototype DC Current Mechanism infrastructure on the heap.
 *
 * @param[in]  init_cuda  flag for directing CUDA protoype initialization
 */
extern void initDCCurrMech(const bool init_cuda);

#endif /* DCCURRENTMECH_H */
