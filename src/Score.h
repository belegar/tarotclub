/*=============================================================================
 * Tarot Club - Score.h
 *=============================================================================
 * Calcule les scores en fonction des paramètres du jeu
 *=============================================================================
 * Tarot Club est un jeu de Tarot français
 * Copyright (C) 2003-2005  Anthony Rabine
 * anthony@ooso.org
 * http://tarotclub.ooso.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *=============================================================================
 */

#ifndef _SCORE_H
#define _SCORE_H

// Game includes
#include "Card.h"
#include "Deck.h"
#include "../defines.h"

/*****************************************************************************/
class Score
{

private:
   
   Place plis[78];      // indique quelle carte a été remportée par quel joueur
   int   scores[5];     // les totaux des cinq joueurs (maxi)
   bool  gagne;         // vrai si le coup est gagné par l'attaque
   Place carteExcuse;   // A qui appartient l'excuse

   // Informations sur le petit au bout
   bool  petitAuBout;   // vrai si le petit a été mené au bout
   bool  petitAttaque;  // vrai si le petit au bout a été remporté par l'attaque
   // Informations sur le Chelem
   bool  chelemDeclare; // vrai si un Chelem a été déclaré
   bool  chelemRealise; // vrai si le Chelem a été réalisé
   bool  chelemDefense; // vrai si le Chelem a été réalisé par la défense
   // Informations sur les poignées
   bool  poigneeDefense;   // vrai si la défense a déclaré une poignée
   bool  poigneeAttaque;   // vrai si l'attaque a déclaré une poignée
   Poignee  typePoigneeD;  // (Défense) simple, double ou triple
   Poignee  typePoigneeA;  // (Attaque) simple, double ou triple

   // Informations calculées à partir du jeu
   ScoreInfos  score_inf;

public:
   Score();

   void  init();
   void  reset();
   void  setPli( int i, Place p );
   void  calcul( Deck &mainDeck, Deck &deckChien, GameInfos *infos );
   
   // Mutateurs
   void  setChelemDeclare( bool );
   void  setExcuse( Place p );

   // Accesseurs
   Place    getExcuse();
   int      getScore( int i );
   ScoreInfos  *getScoreInfos();

};


#endif // _SCORE_H

//=============================================================================
// End of file Score.h
//=============================================================================
