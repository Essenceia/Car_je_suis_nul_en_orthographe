/*
 * custom_codeur.h
 *
 *  Created on: Jan 18, 2017
 *      Author: pooki
 */

#ifndef INC_CUSTOM_CODEUR_H_
#define INC_CUSTOM_CODEUR_H_
#include <stdint.h>
/*
 * Branchement sur AS5040
 * Clk(10)- timer ssi
 * Csn(11) - Chip select
 * Do(9) - Data SSI
 * A(3) - channel A
 * B(4) - channel B
 * Index(6) - Index
 */
void InitCodeur(void);
/*
 * Afin de recupere la position relle du codeur a l'initialisation
 */
void InitSSICodeur(void);
void RecupePosAbsSSICodeur(void);
void GetNextBitSSICodeur(void);
void DeInitSSICodeur(void);
inline void ClearPendingBitSSICodeur(void);

typedef struct Ssi_msg {
	uint8_t size;
	uint8_t dataready;
	uint16_t trame;
} Ssi_msg;
typedef struct Codeur {
	int16_t offset;
	uint16_t* posabscodeur; //lie directement avec la structure dans state
	int16_t pos; //position du codeur - relatif pas absolu Todo: calculer pos relatif codeur
	int16_t nombredetour;
	Ssi_msg msg;
} Codeur;

#endif /* INC_CUSTOM_CODEUR_H_ */
