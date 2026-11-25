
#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include <stdint.h>


#define TICK_MS                 10      /* scheduler tick = TIM2 period (ms)   */
#define MS_TO_TICKS(ms)         ((ms) / TICK_MS)

#define NUM_ROADS               2
#define NUM_BUTTONS             4       /* BTN_1..BTN_4 (D7..D10)              */
#define NUM_SW_TIMERS           10      /* software timers available           */

/* Default light durations, in seconds (tuning mode changes these) */
#define DEFAULT_TIME_RED        5
#define DEFAULT_TIME_GREEN      3
#define DEFAULT_TIME_AMBER      2

/* Limits for tuning mode (seconds). Constraint: RED = GREEN + AMBER.       */
#define MIN_TIME                1
#define MAX_TIME                99


typedef enum {
    LIGHT_OFF = 0,
    LIGHT_RED,
    LIGHT_GREEN,
    LIGHT_AMBER
} LightColor;

/* Top-level mode of the whole system */
typedef enum {
    MODE_AUTO = 0,
    MODE_MANUAL,
    MODE_TUNING
} SystemMode;

/* Single state variable shared by all FSMs (as in the suggested skeleton).  */
typedef enum {
    /* --- Automatic mode (owner: A) --- */
    AUTO_INIT = 0,
    AUTO_RED1_GREEN2,
    AUTO_RED1_AMBER2,
    AUTO_GREEN1_RED2,
    AUTO_AMBER1_RED2,

    /* --- Manual mode (owner: B) --- */
    MAN_INIT,
    MAN_RED1_GREEN2,
    MAN_RED1_AMBER2,
    MAN_GREEN1_RED2,
    MAN_AMBER1_RED2,

    /* --- Tuning mode (owner: C) --- */
    TUNE_INIT,
    TUNE_RED,
    TUNE_GREEN,
    TUNE_AMBER
} SystemState;


extern SystemMode  mode;            /* current top-level mode                 */
extern SystemState status;          /* current state of the active FSM       */

/* Light durations in seconds. Written by fsm_tuning, read by fsm_automatic. */
extern uint8_t time_red;
extern uint8_t time_green;
extern uint8_t time_amber;

/* Latest sensor values. Written by dht20 task, read by LCD display.         */
extern float dht20_temperature;     /* degrees Celsius                       */
extern float dht20_humidity;        /* percent RH                            */


#define BTN_MODE        0   /* switch mode: AUTO -> MANUAL -> TUNING -> AUTO  */
#define BTN_NEXT        1   /* manual: next state / tuning: select light      */
#define BTN_INC         2   /* tuning: increase duration                      */
#define BTN_CONFIRM     3   /* tuning: confirm value                          */


void global_init(void);     /* sets defaults for mode, status, time_*         */

#endif /* INC_GLOBAL_H_ */