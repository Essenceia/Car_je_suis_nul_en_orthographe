/*
 * custom_can_config.h
 *
 *  Created on: Jan 10, 2017
 *      Author: pooki
 */

#ifndef INC_CUSTOM_CAN_CONFIG_H_
#define INC_CUSTOM_CAN_CONFIG_H_
#include <stdint.h>

#include "../inc/struct_can_msg.h"
#define CAN_USE_CAN1 ((uint8_t)0x00) //If == 0 then use CAN2 ; PA11 et 12 sortent sur la micro usb ... >w> pas de pot
//Rmq : avec cette configuration un seul can est actife a la foix
#define CAN_NVIC_PREEPT_PRIO  ((uint8_t)0x01)
#define CAN_NVIC_SUB_PRIO   ((uint8_t)0x00)
#define CAN_ID ((uint32_t) 0xEE4 ) //Standard identifier - picer sur le code pic
#define CAN_MSG_SIZE ((uint8_t) 0x08) //taille d'une trame a la translission
#define CAN_EXTID ((uint32_t)0x01) // ont s'en fiche puisqu'ont utilise un standard id pas l'extended

#endif /* INC_CUSTOM_CAN_CONFIG_H_ */
