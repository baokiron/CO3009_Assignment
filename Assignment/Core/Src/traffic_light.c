#include "traffic_light.h"
#include "main.h"

static const uint8_t SIG[4][2] = {
    [LIGHT_OFF]   = {0, 0},   /* <- sua theo bang do */
    [LIGHT_RED]   = {0, 1},   /* <- sua theo bang do */
    [LIGHT_GREEN] = {1, 0},   /* <- sua theo bang do */
    [LIGHT_AMBER] = {1, 1},   /* <- sua theo bang do */
};


static LightColor last_color[NUM_ROADS + 1];

static void traffic_write(uint8_t road, uint8_t a, uint8_t b)
{
    GPIO_PinState sa = a ? GPIO_PIN_SET : GPIO_PIN_RESET;
    GPIO_PinState sb = b ? GPIO_PIN_SET : GPIO_PIN_RESET;

    if (road == 1) {
        HAL_GPIO_WritePin(TL1_A_GPIO_Port, TL1_A_Pin, sa);
        HAL_GPIO_WritePin(TL1_B_GPIO_Port, TL1_B_Pin, sb);
    } else {
        HAL_GPIO_WritePin(TL2_A_GPIO_Port, TL2_A_Pin, sa);
        HAL_GPIO_WritePin(TL2_B_GPIO_Port, TL2_B_Pin, sb);
    }
}

void traffic_set(uint8_t road, LightColor color)
{
    if ((road != 1 && road != 2) || (unsigned)color > (unsigned)LIGHT_AMBER) {
        return;                         /* tham so sai: bo qua, khong dung den */
    }
    traffic_write(road, SIG[color][0], SIG[color][1]);
    last_color[road] = color;
}

void traffic_init(void)
{
    traffic_set(1, LIGHT_OFF);
    traffic_set(2, LIGHT_OFF);
}
