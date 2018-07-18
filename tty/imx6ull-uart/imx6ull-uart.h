/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * i.MX 6ULL UART driver
 *
 * Copyright 2018 Phoenix Systems
 * Author: Kamil Amanowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _UARTDRV_H_
#define _UARTDRV_H_

enum { urxd = 0, utxd = 16, ucr1 = 32, ucr2, ucr3, ucr4, ufcr, usr1, usr2,
	uesc, utim, ubir, ubmr, ubrc, onems, uts, umcr };

enum { MODE_RAW, MODE_TTY };


enum { FL_COOL = 1, FL_SYNC = 2 };


enum { PAR_NONE, PAR_ODD, PAR_EVEN, PAR_MARK, PAR_SPACE };

#endif /* _UARTDRV_H_ */
