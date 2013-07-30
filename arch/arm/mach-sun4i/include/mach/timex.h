/*
 * arch/arch/mach-sun4i/include/mach/timex.h
 * (C) Copyright 2010-2015
 * Reuuimlla Technology Co., Ltd. <www.reuuimllatech.com>
 * Benn Huang <benn@reuuimllatech.com>
 *
 * core header
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 */

#ifndef __SW_TIMEX_H
#define __SW_TIMEX_H

#define SYS_TIMER_SCAL      (16)            /* timer clock source pre-divsion   */
#define SYS_TIMER_CLKSRC    (24000000)      /* timer clock source               */
#define TMR_INTER_VAL       (SYS_TIMER_CLKSRC/(SYS_TIMER_SCAL*HZ))

#define CLOCK_TICK_RATE     TMR_INTER_VAL 

#endif
