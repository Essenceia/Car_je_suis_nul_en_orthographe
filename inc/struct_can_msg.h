/*
 * struct_can_msg.h
 *
 *  Created on: Jan 13, 2017
 *      Author: pooki
 */

#ifndef INC_STRUCT_CAN_MSG_H_
#define INC_STRUCT_CAN_MSG_H_
#include <stdint.h>
typedef enum{
	STANDARD,
	MCPROTOCOL
}DESTINATION;
/*
 * valide - vaut 1+ si la trame est valide 0 sinon
 * size - nombre d'octee dans message
 * NoMsg - numeros du message
 * Commade - numeros de la commande , 0 si il n'en a pas
 */
typedef struct{
	uint8_t valide;
	uint8_t size ;
	uint8_t NoMsg ;
	uint8_t Commande ;
	int8_t *message; // pointeur sur tableau de taille du message

}CAN_Msg;
void DestroyCAN_Msg(CAN_Msg *deadmeat); /* TOUJOURS APPELLER !!!
Afin de ne pas cree d'overhead avec un constructeur
(comme c'est une structure qui risque de revenir souvent ont n'a pas chainer cette methode */
CAN_Msg * NewCAN_Msg(uint16_t lengthdata);
extern inline uint8_t ValideCAN_Msg(CAN_Msg *me);
#endif /* INC_STRUCT_CAN_MSG_H_ */
