#include "fsm_manual.h"
#include "global.h"
#include "button.h"
#include "traffic_light.h"
#include "lcd.h"

static void manual_flush_buttons(void)          /* bo cac lan nhan con ton tu che do truoc */
{
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
        isButtonPressed(i);
        isButtonLongPressed(i);
    }
}

static void manual_enter(SystemState s, LightColor c1, LightColor c2, const char *line)
{
    traffic_set(1, c1);
    traffic_set(2, c2);
    lcd_set_cursor(0, 0);
    lcd_print(line);                            /* 16 ky tu */
    status = s;
}

/* Dat den + man hinh cua trang thai s (khong doi status ngoai s). */
static void manual_apply(SystemState s)
{
    switch (s) {
    case MAN_RED1_GREEN2:
        manual_enter(s, LIGHT_RED,   LIGHT_GREEN, "MANUAL 1:R 2:G  ");
        break;
    case MAN_RED1_AMBER2:
        manual_enter(s, LIGHT_RED,   LIGHT_AMBER, "MANUAL 1:R 2:A  ");
        break;
    case MAN_GREEN1_RED2:
        manual_enter(s, LIGHT_GREEN, LIGHT_RED,   "MANUAL 1:G 2:R  ");
        break;
    case MAN_AMBER1_RED2:
        manual_enter(s, LIGHT_AMBER, LIGHT_RED,   "MANUAL 1:A 2:R  ");
        break;
    default:
        break;
    }
}

static SystemState manual_next(SystemState s)
{
    switch (s) {
    case MAN_RED1_GREEN2: return MAN_RED1_AMBER2;
    case MAN_RED1_AMBER2: return MAN_GREEN1_RED2;
    case MAN_GREEN1_RED2: return MAN_AMBER1_RED2;
    default:              return MAN_RED1_GREEN2;
    }
}

void fsm_manual_run(void)
{
    switch (status) {
    case MAN_INIT:
        traffic_set(1, LIGHT_RED);              /* an toan truoc: ca hai duong do */
        traffic_set(2, LIGHT_RED);
        manual_flush_buttons();
        manual_apply(MAN_RED1_GREEN2);
        break;

    case MAN_RED1_GREEN2:
    case MAN_RED1_AMBER2:
    case MAN_GREEN1_RED2:
    case MAN_AMBER1_RED2:
        if (isButtonPressed(BTN_MODE)) {
            mode   = MODE_TUNING;
            status = TUNE_INIT;                 /* TUNE_INIT dat hai duong ve do */
        } else if (isButtonPressed(BTN_NEXT)) {
            manual_apply(manual_next(status));
        }
        break;

    default:
        break;
    }
}