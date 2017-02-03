/*
 * configurations.h
 * Fichier de configuration qui seront utiliser dans state.c
 *  Created on: Jan 25, 2017
 *      Author: pooki
 */

#ifndef INC_CONFIGURATIONS_H_
#define INC_CONFIGURATIONS_H_
#include <stdint.h>

/* Temperature */
#define MaxTemperature ((uint8_t)0x46) //70 en decimale

/* Tension */
#define UMax ((uint16_t)0xFFFF)
#define UMin ((uint16_t)0xFFFF)
#define UTableau ((uint16_t)0xFFFF)
#define UStat ((uint16_t)0xFFFF)
#define UInit ((uint16_t)0xFFFF)
#define UModeVitesse ((uint16_t)0xFFFF)

/*Position autoriser*/
#define MaxPosition  ((uint32_t)1024000)
#define MinPosition  ((uint32_t)-1024000)

/*Decalage autoriser*/
#define DecalageDecrochage ((uint16_t)0xFFFF)
#define DecalageDecrochageArret ((uint16_t)0xFFFF)

/* Configuration particuliere a la localisation du moteur controller sur le robot*/
#define AvecInitContact ((uint8_t)0x01) //booleen
#define PosOrigine       ((int32_t)0)
#define PasVis ((uint16_t)0xFFFF)

/*Configuration type de mouvement */
//#define AvecModeVitesse  ((uint8_t)0x00) Todo: savoir si on remet le mode vitesse
#define ToleranceMvt      ((uint8_t)0xFF)//permet d'autoriser le mvm trapezoidiale
#define NiveauAsstPos ((uint8_t)0x01)

/*Informations generale*/
#define VersionSoft ((uint32_t)0x00000001)//Version du software
#define AdressePropre ((uint8_t)0b00000001) //Adresse propre du moteur
#define AdresseDest    ((uint8_t)0x79) //Adresse de destination
#define AdresseDestMC  ((uint8_t)0x88) //Adresse de destination pour le MC afin de les interpreter differament des trames normals
/*Getters*/
/*Tension*/
extern inline uint16_t GetUMax(void);
extern inline uint16_t GetUMin(void);
extern inline uint16_t GetUTableau(void);
extern inline uint16_t GetUStat(void);
extern inline uint16_t GetUInit(void);
extern inline uint16_t GetUModeVitesse(void);

/*Position*/

extern inline uint32_t GetMaxPosition(void);
extern inline uint32_t GetMinPosition(void);
extern inline int32_t GetPosOrigine(void);
/*Decalade*/
extern inline uint16_t GetDecalageDecrochage(void);
extern inline uint16_t GetDecalageDecrochageArret(void);
/*Configuration mouvement*/
extern inline uint8_t GetToleranceMvt(void);
extern inline uint8_t GetNiveauAsstPos(void);

/*Configuration caracteristiques physique */
extern inline uint16_t GetPasVis(void);

/*Informations generale*/
extern inline uint8_t GetAdressePropre(void);
extern inline uint8_t GetAdresseDest(void);
extern inline uint8_t GetAdresseDestMC(void);
extern inline uint32_t GetVersion(void);
#endif /* INC_CONFIGURATIONS_H_ */

