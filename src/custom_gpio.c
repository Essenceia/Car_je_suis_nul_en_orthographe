/*
 * custom_gpio.c
 *
 *  Created on: Jan 11, 2017
 *      Author: pooki
 */
#include "../inc/custom_gpio.h"

#include "stm32f4xx_gpio.h"

//Todo: mettre les valheurs correcte pour les sorties des leds,a l'heur actuelle c'est mis au hasard
#define LED_ROUGE  ((uint32_t)GPIO_Pin_14)
#define LED_MOTEUR  ((uint32_t)GPIO_Pin_13)
#define LED_VERTE  ((uint32_t)GPIO_Pin_15)
#define LED_GROUPE ((GPIO_TypeDef *)GPIOD) //Todo : s'assurer que toutes les leds appartienent bien au meme groupe
#define LED_RCC    ((uint32_t)RCC_AHB1Periph_GPIOD)

static uint8_t DebugLedEnabled = 0x00;
/* On active les 4 leds du centre de la board afin de les utiliser comme interface de debug*/
void GpioDebugLedInit(void)
{
	//Enable the GPIOD Clock
	  RCC_AHB1PeriphClockCmd(LED_RCC,ENABLE);


	// GPIOD Configuration
	  GPIO_InitTypeDef GPIO_InitStruct;
	  GPIO_InitStruct.GPIO_Pin = LED_ROUGE | LED_VERTE | LED_MOTEUR;
	  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;

	  GPIO_Init(LED_GROUPE, &GPIO_InitStruct);


	// GPIOD-PIN-15,13,12,14 ON
	  GPIO_SetBits(LED_GROUPE, LED_ROUGE | LED_VERTE | LED_MOTEUR);

	  //assert check
	  DebugLedEnabled = 0x01; //Todo: enlever le protection dans le release
}
/*
 * ToggleLed : Fonction utiliser pour le debug, toggle des leds, entre entre 1-2 compris
 */
void ToggleLed(int8_t lednum)
{
	if(DebugLedEnabled){
		switch(lednum){
		case 0x00: GPIO_ToggleBits(LED_GROUPE, LED_MOTEUR);
				break;
		case 0x02: GPIO_ToggleBits(LED_GROUPE, LED_ROUGE);
		break;
		case 0x01: GPIO_ToggleBits(LED_GROUPE, LED_VERTE);
		break;
		default: break;
		}
	}
}
void OffLed(void){
	if(DebugLedEnabled)GPIO_ResetBits(LED_GROUPE,  LED_ROUGE | LED_VERTE | LED_MOTEUR);
}
void OnLed(void){
	if(DebugLedEnabled)GPIO_SetBits(LED_GROUPE,LED_ROUGE | LED_VERTE | LED_MOTEUR);

}
void ToggleAll(void){
	if(DebugLedEnabled)GPIO_ToggleBits(LED_GROUPE,LED_ROUGE | LED_VERTE | LED_MOTEUR);

}
void DispatchLedCommande(int8_t eteindre,int8_t id)
{
	if(DebugLedEnabled){
		uint32_t LED;
		if(id == 1){
			//On a choisi la Led verte sinon rouge
			LED = LED_VERTE;
		}
		else LED = LED_ROUGE;
		if(eteindre){
			GPIO_ResetBits(LED_GROUPE,  LED);
		}
		else GPIO_SetBits(LED_GROUPE,  LED);
	}else{
		//Todo: Handler led not enabled
	}
}





