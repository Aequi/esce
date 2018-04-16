#include "HalAdc.h"
#include "HalCommon.h"
#include "stm32f0xx_conf.h"

#include <stddef.h>

static uint16_t adcDataArray[ADC_BUFF_SIZE * ADC_CHANNEL_COUNT];

static HalAdcDataReady halAdcDataReadyCallback;

static void halAdcDmaHandler(bool isHalf)
{
    if (!isHalf) {
        if (halAdcDataReadyCallback != NULL) {
            halAdcDataReadyCallback(adcDataArray, sizeof(adcDataArray) / sizeof(*adcDataArray));
        }
    }
}

void halAdcInit(HalAdcDataReady halAdcDataReadyCb)
{
    halAdcDataReadyCallback = halAdcDataReadyCb;
    DMA_InitTypeDef dmaInitStructure;

    ADC_GetCalibrationFactor(ADC1);
    ADC_ClockModeConfig(ADC1, ADC_ClockMode_AsynClk);
    ADC_InitTypeDef adcInitStructure;
    ADC_StructInit(&adcInitStructure);
    adcInitStructure.ADC_ContinuousConvMode = ENABLE;
    adcInitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    adcInitStructure.ADC_Resolution = ADC_Resolution_12b;
    adcInitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
    adcInitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Falling;
    adcInitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC4;
    ADC_Init(ADC1, &adcInitStructure);
    ADC_DMACmd(ADC1, ENABLE);
    dmaCh1Cb = halAdcDmaHandler;

    ADC_ChannelConfig(ADC1, ADC_Channel_1, ADC_SampleTime_1_5Cycles);
    ADC_ChannelConfig(ADC1, ADC_Channel_5, ADC_SampleTime_1_5Cycles);
    ADC_ChannelConfig(ADC1, ADC_Channel_6, ADC_SampleTime_1_5Cycles);

    dmaInitStructure.DMA_BufferSize = sizeof(adcDataArray) / sizeof(*adcDataArray);
    dmaInitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    dmaInitStructure.DMA_M2M = DMA_M2M_Disable;
    dmaInitStructure.DMA_MemoryBaseAddr = (uint32_t) adcDataArray;
    dmaInitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    dmaInitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dmaInitStructure.DMA_Mode = DMA_Mode_Circular;
    dmaInitStructure.DMA_PeripheralBaseAddr = (uint32_t) &ADC1->DR;
    dmaInitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    dmaInitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dmaInitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_Init(DMA1_Channel1, &dmaInitStructure);
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
    DMA_Cmd(DMA1_Channel1, ENABLE);

    ADC_ContinuousModeCmd(ADC1, DISABLE);
    ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_OneShot);

    ADC_Cmd(ADC1, ENABLE);
    ADC_StartOfConversion(ADC1);
}
