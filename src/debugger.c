/*
 * debugger.c
 *
 *  Created on: Jan 17, 2017
 *      Author: pooki
 */
#include "../inc/debugger.h"
/*
 * Pointeur sur fonction vers ITM_SendChar.
 * Warning: ne pas s'y meprendre il est initialiser dans le Init.
 * Si retirer alors le debugger cesse de fonctionner.
 */
static DebugerSendChar Sender;
static DebugerWaiter Delay;//todo: reactiver le delay
static SystemReset Reset;
/*
 * Verification de l'initialisation
 */
static uint8_t is_init = 0x00;

void DebugerPrint(char *msg){
	if(is_init){
	while((*msg)||(*msg != '\0'))
	{
     Sender(*msg++);
	}
	}

}
void DebugerPrintNum(uint32_t d,uint8_t base)
{
	if(is_init){
	char buf = ' ';
	uint32_t div = 1;
	while (d/div >= base)
		div *= base;

	while (div != 0)
	{
		uint8_t num = d/div;
		d = d%div;
		div /= base;
		if (num > 9)
			buf = (num-10) + 'A';
		else
			buf = num + '0';
		Sender(buf);
	}
	}
}
void DebugerDelay(uint32_t time){
	if(is_init){
		//Delay(time);
	}
}
/*
 * Initilalisation de la console de debug , doit ce faire en premier dans le main:
 * exemple d'utilisation:	DebugerInit(&ITM_SendChar);
 *
 * Permet d'avoir une hierarchie des includes plus propre
 *
 */
void DebugerInit(uint32_t (*f)(uint32_t),void (*d) (uint32_t),void (*r)(void)){
	is_init ++;
	Sender = (*f);
	//Delay = (*d);
	Reset = (*r);
}
void DebugerReset(void){
	DebugerPrint("DebugerReset : Calling reset \n");
	Reset();
}








