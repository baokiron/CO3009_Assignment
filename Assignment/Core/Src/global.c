/*
 * global.c - Definitions of the shared variables declared in global.h
 */

#include "global.h"

SystemMode  mode   = MODE_AUTO;
SystemState status = AUTO_INIT;

uint8_t time_red   = DEFAULT_TIME_RED;
uint8_t time_green = DEFAULT_TIME_GREEN;
uint8_t time_amber = DEFAULT_TIME_AMBER;

float dht20_temperature = 0.0f;
float dht20_humidity    = 0.0f;

void global_init(void)
{
    mode   = MODE_AUTO;
    status = AUTO_INIT;

    time_red   = DEFAULT_TIME_RED;
    time_green = DEFAULT_TIME_GREEN;
    time_amber = DEFAULT_TIME_AMBER;

    dht20_temperature = 0.0f;
    dht20_humidity    = 0.0f;
}