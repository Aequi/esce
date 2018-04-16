#include <stdint.h>
#include <stdbool.h>

#include "stm32f0xx_conf.h"

#include "HalCommon.h"
#include "HalGpio.h"
#include "HalAdc.h"
#include "HalPwm.h"

#include "MotorController.h"

static inline uint32_t adcIirFilt(uint32_t value, uint32_t newValue)
{
    #define A   4
    #define B   ((1 << 8) - A)

    return (newValue * B + value * A) >> 8;
}

void startLed(void)
{
    volatile uint32_t cntr;
    const uint32_t delay = 1000000;

    cntr = 0;
    halGpioLedOn(true);
    while (cntr++ < delay) {

    }

    cntr = 0;
    halGpioLedOn(false);
    while (cntr++ < delay) {

    }
}

static uint32_t bEmf, dcBus, potValue;

void adcDataReady(const uint16_t data[], uint32_t length)
{
    bEmf = adcIirFilt(bEmf, data[0]);
    dcBus = adcIirFilt(dcBus, data[1]);
    potValue = data[2];

    motorControllerProcess(bEmf, dcBus);
    const Phase phase = motorControllerGetPhase();

    halAdcStart((uint32_t) phase);
}

int main(void)
{
    const uint32_t maxDutyCycle = 2048;
    const uint32_t pwmOnSamplingDelay = 300;
    halCommonInit();
    halGpioInit();
    startLed();
    motorControllerInit();
    halAdcInit(adcDataReady);
    halPwmInit(maxDutyCycle, pwmOnSamplingDelay);
    halPwmStart(true);
    halPwmDisable();

    bool ledState = false;
    volatile uint32_t ledCounter = 0;
    volatile uint32_t ledCounterDelay = 2000;

    uint32_t dutyCycle = 0;

    const uint32_t potValMotorStart = 50;

    for (;;) {
        if (ledCounter++ >= ledCounterDelay) {
            ledState = !ledState;
            ledCounter = 0;
        }
        ledCounterDelay = (ADC_MAX_VAL - potValue) << 1;
        if (potValue > potValMotorStart) {
            motorControllerStartMotor(true);
            halGpioLedOn(ledState);
            dutyCycle = potValue * maxDutyCycle / ADC_MAX_VAL;
            if (dutyCycle < pwmOnSamplingDelay) {
                dutyCycle = pwmOnSamplingDelay;
            }
            motorControllerSetDutyCycle(dutyCycle);
        } else {
            halPwmDisable();
            motorControllerStartMotor(false);
            halGpioLedOn(false);
            dutyCycle = 0;
            motorControllerSetDutyCycle(dutyCycle);
        }
    }
}
