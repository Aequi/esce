#ifndef __MOTOR_CONTROLLER_H__
#define __MOTOR_CONTROLLER_H___

#include <stdbool.h>
#include <stdint.h>

#include "HalPwm.h"

void motorControllerInit(void);
void motorControllerStartMotor(bool isStart);
void motorControllerSetDutyCycle(uint32_t dutyCycle);
Phase motorControllerGetPhase(void);
void motorControllerProcess(uint16_t bEmf, uint16_t dcBus);

#endif
