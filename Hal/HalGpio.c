#include "HalGpio.h"
#include "stm32f0xx_conf.h"

#define HAL_GPIO_LED_PIN    GPIO_Pin_11
#define HAL_GPIO_LED_PORT   GPIOA

#define HAL_GPIO_ADC_PORT   GPIOA
#define HAL_GPIO_ADC_PIN_0  GPIO_Pin_0
#define HAL_GPIO_ADC_PIN_1  GPIO_Pin_1
#define HAL_GPIO_ADC_PIN_2  GPIO_Pin_2
#define HAL_GPIO_ADC_PIN_3  GPIO_Pin_3
#define HAL_GPIO_ADC_PIN_4  GPIO_Pin_4
#define HAL_GPIO_ADC_PIN_5  GPIO_Pin_5
#define HAL_GPIO_ADC_PIN_6  GPIO_Pin_6

#define G3_H_T1_C3_N        GPIO_Pin_7
#define G3_H_T1_C3_N_S      GPIO_PinSource7
#define G2_H_T1_C2_N        GPIO_Pin_0
#define G3_H_T1_C2_N_S      GPIO_PinSource0
#define G1_H_T1_C1_N        GPIO_Pin_1
#define G1_H_T1_C1_N_S      GPIO_PinSource1

#define G3_H_T1_C3_P        GPIO_Pin_8
#define G3_H_T1_C3_P_S      GPIO_PinSource8
#define G2_H_T1_C2_P        GPIO_Pin_9
#define G3_H_T1_C2_P_S      GPIO_PinSource9
#define G1_H_T1_C1_P        GPIO_Pin_10
#define G1_H_T1_C1_P_S      GPIO_PinSource10

#define HAL_GPIO_PA_PORT    GPIOA
#define HAL_GPIO_PB_PORT    GPIOB

void halGpioInit(void)
{
    GPIO_InitTypeDef gpioInitStructure = {.GPIO_Pin =  HAL_GPIO_LED_PIN,
                                          .GPIO_Mode = GPIO_Mode_OUT,
                                          .GPIO_Speed = GPIO_Speed_50MHz,
                                          .GPIO_OType = GPIO_OType_PP,
                                          .GPIO_PuPd = GPIO_PuPd_DOWN };

    GPIO_Init(HAL_GPIO_LED_PORT, &gpioInitStructure);

    GPIO_ResetBits(HAL_GPIO_LED_PORT, gpioInitStructure.GPIO_Pin);

    gpioInitStructure.GPIO_Pin = HAL_GPIO_ADC_PIN_0 | HAL_GPIO_ADC_PIN_1 | HAL_GPIO_ADC_PIN_2 | HAL_GPIO_ADC_PIN_3
                               | HAL_GPIO_ADC_PIN_4 | HAL_GPIO_ADC_PIN_5 | HAL_GPIO_ADC_PIN_6;
    gpioInitStructure.GPIO_Mode = GPIO_Mode_AN;
    gpioInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    gpioInitStructure.GPIO_OType = GPIO_OType_PP;
    gpioInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

    GPIO_Init(HAL_GPIO_ADC_PORT, &gpioInitStructure);

    gpioInitStructure.GPIO_Pin = G3_H_T1_C3_N | G3_H_T1_C3_P | G2_H_T1_C2_P | G1_H_T1_C1_P;
    gpioInitStructure.GPIO_Mode = GPIO_Mode_AF;
    gpioInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    gpioInitStructure.GPIO_OType = GPIO_OType_PP;
    gpioInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

    GPIO_Init(HAL_GPIO_PA_PORT, &gpioInitStructure);

    gpioInitStructure.GPIO_Pin = G2_H_T1_C2_N | G1_H_T1_C1_N;
    GPIO_Init(HAL_GPIO_PB_PORT, &gpioInitStructure);

    GPIO_PinAFConfig(HAL_GPIO_PA_PORT, G3_H_T1_C3_N_S, GPIO_AF_2);
    GPIO_PinAFConfig(HAL_GPIO_PA_PORT, G3_H_T1_C3_P_S, GPIO_AF_2);
    GPIO_PinAFConfig(HAL_GPIO_PA_PORT, G3_H_T1_C2_P_S, GPIO_AF_2);
    GPIO_PinAFConfig(HAL_GPIO_PA_PORT, G1_H_T1_C1_P_S, GPIO_AF_2);

    GPIO_PinAFConfig(HAL_GPIO_PB_PORT, G3_H_T1_C2_N_S, GPIO_AF_2);
    GPIO_PinAFConfig(HAL_GPIO_PB_PORT, G1_H_T1_C1_N_S, GPIO_AF_2);
}

void halGpioLedOn(bool isEnabled)
{
    GPIO_WriteBit(HAL_GPIO_LED_PORT, HAL_GPIO_LED_PIN, isEnabled ? Bit_SET : Bit_RESET);
}

