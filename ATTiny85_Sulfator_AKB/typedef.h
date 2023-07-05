#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "gpio.h"

#ifndef E2START
#define E2START				0
#endif

#define EMPTY_BYTE			(0x00)
#define HIGH				(0x1)
#define LOW					(0x0)
#define TRUE				(0x1)
#define FALSE				(0x0)
#define ERROR				(1)
#define UNKNOWN_ERROR		(-1)
#define SUCCESS				(0)

#define ARRAY_SIZE(x)			(sizeof((x)) / sizeof((x)[0]))
#define IS_SREG_I_ENABLED()		(SREG & _BV(SREG_I))
#define IS_NUMERICAL(v)			(((v) >= 0 && (v) <= 9)? (v): -1)
#define CONV_NUM_2_ALPHA(v)		((v) + '0')
#define CONV_ALPHA_2_NUM(v)		((v) - '0')
#define BIT_AS_BOOLEAN(a, b)	(((a) & (1 << b)) && TRUE)

typedef unsigned char bool;
typedef unsigned char byte;

void main(void) __attribute__((noreturn));

#endif
