/**
  ******************************************************************************
  * @file    CAN_F0XX_PhysicalLayerCommunication_Class.c
  * @author  STMicroelectronics - System Lab - MC Team
  * @version 4.3.0
  * @date    22-Sep-2016 15:29
  * @brief   Private implementation for Physical Layer Communication for F0XX
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

/* Includes ------------------------------------------------------------------*/

#include "../inc/CAN_PhysicalLayerCommunication_Class.h"

#include "PhysicalLayerCommunication_Class.h"
#include "PhysicalLayerCommunication_Private.h"
#include "CAN_PhysicalLayerCommunication_Private.h"
#include "UIIRQHandlerPrivate.h"

#ifdef MC_CLASS_DYNAMIC
  #include "stdlib.h" /* Used for dynamic allocation */
#else
  
  #define MAX_CAN_COM_NUM 1

  _CCAN_t CAN_COMpool[MAX_CAN_COM_NUM];
  uint8_t CAN_COM_Allocated = 0;
#endif

/* Private function prototypes -----------------------------------------------*/
void CAN_HWInit(pCANParams_t pCANParams);
void* CAN_IRQ_Handler(void* this,unsigned char flags, unsigned short rx_data);
static void CAN_StartReceive(CCOM this);
static void CAN_StartTransmit(CCOM this);
static uint16_t CAN_GPIOPin2Source(uint16_t GPIO_Pin);
static uint8_t CAN_AF(CAN_TypeDef* CANx, GPIO_TypeDef* GPIOx);

/**
  * @brief  Creates an object of the class "Physical Layer Communication"
  * @param  pSensorParam: Physical Layer parameters
  * @retval oCOM: new Physical Layer object
  */
CCAN_COM CAN_NewObject(pCANParams_t pCANParams)
{
  _CCOM _oCOM;
  _CCAN _oCAN;
  
  _oCOM = (_CCOM)COM_NewObject();

  #ifdef MC_CLASS_DYNAMIC
    _oCAN = (_CCAN)calloc(1,sizeof(_CCAN_t));
  #else
    if (CAN_COM_Allocated  < MAX_CAN_COM_NUM)
    {
      _oCAN = &CAN_COMpool[CAN_COM_Allocated++];
    }
    else
    {
      _oCAN = MC_NULL;
    }
  #endif
  
  _oCAN->pDParams_str = pCANParams;
  _oCOM->DerivedClass = (void*)_oCAN;
  _oCOM->Methods_str.pStartReceive = &CAN_StartReceive;
  _oCOM->Methods_str.pStartTransmit = &CAN_StartTransmit;
  
  _oCOM->Methods_str.pIRQ_Handler = &CAN_IRQ_Handler;
  Set_UI_IRQ_Handler(pCANParams->bUIIRQn, (_CUIIRQ)_oCOM);
  
  //Init Struct communication
  COM_ResetTX((CCOM)_oCOM);
  COM_ResetRX((CCOM)_oCOM);
  
  CAN_HWInit(pCANParams);
  
  return ((CCAN_COM)_oCOM);
}

void CAN_HWInit(pCANParams_t pCANParams)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable CAN clock: UASRT1 -> APB2, CAN2-5 -> APB1 */
  if (pCANParams->wCANClockSource == RCC_APB2Periph_CAN1)
  {
    RCC_APB2PeriphClockCmd(pCANParams->wCANClockSource, (FunctionalState)(ENABLE));
  }
  else
  {
    RCC_APB1PeriphClockCmd(pCANParams->wCANClockSource, (FunctionalState)(ENABLE));
  }  
  
  /* CAN Init structure */
  /* Configure CAN */
  CAN_Init(pCANParams->CANx, pCANParams->CAN_InitStructure);
    
  /* Configures the GPIO ports for CAN. */
  GPIO_PinAFConfig(pCANParams->hRxPort, CAN_GPIOPin2Source(pCANParams->hRxPin), CAN_AF(pCANParams->CANx, pCANParams->hRxPort));
  GPIO_PinAFConfig(pCANParams->hTxPort, CAN_GPIOPin2Source(pCANParams->hTxPin), CAN_AF(pCANParams->CANx, pCANParams->hRxPort));
  
  /* Configure Rx, Tx pins */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_InitStructure.GPIO_Pin = pCANParams->hRxPin;
  GPIO_Init(pCANParams->hRxPort, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = pCANParams->hTxPin;
  GPIO_Init(pCANParams->hTxPort, &GPIO_InitStructure);
  
  if (pCANParams->NVIC_InitStructure->NVIC_IRQChannelCmd == ENABLE)
  {
    /* Enable CAN Receive and Transmit interrupts */
    if ((pCANParams->CAN_InitStructure->CAN_Mode & CAN_Mode_Rx) == CAN_Mode_Rx)
    {
      CAN_ITConfig(pCANParams->CANx, CAN_IT_RXNE, (FunctionalState)(ENABLE));
    }
    if ((pCANParams->CAN_InitStructure->CAN_Mode & CAN_Mode_Tx) == CAN_Mode_Tx)
    {
      CAN_ITConfig(pCANParams->CANx, CAN_IT_TXE, (FunctionalState)(DISABLE));
    }
    /* Enable the CANy Interrupt */
    NVIC_Init(pCANParams->NVIC_InitStructure);
  }
  
  /* Enable the CAN */
  CAN_Cmd(pCANParams->CANx, (FunctionalState)(ENABLE));
}

/*******************************************************************************
* Function Name  : CAN_IRQ_Handler
* Description    : Interrupt function for the serial communication
* Input          : none 
* Return         : none
*******************************************************************************/
void* CAN_IRQ_Handler(void* this,unsigned char flags, unsigned short rx_data)
{
  void* pRetVal = MC_NULL;
  if (flags == 0) // Flag 0 = RX
  {
    /* Read one byte from the receive data register */
    if (((_CCOM)this)->Vars_str.PL_Data.RX.Buffer != MC_NULL && 
        ((_CCOM)this)->Vars_str.PL_Data.RX.BufferTransfer < ((_CCOM)this)->Vars_str.PL_Data.RX.BufferCount)
    {
      rx_data = ((_CCAN)(((_CCOM)this)->DerivedClass))->pDParams_str->CANx->RDR;
      ((_CCOM)this)->Vars_str.PL_Data.RX.Buffer[((_CCOM)this)->Vars_str.PL_Data.RX.BufferTransfer++] = (uint16_t)(rx_data & (uint16_t)0x01FF);
            
      pRetVal = ReceivingFrame(((_CCOM)this)->Vars_str.parent,((_CCOM)this)->Vars_str.PL_Data.RX.Buffer,((_CCOM)this)->Vars_str.PL_Data.RX.BufferTransfer);
    }
  }
  
  if (flags == 1) // Flag 1 = TX
  {
    /* Write one byte to the transmit data register */
    CAN_SendData(((_CCAN)(((_CCOM)this)->DerivedClass))->pDParams_str->CANx, ((_CCOM)this)->Vars_str.PL_Data.TX.Buffer[((_CCOM)this)->Vars_str.PL_Data.TX.BufferTransfer++]);
    
    if (((_CCOM)this)->Vars_str.PL_Data.TX.BufferCount <= ((_CCOM)this)->Vars_str.PL_Data.TX.BufferTransfer)
    {
      /* Disable the CAN Transfer interrupt */
      CAN_ITConfig(((_CCAN)(((_CCOM)this)->DerivedClass))->pDParams_str->CANx, CAN_IT_TXE, (FunctionalState)(DISABLE));
  
      SendingFrame(((_CCOM)this)->Vars_str.parent,((_CCOM)this)->Vars_str.PL_Data.TX.Buffer, ((_CCOM)this)->Vars_str.PL_Data.TX.BufferTransfer);

      //Init communication for next transfer;
      //PL_ResetTX();
    }
  }
  if (flags == 2) // Flag 2 = Send overrun error
  {
    FCP_SendOverrunMeassage(((_CCOM)this)->Vars_str.parent);
  }
  if (flags == 3) // Flag 3 = Send timeout error
  {
    FCP_SendTimeoutMeassage(((_CCOM)this)->Vars_str.parent);
  }
  if (flags == 4) // Flag 4 = Send ATR message
  {
    FCP_SendATRMeassage(((_CCOM)this)->Vars_str.parent);
  }
  return pRetVal;
}

/**
  * @brief  Start receive from the channel (IRQ enabling implementation)
  * @param  this: COM object 
  * @retval None
  */
static void CAN_StartReceive(CCOM this)
{
  CAN_ITConfig(((_CCAN)(((_CCOM)this)->DerivedClass))->pDParams_str->CANx, CAN_IT_RXNE, (FunctionalState)(ENABLE));
}

/**
  * @brief  Start transmit to the channel (IRQ enabling implementation)
  * @param  this: COM object 
  * @retval None
  */
static void CAN_StartTransmit(CCOM this)
{
  CAN_ITConfig(((_CCAN)(((_CCOM)this)->DerivedClass))->pDParams_str->CANx, CAN_IT_TXE, (FunctionalState)(ENABLE));
}

/**
  * @brief  It is an internal function used to compute the GPIO Source 
  *         value starting from GPIO pin value. The GPIO Source value 
  *         is used for AF remapping.
  * @param  GPIO_Pin Pin value to be converted.
  * @retval uint16_t The GPIO pin source value converted.
  */
static uint16_t CAN_GPIOPin2Source(uint16_t GPIO_Pin)
{
  uint16_t hRetVal;
  switch (GPIO_Pin)
  {
  case GPIO_Pin_0:
    {
      hRetVal = GPIO_PinSource0;
      break;
    }
  case GPIO_Pin_1:
    {
      hRetVal = GPIO_PinSource1;
      break;
    }
  case GPIO_Pin_2:
    {
      hRetVal = GPIO_PinSource2;
      break;
    }
  case GPIO_Pin_3:
    {
      hRetVal = GPIO_PinSource3;
      break;
    }
  case GPIO_Pin_4:
    {
      hRetVal = GPIO_PinSource4;
      break;
    }
  case GPIO_Pin_5:
    {
      hRetVal = GPIO_PinSource5;
      break;
    }
  case GPIO_Pin_6:
    {
      hRetVal = GPIO_PinSource6;
      break;
    }
  case GPIO_Pin_7:
    {
      hRetVal = GPIO_PinSource7;
      break;
    }
  case GPIO_Pin_8:
    {
      hRetVal = GPIO_PinSource8;
      break;
    }
  case GPIO_Pin_9:
    {
      hRetVal = GPIO_PinSource9;
      break;
    }
  case GPIO_Pin_10:
    {
      hRetVal = GPIO_PinSource10;
      break;
    }
  case GPIO_Pin_11:
    {
      hRetVal = GPIO_PinSource11;
      break;
    }
  case GPIO_Pin_12:
    {
      hRetVal = GPIO_PinSource12;
      break;
    }
  case GPIO_Pin_13:
    {
      hRetVal = GPIO_PinSource13;
      break;
    }
  case GPIO_Pin_14:
    {
      hRetVal = GPIO_PinSource14;
      break;
    }
  case GPIO_Pin_15:
    {
      hRetVal = GPIO_PinSource15;
      break;
    }
  default:
    {
      hRetVal = 0u;
      break;
    }
  }
  return hRetVal;
}

static uint8_t CAN_AF(CAN_TypeDef* CANx, GPIO_TypeDef* GPIOx)
{
  uint8_t hRetVal = 0u;
  if (CANx == CAN1)
  {
    if (GPIOx == GPIOA)
    {
      hRetVal = GPIO_AF_1;
    }
    if (GPIOx == GPIOB)
    {
      hRetVal = GPIO_AF_0;
    }
  }
  if (CANx == CAN2)
  {
    if (GPIOx == GPIOA)
    {
      hRetVal = GPIO_AF_1;
    }
    if (GPIOx == GPIOD)
    {
      hRetVal = GPIO_AF_0;
    }
  }
  if (CANx == CAN3)
  {
    if (GPIOx == GPIOB)
    {
      hRetVal = GPIO_AF_4;
    }
    if (GPIOx == GPIOC)
    {
      hRetVal = GPIO_AF_1;
    }
    if (GPIOx == GPIOD)
    {
      hRetVal = GPIO_AF_0;
    }
  }
  if (CANx == CAN4)
  {
    if (GPIOx == GPIOA)
    {
      hRetVal = GPIO_AF_4;
    }
    if (GPIOx == GPIOC)
    {
      hRetVal = GPIO_AF_0;
    }
  }
  return hRetVal;
}

/******************* (C) COPYRIGHT 2016 STMicroelectronics *****END OF FILE****/
