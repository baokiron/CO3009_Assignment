#include "scheduler.h"
#include <stddef.h>

static sTask    SCH_tasks_G[SCH_MAX_TASKS];
static uint32_t SCH_count = 0U;
static uint32_t SCH_next_id = 1U;

#if defined(__arm__) || defined(__thumb__)
static inline uint32_t sch_enter_critical(void)
{
    uint32_t primask;
    __asm volatile ("mrs %0, primask\n\tcpsid i" : "=r" (primask) :: "memory");
    return primask;
}

static inline void sch_exit_critical(uint32_t primask)
{
    __asm volatile ("msr primask, %0" :: "r" (primask) : "memory");
}
#else
static inline uint32_t sch_enter_critical(void) { return 0U; }
static inline void sch_exit_critical(uint32_t primask) { (void)primask; }
#endif

static uint32_t sch_insert_task(void (*pFunc)(void), uint32_t delay,
                                uint32_t period, uint32_t taskID)
{
    uint32_t primask = sch_enter_critical();

    if (pFunc == NULL || SCH_count >= SCH_MAX_TASKS) {
        sch_exit_critical(primask);
        return SCH_INVALID_TASK_ID;
    }

    uint32_t delay_rem = delay;
    uint32_t idx = 0U;

    while (idx < SCH_count && delay_rem >= SCH_tasks_G[idx].Delay) {
        delay_rem -= SCH_tasks_G[idx].Delay;
        idx++;
    }

    if (idx < SCH_count) {
        SCH_tasks_G[idx].Delay -= delay_rem;
        for (uint32_t j = SCH_count; j > idx; j--) {
            SCH_tasks_G[j] = SCH_tasks_G[j - 1U];
        }
    }

    SCH_tasks_G[idx].pTask  = pFunc;
    SCH_tasks_G[idx].Delay  = delay_rem;
    SCH_tasks_G[idx].Period = period;
    SCH_tasks_G[idx].RunMe  = 0U;
    SCH_tasks_G[idx].TaskID = taskID;
    SCH_count++;

    sch_exit_critical(primask);
    return taskID;
}

void SCH_Init(void)
{
    uint32_t primask = sch_enter_critical();
    SCH_count = 0U;
    SCH_next_id = 1U;
    for (uint32_t i = 0U; i < SCH_MAX_TASKS; i++) {
        SCH_tasks_G[i].pTask  = NULL;
        SCH_tasks_G[i].Delay  = 0U;
        SCH_tasks_G[i].Period = 0U;
        SCH_tasks_G[i].RunMe  = 0U;
        SCH_tasks_G[i].TaskID = SCH_INVALID_TASK_ID;
    }
    sch_exit_critical(primask);
}

uint32_t SCH_Add_Task(void (*pFunc)(void), uint32_t DELAY, uint32_t PERIOD)
{
    if (pFunc == NULL) {
        return SCH_INVALID_TASK_ID;
    }

    uint32_t primask = sch_enter_critical();
    uint32_t id = SCH_next_id++;
    if (SCH_next_id == SCH_INVALID_TASK_ID) {
        SCH_next_id = 1U;
    }
    sch_exit_critical(primask);

    return sch_insert_task(pFunc, DELAY, PERIOD, id);
}

uint8_t SCH_Delete_Task(uint32_t taskID)
{
    if (taskID == SCH_INVALID_TASK_ID) {
        return 0U;
    }

    uint32_t primask = sch_enter_critical();
    uint32_t idx = 0U;

    while (idx < SCH_count && SCH_tasks_G[idx].TaskID != taskID) {
        idx++;
    }

    if (idx >= SCH_count) {
        sch_exit_critical(primask);
        return 0U;
    }

    if (idx + 1U < SCH_count) {
        SCH_tasks_G[idx + 1U].Delay += SCH_tasks_G[idx].Delay;
    }

    for (uint32_t j = idx; j + 1U < SCH_count; j++) {
        SCH_tasks_G[j] = SCH_tasks_G[j + 1U];
    }

    SCH_count--;
    SCH_tasks_G[SCH_count].pTask  = NULL;
    SCH_tasks_G[SCH_count].Delay  = 0U;
    SCH_tasks_G[SCH_count].Period = 0U;
    SCH_tasks_G[SCH_count].RunMe  = 0U;
    SCH_tasks_G[SCH_count].TaskID = SCH_INVALID_TASK_ID;

    sch_exit_critical(primask);
    return 1U;
}

void SCH_Update(void)
{
    if (SCH_count == 0U) {
        return;
    }
    if (SCH_tasks_G[0].Delay > 0U) {
        SCH_tasks_G[0].Delay--;
    }
    if (SCH_tasks_G[0].Delay == 0U) {
        SCH_tasks_G[0].RunMe = 1U;
    }
}

void SCH_Dispatch_Tasks(void)
{
    while (1) {
        uint32_t primask = sch_enter_critical();
        if (SCH_count == 0U || SCH_tasks_G[0].RunMe == 0U) {
            sch_exit_critical(primask);
            break;
        }

        sTask current = SCH_tasks_G[0];

        for (uint32_t i = 0U; i + 1U < SCH_count; i++) {
            SCH_tasks_G[i] = SCH_tasks_G[i + 1U];
        }
        SCH_count--;
        SCH_tasks_G[SCH_count].pTask  = NULL;
        SCH_tasks_G[SCH_count].Delay  = 0U;
        SCH_tasks_G[SCH_count].Period = 0U;
        SCH_tasks_G[SCH_count].RunMe  = 0U;
        SCH_tasks_G[SCH_count].TaskID = SCH_INVALID_TASK_ID;

        if (SCH_count > 0U && SCH_tasks_G[0].Delay == 0U) {
            SCH_tasks_G[0].RunMe = 1U;
        }
        sch_exit_critical(primask);

        if (current.Period > 0U) {
            sch_insert_task(current.pTask, current.Period, current.Period, current.TaskID);
        }

        if (current.pTask != NULL) {
            current.pTask();
        }
    }
}