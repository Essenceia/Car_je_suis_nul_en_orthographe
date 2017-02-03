/*
 * debugger.h
 *
 *  Created on: Jan 16, 2017
 *      Author: pooki
 */

#ifndef INC_DEBUGGER_H_
#define INC_DEBUGGER_H_
#include <stdint.h>

typedef uint32_t (*DebugerSendChar)(uint32_t ch);
typedef void (*DebugerWaiter)(uint32_t t);
typedef void (*SystemReset)(void);
void DebugerInit(uint32_t (*f)(uint32_t),void (*d)(uint32_t),void (*r)(void));
void DebugerPrint(char *msg);
void DebugerPrintNum(uint32_t d, uint8_t base);
void DebugerDelay(uint32_t t);
void DebugerReset(void);
#endif /* INC_DEBUGGER_H_ */
