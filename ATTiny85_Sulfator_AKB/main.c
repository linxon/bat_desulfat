/*
 * main.c
 *
 * Created: 07.07.2023 20:40:29
 *  Author: Yury Martynov (linxon)
 */

#include "main.h"


/*
Алгоритм работы
	1) При включении устройства переводим в режим "разряда АКБ" (включаем разрядную лампу). Разряжаем до 10.5В
	   и тут же замеряем напряжение на аккумуляторе. Если оно меньше 10.5В - отключаем разрядную лампу и идем дальше.
	2) Переводим в режим "зарядка наполовину". Заряжаем до тех пор, пока на клемниках не будет
	   напряжение больше 12.2В (более 50% заряда АКБ)
	3) Напряжение проверяем через нагрузку (разрядная лампа) каждые 10минут. Разрядная лампа горит 1 минуту (напр. замеряем вконце)
	4) Если напряжение больше 12.2В, то переходим в режим "импульсная зарядка". Иначе идем в пункт 2
	5) Держим АКБ в режиме "импульсная зарядка" в течении 1-8 часов на одном цикле. Если эти 1-8 часов истекли - переходим в пункт 8
	6) Напряжение проверяем через нагрузку (разрядная лампа) каждые 30 минут. Разрядная лампа горит 3 минуты (напр. замеряем вконце)
	7) Если напряжение больше 12.7В, то переходим к концу цикла (пункт 8). Иначе идем в пункт 5
	8) Проверяем конец цикла заряда-разряда АКБ. Если цикл повторяется уже более 5-ти дней - выходим
	   из цикла, разряжаем АКБ и включаем "полную зарядку". Иначе сразу же переходим в пункт 1.
	9) "Полную зарядку" делаем около 8-ми часов, потом мигаем разрядной лампой раз в 500мс. Десульфатация завершена.

Дополнительно
	1) "Импульсная зарядка" задается внешним потенциометром. От 1-го и до 8-ми часов
	2) ...

*/


static uint64_t last_timer_v;
static uint64_t last_led_blink_v;

static time_t curr_time, start_time, end_time;
static int8_t start_mday, last_mday;
static struct tm time_d = {
	.tm_year = (2023 - 1900),
	.tm_mon = JANUARY,
	.tm_wday = MONDAY,
	.tm_mday = 1,
	.tm_hour = 0,
	.tm_min = 0,
	.tm_sec = 0
};

static charge_cycle_t m_cycle = {
	.half_curr_state = FALSE,
	.pulse_curr_state = FALSE
};

static uint8_t pot_charge_set;
static uint8_t led_blink_times;

static volatile float v_bat = 0.0;

ISR(TIM0_COMPA_vect) { sw_usart_ISR_timer_handler(); }
ISR(TIM1_COMPA_vect) {
	TCNT1 = 0;
	timer_ISR_millis_counter();

	if ((millis_counter - last_timer_v) > 1000) {
		system_tick();
		curr_time = time(NULL);
		last_timer_v = millis_counter;
	}
}

void main(void) {
	init_me();

	do {

		wdt_reset();
		localtime_r(&curr_time, &time_d);

		timer_millis_task_tick(&blink_led, &last_led_blink_v, 2500);

		// считаем, сколько уже дней работает цикл заряда-разряда АКБ
		if (last_mday != time_d.tm_mday) {
			last_mday = time_d.tm_mday;
			start_mday++;
		}

		// если за эти дни мы не вышли из цикла, то уходим в аварию
		if (start_mday >= MAX_CHARGE_CYCLE_IN_DAYS+1 && !m_cycle.full_curr_state)
			charge_err();

		// считаем, что у нас на потенциометре
		ADC_SET_CHANNEL(ADC_CHANNEL_3);
		ADC_START_CONV();
		pot_charge_set = (uint8_t) (ADC / (1024 / MAX_CHARGE_TIME_IN_HOUR)) + 1;

		// батарею нужно разрядить до 10.5
		if (m_cycle.discharging) {
			if (DISCHARGING_RL_STATE() == LOW) {
				TIMER0_DISABLE_PWM_CLK(); // отключаем подачу импульсов
				DISCHARGING_RL_ENABLE(); // переключаем на режим разряда АКБ
				led_blink_times = 1;
			}

			// ждем, когда батарея разрядится до 10.5В
			if (get_battery_level() > TARGET_CHARGE_LEVEL_DISCH)
				continue;

			if (start_mday <= MAX_CHARGE_CYCLE_IN_DAYS)
				m_cycle.state = CHARGE_CYCLE_STATE_HALF; // батарея разрядилась - переходим к следующему режиму
			else
				m_cycle.state = CHARGE_CYCLE_STATE_FULL; // или завершаем зарядку. Переходим на FULL CHARGE

			continue;
		}

		// перед импульсным режимом батарею нужно зарядить до 50%
		if (m_cycle.half_charging) {
			if (!m_cycle.half_curr_state) {
				TIMER0_DISABLE_PWM_CLK(); // отключаем подачу импульсов
				DISCHARGING_RL_DISABLE(); // переключаем на режим заряда АКБ
				start_time = time(NULL);
				m_cycle.half_curr_state = TRUE;
				led_blink_times = 2;
			}

			// заряжаем 10 минут
			if ((time(NULL) - start_time) < MIN_TO_SEC(10))
				continue;

			// разряжаем (11 - 10 = 1 минуту), потом проверяем напряжение под нагрузкой
			DISCHARGING_RL_ENABLE();
			if ((time(NULL) - start_time) < MIN_TO_SEC(11))
				continue;

			m_cycle.half_curr_state = FALSE;

			// через минуту проверяем напряжение.
			if (get_battery_level() < TARGET_CHARGE_LEVEL_HALF)
				continue; // если оно меньше - заряжаем еще

			m_cycle.state = CHARGE_CYCLE_STATE_PULSE;
			continue;
		}

		// заряжаем в импульсном режиме. Делаем десульфатацию АКБ
		if (m_cycle.pulse_charging) {
			if (!m_cycle.pulse_curr_state) {
				DISCHARGING_RL_DISABLE(); // переключаем на режим заряда АКБ
				_delay_ms(500); // ждем, когда реле переключится и разогреется зарядная лампа
				TIMER0_ENABLE_PWM_CLK(); // и включаем режим пульсации
				start_time = end_time = time(NULL); // обновляем время ожидания
				m_cycle.pulse_curr_state = TRUE;
				m_cycle.full_curr_state = FALSE;
				led_blink_times = 3;
			}

			// через 30 минут делаем пробу
			if ((time(NULL) - start_time) >= MIN_TO_SEC(30) && !m_cycle.full_curr_state) {
				TIMER0_DISABLE_PWM_CLK();
				_delay_ms(100);
				DISCHARGING_RL_ENABLE();

				// разряжаем (33 - 30 = 3 минуты), потом проверяем напряжение под нагрузкой
				if ((time(NULL) - start_time) < MIN_TO_SEC(33))
					continue;

				if (get_battery_level() >= TARGET_CHARGE_LEVEL_FULL)
					m_cycle.full_curr_state = TRUE;

				// иначе же продолжаем заряжать
				DISCHARGING_RL_DISABLE();
				_delay_ms(500); // ждем, когда реле переключится и разогреется зарядная лампа
				TIMER0_ENABLE_PWM_CLK();

				start_time = time(NULL);
			}

			// ждем 1-8 часов зарядки в режиме пульсации
			if ((time(NULL) - end_time) < HOUR_TO_SEC(pot_charge_set)
				&& !m_cycle.full_curr_state)
				continue;

			m_cycle.pulse_curr_state = FALSE;
			m_cycle.full_curr_state = FALSE;
			m_cycle.state = CHARGE_CYCLE_STATE_DISCH;

			continue;
		}

		// завершаем цикл полной, восьмичасовой зарядкой АКБ
		if (m_cycle.full_charging) {
			if (!m_cycle.full_curr_state) {
				DISCHARGING_RL_DISABLE();
				TIMER0_DISABLE_PWM_CLK();
				start_time = time(NULL);
				m_cycle.full_curr_state = TRUE;
				led_blink_times = 4;
			}

			// делаем полную зарядку в течении 8-ми часов
			if ((time(NULL) - start_time) < HOUR_TO_SEC(8))
				continue;

			m_cycle.full_curr_state = FALSE;
			charge_ok(); // и уходим в режим индикации "Готово"
		}

	} while(1);
}

void init_me(void) {
	wdt_enable(WDTO_2S);

	GPIO_B.ddr |= _BV(LED_STATUS_PIN);
	GPIO_B.port |= _BV(LED_STATUS_PIN);

	// ADC
	GPIO_B.ddr &= ~_BV(POT_CHARGE_SET_PIN) | ~_BV(BAT_LEVEL_ADC_PIN);
	ADMUX |= _BV(REFS2); // опорное напряжеие на внутренний 2.56V (без внешнего конденсатора)
	ADMUX |= _BV(REFS1);
	ADMUX &= ~_BV(REFS0);
	ADCSRA |= _BV(ADEN);

	GPIO_B.ddr |= _BV(DISCH_PIN);
	GPIO_B.port &= ~_BV(DISCH_PIN);
	TIMER0_PWM_SETUP();

	// Настраиваем таймер для timer_ISR_millis_counter()
	TCCR1 |= _BV(CS11) | _BV(CS12); // выбираем clk/32
	TCCR1 &= ~_BV(COM1A0) | ~_BV(COM1A1);
	PLLCSR &= ~_BV(PCKE); // заранее отключаем PLLCSR (включаем синхронный режим)
	TIMSK |= _BV(OCIE1A); // включаем прерывание по совпадению
	OCR1A = 250; // Получаем clk/32/250 = 1000Hz (1ms)

	TIMER0_DISABLE_PWM_CLK(); // включаем только по необходимости
	DISCHARGING_RL_DISABLE();
	m_cycle.state = CHARGE_CYCLE_STATE_DISCH;

	set_system_time(mktime(&time_d));
	curr_time = time(NULL);

	//usart_init();

	_delay_ms(200);

	sei();
}

void send_uart_msg(const char *s) {
	UART_BEGIN();
	while (*s != '\0')
		tx_usart((char) *s++, NULL);
	UART_END();
}

float get_battery_level(void) {
	_delay_ms(100);

	ADC_SET_CHANNEL(ADC_CHANNEL_0);
	ADC_START_CONV();

	/*
		15.0 - 2.29
		14.0 - 2.14
		13.0 - 1.99
		12.0 - 1.84
		11.0 - 1.69
		10.0 - 1.54
		2.29 / d0.15 = 15.26
	*/
	return v_bat = (ADC / (918 / 15.0));
}

void charge_err(void) {
	do {
		wdt_reset();
		TIMER0_DISABLE_PWM_CLK();
		DISCHARGING_RL_ENABLE();

		// сигнализируем об ошибке
		DISCHARGING_RL_ENABLE();
		GPIO_B.port |= _BV(LED_STATUS_PIN);
		_delay_ms(500);
		DISCHARGING_RL_DISABLE();
		GPIO_B.port &= ~_BV(LED_STATUS_PIN);
		_delay_ms(500);

		/*
		if (err_event_ok) // и ждем подтверждения
			break;
		*/
	} while (1);
}

void charge_ok(void) {
	do {
		wdt_reset();
		TIMER0_DISABLE_PWM_CLK();

		DISCHARGING_RL_ENABLE();
		GPIO_B.port |= _BV(LED_STATUS_PIN);
		_delay_ms(500);
		DISCHARGING_RL_DISABLE();
		GPIO_B.port &= ~_BV(LED_STATUS_PIN);
		_delay_ms(5000);

	} while (1);
}

void callback_send_info(void) {
	char c_buff[12];

	if (TIMER0_PWM_IS_ENABLED())
		return;
/*
	send_uart_msg("VER:0.2;");
	send_uart_msg("BL:");
	my_ftoa(bat_voltage, c_buff, 2);
	send_uart_msg(c_buff);
	send_uart_msg(";CYC:");
	my_itoa_8(m_cycle.state, c_buff);
	send_uart_msg(c_buff);
	send_uart_msg(";\n");
*/
}

void blink_led(void) {
	for (uint8_t i = 0; i < led_blink_times; ++i) {
		GPIO_B.port |= _BV(LED_STATUS_PIN);
		_delay_ms(250);
		GPIO_B.port &= ~_BV(LED_STATUS_PIN);
		_delay_ms(100);
	}
}
