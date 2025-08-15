#include "potentiometer_manager.h"
#include <stdlib.h>
#define ABS(x) ((x) < 0.f ? -(x) : (x))

float* readings;
uint16_t readings_num;
float hysteresis;

void potentiometer_manager_init(uint16_t potentiometer_num, float hysteresis) {
    readings = (float*)malloc(potentiometer_num * sizeof(float));
    readings_num = potentiometer_num;
}

void potentiometer_manager_monitor_potentiometers(float* raw_readings, void(*notify_change)(uint16_t potentiometer_id, float reading)) {
    for (uint16_t i = 0; i < readings_num; i++) {
        float raw_reading = raw_readings[i];
        float last_reading = readings[i];
        if (ABS(raw_reading - last_reading) > hysteresis) {
            readings[i] = raw_reading;
            notify_change(i, last_reading);
        }
    }
}
