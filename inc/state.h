/*
 * State.h
 *
 *  Created on: Jan 12, 2017
 *      Author: pooki
 *
 *  About : singleton accessible par tout les fichiers contenant les parametres d'etats
 */

#ifndef INC_STATE_H_
#define INC_STATE_H_
#include <stdint.h>
/*defines */
#define MASK_MEMOIR_DISPO     (uint8_t) 0b00000001
#define MASK_MOUVEMENT_TERMINER (uint8_t)0b00000010
#define MASK_SERVO_ON         (uint8_t) 0b00000100
#define MASK_POS_INDETERMINER (uint8_t) 0b00001000 //Todo : Comprendre ce que franck a voulu dire
#define MASK_POS_INACESSIBLE  (uint8_t) 0b00010000
#define MASK_DEFAULT_TRAJ     (uint8_t) 0b00100000
#define MASK_DEFAULT_TEMPE    (uint8_t) 0b01000000
#define MASK_DECROCHE_MOT     (uint8_t) 0b10000000
/*enums*/
typedef enum {
	TEST_TRAPEZE,
	TEST_STANDARD
}TYPEOFTEST;
/*structures*/

typedef struct _Capteur {
	uint8_t Etatcapt;
	uint8_t Contact;
	uint16_t CourantMaxMeusure;
	uint16_t CourantMeusure;
	uint16_t CoupleMaxInit;
	uint16_t TempMot;
	uint16_t TensionCommande;
} Capteur;
typedef struct _StatsTableau {
	uint16_t CompteurEnvoiPoints;
	uint8_t LimiteTempsTableau;
	uint8_t CoeffTensionTableau;
} StatsTableau;
typedef struct _StatsMouvement {
	int16_t Decalage;
	uint16_t PtCodeurTour;
	uint16_t SensInit;
	uint16_t CodeurOrigine;
	uint16_t Poscodeur;
	uint16_t Phase;
	uint32_t PosAbscodeur;

} StatsMouvement;
typedef struct _MouvementTrapeze{
	uint16_t ConsAcceleration;
	uint16_t  ConsVitesse;
	uint16_t ConsDeceleration;
	int32_t ConsPosition;
}MouvementTrapeze;
typedef struct _Info {
	uint8_t AvecInfoFinMvt;
	uint8_t Arreturgence; /* != 0 signifie un arret d'urgence*/
	uint8_t Octeetat; /* Octee d'etat*/
	/*
	 * 0 - 0 = mémoire disponible dans le tableau des points de trajectoire, 1 = tableau plein
	 *- - 0 = mouvement terminé, 1 = mouvement en cours
	 *2 - 0 = moteur servo off, 1 = moteur servo on
	 *3 - 0= Mise à 0 faite, 1 = position indéterminée
	 *4 - En dehors des limites de la course ou manque de points pour le mode trajectoire.
	 *5 - Défaut trajectoire
	 *6 - Défaut température
	 *7 - Décrochage moteur
	 */
	uint32_t Version;
} Info;
typedef struct _TableauDePoint {
	uint8_t Poslec;/*Position ou nous sommes dans la lecture du tableau*/
	uint8_t Bufwrt; /*Nombre de valheurs pas encore consomer aue nous avons en memoire*/
	int32_t tabpnt[256];/*Tableau contenant le buffer de points sur 256*4 octees , contient 256 * 2 points
	 Se comporte comme un heap, grandit vers le bas*/
} TableauDePoint;
typedef struct _State {
	StatsTableau stab;
	StatsMouvement smvm;
	Capteur cap;
	Info info;
	TableauDePoint tab;
	MouvementTrapeze tmvm;
	uint8_t oldcommande; /*ancienne commande que ont a envoyer */
	int32_t (*get_next)(struct State*); //recuperre le prochain point
	void (*set_next)(struct State*, int32_t); //rajoute le point au heap
	void (*set_oldcom)(struct State*, uint8_t); //met a jour l'ancienne commande
	void (*resettabs)(struct State*); //reste les tableau de point/ mvm trapeze et en vitesse
} State;


State* new_instance(void);
State* get_instance(void);
void DestoryState(State *deadbeaf);
uint8_t TestState(State* st,TYPEOFTEST id);


#endif /* INC_STATE_H_ */
