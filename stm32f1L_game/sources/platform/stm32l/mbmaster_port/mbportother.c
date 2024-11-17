/* 
 * MODBUS Library: Skeleton port
 * Copyright (c) 2008 Christian Walter <cwalter@embedded-solutions.at>
 * All rights reserved.
 *
 * $Id: mbportother.c,v 1.1 2008-04-06 07:46:23 cwalter Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include <stdlib.h>

/* ----------------------- Platform includes --------------------------------*/
#include "mbport.h"
#include "platform.h"
#include "sys_dbg.h"

/* ----------------------- Modbus includes ----------------------------------*/

/* ----------------------- Defines ------------------------------------------*/

/* ----------------------- Type definitions ---------------------------------*/

/* ----------------------- Static variables ---------------------------------*/

//static UBYTE    ubNesting = 0;

/* ----------------------- Static functions ---------------------------------*/

/* ----------------------- Start implementation -----------------------------*/

void
vMBPAssert( void )
{
#if 0
	volatile BOOL   bBreakOut = FALSE;

	vMBPEnterCritical(  );
	while( !bBreakOut );
#else
	FATAL("MBP", 0x01);
#endif
}

void
vMBPEnterCritical( void )
{
#if 0
	/* Disable interrupts and/or scheduler. Disabling interrupts is
	 * necessary if you call ANY of the MODBUS functions from an
	 * interrupt. Disabling the scheduler is necessary if you call
	 * any of the MODBUS functions from another thread.
	 * In general you should ALWAYS start by disabling both.
	 */

	/* Code for disabling interrupts and the scheduler. */
	if( ubNesting == 0 )
	{
		/* Store old processor status register, ... */
	}
	ubNesting++;
#else
	entry_critical();
#endif
}

void
vMBPExitCritical( void )
{
#if 0
	/* Code for disabling interrupts and the scheduler. */
	ubNesting--;
	if( 0 == ubNesting )
	{
		/* Check old status register if interrupts have been enabled.
		 * If yes reenable them. Of course the same holds for the
		 * scheduler.
		 */
		if( 0 /* Interrupts where enabled */ )
		{
			/* Code for enabling the interrupts and the scheduler. */
		}
	}
#else
	exit_critical();
#endif
}
