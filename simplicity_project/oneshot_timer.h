/*
 * timer.h
 *
 *  Created on: Jul 24, 2020
 *      Author: jgamm
 */

#ifndef ONESHOT_TIMER_H_
#define ONESHOT_TIMER_H_

#include <stdbool.h>
#include <stdint.h>
#include "em_timer.h"

#define ONESHOT_TIMER_NUM 1
#define ONESHOT_CC_NUM    0

#if ONESHOT_TIMER_NUM == 0
#define ONESHOT_TIMER TIMER0
#define ONESHOT_IRQHandler TIMER0_IRQHandler
#define ONESHOT_IRQn TIMER0_IRQn
#define cmuClock_ONESHOT cmuClock_TIMER0
#elif ONESHOT_TIMER_NUM == 1
#define ONESHOT_TIMER TIMER1
#define ONESHOT_IRQHandler TIMER1_IRQHandler
#define ONESHOT_IRQn TIMER1_IRQn
#define cmuClock_ONESHOT cmuClock_TIMER1
#elif ONESHOT_TIMER_NUM == 2
#define ONESHOT_TIMER TIMER2
#define ONESHOT_IRQHandler TIMER2_IRQHandler
#define ONESHOT_IRQn TIMER2_IRQn
#define cmuClock_ONESHOT cmuClock_TIMER2
#elif ONESHOT_TIMER_NUM == 3
#define ONESHOT_TIMER TIMER3
#define ONESHOT_IRQHandler TIMER3_IRQHandler
#define ONESHOT_IRQn TIMER3_IRQn
#define cmuClock_ONESHOT cmuClock_TIMER3
#elif ONESHOT_TIMER_NUM == 4
#define ONESHOT_TIMER TIMER4
#define ONESHOT_IRQHandler TIMER4_IRQHandler
#define ONESHOT_IRQn TIMER4_IRQn
#define cmuClock_ONESHOT cmuClock_TIMER4
#else
#error Invalid timer
#endif
#if ONESHOT_CC_NUM == 0
#define ONESHOT_IF_CC TIMER_IF_CC0
#define ONESHOT_IEN_CC TIMER_IEN_CC0
#elif ONESHOT_CC_NUM == 1
#define ONESHOT_IF_CC TIMER_IF_CC1
#define ONESHOT_IEN_CC TIMER_IEN_CC1
#elif ONESHOT_CC_NUM == 2
#define ONESHOT_IF_CC TIMER_IF_CC2
#define ONESHOT_IEN_CC TIMER_IEN_CC2
#else
#error Invalid channel
#endif

#define ONESHOT_is_set_fine() (((ONESHOT_TIMER->IF) & ONESHOT_IF_CC) == 0)

void ONESHOT_init(void);

void ONESHOT_configure(uint32_t time_us);

void ONESHOT_set(void);

bool ONESHOT_is_set_coarse(void);

void ONESHOT_delay(int time_ms);

void ONESHOT_clear(void);

#endif /* ONESHOT_TIMER_H_ */
