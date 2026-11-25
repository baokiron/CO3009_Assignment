

#include "dht20.h"
#include "global.h"
#include "scheduler.h"
#include "main.h"

extern I2C_HandleTypeDef hi2c1;

#define DHT20_ADDR        (0x38 << 1)
#define DHT20_TIMEOUT_MS  10

static uint8_t dht20_crc8(const uint8_t *d, uint8_t n)
{
    uint8_t crc = 0xFF;                                  
    for (uint8_t i = 0; i < n; i++) {
        crc ^= d[i];
        for (uint8_t b = 0; b < 8; b++) {
            crc = (crc & 0x80) ? (uint8_t)((crc << 1) ^ 0x31) : (uint8_t)(crc << 1);
        }
    }
    return crc;
}

/* Goi MOT LAN truoc khi chay scheduler (dung HAL_Delay duoc phep o day). */
void dht20_init(void)
{
    uint8_t cmd = 0x71;
    uint8_t st  = 0;

    HAL_Delay(100);                                      
    if (HAL_I2C_Master_Transmit(&hi2c1, DHT20_ADDR, &cmd, 1, DHT20_TIMEOUT_MS) != HAL_OK) return;
    if (HAL_I2C_Master_Receive(&hi2c1, DHT20_ADDR, &st, 1, DHT20_TIMEOUT_MS) != HAL_OK) return;

    if ((st & 0x18) != 0x18) {                           
        static const uint8_t regs[3] = { 0x1B, 0x1C, 0x1E };
        for (uint8_t i = 0; i < 3; i++) {
            uint8_t b[3] = { regs[i], 0x00, 0x00 };
            HAL_I2C_Master_Transmit(&hi2c1, DHT20_ADDR, b, 3, DHT20_TIMEOUT_MS);
        }
    }
    HAL_Delay(10);
}


uint8_t dht20_trigger(void)
{
    uint8_t cmd[3] = { 0xAC, 0x33, 0x00 };
    return (HAL_I2C_Master_Transmit(&hi2c1, DHT20_ADDR, cmd, 3, DHT20_TIMEOUT_MS) == HAL_OK);
}


uint8_t dht20_read(void)
{
    uint8_t  b[7];
    uint32_t hraw, traw;                                 
    if (HAL_I2C_Master_Receive(&hi2c1, DHT20_ADDR, b, 7, DHT20_TIMEOUT_MS) != HAL_OK) return 0;
    if (b[0] & 0x80) return 0;                           
    if (dht20_crc8(b, 6) != b[6]) return 0;              

    hraw = ((uint32_t)b[1] << 12) | ((uint32_t)b[2] << 4) | (uint32_t)(b[3] >> 4);
    traw = ((uint32_t)(b[3] & 0x0F) << 16) | ((uint32_t)b[4] << 8) | (uint32_t)b[5];

    dht20_humidity    = (float)hraw * 100.0f / 1048576.0f;            
    dht20_temperature = (float)traw * 200.0f / 1048576.0f - 50.0f;
    return 1;
}

static void dht20_collect(void)                          /* task one-shot */
{
    dht20_read();
}

void dht20_task(void)                                    /* chu ky ~2 s */
{
    if (dht20_trigger()) {
        SCH_Add_Task(dht20_collect, MS_TO_TICKS(100), 0);
    }
}
