/*=============================================================================
 * TarotClub - Deal.h
 *=============================================================================
 * This class stores a complete deal history and calculate final points
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

#ifndef _DEAL_H
#define _DEAL_H

// Game includes
#include <QList>
#include "Card.h"
#include "Deck.h"
#include "defines.h"
#include "GameState.h"
#include "Score.h"
#include "Trick.h"

/*****************************************************************************/
class Deal
{

private:

    Team winner;        // winner of the deal
    Place foolOwner;

    // We store played cards to count points
    Deck dog;
    Deck attackHandle;
    Deck defenseHandle;
    Deck tricks[24];    // 24 tricks max with 3 players

    // Bonus: little oudler (1 of trump) played at the last trick
    bool littleEndianOudler;     // true if the little oudler has been played at the last trick
    Team littleEndianOwner;    // who has won the bonus

    // Bonus: Chelem
    bool slamDone;  // true if the slam has been successfully done
    Team slamOwner; // the defense can also perform a slam if everything goes wrong

    // Bonus: Handle
    bool  poigneeDefense;   // vrai si la défense a déclaré une poignée
    bool  poigneeAttaque;   // vrai si l'attaque a déclaré une poignée
    Poignee  typePoigneeD;  // (Défense) simple, double ou triple
    Poignee  typePoigneeA;  // (Attaque) simple, double ou triple

    // Informations calculées à partir du jeu
    Score  score;

    // scores of previous deals
    int deal;
    int scores[MAX_ROUNDS][5];   // score of each turn players, 5 players max

public:
    Deal();

    void Initialize();
    void reset();
    void setPli(int i, Place p);
    void calcul(Deck &mainDeck, Deck &deckChien, GameState &info);

    // Mutateurs
    void setChelemDeclare(bool);
    void setExcuse(Place p);
    void setPoigneeDefense(Poignee p);
    void setPoigneeAttaque(Poignee p);
    void setPoints(const GameState &infos);
    void SetScoreInfo(const ScoreInfo &inf);

    // Accesseurs
    Place getExcuse();
    Place getPli(int i);
    int  getTotalPoints(Place p);
    ScoreInfo  &GetScoreInfo();
    QList<Place> getPodium();
};


#endif // _DEAL_H

//=============================================================================
// End of file Deal.h
//=============================================================================
