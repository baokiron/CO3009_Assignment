/*
 * software_timer.h - Software timers for FSMs
 */

#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#include "global.h"

extern uint8_t timer_flag[NUM_SW_TIMERS];

void setTimer(uint8_t index, uint32_t duration);
void clearTimer(uint8_t index);
void timer_run(void);               /* scheduled task, period 1 tick */

#endif /* INC_SOFTWARE_TIMER_H_ */