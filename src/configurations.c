/*
 * configurations.c
 * Declation des getters avec les blindages approprier quand ils ont etaient defini d
 * dans le code PIC.
 *  Created on: Jan 25, 2017
 *      Author: pooki
 */
#include "../inc/configurations.h"
#include "../inc/configurations_default.h"



inline  uint32_t GetMaxPosition(void) {
	return MaxPosition;
}
inline  uint32_t GetMinPosition(void) {
	return MinPosition;
}

inline int32_t GetPosOrigine(void){
	return ((PosOrigine > GetMaxPosition()|| PosOrigine < GetMinPosition())? PosOrigineDef : PosOrigine);
}

inline  uint16_t GetUMax(void){
	return (((UMax<4000)||(UMax>28300))? UMaxDef : UMax);
}
inline  uint16_t GetUMin(void){
	return ((UMin < 1000 || UMin > 28300)? UMinDef : UMin) ;
}
inline  uint16_t GetUTableau(void){
	return ((UTableau < 4000 || UTableau > 28300) ? UTableauDef : UTableau);
}
inline uint16_t GetUStat(void){
	return ((UStat < 1000 || UStat > 12000) ? UStatDef : UStat );
}
inline  uint16_t GetUInit(void){
	return ((UInit < 1000 || UInit > 20000)? UInitDef : UInit );
}
inline  uint16_t GetUModeVitesse(void){
	return UModeVitesse;
}
inline  uint16_t GetPasVis(void){
	return ((PasVis < 100 || PasVis > 65000)? PasVisDef : PasVis);
}
inline uint16_t GetDecalageDecrochage(void){
	return DecalageDecrochage;
}
inline  uint16_t GetDecalageDecrochageArret (void){
	return DecalageDecrochageArret;
}
inline  uint8_t GetToleranceMvt(void){
	return ((ToleranceMvt < 0 || ToleranceMvt > 10)? ToleranceMvtDef : ToleranceMvt);
}
inline  uint8_t GetNiveauAsstPos(void){
	return ((NiveauAsstPos > 4 || NiveauAsstPos < 1)? NiveauAsstPosDef : NiveauAsstPos);
}
inline uint8_t GetAdressePropre (void){
	return ((AdressePropre == AdresseDest)? AdressePropreDef : AdresseDest);
}
inline uint8_t GetAdresseDest (void){
	return AdresseDest;
}
inline uint32_t GetVersion (void){
	return VersionSoft;
}
inline uint8_t GetAdresseDestMC(void){
	return AdresseDestMC;
}



















