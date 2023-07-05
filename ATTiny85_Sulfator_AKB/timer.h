/*
 * timer.c
 *
 * Created: 03.03.2022 18:23:14
 *  Author: linxon
 */


#ifndef _TIMER_H_
#define _TIMER_H_		1

#include <time.h>
#include "typedef.h"
#include "functions.h"

#define TIMER_MAX_TASK_SUPPORT	4

#define SEC_TO_MS(sec)					(sec * 1000)
#define MIN_TO_SEC(min)					(min * 60u)
#define HOUR_TO_SEC(hour)				(hour * ONE_HOUR)
#define DAY_TO_SEC(day)					(day * ONE_DAY)

typedef enum {
	TIMER_DELAY_MODE_MS,
	TIMER_DELAY_MODE_US
} TIMER_DELAY_MODE_t;

typedef struct timer_counter_t {
	const uint16_t count;
	const uint8_t div;
	uint16_t work_cnt;
} TIMER_COUNTER_t;

typedef struct timer_task_sp_t {
	byte *sp;
} TIMER_TASK_SP_t;

typedef struct timer_task_t {
	void (*p_func)(void);
	uint8_t delay;
	uint8_t period;
	volatile uint8_t state;

	struct timer_task_sp_t context;
} TIMER_TASK_t[TIMER_MAX_TASK_SUPPORT];

/*
ѕример:
	while (timer_loop_begin(1, 10, 5)) { // 10 * 5 = 50ms (закончить цикл после 50ms) - 10 попыток через каждые 5ms

		<code here>

		timer_loop_until_is_set(1);
	}
*/

extern volatile uint64_t millis_counter;

extern void timer_ISR_millis_counter(void);
extern void timer_millis_task_tick(void (*callback)(void), uint64_t *last_tick, uint16_t delay_ms);
extern bool timer_loop_begin(struct timer_counter_t *);
extern bool timer_loop_until_is_set(TIMER_DELAY_MODE_t);
extern void timer_reset_counter(void);

extern void timer_ISR_next_task(void);
extern void timer_task_init(struct timer_task_t (*)[], uint8_t);

#endif /* _TIMER_H_ */
