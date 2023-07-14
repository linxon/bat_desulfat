/*
 * main.h
 *
 * Created: 07.07.2023 20:40:29
 *  Author: linxon
 */


#ifndef _MAIN_H_
#define _MAIN_H_

#include "typedef.h"
#include "timer.h"
#include "usart.h"


#define PULSE_PWM_PIN					(PB1)
#define DISCH_PIN						(PB0)
#define POT_CHARGE_SET_PIN				(PB3)
#define BAT_LEVEL_ADC_PIN				(PB5)
#define LED_STATUS_PIN					(PB2) // PB2 используется так же в UART режиме

#define MAX_CHARGE_CYCLE_IN_DAYS		(5u) // 5 дней работаем
#define MAX_CHARGE_TIME_IN_HOUR			(8u)
#define MAX_DISCH_TIME_IN_HOUR			MAX_CHARGE_TIME_IN_HOUR

#define CHARGE_CYCLE_STATE_DISCH		(0x1)
#define CHARGE_CYCLE_STATE_HALF			(0x2)
#define CHARGE_CYCLE_STATE_PULSE		(0x4)
#define CHARGE_CYCLE_STATE_FULL			(0x8)

#define TARGET_CHARGE_LEVEL_DISCH		(10.5) //  //11.8
#define TARGET_CHARGE_LEVEL_HALF		(12.2) // 10.9 - под нагрузкой 20W
#define TARGET_CHARGE_LEVEL_PULSE		(12.7) // 11.4
#define TARGET_CHARGE_LEVEL_FULL		TARGET_CHARGE_LEVEL_PULSE

#define DISCHARGING_RL_ENABLE()			(GPIO_B.port |= _BV(DISCH_PIN))
#define DISCHARGING_RL_DISABLE()		(GPIO_B.port &= ~_BV(DISCH_PIN))
#define DISCHARGING_RL_STATE()			(BIT_AS_BOOLEAN(GPIO_B.pin, DISCH_PIN))

#define TIMER0_RESET() (						\
	TCCR0A = 0x0,								\
	TCCR0B = 0x0,								\
	OCR0A = 0,									\
	OCR0B = 0									\
)

#define TIMER0_PWM_SETUP() (					\
	GPIO_B.ddr |= _BV(PULSE_PWM_PIN),			\
	TCCR0A |= _BV(WGM00) | _BV(WGM01),  /* включаем 7-й режим Fast PWM */ \
	TCCR0A &= ~_BV(COM0B0) | ~_BV(COM0B1) | ~_BV(COM0A0) | ~_BV(COM0A1), \
	TCCR0B |= _BV(CS00) | _BV(CS01) | _BV(WGM02), /* выбираем clk/64 */ \
	OCR0A = 125, /* задаем TOP значение. ѕолучаем clk/64/125 = 1000Hz */ \
	OCR0B = 13 /* 125/10% = 12.5 */				\
)

#define TIMER0_ENABLE_PWM_CLK() (				\
	TCCR0A &= ~_BV(COM0B0),	/* как Fast PWM */	\
	TCCR0A |= _BV(COM0B1)						\
)
#define TIMER0_DISABLE_PWM_CLK() (				\
	TCCR0A &= ~_BV(COM0B0),	/* как обычный I/O порт */ \
	TCCR0A &= ~_BV(COM0B1),						\
	GPIO_B.port &= ~_BV(PULSE_PWM_PIN)/* LOW */	\
)
#define TIMER0_PWM_IS_ENABLED()			(BIT_AS_BOOLEAN(TCCR0B, CS00) && BIT_AS_BOOLEAN(TCCR0B, CS01))

#define UART_BEGIN()					(TIMER0_RESET(), USART_TIMER0_SETUP())
#define UART_END()						(TIMER0_RESET(), TIMER0_PWM_SETUP())

#define SWITCH_TO_NEXT(next_state, curr_state)	\
	do {										\
		next_state = curr_state;				\
		if (next_state >= 0x8)					\
			next_state = 0x1;					\
		else if (next_state != 0)				\
			next_state <<= 1;					\
		else									\
			next_state = 0x1;					\
	} while (0)

#define ADC_CHANNEL_0					(0x0)
#define ADC_CHANNEL_1					(0x1)
#define ADC_CHANNEL_2					(0x2)
#define ADC_CHANNEL_3					(0x3)
#define ADC_SET_CHANNEL(ch)				(ADMUX &= 0xF0, ADMUX |= (ch << 0))
#define ADC_START_CONV()						\
	do {										\
		ADCSRA |= _BV(ADSC);					\
		while (ADCSRA & (1 << ADSC));			\
	} while(0)


typedef union charge_cycle {
	byte state;
	struct {
		byte discharging		: 1;
		byte half_charging		: 1;
		byte pulse_charging		: 1;
		byte full_charging		: 1;
		byte disch_curr_state	: 1;
		byte half_curr_state	: 1;
		byte pulse_curr_state	: 1;
		byte full_curr_state	: 1;
	};
} charge_cycle_t;

static void init_me(void);
static void send_uart_msg(const char *s);
static float get_battery_level(void);
static void charge_err(void);
static void charge_ok(void);
static void callback_send_info(void);
static void callback_blink_led(void);

#endif /* _MAIN_H_ */
