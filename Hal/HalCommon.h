#ifndef __HAL_COMMON_H__
#define __HAL_COMMON_H__

#include <stdbool.h>

extern void (*dmaCh1Cb)(bool isHalf);
extern void (*dmaCh2Cb)(bool isHalf);
extern void (*dmaCh3Cb)(bool isHalf);
extern void (*dmaCh4Cb)(bool isHalf);
extern void (*dmaCh5Cb)(bool isHalf);

void halCommonInit();

#endif
