/*
 * scheduler.h - O(1) cooperative task scheduler
 */

#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include <stdint.h>

#define SCH_MAX_TASKS       40
#define SCH_INVALID_TASK_ID 0U

typedef struct {
    void     (*pTask)(void);    /* pointer to the task function */
    uint32_t Delay;             /* remaining ticks (relative to previous task) */
    uint32_t Period;            /* period in ticks, 0 = one-shot task */
    uint8_t  RunMe;             /* flag: task ready to run */
    uint32_t TaskID;            /* unique non-zero task identifier */
} sTask;

void     SCH_Init(void);
uint32_t SCH_Add_Task(void (*pFunc)(void), uint32_t DELAY, uint32_t PERIOD);
uint8_t  SCH_Delete_Task(uint32_t taskID);
void     SCH_Update(void);          /* call in TIM2 interrupt callback */
void     SCH_Dispatch_Tasks(void);  /* call in while(1) */

#endif /* INC_SCHEDULER_H_ */