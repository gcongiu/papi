/****************************/
/* THIS IS OPEN SOURCE CODE */
/****************************/

/** 
 * @file    linux-L2unit.h
 * CVS:     $Id: linux-L2unit.h,v 1.2 2011/03/18 21:40:22 jagode Exp $
 * @author  Heike Jagode
 *          jagode@eecs.utk.edu
 * Mods:	<your name here>
 *			<your email address>
 * BGPM / L2unit component 
 * 
 * Tested version of bgpm (early access)
 *
 * @brief
 *  This file has the source code for a component that enables PAPI-C to 
 *  access hardware monitoring counters for BG/Q through the bgpm library.
 */

#ifndef _PAPI_L2UNIT_H
#define _PAPI_L2UNIT_H

#include "papi.h"
#include "papi_internal.h"
#include "papi_memory.h"
#include "../../../linux-bgq-common.h"


/*************************  DEFINES SECTION  ***********************************
 *******************************************************************************/

/* this number assumes that there will never be more events than indicated */
#define L2UNIT_MAX_COUNTERS PEVT_L2UNIT_LAST_EVENT
#define OFFSET ( PEVT_PUNIT_LAST_EVENT + 1 )

/* Stores private information for each event */
typedef struct L2UNIT_register
{
	unsigned int selector;
	/* Signifies which counter slot is being used */
	/* Indexed from 1 as 0 has a special meaning  */
} L2UNIT_register_t;


/* Used when doing register allocation */
typedef struct L2UNIT_reg_alloc
{
	L2UNIT_register_t ra_bits;
} L2UNIT_reg_alloc_t;


/* Holds control flags */
typedef struct L2UNIT_control_state
{
	int EventGroup;
	int EventGroup_local[512];
	int count;
	long long counters[L2UNIT_MAX_COUNTERS];
	int bgpm_eventset_applied;	// BGPM eventGroup applied yes or no flag
} L2UNIT_control_state_t;


/* Holds per-thread information */
typedef struct L2UNIT_context
{
	L2UNIT_control_state_t state;
} L2UNIT_context_t;


#endif /* _PAPI_L2UNIT_H */