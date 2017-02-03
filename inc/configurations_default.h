/*
 * configurations_default.h
 *Valheurs par defaults, paramettres a ne pas modifier contrairement au
 *Valheurs du fichier de configuration "configurations.h"
 * N'a pas pour but d'etre inclu dans d'autres ficiers que "
 *  Created on: Jan 26, 2017
 *      Author: pooki
 */

#ifndef INC_CONFIGURATIONS_DEFAULT_H_
#define INC_CONFIGURATIONS_DEFAULT_H_

#define MOTEUR M60 //M70, M82 ou B10

/*definition des parametres qui ne varient pas celon le type de moteur*/
#define AdressePropreDef ((uint8_t)0x10)
#define UMaxDef ((uint16_t)20000)
#define UTableauDef ((uint16_t)16000)
#define  DecalageDecrochageDef  ((uint16_t)0x0080) //comme dans le code PIC on avait ddarret < ddef j'ai dessider de mofifier la config : ceci pourrait etre source d'erreur
#define  DecalageDecrochageArretDef  ((uint16_t)0x0100)
#define NiveauAsstPosDef        ((uint8_t)2)			// 1 = Tension fixe, 2 asservissement position faible, 3 asservissement position fort, 5 = mise en but�e (robot moteur 2)
//#define UModeVitesseDef ((uint16_t)0xFFFF)
#define UMinDef ((uint16_t)3000)
#define PosOrigineDef ((int32_t) 0)
#define ToleranceMvtDef ((uint8_t )0) 	// 0 = respect de la consigne, 1 = possibilit� de r�duction de vitesse
/*definition des paramettres qui varient*/
#if MOTEUR == M60
#undef UMaxDef //lorsque seullement une valheur varie on la re-defini
#define UMaxDef ((uint16_t)28300)
#define PasVisDef ((uint16_t)3000)
#define UStatDef ((uint16_t)2000)
#define UInitDef ((uint16_t)3000)

#elif MOTEUR == M70
#define PasVisDef ((uint16_t)5000)
#define UStatDef ((uint16_t)1200)
#undef UTableauDef
#define UTableauDef ((uint16_t)20000)
#define UInitDef ((uint16_t)2000)

#elif MOTEUR == M82
#define PasVisDef ((uint16_t)40000)
#define UStatDef ((uint16_t)1000)
#define UInitDef ((uint16_t)1500)

#elif MOTEUR == B10
#define PasVisDef ((uint16_t)1250)
#define UStatDef ((uint16_t)5000)
#define UInitDef ((uint16_t)10000)
#endif




#endif /* INC_CONFIGURATIONS_DEFAULT_H_ */
