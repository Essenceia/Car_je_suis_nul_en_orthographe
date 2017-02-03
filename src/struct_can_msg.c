/*
 * struct_can_msg.c
 *
 *  Created on: Jan 13, 2017
 *      Author: pooki
 */

#include "../inc/struct_can_msg.h"

#include <stdlib.h>

void DestroyCAN_Msg(CAN_Msg *tokill){
   free(tokill->message);
   free(tokill);
}
CAN_Msg * NewCAN_Msg(uint16_t lengthdata){
	CAN_Msg *tbtn = (CAN_Msg *)calloc(1u,sizeof(CAN_Msg));//Tout Beaux Tout Neuf
	if((lengthdata >= 1u)&&(lengthdata<= 8u)){
		tbtn->message = (int8_t *)calloc(lengthdata,sizeof(int8_t));
		tbtn->valide = 0xFF;
	}
	return tbtn;

}
uint8_t ValideCAN_Msg(CAN_Msg *me){
	return ((me->valide!= 0x00)? 0xFF : 0x00);
}
