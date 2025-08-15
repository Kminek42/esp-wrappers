#pragma once
#include "stdint.h"

void potentiometer_manager_init(uint16_t potentiometer_num);

void potentiometer_manager_monitor_potentiometers(float* raw_readings, void(*notify_change)(uint16_t potentiometer_id, float reading));
