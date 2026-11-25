#include "fsm_automatic.h"
#include "global.h"
#include "software_timer.h"
#include "button.h"
#include "traffic_light.h"
#include "lcd.h"

#define TIMER_AUTO_LIGHT        0U
#define TIMER_AUTO_COUNTDOWN    1U

static uint8_t countdown_r1 = 0U;
static uint8_t countdown_r2 = 0U;

static void auto_print_two_digits(uint8_t val)
{
    if (val < 10U) {
        lcd_print("0");
    }
    lcd_print_int((int32_t)val);
}

static void auto_update_lcd_countdown(void)
{
    lcd_set_cursor(1U, 0U);
    lcd_print("R1:");
    auto_print_two_digits(countdown_r1);
    lcd_print("s  R2:");
    auto_print_two_digits(countdown_r2);
    lcd_print("s ");
}

static void auto_enter_state(SystemState next_state, LightColor c1, LightColor c2,
                             uint8_t duration_sec, uint8_t cd1, uint8_t cd2)
{
    /* Dat den do truoc (neu co) de dam bao hai duong khong bao gio cung xanh */
    if (c1 == LIGHT_RED) {
        traffic_set(1U, LIGHT_RED);
        traffic_set(2U, c2);
    } else {
        traffic_set(2U, c2);
        traffic_set(1U, c1);
    }

    setTimer(TIMER_AUTO_LIGHT, MS_TO_TICKS((uint32_t)duration_sec * 1000U));
    countdown_r1 = cd1;
    countdown_r2 = cd2;
    auto_update_lcd_countdown();
    status = next_state;
}

static void auto_handle_countdown(void)
{
    if (timer_flag[TIMER_AUTO_COUNTDOWN] == 1U) {
        setTimer(TIMER_AUTO_COUNTDOWN, MS_TO_TICKS(1000U));
        if (countdown_r1 > 0U) {
            countdown_r1--;
        }
        if (countdown_r2 > 0U) {
            countdown_r2--;
        }
        auto_update_lcd_countdown();
    }
}

static uint8_t auto_check_mode_switch(void)
{
    if (isButtonPressed(BTN_MODE)) {
        traffic_set(1U, LIGHT_RED);
        traffic_set(2U, LIGHT_RED);
        clearTimer(TIMER_AUTO_LIGHT);
        clearTimer(TIMER_AUTO_COUNTDOWN);
        mode = MODE_MANUAL;
        status = MAN_INIT;
        return 1U;
    }
    return 0U;
}

void fsm_automatic_run(void)
{
    if (mode != MODE_AUTO) {
        return;
    }

    switch (status) {
    case AUTO_INIT:
        traffic_set(1U, LIGHT_RED);
        traffic_set(2U, LIGHT_RED);
        clearTimer(TIMER_AUTO_LIGHT);
        clearTimer(TIMER_AUTO_COUNTDOWN);

        lcd_clear();
        lcd_set_cursor(0U, 0U);
        lcd_print("MODE: AUTO");

        setTimer(TIMER_AUTO_COUNTDOWN, MS_TO_TICKS(1000U));
        auto_enter_state(AUTO_RED1_GREEN2, LIGHT_RED, LIGHT_GREEN,
                         time_green, time_red, time_green);
        break;

    case AUTO_RED1_GREEN2:
        if (auto_check_mode_switch()) {
            return;
        }
        auto_handle_countdown();
        if (timer_flag[TIMER_AUTO_LIGHT] == 1U) {
            auto_enter_state(AUTO_RED1_AMBER2, LIGHT_RED, LIGHT_AMBER,
                             time_amber, time_amber, time_amber);
        }
        break;

    case AUTO_RED1_AMBER2:
        if (auto_check_mode_switch()) {
            return;
        }
        auto_handle_countdown();
        if (timer_flag[TIMER_AUTO_LIGHT] == 1U) {
            auto_enter_state(AUTO_GREEN1_RED2, LIGHT_GREEN, LIGHT_RED,
                             time_green, time_green, time_red);
        }
        break;

    case AUTO_GREEN1_RED2:
        if (auto_check_mode_switch()) {
            return;
        }
        auto_handle_countdown();
        if (timer_flag[TIMER_AUTO_LIGHT] == 1U) {
            auto_enter_state(AUTO_AMBER1_RED2, LIGHT_AMBER, LIGHT_RED,
                             time_amber, time_amber, time_amber);
        }
        break;

    case AUTO_AMBER1_RED2:
        if (auto_check_mode_switch()) {
            return;
        }
        auto_handle_countdown();
        if (timer_flag[TIMER_AUTO_LIGHT] == 1U) {
            auto_enter_state(AUTO_RED1_GREEN2, LIGHT_RED, LIGHT_GREEN,
                             time_green, time_red, time_green);
        }
        break;

    default:
        break;
    }
}