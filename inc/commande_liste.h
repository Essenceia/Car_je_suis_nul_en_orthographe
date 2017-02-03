/*
 * commande_liste.h
 *
 *  Created on: Feb 2, 2017
 *      Author: pooki
 *
 *  On fait une liste des numeros de message et de commande qui sont reconnu dans le code afin de facillement
 *  les reconfigurer si ceux identifier dans le code ne correspondent pas
 */

#ifndef INC_COMMANDE_LISTE_H_
#define INC_COMMANDE_LISTE_H_
/*
 * Numero de message, correspondent au 3 Most Significant Bits du SLI et aux 3 LSB du Standand Identifier
 * dans le trame can : ranger dans NoMessage de la structure CAN_Msg
 *
 * Il ne peut prendre des valheurs que entre 000 et 111 , c'est a dire 0 et 7
 */
#define NM_NEW                 ((uint8_t)0x01)        /*nouveux message, independante d'une commande precedante*/
#define NM_NEXT                ((uint8_t)0x02)        /*message dependant du contenu du message precedant*/
#define NM_MCP                 ((uint8_t)0x03)        /* message s'adressant au motor control protocol */
#define NM_POINT               ((uint8_t)0x05)        /* nous allons envoyer des points, auqune commande a attendre, tout les octee de la trame seront occuper par de l'information*/
#define NM_LED                 ((uint8_t)0x07)        /* obsolet a mon gout mais utiliser dans le code PIC , commande speciale pour l'allumage des leds */

/*
 * Code Commande , stoquer dans l'octee 0 de la trame
 */
#define CM_LECTURE_OCTEE_ETAT  ((uint8_t)0x21)
#define CM_LECTURE_STATE       ((uint8_t)0x31)        /* Interoge un etat du moteur , le parametre particulier a interoger sera defini dans la trame a la position 1 -> message[0] */
#define CM_LECTURE_TRAME       ((uint8_t)0x32)        /* Lecture de la valheur d'un point en memoire */
#define CM_COMAND_VITESSE      ((uint8_t)0x41)
#define CM_TRAPEZE             ((uint8_t)0x52)
#define CM_ARRET               ((uint8_t)0x71)
#define CM_REVEIL              ((uint8_t)0x81)
#define CM_RESET               ((uint8_t)0x61)        /* Reset des parametres a zero */
#define CM_DEPART              ((uint8_t)0xA1)        /* Lance le mouvement apres avoir recu des points a l'unitee */
#define CM_REGLAGE             ((uint8_t)0xB1)
#define CM_COMPPRESET          ((uint8_t)0xF1)        /* Reset compteur point a 0 et renvoie l'octee d'etat*/

#endif
