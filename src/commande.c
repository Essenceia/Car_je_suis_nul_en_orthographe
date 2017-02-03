/*
 * commande.c
 *
 *  Created on: Jan 13, 2017
 *      Author: pooki
 */

#include "../inc/commande.h"

#include <stdlib.h>

#include "../inc/commande_liste.h"
#include "../inc/commande_private.h"
#include "../inc/configurations.h"
#include "../inc/custom_codeur.h"
#include "../inc/custom_gpio.h"
#include "../inc/state.h"
typedef void (*sendcommande)(CAN_Msg*, DESTINATION);
typedef void (*mc_sendcommande)(void *);
/* Variables */
static uint8_t isinit = 0x00;
static State* icommande;
static sendcommande SendCommande;
static CAN_Msg* stdrsp;
static mc_sendcommande MC_SendCommande;

void InitCommande(void (*f)(CAN_Msg)) {
	if (isinit == 0x00) {
		icommande = get_instance();
		SendCommande = (*f);
		stdrsp = (CAN_Msg *) malloc(sizeof(CAN_Msg));
		stdrsp->size = 0x08;
		stdrsp->valide = 0xFF;
		stdrsp->message = (int8_t *) calloc(8, sizeof(int8_t));
		isinit++;
	}
}
/*
 * S'occupe du dispatch des messages et de la destruction du frame CAN_Msg
 */

void DispatchNoMsg(CAN_Msg *msg) {
	if (msg->valide != 0x00 && isinit) {
		switch (msg->NoMsg) {
		case NM_NEW: /*Dispatch d'une nouvelle commande*/
			DispatchNewCommande(msg);
			break;
		case NM_NEXT: /*Dispatch d'une commande qui suit une precedante*/
			DispatchNextCommande(msg);
			break;
		case NM_MCP:/* Envoie du message au mc protocol */
			if (isinit > 1) { // verification que ont a bien initaliser le sender
				mc_sendcommande (msg); //pointeur sur fonction vers MCP_custom_Recive
			}
			break;
		case NM_POINT: /*rajoute des points dans la memoire*/
			/*Todo: verifier que ont passe bien les bits dans le bonne ordre de maniere a ce que le point soit correct , eg: que sa ne soit pas 3,2,1,0
			 */
			AjoutPoint(msg);
			break;
		case NM_LED:/*Allumage des diodes du moteur*/
			if (msg->size >= 2)
				DispatchLedCommande(msg->message[0], msg->message[1]);
			break;
		default:
			DebugerPrint("DispatchNoMsg: NoMessage invalide \n");
			break;
		}
	} else {
		DebugerPrint("DispatchNoMsg: CAN_Msg invalide \n");
	}

}
void DispatchNewCommande(CAN_Msg *msg) {
	icommande->set_oldcom(icommande, msg->Commande);
	switch (msg->Commande) {
	case CM_LECTURE_STATE:
		/*
		 * Lecture de position
		 */
		LectureState(msg->message[0]);
		break;
	case CM_LECTURE_TRAME:
		/*
		 * Lecture d'un point en memoire
		 */
		RenvoiPoint(msg->message[0]);
	case CM_COMAND_VITESSE: //Todo: re-implementer le mode vitesse si il est encore utiliser
		break;
	case CM_TRAPEZE: //mouvement trapeze
		if (msg->size == 8) {
			//Todo: demander a franc pourquoi il multiplie par 256?
			icommande->tmvm.ConsAcceleration = msg->message[0]
					+ 0x100 * msg->message[3];
			icommande->tmvm.ConsVitesse = msg->message[2]
					+ 0x100 * msg->message[5];
			icommande->tmvm.ConsDeceleration = msg->message[4]
					+ 0x100 * msg->message[7];
			icommande->tmvm.ConsPosition = 0x0000 | msg->message[6];
		} else
			DebugerPrint(
					"DispatchNewCommande: Commande 0x05 ,erreur : il manque de l'information \n");
		break;
	case CM_LECTURE_OCTEE_ETAT:
		EnvoiOcteEtat();
		break;
	case CM_RESET:
		RAZ(msg);
		break;
	case CM_DEPART:
		if (TestState(icommande, TEST_STANDARD)) {
			//DepartSuivi();
			icommande->info.Octeetat &= ~MASK_MOUVEMENT_TERMINER; // Mouvement en cours , on passe le bit a 0
			//Todo: le repasser a 0 quand le mvm est terminer
			EnvoiOcteEtat();
			//SuiviTableau();
		}
		break;
	case CM_COMPPRESET:
		//Obsolet
		EnvoiOcteEtat();
		icommande->stab.CompteurEnvoiPoints = 0x0000;
		break;
	default:
		EnvoiOcteEtat();
		DebugerPrint("DispatchNewCommance : code numero ");
		DebugerPrintNum(msg->Commande, 16);
		DebugerPrint(
				" Erreur dans la transmition , ce code n'est pas implementer\n");
		break;
	}
}
void DispatchNextCommande(CAN_Msg *msg) {
	int16_t tmp = 0x0000;
	switch (icommande->oldcommande) {
	/*case CM_RESET:
	 RAZ(msg);
	 break;*/
	case CM_TRAPEZE:
		if (msg->size >= 4) {
			tmp = msg->message[0];
			tmp <<= 8;
			tmp |= msg->message[1];
			icommande->tmvm.ConsPosition += tmp * 0x100;
			icommande->tmvm.ConsPosition += msg->message[3] * 0x1000000; //Todo: demandez a franc pourquoi il multiplie par 1677216 ... = = je comprend pas pop
			if (TestState(icommande, TEST_TRAPEZE)) {
				//Todo: Implementer le mouvement en trapezze a 90 degre ici
				SendInfoCommande();
			} else {
				icommande->info.Octeetat |= MASK_POS_INACESSIBLE;
				EnvoiOcteEtat();
			}
		} else
			DebugerPrint(
					"DispatchNextCommande: Commande 0x05 ,erreur : il manque de l'information \n");

		break;
	default:
		EnvoiOcteEtat();
		DebugerPrint("DispatchNewCommance : code numero ");
		DebugerPrintNum(msg->Commande, 16);
		DebugerPrint(
				" Erreur dans la transmition , ce code n'est pas implementer\n");
		break;
	}

}
/*
 * Afin d'economiser des allocaions nous appellons une autre fonction.
 * allocations interditent dans les switches
 */
void AjoutPoint(CAN_Msg *msg) {

	if (msg->size >= 0x04)
		icommande->set_next(icommande,
				Convert8to32(msg->message[0], msg->message[1], msg->message[2],
						msg->message[3]));

	if (msg->size == 0x08)
		icommande->set_next(icommande,
				Convert8to32(msg->message[4], msg->message[5], msg->message[6],
						msg->message[7]));

}
int32_t Convert8to32(int8_t t0, int8_t t1, int8_t t2, int8_t t3) {
	int32_t val = 0x00000000;
	val |= ((int32_t) t0 << 24); //Todo : faire un test pour verifier la coherance du resultat
	val |= ((int32_t) t1 << 16);
	val |= ((int32_t) t2 << 8);
	val |= (int32_t) t0;
	return (int32_t) val;
}
void LectureState(uint8_t type) {
//Todo : rajouter tout les recuperations de parametres capteurs
	//Remplissage de la trame avec les donnes demander par le numeros de message
	stdrsp->message[1] = icommande->info.Octeetat;
	stdrsp->size = 0x04;
	switch (type) {
	case 0: // Lecture de CodeurOrigine
		Put16((stdrsp->message + 2), icommande->smvm.CodeurOrigine);
		break;
	case 1: // Lecture de la position codeur
		Put16((stdrsp->message + 2), icommande->smvm.Poscodeur);

		break;
	case 2: // Lecture de la position absolue
		//Todo : attention les trames sont envoyer a l'envert dans le code PIC de base!!
		Put32((stdrsp->message + 2), icommande->smvm.PosAbscodeur);
		stdrsp->size = 0x06;
		break;
	case 3: // Lecture du courant max mesur�
		Put16((stdrsp->message + 2), icommande->cap.CourantMaxMeusure);
		break;
	case 4: // Lecture du courant instantan�
		Put16((stdrsp->message + 2), icommande->cap.CourantMeusure);
		break;
	case 5: // Lecture du couple � l'initialisation
		Put16((stdrsp->message + 2), icommande->cap.CoupleMaxInit);
		break;
	case 6: // Lecture de la temp�rature moteur
		Put16((stdrsp->message + 2), icommande->cap.TempMot);
		break;
	case 7: // Envoi d'un nombre particulier pour valider la transmission
		Put32((stdrsp->message + 2), 0x89ABCDEF);
		stdrsp->size = 0x06;
		break;
	case 8: // lecture de la tension
		Put16((stdrsp->message + 2), icommande->cap.TensionCommande);
		break;
	case 9: // Lecture de la phase
		Put16((stdrsp->message + 2), icommande->smvm.Phase);
		break;
	case 0x0A: // Lecture du r�glage UMax = courant max
		Put16((stdrsp->message + 2), GetUMax());
		break;
	case 0x0B: // Lecture du r�glage UMin = courant min
		Put16((stdrsp->message + 2), GetUMin());
		break;
	case 0x0C: // Lecture du r�glage UTableau
		Put16((stdrsp->message + 2), GetUTableau());
		break;
	case 0x0D: // Lecture du r�glage UStat
		Put16((stdrsp->message + 2), GetUStat());
		break;
	case 0x0E: // Lecture du r�glage UInit
		Put16((stdrsp->message + 2), GetUInit());
		break;
	case 0x0F: // Lecture du r�glage UModeVitesse
		Put16((stdrsp->message + 2), GetUModeVitesse());
		break;
	case 0x10: // Lecture du r�glage PasVis
		Put16((stdrsp->message + 2), GetPasVis());

		break;
	case 0x11: // Lecture de NiveauAsstPos
		stdrsp->message[3] = GetNiveauAsstPos();
		stdrsp->message[2] = 0x00;

		break;
	case 0x12: // Lecture de Contact
		stdrsp->message[3] = icommande->cap.Contact;
		stdrsp->message[2] = 0x00;
		break;
	case 0x13: // Lecture de SensInit
		Put16((stdrsp->message + 2), icommande->smvm.SensInit);

		break;
	case 0x14: // Lecture de PosMax
		Put32((stdrsp->message + 2), GetMaxPosition());
		stdrsp->size = 0x06;
		break;
	case 0x15: // Lecture de PosMin
		Put32((stdrsp->message + 2), GetMinPosition());
		stdrsp->size = 0x06;
		break;
	case 0x16: // Lecture de PosOrigine
		Put32((stdrsp->message + 2), GetPosOrigine());
		stdrsp->size = 0x06;
		break;
	case 0x17: // Lecture de PosAbsMoteur();
		RecupePosAbsSSICodeur();
		DebugerDelay(500); //le temps que le message nous soient transmit
		Put16((stdrsp->message + 2), icommande->smvm.PosAbscodeur);
		break;
	case 0x18: // Lecture de ToleranceMvt
		stdrsp->message[3] = GetToleranceMvt();
		stdrsp->message[2] = 0x00; // afin de garder la trame de la meme longeur
		break;
	case 0x19: // Lecture de CoeffTensionTableau
		Put16((stdrsp->message + 2), icommande->stab.LimiteTempsTableau);
	case 0x1A: // Lecture de LimiteTempsTableau
		Put16((stdrsp->message + 2), icommande->stab.CoeffTensionTableau);
		break;
	case 0x1B: // Lecture de Version ( Version du logiciel)
		Put16((stdrsp->message + 2), icommande->info.Version);
		break;
	case 0x1C: // Lecture de AvecInfoFinMvt
		Put16((stdrsp->message + 2), (0x0000 | icommande->info.AvecInfoFinMvt));
		break;
	case 0x1D: // Lecture de DecalageDecrochage
		Put16((stdrsp->message + 2), GetDecalageDecrochage());
		break;
	case 0x1E: // Lecture de DecalageDecrochageArret
		Put16((stdrsp->message + 2), GetDecalageDecrochageArret());
		break;
	case 0x20:
		Put16((stdrsp->message + 2), 0x0000); //Todo: demander a franc ce que se signifie , pour le moment TypeDefault n'est pas implementer
		Put16((stdrsp->message + 4), icommande->stab.CompteurEnvoiPoints);
		Put16((stdrsp->message + 6), (0x0000 | icommande->tab.Poslec));
		stdrsp->size = 0x08;
		break;
	case 0x21:
		Put16((stdrsp->message + 2), icommande->smvm.PtCodeurTour);
		break;
	case 0x22: //Renvoie les point precedament consommer
		stdrsp->message[2] = icommande->tab.Poslec;
		stdrsp->message[3] = icommande->tab.Poslec + icommande->tab.Bufwrt;
		Put32((stdrsp->message + 4),
				icommande->tab.tabpnt[icommande->tab.Poslec]);
		icommande->stab.CompteurEnvoiPoints += 2;
		stdrsp->size = 0x08;
		break;
	case 0x23:
		Put16((stdrsp->message + 6),
				icommande->tab.tabpnt[icommande->tab.Poslec - 2] & 0xFFFF);
		Put32((stdrsp->message + 2),
				icommande->tab.tabpnt[icommande->tab.Poslec - 1]);
		icommande->stab.CompteurEnvoiPoints += 6;
		stdrsp->size = 0x08;
		break;
	case 0x24:
		Put16((stdrsp->message + 2),
				(icommande->tab.tabpnt[icommande->tab.Poslec - 2] & 0xFFFF0000)
						>> 16);
		Put32((stdrsp->message + 4),
				icommande->tab.tabpnt[icommande->tab.Poslec - 3]);
		icommande->stab.CompteurEnvoiPoints += 6;
		stdrsp->size = 0x08;
		break;
	case 0x25:
		Put32((stdrsp->message + 2), icommande->smvm.Poscodeur);
		Put16((stdrsp->message + 6), icommande->smvm.Decalage);
		stdrsp->size = 0x08;
		break;
	}
	//envoie des donnes
	SendCommande(stdrsp, STANDARD);

}
void EnvoiOcteEtat(void) {
	stdrsp->message[0] = icommande->info.Octeetat;
	Put32((stdrsp->message + 1), icommande->smvm.PosAbscodeur);
	stdrsp->message[5] = icommande->cap.Etatcapt;
	stdrsp->message[6] = icommande->cap.TensionCommande >> 7;
	stdrsp->size = 0x07;
	SendCommande(stdrsp, STANDARD);

}
void Put16(int8_t *m, int16_t i) {
	(*m++) = (0xFF00 & i) >> 8;
	(*m++) = 0xFF & i;
}
void Put32(int8_t *m, int32_t i) {
	(*m++) = (0xFF000000 & i) >> 24;
	(*m++) = (0xFF0000 & i) >> 16;
	(*m++) = (0xFF00 & i) >> 8;
	(*m++) = 0xFF & i;
}
void SendInfoCommande(void) {
	if (icommande->info.AvecInfoFinMvt) {
		EnvoiOcteEtat();
	}
}
/*
 * Reset du moteur
 */
void RAZ(CAN_Msg *msg) {
	if (msg->size >= 1) {
		icommande->resettabs(icommande);
		switch (msg->message[0]) {
		case 0x01: /*AlignePhase();
		 TensionCommande = UStat;
		 MarcheMoteur();
		 ValCodeurInit = POSCNT;
		 ConsignePosition(0);
		 PositionAbsolue = PosOrigine;*/
			break;
		case 0x00:
#ifdef AvecInitContact //declarer dans configuration.h
			//InitContact();
			if (!(icommande->info.Arreturgence)) {
				//InitPhase();
				//desider de modifier le code et ne rien faire en cas d'arret d'urgence
				//ConsignePosition(PosOrigine);
			} else
				DebugerPrint("RAZ :nous sommes en arret d'urgence");

#endif
			break;
		case 0x02:
		 //Recherche de la position absolu lecture du codeur
		RecupePosAbsSSICodeur();
		EnvoiOcteEtat();
		break;
		case 0x03:
			if (msg->size == 0x08) {
				if ((msg->message[1] == 0x61) && (msg->message[2] == 0x03)
						&& (msg->message[3] == 0x61)
						&& (msg->message[4] == 0x03)
						&& (msg->message[5] == 0x61)
						&& (msg->message[6] == 0x03)
						&& (msg->message[7] == 0x61)) {
					DebugerReset();
				}
			}
			break;
		default:
			DebugerPrint("RAZ: Erreur, code inconu\n");
			break;
		}
		icommande->resettabs(icommande);
		icommande->info.Octeetat = 0 ;
		icommande->info.Octeetat |= MASK_MEMOIR_DISPO | MASK_POS_INDETERMINER;
		EnvoiOcteEtat();
	}
}
/*
 * Mise en marche du moteur
 */
void MarcheMoteur(void) {
	//TensionCommande = UStat; // on remet la tension pr�vue pour un arr�t en position
	icommande->tab.Bufwrt = 0xFF; // Comme c'est un char, on ne peut pas mettre 256, Nombre de points dispo est en fait le nombre de points disponibles - 1
	icommande->tab.Poslec = 0x00;
	icommande->info.Octeetat |= 0b01001100; // On annule �ventuellement l'alarme de d�crochage du moteur ou le d�faut de trajectoire ou le d�faut de limites course
	//TypeDefaut = 0; Todo:Comprendre a quoi ceci sert
	/*AlignePhase();
	 MarcheMoteur();
	 ConsignePosition(PositionAbsolue);
	 EnvoiOctetEtat();*/
}
/*
 * reupere la commande d'envoir de mc protocole
 */
void InitMCCommance(void (*f)(void *)) {
	if (isinit) {
		MC_SendCommande = (*f);
		isinit++;
	}
}
/*
 * Envoi des commandes venant du MC protocole, speciale car il permet de
 * decouper en troncons de 8 octees.
 */
void SendMCInterface(uint8_t code, uint8_t *buf, uint8_t sizemone) {
	if (sizemone && isinit) {
		uint8_t i = 0;
		CAN_Msg *newmsg = NewCAN_Msg(((sizemone < 8) ? sizemone : 7));
		newmsg->message[i] = code;
		sizemone++;
		goto JUMPIN;
		while (sizemone) {
			for (i = 0; (i < 8) && (sizemone); sizemone--) {
				newmsg->message[i] = (*buf++);
				JUMPIN: i++;
			}
			newmsg->size = i;
			SendCommande(newmsg, MCPROTOCOL);
		}
		DestroyCAN_Msg(newmsg);
	}
}
void RenvoiPoint(uint8_t indexpoint) {
	//on utilise standard response
	int32_t point = icommande->tab.tabpnt[indexpoint];
	stdrsp->message[3] = point;
	point >> 8;
	stdrsp->message[2] = point;
	point >> 8;
	stdrsp->message[1] = point;
	point >> 8;
	stdrsp->message[0] = point;
	point >> 8;
	stdrsp->size = 0x04;
	SendCommande(stdrsp,STANDARD);
}



















