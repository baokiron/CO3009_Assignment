
#include "software_timer.h"

uint8_t timer_flag[NUM_SW_TIMERS] = {0};
static uint32_t timer_counter[NUM_SW_TIMERS] = {0};

void setTimer(uint8_t index, uint32_t duration)
{
    if (index >= NUM_SW_TIMERS) {
        return;
    }

    if (duration == 0U) {
        timer_counter[index] = 0U;
        timer_flag[index] = 1U;
    } else {
        timer_counter[index] = duration;
        timer_flag[index] = 0U;
    }
}

void clearTimer(uint8_t index)
{
    if (index >= NUM_SW_TIMERS) {
        return;
    }

    timer_counter[index] = 0U;
    timer_flag[index] = 0U;
}

void timer_run(void)
{
    for (uint8_t i = 0U; i < NUM_SW_TIMERS; i++) {
        if (timer_counter[i] > 0U) {
            timer_counter[i]--;
            if (timer_counter[i] == 0U) {
                timer_flag[i] = 1U;
            }
        }
    }
}