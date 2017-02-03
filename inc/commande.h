/*
 * commande.h
 *
 *  Created on: Jan 13, 2017
 *      Author: pooki
 */

#ifndef INC_COMMANDE_H_
#define INC_COMMANDE_H_
#include "../inc/struct_can_msg.h"

/*Fomction derriere une idee de "dispatche" en deux etapes:
 * Premierement identifier le Numeros du Message et ont voie si ont passe a regarder le numeros de la commade
 * Deuxiemement si ont a passe le premier stade on regarde le numero de la commande
 */
//Todo : Transformer en un singleton avec des pointeurs sur les fonctions qui effectuent les actions
void DispatchNoMsg(CAN_Msg *msg);
void InitCommande(void (*f)(CAN_Msg));
void InitMCCommance(void (*f) (void *));
void SendMCInterface(uint8_t code, uint8_t *buf, uint8_t sizemone);
#endif /* INC_COMMANDE_H_ */
