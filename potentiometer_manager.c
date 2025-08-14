#include "potentiometer_manager.h"
#include <stdlib.h>
#define ABS(x) ((x) < 0 ? -(x) : (x))

#define HISTERESIS (4096 / 128)

uint16_t* readings;
uint16_t readings_num;

void potentiometer_manager_init(uint16_t potentiometer_num) {
    readings = (uint16_t*)malloc(potentiometer_num * sizeof(uint16_t));
    readings_num = potentiometer_num;
}

void potentiometer_manager_monitor_potentiometers(uint16_t* raw_readings, void(*notify_change)(uint16_t potentiometer_id, uint8_t reading)) {
    for (uint16_t i = 0; i < readings_num; i++) {
        uint16_t raw_reading = raw_readings[i];
        uint16_t last_reading = readings[i];
        if (ABS(raw_reading - last_reading) > HISTERESIS) {
            last_reading = raw_reading;
            notify_change(i, last_reading >> 5);
        }
    }
}
