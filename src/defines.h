/*=============================================================================
 * TarotClub - defines.h
 *=============================================================================
 * Global types
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * This file must be used under the terms of the CeCILL.
 * This source file is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at
 * http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
 *
 *=============================================================================
 */

#ifndef _DEFINES_H
#define _DEFINES_H

#include <QtCore>


/*****************************************************************************/
// Game definitions

#define TAROT_VERSION   "2.1.0a1"
#define TAROT_TITRE     "TarotClub"
#define TAROT_VNAME     "GLaDOS"

#define NB_LANGUES      2
#define QT_STREAMVER    QDataStream::Qt_4_4
#define MAX_ROUNDS      5
#define NB_PLAYERS      4
#define NB_HAND_CARDS   18
#define NB_CHIEN_CARDS  6

#define TEXT_BOX_WIDTH  115
#define TEXT_BOX_HEIGHT 30

/*****************************************************************************/
enum CardColor    { PIC=0, COEUR=1, TREFLE=2, CARREAU=3, NO_COLOR=10 };
enum CardType     { CARTE=0, ATOUT=1, EXCUSE=2 };
enum Place        { SUD=0, EST=1, NORD=2, OUEST=3, BROADCAST=462, HYPERSPACE=0xFFFF };
enum PlaceBot     { BOT_EAST=0, BOT_NORTH=1, BOT_WEST=2 };
enum Sequence {
   VIDE,
   DISTRIBUTION,
   ENCHERES,
   MONTRE_CHIEN,
   WAIT_CHIEN,
   CHIEN,
   BUILD_POIGNEE,
   SHOW_POIGNEE,
   GAME,
   SEQ_WAIT_PLI,
   SEQ_WAIT_PLAYER
};
enum Contrat      { PASSE=0, PRISE=1, GARDE=2, GARDE_SANS=3, GARDE_CONTRE=4 };
enum Filter       { AUCUN, JEU };
enum Poignee      { P_SANS, P_SIMPLE, P_DOUBLE, P_TRIPLE };
enum SexType      { MALE, FEMALE };
enum CursorType   { FORBIDDEN, ARROW };
enum GameType     { LOC_ONEDEAL, LOC_TOURNAMENT, NET_GAME_SERVER, NET_GAME_CLIENT };
enum DealType     { RANDOM_DEAL, CUSTOM_DEAL, NUMBERED_DEAL };

/*****************************************************************************/
// client -> server
#define NET_CLIENT_MSG        0x10
#define NET_CLIENT_INFOS      0x11
#define NET_CLIENT_ENCHERE    0x13
#define NET_CLIENT_VU_CHIEN   0x14  // tous les clients doivent prévenir le serveur qu'ils ont bien vus le chien
#define NET_CLIENT_CHIEN      0x15
#define NET_CLIENT_CARTE      0x16
#define NET_CLIENT_VU_PLI     0x17  // tous les clients doivent prévenir le serveur qu'ils ont bien vus le pli
#define NET_CLIENT_POIGNEE    0x18  // un client veut déclarer une poignée
#define NET_CLIENT_READY      0x19
#define NET_CLIENT_ERROR      0x20

// server -> client
#define NET_MESSAGE           0x70
#define NET_IDENTIFICATION    0x71
#define NET_LISTE_JOUEURS     0x72
#define NET_RECEPTION_CARTES  0x73
#define NET_SELECTION_JOUEUR  0x74
#define NET_DEMANDE_ENCHERE   0x75
#define NET_ENCHERE_JOUEUR    0x76
#define NET_MONTRE_CHIEN      0x77
#define NET_FAIT_CHIEN        0x78
#define NET_DEPART_DONNE      0x79
#define NET_JOUE_CARTE        0x80
#define NET_MONTRE_CARTE      0x81
#define NET_SERVER_WAIT_PLI   0x82
#define NET_FIN_DONNE         0x83
#define NET_SERVER_REDIST     0x84  // tous les joueurs ont passé, il faut relancer une distribution
#define NET_SHOW_POIGNEE      0x85  // montre une poignée à tous les joueurs
#define NET_FIN_PARTIE        0x86  // fin du tournoi

/*****************************************************************************/

typedef struct {
   Place    preneur;       // who has taken
   Contrat  contrat;       // taker's contract
   int      gameCounter;   // game counter, first player of the game will start at 1
} GameInfos;

typedef struct {
   float attaque;
   float defense;
   int   bouts;
   int   pointsAFaire;
   int   difference;
   int   points_petit_au_bout;
   int   multiplicateur;
   int   points_poignee;
   int   points_chelem;
   int   points_defense;

} ScoreInfos;

/*****************************************************************************/


#endif // _DEFINES_H

//=============================================================================
// End of file defines.h
//=============================================================================
