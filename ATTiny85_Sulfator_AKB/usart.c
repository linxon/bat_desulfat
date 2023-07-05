#include "usart.h"


#ifndef USART_DISABLE_STD
FILE usart_out = FDEV_SETUP_STREAM(tx_usart, NULL, _FDEV_SETUP_WRITE);
#endif

volatile USART_STAT_REG_t SWUCSR;
static volatile uint16_t sw_shift_reg;

void inline __attribute__((always_inline)) sw_usart_ISR_timer_handler(void) {
	if (sw_shift_reg & (1 << SWUCSR.tx_data_idx++))
		USART_TX_SET_HIGH();
	else
		USART_TX_SET_LOW();

	if (SWUCSR.tx_data_idx == (USART_DATA_LENGTH + 2)) { // start bit + 8 bit + stop bit
		SWUCSR.tx_data_idx = 0;
		USART_TIMER0_RESET();
	}
}

void usart_init(void) {
	USART_GPIO.ddr |=  _BV(USART_GPIO_TX_PIN);

	USART_TX_SET_HIGH();
	//USART_TIMER0_SETUP();
}

int tx_usart(char data, FILE *stream) {
#ifndef USART_DISABLE_ASCII
	// это можно включить только в том случае, если используется передача в ascii формате
	if (data == '\n')
		tx_usart('\r', stream);
#else
	(void) stream;
#endif

	// подготавливаем фрейм для отправки
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		sw_shift_reg &= ~_BV(USART_START_BIT_IDX);
		sw_shift_reg |= (data << USART_DATA_IDX);
		sw_shift_reg |= _BV(USART_STOP_BIT_IDX);

		USART_TIMER0_SET_COMP_V(USART_TIMER0_TICKS2COUNT);
		USART_TIMER0_INTRR_ENABLE();
	}

	while (USART_TIMER0_INTRR_IS_ENABLED()); // ждем окончания передачи данных
	sw_shift_reg = 0;

	return SUCCESS;
}
