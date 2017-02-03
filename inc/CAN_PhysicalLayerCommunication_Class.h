/**
  ******************************************************************************
  * @file    CAN_PhysicalLayerCommunication_Class.h
  * @author  STMicroelectronics - System Lab - MC Team
  * @version 4.3.0
  * @date    22-Sep-2016 15:29
  * @brief   This file contains interface of CAN Physical Layer Communication class
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CAN_PLC_H
#define __CAN_PLC_H

#include "../inc/MC_type.h"

/** @addtogroup STM32_PMSM_UI_Library
  * @{
  */

/** @addtogroup CAN_PhysicalCommunicationLayer
  * @{
  */
  
/** @defgroup CAN_COM_class_exported_types CAN physical layer communication class exported types
* @{
*/

/* Exported types ------------------------------------------------------------*/
/** 
  * @brief  UserInterface class parameters definition  
  */
typedef const struct
{
  CAN_TypeDef* CANx;
  uint32_t wCANRemapping;
  uint32_t wCANClockSource;
  GPIO_TypeDef* hRxPort;
  uint16_t hRxPin;
  GPIO_TypeDef* hTxPort;
  uint16_t hTxPin;
  uint8_t bUIIRQn;
  CAN_InitTypeDef* CAN_InitStructure;
  NVIC_InitTypeDef* NVIC_InitStructure;
}CANParams_t, *pCANParams_t;

/* Exported constants --------------------------------------------------------*/

/** 
  * @brief  Public UserInterface class definition 
  */
typedef struct CCAN_COM_t *CCAN_COM;

/**
* @}
*/

/** @defgroup CAN_COM_class_exported_methods CAN physical layer communication class exported methods
  * @{
  */

/*Methods*/
CCAN_COM CAN_NewObject(pCANParams_t pCANParams);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif /* __CAN_PLC_H */

/******************* (C) COPYRIGHT 2016 STMicroelectronics *****END OF FILE****/
