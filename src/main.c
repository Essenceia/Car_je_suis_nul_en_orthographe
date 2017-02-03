/* Includes */
#include "../inc/commande.h"
#include "../inc/custom_can.h"
#include "../inc/custom_codeur.h"
#include "../inc/custom_gpio.h"
#include "../inc/custom_nvic.h"
#include "../inc/debugger.h"
#include "stm32f4xx.h"
//#include "custom_DebugerPrint.h"
/* Private macro */
/* Private variables */
/* Private function prototypes */
/* Private functions */
//Todo: ranger dans une library specialiser sans que sa fasse planter tout le systeme
static uint32_t DelayInit_1ms(void);
static void Delay(__IO uint32_t nTime);
static __IO uint32_t TimingDelay;
static void init(void)__attribute__((constructor));
/**
 **===========================================================================
 **
 **  Abstract: main program
 **
 **===========================================================================
 */
int main(void) {
//0.Initialisation des ressources
	//1.Declaration des variables
	CAN_Msg Msg;
	Msg.size = 8;
	Msg.valide = 1;
	Msg.message[0] = 0xFF;
	Msg.message[1] = 0x00;
	Msg.message[2] = 0xFF;
	Msg.message[3] = 0x00;
	Msg.message[4] = 0xFF;
	Msg.message[5] = 0x00;
	Msg.message[6] = 0xFF;
	Msg.message[7] = 0x00;
	while (1) {
		DebugerPrint("Loop\n");
		ToggleLed(0);
		DebugerPrint("0\n");
		ToggleLed(1);
		DebugerPrint("1\n");
		ToggleLed(2);
		DebugerPrint("2\n");
		SendCan(&Msg,STANDARD);
		DebugerDelay(300);

	}

}

//Todo: ranger dans une librairie personaliser

void Delay(__IO uint32_t nTime) {
	TimingDelay = nTime;

	while (TimingDelay != 0)
		;
}
void TimingDelay_Decrement(void) {
	if (TimingDelay != 0) {
		TimingDelay--;
	}
}
uint32_t DelayInit_1ms(void) {
	return SysTick_Config(SystemCoreClock / 1000);
}
void init(void) {
	while (DelayInit_1ms())
		;
	DebugerInit(&ITM_SendChar, &Delay,&NVIC_SystemReset);
	//0.2 Led
	GpioDebugLedInit();
	//0.3 Initialisation des interrupts
	NVIC_CAN_Init(); // Init de Interupt sur CAN
	ToggleLed(0);
	//0.4 CAN
	InitCan(); //CANx voir fichier de confige can
	ToggleLed(1);
	//0.5 Codeur
	InitCodeur();
	InitCommande(&SendCan);
	//0.5 Timer de delay - Todo: enlever quand debug terminer
	DebugerPrint("init end\n");
}

