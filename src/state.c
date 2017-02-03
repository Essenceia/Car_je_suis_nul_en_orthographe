/*
 * State.c
 *
 *  Created on: Jan 12, 2017
 *      Author: pooki
 */

#include "../inc/state.h"

#include <stdlib.h>

#include "../inc/configurations.h"
#include "../inc/debugger.h"
#include "../inc/struct_can_msg.h"

/*Fonctions Privee*/
int32_t PASUTILISER_getnext(State *self);
void PASUTILISER_setnext(State *self, int32_t nxt);
void PASUTILISER_setoldcommande(State *self, uint8_t commande);
void PASUTILISER_reset(State *self);
/* La creation d'une nouvelle instance d'etat lancera la recuperation des tout les
 * parametres d'etats tel que le tension utliser actuellement dans le systeme.
 * Remarque : Ne recuperera pas les paremetres qui dependent du codeur
 */
State* new_instance(void) {
	State *new = (State *) calloc(1, sizeof(State)); //initialiser la strucutre a zero
	new->get_next = PASUTILISER_getnext;
	new->set_next = PASUTILISER_setnext;
	new->set_oldcom = PASUTILISER_setoldcommande;
	new->resettabs = PASUTILISER_reset;
	return new;
}
State * get_instance(void) {
	static State* _instance;
	static uint8_t StateInit = 0x00;
	if (StateInit == 0x00) {
		StateInit = 0xFF;
		_instance = new_instance();
	}
	return _instance;
}
void DestoryState(State *deadbeaf) {
	free(deadbeaf->get_next);
	free(deadbeaf->set_next);
	free(deadbeaf->set_oldcom);
	free(deadbeaf); /*Ceci deallou la structure vers lequel le pointeur pointe pas le pointeur en lui meme qui est statique*/
}
int32_t PASUTILISER_getnext(State *self) {
	if (self->info.Octeetat & MASK_MEMOIR_DISPO) {
		self->info.Octeetat &= ~(MASK_MEMOIR_DISPO ); /* On consome un point donc le buffer n'est plus a capaciter maximal */
	}
	if (self->tab.Bufwrt) {
		self->tab.Bufwrt--;
		self->tab.Poslec++;
		return (self->tab.tabpnt[self->tab.Poslec]);
	} else {
		//Todo : trouver un moyen de blinder
		return (int32_t) 0x00000000; //Todo: afficher un message d'erreur , il ne nous reste plus de valheurs d'avance*/
	}
}
void PASUTILISER_setnext(State *self, int32_t nxt) {
	if (self->tab.Bufwrt != 0xFF) {
		self->tab.Bufwrt++;
		self->tab.tabpnt[self->tab.Poslec + self->tab.Bufwrt] = nxt;
	} else {
		DebugerPrint(
				"SetNext: le buffer est deja plein, nous ne pouvons pas recevoire de nouveaux points");
		self->info.Octeetat |= MASK_MEMOIR_DISPO; /* On met l'octe d'etat a 1 car le buffer est plein*/
	}
}
/*
 * Modifie la valheur de la derniere commande donner.
 * Nous permet de rajouter des conditions sur la mise a jour de l'historique des commandes
 */
void PASUTILISER_setoldcommande(State *self, uint8_t commande) {
	//Todo: implementer les comditions de mise a jour des commandes, eg: si on recoit une requette est t'elle compter comme la derniere commande
	if ((commande >= 0x01) && (commande <= 0x0F)) {
		self->oldcommande = commande;
	} else {
		DebugerPrint(
				"SetOldCommande: le numeros de commande envoyer est invalide \n");
	}
}
/*
 * Permet de tester l'etat et verifier que tout fonctionner bien -> permet d'autoriser un pouveau mouvement
 */
uint8_t TestState(State* st,TYPEOFTEST id) {
	if(id==TEST_TRAPEZE){return ((!st->info.Arreturgence)
			&& (!(st->info.Octeetat & MASK_DECROCHE_MOT ))
			&& (st->tmvm.ConsPosition <= MaxPosition)
			&& (st->tmvm.ConsPosition >= MinPosition)
			&& (st->cap.TempMot <= MaxTemperature)
			&& (st->info.Octeetat & MASK_SERVO_ON));}
	else {return ((!st->info.Arreturgence)
			&&(st->info.Octeetat & MASK_SERVO_ON)
			&&(!(st->info.Octeetat & MASK_DECROCHE_MOT))
			&&(st->cap.TempMot <= MaxTemperature));}
}
void PASUTILISER_reset(State *self){
	/* On reset tout a zero */
	self->info.Arreturgence = 0;
	memset(&self->tmvm, 0, sizeof(self->tmvm));
	memset(&self->tab,0,sizeof(self->tab));
	//memset(self->vmvm,0,sizeof(struct MouvementVitesse));
	memset(&self->smvm,0,sizeof(self->smvm));
}












