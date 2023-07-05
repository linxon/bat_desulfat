/*
 * timer.c
 *
 * Created: 03.03.2022 18:23:03
 *  Author: linxon
 */

#include "timer.h"


volatile uint64_t millis_counter = 0;

static TIMER_COUNTER_t *tmr_loop_d;
static bool loop_end = FALSE;

static TIMER_TASK_t *tmr_task_d;
static uint8_t task_count;
static uint8_t curr_task;


void inline __attribute__((always_inline)) timer_ISR_millis_counter(void) {
	millis_counter++;
}

void timer_millis_task_tick(void (*callback)(void), uint64_t *last_tick, uint16_t delay_ms) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		if ((millis_counter - *last_tick) < delay_ms)
			return;

		*last_tick = millis_counter;
	}

	callback();
}

bool timer_loop_begin(struct timer_counter_t *t) {
	tmr_loop_d = t;

	if (loop_end == TRUE)
		return loop_end = FALSE;

	if (tmr_loop_d->work_cnt == 0) {
		loop_end = FALSE;
		tmr_loop_d->work_cnt = tmr_loop_d->count;
	}

	return TRUE;
}

bool timer_loop_until_is_set(TIMER_DELAY_MODE_t m) {
	(void) --tmr_loop_d->work_cnt;

	if (tmr_loop_d->work_cnt == 0) {
		loop_end = TRUE;
		return FALSE;
	}

	if (m == TIMER_DELAY_MODE_MS)
		my_delay_ms(tmr_loop_d->div);
	else
		my_delay_us(tmr_loop_d->div);

	return TRUE;
}

inline void timer_reset_counter(void) {
	tmr_loop_d->work_cnt = 0;
}

void timer_task_dispatch(byte *sp) {

}

void timer_ISR_next_task(void) {

	// сохраняем стек и выбираем следующий процесс
	//timer_task_stack_push((*tmr_task_d)[curr_task].context.sp);
	(*tmr_task_d)[curr_task].p_func();

	curr_task++;

	if (curr_task == task_count)
		curr_task = 0;
}

void timer_task_init(struct timer_task_t (*t)[], uint8_t size) {
	tmr_task_d = t;
	task_count = size;
}

