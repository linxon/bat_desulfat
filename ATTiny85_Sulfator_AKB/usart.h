#ifndef _USART_H_
#define _USART_H_ 				1

//#define BAUD					38400
//#include <util/setbaud.h>
#include "typedef.h"

#ifndef USART_BAUDRATE
#warning "USART_BAUDRATE: baudrate is not configured - use 9600UL"
#define USART_BAUDRATE					9600UL
#endif

#define USART_GPIO						GPIO_B
#define USART_GPIO_TX_PIN				PB2

#ifndef USART_DATA_LENGTH
#define USART_DATA_LENGTH				(8)
#endif

#define USART_TIMER_MODE_FREE			(0)
#define USART_TIMER_MODE_TX				(1)
#define USART_TIMER_MODE_RX				(2)

#define USART_START_BIT_IDX				(0)
#define USART_DATA_IDX					(1)
#define USART_STOP_BIT_IDX				(USART_DATA_LENGTH + 1)

#define USART_STATUS_OK					(0)
#define USART_STATUS_START_BIT_ERR		(1)
#define USART_STATUS_FRAME_ERR			(2)
#define USART_STATUS_OVERRUN_ERR		(3)
#define USART_STATUS_SHIFT_REG_RDY		(4)
#define USART_STATUS_SHIFT_REG_NRDY		(5)

#define USART_RX_PIN_STATUS()			BIT_AS_BOOLEAN(USART_GPIO.pin, USART_GPIO_RX_PIN)
#define USART_TX_SET_HIGH()				(USART_GPIO.port |=  _BV(USART_GPIO_TX_PIN))
#define USART_TX_SET_LOW()				(USART_GPIO.port &= ~_BV(USART_GPIO_TX_PIN))

#define USART_TIMER0_TICKS2COUNT		(F_CPU / (USART_BAUDRATE * 8UL) - 1) // clk/8
//#define USART_TIMER0_COMMAND_FREQ		(166666UL) // команды забирают себе примерно 6 микросекунд: 1 / 6e-6 = 166666 Hz
//#define USART_TIMER0_PRESCALLER_FREQ	(F_CPU / 8UL) // предделитель таймера настроен на clk/8

// вычитаем время работы команд и сдвигаем влево проверочный тик стартового бита
#define USART_TIMER0_COEFF_SUBTICK		((USART_TIMER0_TICKS2COUNT / 8)) // + USART_TIMER0_PRESCALLER_FREQ / USART_TIMER0_COMMAND_FREQ)
#define USART_TIMER0_HALF_TICKS2COUNT	(USART_TIMER0_TICKS2COUNT / 2 - USART_TIMER0_COEFF_SUBTICK)

#define USART_TIMER0_SETUP() (							\
	TIMSK &= ~_BV(OCIE0A),								\
	TCCR0A = _BV(WGM01),								\
	TCCR0B = 0,											\
	TCNT0 = 0											\
)

#define USART_TIMER0_GET_COMP_V() (						\
	OCR0A												\
)

#define USART_TIMER0_SET_COMP_V(v) (					\
	OCR0A = (v)											\
)

#define USART_TIMER0_RESET() (							\
	TIMSK &= ~_BV(OCIE0A),								\
	TCCR0B = 0,											\
	GTCCR |= _BV(PSR0),									\
	TCNT0 = 0											\
)

#define USART_TIMER0_INTRR_ENABLE() (					\
	TIMSK |= _BV(OCIE0A),								\
	TCCR0B = _BV(CS01)									\
)

#define USART_TIMER0_INTRR_IS_ENABLED() (				\
	TIMSK & _BV(OCIE0A)									\
)

typedef struct {
	byte rx_data_idx	:4; // не используем
	byte tx_data_idx	:4;
	byte timer_mode		:2;
	byte status			:5;
} USART_STAT_REG_t;

#ifndef USART_DISABLE_STD
extern FILE usart_in;
extern FILE usart_out;
#endif

extern volatile USART_STAT_REG_t SWUCSR;

/*
	ПРИМЕР:

	char d;

	ISR(TIM0_COMPA_vect)	{ sw_usart_ISR_timer_handler(); }
	ISR(INT0_vect)			{ sw_usart_ISR_rx_handler(); }

	void main(void) {
		usart_init();
		GPIO_B.ddr |= _BV(PB0);

		do {

			d = rx_usart(NULL);

			if (d == 'A')
				GPIO_B.port ^= _BV(PB0);

			tx_usart('B', NULL);

		} while(1);
	}
*/
extern void sw_usart_ISR_timer_handler(void);
extern void usart_init(void);
extern int tx_usart(char, FILE *);

#endif /* _USART_H_ */
