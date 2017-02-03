/*
 * custom_gpio.h
 *
 *  Created on: Jan 11, 2017
 *      Author: pooki
 */

#ifndef INC_CUSTOM_GPIO_H_
#define INC_CUSTOM_GPIO_H_
#include <stdint.h>

void GpioDebugLedInit(void);
void DispatchLedCommande(int8_t eteindre,int8_t id);
/* Fonctions de tests , ne doivent pas apparaitre dans la version finale*/
//Todo : enlever les occurences du code finale
void ToggleLed(int8_t lednum);
void OffLed(void);
void OnLed(void);
void ToggleAll(void);


#endif /* INC_CUSTOM_GPIO_H_ */
