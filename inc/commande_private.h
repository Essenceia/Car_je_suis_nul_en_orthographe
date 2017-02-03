/*
 * commande_private.h
 *
 *  Created on: Jan 30, 2017
 *      Author: pooki
 */

#ifndef INC_COMMANDE_PRIVATE_H_
#define INC_COMMANDE_PRIVATE_H_

/* Methodes */
void SendInfoCommande(void);
void RAZ(CAN_Msg *msg);
void MarcheMoteur(void);
void DispatchNewCommande(CAN_Msg *msg);
void DispatchNextCommande(CAN_Msg *msg);
void AjoutPoint(CAN_Msg *msg);
void LectureState(uint8_t type);
int32_t Convert8to32(int8_t t0,int8_t t1,int8_t t2, int8_t t3);
void Put16(int8_t *m,int16_t i);
void Put32(int8_t *m, int32_t i);
void EnvoiOcteEtat(void);
void RenvoiPoint(uint8_t indexpoint);
void ResetTableau(void);
#endif /* INC_COMMANDE_PRIVATE_H_ */
