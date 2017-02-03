/*
 * custom_can.h
 *
 *  Created on: Jan 10, 2017
 *      Author: pooki
 */

#ifndef INC_CUSTOM_CAN_H_
#define INC_CUSTOM_CAN_H_

#include "../inc/custom_can_config.h"
#include "stm32f4xx_gpio.h"

//Todo: cree une structure config pour pouvoire facillement modifier les valheurs des configes du can
void InitCan(void);
void SendCan(CAN_Msg *tabmsg,DESTINATION dest);
void ReciveCan(CAN_TypeDef *CANx,uint8_t Fifon, CAN_Msg *msg);
void TrashReciveCan(CAN_TypeDef *CANx,uint8_t Fifon);


#endif /* INC_CUSTOM_CAN_H_ */
