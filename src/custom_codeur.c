/*
 * custom_codeur.c
 *
 *  Created on: Jan 18, 2017
 *      Author: pooki
 */
#include "../inc/custom_codeur.h"

#include "../inc/debugger.h"
#include "../inc/state.h"
#include "../inc/TIMhelper.h"
#include "../inc/debugger.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_crc.h"
#include "misc.h"

#define SUPER_CRADE ((uint8_t)0xFF)//Todo: enlever cette solution temporaire
#define ENACI_Pwm_Freq  ((uint32_t)500000) //500kHz
#define ENACI_Spi_Pin_Ic ((uint16_t)GPIO_Pin_2)
#define ENACI_Spi_Pin_Csn ((uint16_t)GPIO_Pin_1) //Source select pour le spi , lance la communication en seriale quand il est sur low
#define ENACI_Spi_Pin_Groupe ((GPIO_TypeDef *)GPIOA)
#define ENACI_Spi_Pin_Pwm ((uint16_t)GPIO_Pin_0)
#define ENACI_Spi_Timer ((TIM_TypeDef *) TIM5_BASE)
#define ENACI_Spi_Timer_interupt_chanel ((IRQn_Type)TIM5_IRQn)
#define ENACI_Spi_Gpio_Clk ((uint32_t)RCC_AHB1Periph_GPIOA)
#define ENACI_Spi_Timer_TIM5 ((uint32_t)RCC_APB1Periph_TIM5 )
/*
 * Variable globale
 */
static Codeur codeur;

/*
 * Fonctions privees
 */
//Prototype
void InitStructCodeur(void);
void DecodeSSIMsgCodeur(void);
void InitTimerCodeur(void);
void InitGpioCodeur(void);
void InitPwmCodeur(void);
void InitItCodeur(void);
void SuperCradeSolutionCodeur(void);
//Declaration
void InitStructCodeur(void) {

	codeur.pos = 0x0000;
	codeur.nombredetour = 0;
	codeur.posabscodeur = &(get_instance()->smvm.PosAbscodeur);
	codeur.offset = 0x0000;
	codeur.msg.dataready = 0x00;
	codeur.msg.size = 0x00;
	codeur.msg.trame = 0x0000;
	DebugerPrint("InitStructCodeur : Sucess \n");
}
void InitCodeur(void) { //Todo: lancer dans l'initialisation principale les hardwares qui sont utiliser en commun dans les deux init
	DebugerPrint("InitCodeur : Begin \n");
	static uint8_t codeur_is_inti = 0;
	if (codeur_is_inti == 0) {
		//1.Init de la structure codeur
		DebugerPrint("InitCodeur : init struct codeur \n");
		InitStructCodeur();
		//2.Recuperation de la position absolu du codeur a l'initialisation
		DebugerPrint("InitCodeur : recuperation de la pos abs sur ssi \n");
		RecupePosAbsSSICodeur(); //Attention : ceci est une fonction non-bloquante on peut initiaier la suivie de l'encodeur en parallelle
		//3.Initialisation du hw pour l'encodeur
		//InitQuadCodeur(); Todo: YANN - a remplacer par ton inisialisation du hw
		//Suite de l'initialisation que tu doit faire

		//4.Recuperation de la posibition absolu
		if (codeur.msg.dataready == 0x00)
			DebugerDelay(500);
		if (codeur.msg.dataready == 0x00) {
			DebugerPrint("Echeque de recuperation de la position relative");
		} else {
			//la position sera dans codeur.posabsdata
		}
	}
	DebugerPrint("InitCodeur : end \n");
}

void InitSSICodeur(void) {
	DebugerPrint("InitSSICodeur : Begin \n");
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE); // pour le crc check
	InitTimerCodeur();
	InitGpioCodeur();
	if(!SUPER_CRADE){
	InitPwmCodeur();
	InitItCodeur();}
	DebugerPrint("InitSSICodeur : End \n");
}
void RecupePosAbsSSICodeur(void) {
	DebugerPrint("RecupePosAbsSSICodeur : Begin \n");
	static uint8_t ENACI_Ssi_is_init = 0x00;

	if (ENACI_Ssi_is_init == 0x00) {
		DebugerPrint("RecupePosAbsSSICodeur : lancement de init ssi \n");
		InitSSICodeur();
		ENACI_Ssi_is_init++;
	}
	//re-set du message comme vide
	GPIO_WriteBit(ENACI_Spi_Pin_Groupe, ENACI_Spi_Pin_Csn, Bit_RESET); //Selection du target
	codeur.msg.trame = 0x0000;
	codeur.msg.size = 0x00;
	codeur.msg.dataready = 0x00;
	//lancement du PWM
	TIM_CtrlPWMOutputs(ENACI_Spi_Timer, ENABLE);
	if (SUPER_CRADE)
		SuperCradeSolutionCodeur();
	//on vas avoir des par les interupts
	DebugerPrint("RecupePosAbsSSICodeur : end \n");
}
void GetNextBitSSICodeur(void) {
	DebugerPrint("GetNextBitSSICodeur : begin \n");
	if (codeur.msg.size <= 0x0F) {
		codeur.msg.trame |= GPIO_ReadOutputDataBit(ENACI_Spi_Pin_Groupe,
		ENACI_Spi_Pin_Ic);
		if (codeur.msg.size != 0x0F) {
			codeur.msg.trame << 1; //shift vers la gauche
		}
		codeur.msg.size++; //on vas monter a 0x10

	} else {
		DecodeSSIMsgCodeur();

	}
	DebugerPrint("GetNextBitSSICodeur : end \n");

}
void DeInitSSICodeur(void) {
	DebugerPrint("DeInitSSICodeur : begin \n");
	TIM_CtrlPWMOutputs(ENACI_Spi_Timer, DISABLE);
	GPIO_WriteBit(ENACI_Spi_Pin_Groupe, ENACI_Spi_Pin_Csn, Bit_SET);
	DebugerPrint("DeInitSSICodeur : end \n");

}
//Inclu dans la lib custon afin de garder l'acce aux define dans le .c
void ClearPendingBitSSICodeur(void) {
	TIM_ClearITPendingBit(ENACI_Spi_Timer, TIM_IT_CC1);
}
void DecodeSSIMsgCodeur(void) {
	//verifie de la transmition CRC check
	DebugerPrint("DecodeSSICodeur : begin \n");

	if (CRC_CalcCRC(0x000000000 | codeur.msg.trame) != 0x00000000) {
		/*Trame que nous devons recevoire :
		 * 15     6   5   4   3   2      1    0
		 * Data[9:0] OCF COF LIN MgInc MgDec CRC
		 *
		 * Examine le status du codeur
		 * Ce referet a la datasheat du as5040 pour la signification des erreur
		 */
		if (!(codeur.msg.trame & 0x0020))
			DebugerPrint(
					"DecodeSSIMsgCodeur : Offset Compensation Not Finished \n");
		if (codeur.msg.trame & 0x0010)
			DebugerPrint(
					"DecodeSSIMsgCodeur : Cordic Overflow, data is invalide \n");
		if (codeur.msg.trame & 0x0008)
			DebugerPrint(
					"DecodeSSIMsgCodeur :Linearity Alarm, data may be invalide \n");
		//Comme l'emment ne devrais pas bouger nous n'examinerons pas les valheurs remvoyer
		//stock la trame data dans la strcuct codeur
		codeur.posabscodeur[0] = ((codeur.msg.trame & 0x7FE0) >> 5);
		codeur.msg.dataready = 0xFF;
		DebugerPrint("   Valheur recu du codeur: ");
		DebugerPrintNum((uint32_t) codeur.posabscodeur[0], 10);
	} else {
		DebugerPrint("DecodeSSIMsgCodeur : CRC check failed \n");
		codeur.posabscodeur[0] = 0x0000;
	}
	DeInitSSICodeur();
	DebugerPrint("DecodeSSICodeur : end \n");

}
void InitTimerCodeur(void) {
	TIM_TimeBaseInitTypeDef Tinit;
	uint32_t TIMER_Frequency = get_timer_clock_frequency();
	uint32_t COUNTER_Frequency = 21000000;
	uint32_t PSC_Value = ((168000000 / 2) / COUNTER_Frequency) - 1;	///(TIMER_Frequency / COUNTER_Frequency) - 1;
	uint16_t ARR_Value = (COUNTER_Frequency / ENACI_Pwm_Freq) - 1; //ont veut un output clk de 500kHz
	RCC_APB1PeriphClockCmd(ENACI_Spi_Timer_TIM5, ENABLE);
	DebugerPrint("InitSSICodeur : enable time base \n");
	TIM_TimeBaseStructInit(&Tinit);
	Tinit.TIM_Period = 665; //ARR_Value; //Todo: faire attention a ce que ont reste sur un timer sur 16 bits
	Tinit.TIM_Prescaler = PSC_Value;
	Tinit.TIM_ClockDivision = 0;
	Tinit.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(ENACI_Spi_Timer, &Tinit);
	TIM_Cmd(ENACI_Spi_Timer, ENABLE);
}
void InitGpioCodeur(void) {
	GPIO_InitTypeDef Ginit;

	//GPIO
	//GPIO pour l'input capture
	RCC_AHB1PeriphClockCmd(ENACI_Spi_Gpio_Clk, ENABLE);
	DebugerPrint("InitSSICodeur : enable gpio \n");
	Ginit.GPIO_Pin = ENACI_Spi_Pin_Ic;
	Ginit.GPIO_Speed = GPIO_Speed_50MHz;
	Ginit.GPIO_PuPd = GPIO_PuPd_NOPULL;
	Ginit.GPIO_Mode = GPIO_Mode_IN; //comme ont est en input on a pas a configurer les autres parametres - Todo: a verifier
	GPIO_Init(ENACI_Spi_Pin_Groupe, &Ginit);
	GPIO_SetBits(ENACI_Spi_Pin_Groupe, ENACI_Spi_Pin_Ic);
	DebugerPrint("InitSSICodeur : spi ic bit set \n");
	//GPIO pour selection et lancement du spi
	Ginit.GPIO_Pin = ENACI_Spi_Pin_Csn;
	Ginit.GPIO_Speed = GPIO_Speed_50MHz; // - meme speed
	Ginit.GPIO_Mode = GPIO_Mode_OUT;
	Ginit.GPIO_OType = GPIO_OType_PP;
	Ginit.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(ENACI_Spi_Pin_Groupe, &Ginit);
	GPIO_SetBits(ENACI_Spi_Pin_Groupe, ENACI_Spi_Pin_Csn); //set le pin a hight
	DebugerPrint("InitSSICodeur : csn ic bit set \n");
	//GPIO pour pwm
	if (!SUPER_CRADE) {
		Ginit.GPIO_Pin = ENACI_Spi_Pin_Pwm;
		Ginit.GPIO_Mode = GPIO_Mode_AF;
		Ginit.GPIO_PuPd = GPIO_PuPd_NOPULL;
		Ginit.GPIO_OType = GPIO_OType_PP;
		Ginit.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_Init(ENACI_Spi_Pin_Groupe, &Ginit);
		GPIO_PinAFConfig(ENACI_Spi_Pin_Groupe, ENACI_Spi_Pin_Pwm,
		ENACI_Spi_Timer); //connecter la sortie au timer pour generer le pwm
	} else {
		Ginit.GPIO_Pin = ENACI_Spi_Pin_Pwm;
		Ginit.GPIO_Mode = GPIO_Mode_OUT;
		Ginit.GPIO_OType = GPIO_OType_PP;
		Ginit.GPIO_PuPd = GPIO_PuPd_UP;
		Ginit.GPIO_Speed = GPIO_Speed_25MHz;
		GPIO_Init(ENACI_Spi_Pin_Groupe, &Ginit);
		GPIO_SetBits(ENACI_Spi_Pin_Groupe, ENACI_Spi_Pin_Pwm); //set le pin a hight
	}
}

void InitPwmCodeur(void) {
	TIM_OCInitTypeDef OCinit; //OC : output control
	TIM_OCStructInit(&OCinit); //Todo: verify setings
	OCinit.TIM_OCMode = TIM_OCMode_PWM1;
	OCinit.TIM_OutputState = TIM_OutputState_Enable;
	OCinit.TIM_OCPolarity = TIM_OCPolarity_High;
	OCinit.TIM_Pulse = 333; // duty cycle 50 %

	TIM_OC1Init(ENACI_Spi_Timer, &OCinit);
	TIM_OC1PreloadConfig(ENACI_Spi_Timer, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(ENACI_Spi_Timer, ENABLE);
}
void InitItCodeur(void) {

	DebugerPrint("InitSSICodeur : set interupt\n");
	NVIC_InitTypeDef Ninit;
	TIM_ClearITPendingBit(ENACI_Spi_Timer, TIM_IT_CC1);
	TIM_ITConfig(ENACI_Spi_Timer, TIM_IT_CC1, ENABLE);

	//NVIC init
	Ninit.NVIC_IRQChannel = ENACI_Spi_Timer_interupt_chanel;
	Ninit.NVIC_IRQChannelPreemptionPriority = 0xF;
	Ninit.NVIC_IRQChannelSubPriority = 0;
	Ninit.NVIC_IRQChannelCmd = ENABLE; //The IRQ channel is enabled
	NVIC_Init(&Ninit);
	DebugerPrint("InitSSICodeur : end \n");
}

void SuperCradeSolutionCodeur(void) {
	DebugerPrint("SuperCradeSolutionCodeur : begin \n");

	//solution super crade car le pwm ne semble pas fonctionner - solution bloquante
	GPIO_WriteBit(ENACI_Spi_Pin_Groupe, ENACI_Spi_Pin_Csn, Bit_RESET);
	DebugerDelay(1);
	for (uint8_t i = 0; i < 0x10; i++) {
		GPIO_ResetBits(ENACI_Spi_Pin_Groupe, ENACI_Spi_Pin_Pwm); //set le pin a hight
		DebugerDelay(5);
		GPIO_SetBits(ENACI_Spi_Pin_Groupe, ENACI_Spi_Pin_Pwm); //set le pin a hight
		DebugerDelay(1);
		GetNextBitSSICodeur();
		DebugerDelay(2);

	}
	DebugerPrint("SuperCradeSolutionCodeur : end\n");

	DecodeSSIMsgCodeur();


}

