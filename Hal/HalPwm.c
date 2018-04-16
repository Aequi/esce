#include "HalPwm.h"
#include "stm32f0xx_conf.h"

#define DEAD_TIME_BDTR_CONSTANT     128u //2us

void halPwmInit(uint32_t maxDutyCycle, uint32_t pwmOnSamplingDelay)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = maxDutyCycle;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
    TIM_ARRPreloadConfig(TIM1, ENABLE);

    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    TIM_OCInitStructure.TIM_Pulse = 0;

    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC1FastConfig(TIM1, TIM_OCFast_Disable);

    TIM_OC2Init(TIM1, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC2FastConfig(TIM1, TIM_OCFast_Disable);

    TIM_OC3Init(TIM1, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC3FastConfig(TIM1, TIM_OCFast_Disable);

    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
    TIM_OCInitStructure.TIM_Pulse = pwmOnSamplingDelay;

    TIM_OC4Init(TIM1, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Disable);
    TIM_OC4FastConfig(TIM1, TIM_OCFast_Disable);

    TIM_BDTRInitTypeDef timBdtrInitStruct;
    TIM_BDTRStructInit(&timBdtrInitStruct);
    timBdtrInitStruct.TIM_DeadTime = DEAD_TIME_BDTR_CONSTANT;
    timBdtrInitStruct.TIM_OSSRState = TIM_OSSRState_Enable;
    TIM_BDTRConfig(TIM1, &timBdtrInitStruct);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

void halPwmStart(bool isStart)
{
    TIM_Cmd(TIM1, isStart ? ENABLE : DISABLE);
}
