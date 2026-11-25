
#include "fsm_tuning.h"
#include "global.h"
#include "software_timer.h"
#include "button.h"
#include "traffic_light.h"
#include "lcd.h"

#define TMR_TUNE        2                
#define FEEDBACK_MS     1500

enum { IDX_RED = 0, IDX_GREEN, IDX_AMBER };

static uint8_t           draft[3];      
static const char       *feedback = "";  
static uint8_t           redraw   = 0;
static const char *const NAME[3]  = { "RED  ", "GREEN", "AMBER" };

static uint8_t tune_is_valid(void)
{
    for (uint8_t i = 0; i < 3; i++) {
        if (draft[i] < MIN_TIME || draft[i] > MAX_TIME) return 0;
    }
    return (draft[IDX_RED] == draft[IDX_GREEN] + draft[IDX_AMBER]);
}

static void tune_flush_buttons(void)     
{
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
        isButtonPressed(i);
        isButtonLongPressed(i);
    }
}

static void tune_print2(uint8_t v)       
{
    lcd_print_int(v / 10);
    lcd_print_int(v % 10);
}

static void tune_draw(void)              
{
    uint8_t idx = (uint8_t)(status - TUNE_RED);

    lcd_set_cursor(0, 0);
    lcd_print("TUNE ");  lcd_print(NAME[idx]);  lcd_print(" ");
    tune_print2(draft[idx]);  lcd_print("s  ");

    lcd_set_cursor(1, 0);
    lcd_print("R");   tune_print2(draft[IDX_RED]);
    lcd_print(" G");  tune_print2(draft[IDX_GREEN]);
    lcd_print(" A");  tune_print2(draft[IDX_AMBER]);
    lcd_print(" ");
    if (feedback[0] != '\0')   lcd_print(feedback);
    else if (tune_is_valid())  lcd_print("OK  ");
    else                       lcd_print("BAD ");
}

static void tune_handle(void)            
{
    uint8_t idx = (uint8_t)(status - TUNE_RED);

    if (isButtonPressed(BTN_MODE)) {                     /* sang AUTO, bo thay doi chua xac nhan */
        clearTimer(TMR_TUNE);
        mode   = MODE_AUTO;
        status = AUTO_INIT;
        return;
    }
    if (isButtonPressed(BTN_NEXT)) {                     /* RED -> GREEN -> AMBER -> RED */
        status = (status == TUNE_AMBER) ? TUNE_RED : (SystemState)(status + 1);
        redraw = 1;
    }
    if (isButtonPressed(BTN_INC)) {                      /* +1, vuot MAX thi ve MIN */
        draft[idx] = (draft[idx] >= MAX_TIME) ? MIN_TIME : (uint8_t)(draft[idx] + 1);
        feedback   = "";
        redraw     = 1;
    }
    if (isButtonPressed(BTN_CONFIRM)) {
        if (tune_is_valid()) {
            time_red   = draft[IDX_RED];
            time_green = draft[IDX_GREEN];
            time_amber = draft[IDX_AMBER];
            feedback   = "SAVE";
        } else {
            feedback   = "ERR!";                         /* giu draft, khong ghi time_* */
        }
        setTimer(TMR_TUNE, MS_TO_TICKS(FEEDBACK_MS));
        redraw = 1;
    }
    if (timer_flag[TMR_TUNE]) {                          /* het thoi gian hien thong bao */
        clearTimer(TMR_TUNE);
        feedback = "";
        redraw   = 1;
    }
    if (redraw) {
        tune_draw();
        redraw = 0;
    }
}

void fsm_tuning_run(void)
{
    switch (status) {
    case TUNE_INIT:
        traffic_set(1, LIGHT_RED);                       /* trang thai an toan: ca hai duong do */
        traffic_set(2, LIGHT_RED);
        tune_flush_buttons();
        draft[IDX_RED]   = time_red;
        draft[IDX_GREEN] = time_green;
        draft[IDX_AMBER] = time_amber;
        feedback = "";
        clearTimer(TMR_TUNE);
        lcd_clear();
        redraw = 1;
        status = TUNE_RED;
        break;
    case TUNE_RED:
    case TUNE_GREEN:
    case TUNE_AMBER:
        tune_handle();
        break;
    default:
        break;
    }
}
