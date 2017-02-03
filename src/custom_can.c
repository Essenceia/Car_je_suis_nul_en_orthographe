/*
 * custom_can.c
 *
 *  Created on: Jan 10, 2017
 *      Author: pooki
 */

#ifndef CUSTOM_CAN_C_
#define CUSTOM_CAN_C_

#include "../inc/custom_can.h"

#include "stm32f4xx_can.h"
#include "stm32f4xx_rcc.h"
#include <stdlib.h>

#include "../inc/custom_can_config.h"
#include "../inc/configurations.h"
#include "../inc/debugger.h"
#include "../inc/state.h"
static CanTxMsg StandardMsg;

/*
 * Fonctions priver
 */
void DebugerCanPrintMsgTx(CanTxMsg *msg);
void DebugerCanPrintMsgRx(CanRxMsg *msg);
extern inline CAN_TypeDef GetCan(void);

void InitCan(void){
	static uint8_t isinitcan = 0x00;
	if(!isinitcan){
	//declare variables
	CAN_TypeDef CANx = GetCan();
	GPIO_InitTypeDef  GPIO_InitStructure;
	CAN_InitTypeDef CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	uint32_t PinTx, PinRx;
	uint16_t PinSourceTx, PinSourceRx;
	uint8_t GPIOAFCAN, FilterBase;
	GPIO_TypeDef *GPIOx;
#ifdef CAN_USE_CAN1
    	PinTx = GPIO_Pin_12;
    	PinRx = GPIO_Pin_11;
    	GPIOx = GPIOA;
    	PinSourceTx = GPIO_PinSource12;
    	PinSourceRx = GPIO_PinSource11;
    	GPIOAFCAN = GPIO_AF_CAN1;
    	FilterBase = 0;
    	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

# else
    	PinTx = GPIO_Pin_13;
    	PinRx = GPIO_Pin_12;
    	GPIOx =GPIOB;
    	PinSourceTx = GPIO_PinSource13;
    	PinSourceRx = GPIO_PinSource12;
    	GPIOAFCAN = GPIO_AF_CAN1;
    	FilterBase = 14;
    	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
#endif
	//1.init GPIO

    //config gpio
	GPIO_InitStructure.GPIO_Pin = PinRx | PinTx;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOx, &GPIO_InitStructure);
	//connect to the AF9
	GPIO_PinAFConfig(GPIOx, PinSourceTx, GPIOAFCAN);
	GPIO_PinAFConfig(GPIOx, PinSourceRx, GPIOAFCAN);


	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
#ifndef CAN_USE_CAN1
	if (! CAN_USE_CAN1)RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
#endif
	CAN_DeInit(&CANx);
	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack; //Todo: desactiver le mode loopback et le mettre en mode CAN_Mode_Normal quand on a fini les tests
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	/* CAN Baudrate = 1MBps , reference au https://vector.com/portal/medien/cmc/application_notes/AN-AND-1-106_Basic_CAN_Bit_Timing.pdf */
	CAN_InitStructure.CAN_BS1 = CAN_BS1_1tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_1tq;
	CAN_InitStructure.CAN_Prescaler = 16;
	CAN_Init(&CANx, &CAN_InitStructure); //Rmq pas besoin de redeclarer CAN1 il est deja declarer dans stm32f4xx.h

	//3. init CAN filter
	CAN_FilterInitStructure.CAN_FilterNumber = 0 + FilterBase; //les filtres du can2 commence a partir de 14, a 0 nous traitons du can1
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;//on met un mask+ID comme filtre
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = (0x0000 | GetAdressePropre()) << 8; //filtrage pour n'autoriser que les messages destiner a cette adresse
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;//communiquer avec tout les moteurs sur l'adresse 0
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0xFF00;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0xFF00;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	//4.Initialisation du msg d'envoie par default
	StandardMsg.IDE  = CAN_ID_STD; //trame de taille standard
	StandardMsg.DLC = CAN_MSG_SIZE;
	StandardMsg.RTR = CAN_RTR_DATA;
	StandardMsg.ExtId = CAN_EXTID; //ont l'initialise mais on ne l'utilisera pas
	StandardMsg.StdId = (GetAdresseDest()| 0x0000)<<3 ; //on initialise avec l'adresse de destination standard
	StandardMsg.Data[0] = 0x01; //trame par default
	StandardMsg.Data[1] = 0x01;
	StandardMsg.Data[2] = 0x01;
	StandardMsg.Data[3] = 0x01;
	StandardMsg.Data[4] = 0x01;
	StandardMsg.Data[5] = 0x01;
	StandardMsg.Data[6] = 0x01;
	StandardMsg.Data[7] = 0x01;
	//Todo: attacher un interupt au CAN
	CAN_ITConfig(&CANx,CAN_IT_FMP0,ENABLE); //Inturupt sur reception dans fifo buffer 0

	isinitcan ++;
	}
}

void SendCan(CAN_Msg *msg,DESTINATION dest)
{   DebugerPrint("SendCan : Enter\n");
	uint8_t l;
    CAN_TypeDef CANx =GetCan();
    StandardMsg.DLC = msg->size;
    if(dest == STANDARD){
    	StandardMsg.StdId =  0x0000 | GetAdresseDest() ;
    }else StandardMsg.StdId =  0x0000 | GetAdresseDestMC() ;
	for(l = 0 ; l < msg->size ; l++)
	  {
		  StandardMsg.Data[l] = msg->message[l];
	  }
	  CAN_Transmit(&CANx,&StandardMsg);
	  DebugerCanPrintMsgTx(&StandardMsg);
	  if (CAN_TransmitStatus(&CANx,0)!= CAN_TxStatus_Ok)
	  {
		 DebugerPrint("SendCan : Transmission failed\n");
	  }
	  DebugerPrint("SendCan : Exit\n");
}
/*
 *Fonction qui decapsulle la trame Can et la range dans notre stucture avec les Numeros de Message et les
 * commandes clairement identifier + tableau avec le data ajuster a la taille. A faire attention a ne pas
 *faire de depassement de tableau !
 */
void ReciveCan(CAN_TypeDef *CANx,uint8_t Fifon, CAN_Msg *msg)
{
	CanRxMsg recmsg;
	uint8_t offset = 0x00;
	CAN_Receive(CANx, Fifon, &recmsg);
	DebugerCanPrintMsgRx(&recmsg);
	/*Extraction du message */
	msg->NoMsg = (uint8_t)recmsg.StdId & 0x07 ; //Nour recuperons le Numero de message contenu dans les bit aux positions 0,1,2 Rmq1 : Nous avons deja eu un traitement qui extrait le standard identifier de la trame identifier , Rmq2 : L'StdId n'est contenu que dans les 11 LSBits (eh oui)
	if(recmsg.DLC){
	if((msg->NoMsg != 0x05)||(msg->NoMsg!=0x02))offset = 0x01; /* Pour les cas particulier ou ont n'a pas de commande */
	msg->message = (int8_t *)malloc(sizeof(int8_t)*(recmsg.DLC-offset));
    if(!offset){
    	msg->Commande = recmsg.Data[0]; //oui , c'est moche mais la forme de la trame nous impose ce traitement
    }
	msg->valide = 0xFF;

	for(uint8_t i=0; i<(recmsg.DLC-offset); i++)/* dans le cas ou on a un message de NoMessage 0x05 on a 8 octee de data si il ne vaut pas 0x05 on a que 7 octee de data car on a 1 o
	* 1 octee ocuper par la commande
	*/
	{
		msg->message[i] = recmsg.Data[i+offset];
	}
	}
	else{
		DebugerPrint("ReciveCan : la trame est vide\n");
	}

}
void TrashReciveCan(CAN_TypeDef *CANx,uint8_t Fifon){
	CanRxMsg recmsg;
	CAN_Receive(CANx, Fifon, &recmsg);
	DebugerCanPrintMsgRx(&recmsg);
}

void DebugerCanPrintMsgTx(CanTxMsg *msg)
{
	DebugerPrint("\n DebugerCanPrintMsgTx (hex):");
	DebugerPrint(" IDE:");
	DebugerPrintNum(msg->IDE , 16);
	DebugerPrint(" RTR:");
	DebugerPrintNum(msg->RTR , 16);
	DebugerPrint(" StdId:");
	DebugerPrintNum(msg->StdId , 16);
	DebugerPrint(" DLC:");
	DebugerPrintNum(msg->DLC , 16);
	DebugerPrint(" Data:");
	for(uint8_t i = 0; i<msg->DLC ; i++)
	{
		DebugerPrintNum(msg->Data[i] , 16);
	}
	DebugerPrint("\n");
}
void DebugerCanPrintMsgRx(CanRxMsg *msg)
{
	DebugerPrint("\n DebugerCanPrintMsg (hex):");
	DebugerPrint(" FMI:");
	DebugerPrintNum(msg->FMI , 16);
	DebugerPrint(" IDE:");
	DebugerPrintNum(msg->IDE , 16);
	DebugerPrint(" RTR:");
	DebugerPrintNum(msg->RTR , 16);
	DebugerPrint(" StdId:");
	DebugerPrintNum(msg->StdId , 16);
	DebugerPrint(" DLC:");
	DebugerPrintNum(msg->DLC , 16);
	DebugerPrint(" Data:");
	for(uint8_t i = 0; i<msg->DLC ; i++)
	{
		DebugerPrintNum(msg->Data[i] , 16);
	}
	DebugerPrint("\n");
}
inline CAN_TypeDef GetCan(void){
	CAN_TypeDef CANx;
#ifdef CAN_USE_CAN1
	CANx = *CAN1;
#else
	CANx = *CAN2;
#endif
	return CANx;
}




































#endif /* CUSTOM_CAN_C_ */
