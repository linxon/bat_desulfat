/*
 * gpio.h
 *
 * Created: 27.02.2022 16:52:01
 *  Author: linxon
 */


#ifndef GPIO_H_
#define GPIO_H_

#ifdef PINA
#define GPIO_A				(*(GPIO_t *) &PINA)
#endif
#ifdef PINB
#define GPIO_B				(*(GPIO_t *) &PINB)
#endif
#ifdef PINC
#define GPIO_C				(*(GPIO_t *) &PINC)
#endif
#ifdef PIND
#define GPIO_D				(*(GPIO_t *) &PIND)
#endif

typedef volatile union {
	uint32_t _reg : 24;
	struct {
		uint8_t pin;
		uint8_t ddr;
		uint8_t port;
	};
} GPIO_t;

#endif /* GPIO_H_ */

