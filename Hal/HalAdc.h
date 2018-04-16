#ifndef __HAL_ADC_H__
#define __HAL_ADC_H__

#include <stdbool.h>
#include <stdint.h>
#include <stm32f0xx.h>

#define ADC_BUFF_SIZE       1
#define ADC_CHANNEL_COUNT   3
#define ADC_MAX_VAL         4095

typedef void (*HalAdcDataReady)(const uint16_t data[], uint32_t length);

void halAdcInit(HalAdcDataReady halAdcDataReadyCb);
static inline void halAdcStart(uint32_t channel)
{
    static const uint32_t adcChannels[]  = {ADC_Channel_1, ADC_Channel_2, ADC_Channel_3, ADC_Channel_1, ADC_Channel_2, ADC_Channel_3};
    ADC1->CR |= (uint32_t) ADC_CR_ADDIS;
    ADC1->CHSELR = ADC_Channel_5 | ADC_Channel_6 | adcChannels[channel];
    ADC1->CR |= (uint32_t) ADC_CR_ADEN;
    ADC1->CR |= (uint32_t) ADC_CR_ADSTART;
}


#endif
