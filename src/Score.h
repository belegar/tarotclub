/*=============================================================================
 * TarotClub - Score.h
 *=============================================================================
 * Calcule les scores en fonction des paramètres du jeu
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * TarotClub is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotClub is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */

#ifndef _SCORE_H
#define _SCORE_H

// Game includes
#include <QList>
#include "Card.h"
#include "Deck.h"
#include "defines.h"

/*****************************************************************************/
class Score
{

private:

   Place plis[78];      // indique quelle carte a été remportée par quel joueur
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

   // scores of each turn
   int turn;
   int scores[MAX_ROUNDS][NB_PLAYERS];     // score of each turn players

public:
   Score();

   void init();
   void reset();
   void setPli( int i, Place p );
   void calcul( Deck &mainDeck, Deck &deckChien, GameInfos &infos );

   // Mutateurs
   void setChelemDeclare( bool );
   void setExcuse( Place p );
   void setPoigneeDefense(Poignee p);
   void setPoigneeAttaque(Poignee p);
   void setPoints(const GameInfos &infos);
   void setScoreInfos(const ScoreInfos &inf);

   // Accesseurs
   Place getExcuse();
   Place getPli(int i);
   int  getTotalPoints(Place p);
   ScoreInfos  *getScoreInfos();
   QList<Place> getPodium();
};


#endif // _SCORE_H

//=============================================================================
// End of file Score.h
//=============================================================================
