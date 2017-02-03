/*
 * TIMhelper.c
 *
 *  Created on: Jan 19, 2017
 *      Author: pooki
 */
#include "../inc/TIMhelper.h"

#include "stm32f4xx_rcc.h"
uint32_t get_timer_clock_frequency (void)
{
  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq (&RCC_Clocks);
  uint32_t multiplier;
  if (RCC_Clocks.PCLK1_Frequency == RCC_Clocks.SYSCLK_Frequency) {
    multiplier = 1;
  } else {
    multiplier = 2;
  }
  return multiplier * RCC_Clocks.PCLK1_Frequency;
}
