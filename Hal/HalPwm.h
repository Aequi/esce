#ifndef __HAL_PWM_H__
#define __HAL_PWM_H__

#include <stdint.h>
#include <stdbool.h>
#include <stm32f0xx.h>

typedef enum Phase {
    PHASE_AB,
    PHASE_AC,
    PHASE_BC,
    PHASE_BA,
    PHASE_CA,
    PHASE_CB,
} Phase;

void halPwmInit(uint32_t maxDutyCycle, uint32_t pwmOnSamplingDelay);
void halPwmStart(bool isStart);

static inline void halPwmSetPhase(Phase phaseNumber)
{
    switch ((phaseNumber)) {
    case PHASE_AB:
        TIM1->CCER = (TIM_CCER_CC3E | TIM_CCER_CC3P) | (TIM_CCER_CC3NE) |
                     (TIM_CCER_CC2E | TIM_CCER_CC2NE | TIM_CCER_CC2NP) |
                     (TIM_CCER_CC1E | TIM_CCER_CC1P | TIM_CCER_CC1NE | TIM_CCER_CC1NP) | TIM_CCER_CC4E;
        TIM1->CCMR1 = 0x5848; // B Low, A PWM
        TIM1->CCMR2 = 0x6868; // C Hi-Z
        break;
    case PHASE_AC:
        TIM1->CCER = (TIM_CCER_CC3E | TIM_CCER_CC3P) | (TIM_CCER_CC3NE) |
                     (TIM_CCER_CC2E | TIM_CCER_CC2P | TIM_CCER_CC2NE | TIM_CCER_CC2NP) |
                     (TIM_CCER_CC1E | TIM_CCER_CC1NE | TIM_CCER_CC1NP) | TIM_CCER_CC4E;
        TIM1->CCMR1 = 0x4858; // C Low, A PWM
        TIM1->CCMR2 = 0x6868; // B Hi-Z
        break;
    case PHASE_BC:
        TIM1->CCER = (TIM_CCER_CC3E | TIM_CCER_CC3P | TIM_CCER_CC3NE | TIM_CCER_CC3NP) |
                     (TIM_CCER_CC2E | TIM_CCER_CC2P) | (TIM_CCER_CC2NE) |
                     (TIM_CCER_CC1E | TIM_CCER_CC1NE | TIM_CCER_CC1NP) | TIM_CCER_CC4E;
        TIM1->CCMR1 = 0x6858; // C Low, B PWM
        TIM1->CCMR2 = 0x6848; // A Hi-Z
        break;
    case PHASE_BA:
        TIM1->CCER = (TIM_CCER_CC3E | TIM_CCER_CC3NE | TIM_CCER_CC3NP) |
                     (TIM_CCER_CC2E | TIM_CCER_CC2P)  | (TIM_CCER_CC2NE) |
                     (TIM_CCER_CC1E | TIM_CCER_CC1P | TIM_CCER_CC1NE | TIM_CCER_CC1NP) | TIM_CCER_CC4E;
        TIM1->CCMR1 = 0x6848; // A Low, B PWM
        TIM1->CCMR2 = 0x6858; // C Hi-Z
        break;
    case PHASE_CA:
        TIM1->CCER = (TIM_CCER_CC3E | TIM_CCER_CC3NE | TIM_CCER_CC3NP) |
                     (TIM_CCER_CC2E | TIM_CCER_CC2P | TIM_CCER_CC2NE | TIM_CCER_CC2NP) |
                     (TIM_CCER_CC1E | TIM_CCER_CC1P) | (TIM_CCER_CC1NE) | TIM_CCER_CC4E;
        TIM1->CCMR1 = 0x4868; // A Low, C PWM
        TIM1->CCMR2 = 0x6858; // B Hi-Z
        break;
    case PHASE_CB:
        TIM1->CCER = (TIM_CCER_CC3E | TIM_CCER_CC3P | TIM_CCER_CC3NE | TIM_CCER_CC3NP) |
                     (TIM_CCER_CC2E | TIM_CCER_CC2NE | TIM_CCER_CC2NP) |
                     (TIM_CCER_CC1E | TIM_CCER_CC1P) | ((TIM_CCER_CC1NE)) | TIM_CCER_CC4E;
        TIM1->CCMR1 = 0x5868; // B Low, C PWM
        TIM1->CCMR2 = 0x6848; // A Hi-Z
        break;
    }
}

static inline void halPwmDisable(void)
{
    TIM1->CCER = (TIM_CCER_CC1E | TIM_CCER_CC1P | TIM_CCER_CC1NE | TIM_CCER_CC1NP) |
                 (TIM_CCER_CC2E | TIM_CCER_CC2P | TIM_CCER_CC2NE | TIM_CCER_CC2NP) |
                 (TIM_CCER_CC3E | TIM_CCER_CC3P | TIM_CCER_CC3NE | TIM_CCER_CC3NP) | TIM_CCER_CC4E;
    TIM1->CCMR1 = 0x4848;
    TIM1->CCMR2 = 0x6848;
}

static inline void halPwmSetDutyCycle(uint32_t dutyCycle)
{
    TIM1->CCR1 = dutyCycle;
    TIM1->CCR2 = dutyCycle;
    TIM1->CCR3 = dutyCycle;
}

#endif /* __HAL_PWM_H__ */
