/*
 * custom_handler.h
 *
 *  Created on: Jan 10, 2017
 *      Author: pooki
 */

#ifndef INC_CUSTOM_HANDLER_H_
#define INC_CUSTOM_HANDLER_H_

#ifdef USE_FULL_ASSERT
//define handler for assert_failed
extern void assert_failed(uint8_t* file, uint32_t line);
#endif

#endif /* INC_CUSTOM_HANDLER_H_ */
