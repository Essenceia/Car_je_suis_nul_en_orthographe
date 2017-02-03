/*
 * custom_nvic.c
 *
 *  Created on: Jan 10, 2017
 *      Author: pooki
 */

#include "../inc/custom_nvic.h"

#include "../inc/custom_can_config.h"
#include "misc.h"

void NVIC_CAN_Init(void)
{
	/*if(NVIC_IS_INIT){*/
	NVIC_InitTypeDef  NVIC_InitStructure;

	if (CAN_USE_CAN1)NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	else NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = CAN_NVIC_PREEPT_PRIO;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = CAN_NVIC_SUB_PRIO;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	/*}*/
}
