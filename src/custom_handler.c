/*
 * custom_handler.c
 *
 *  Created on: Jan 10, 2017
 *      Author: pooki
 */
#include "../inc/custom_handler.h"

#ifdef  USE_FULL_ASSERT
/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
/* User can add his own implementation to report the file name and line number */
printf("\n\r Wrong parameter value detected on\r\n");
printf("       file  %s\r\n", file);
printf("       line  %d\r\n", line);
/* Infinite loop */
while (1)
{
}
}
#endif /* USE_FULL_ASSERT */
