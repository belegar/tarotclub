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
#include "Game.h"
#include "Score.h"

/*****************************************************************************/
class Deal
{
public:
    Deal();

    /**
     * @brief Initialize all the variables to default or zero value
     */
    void Initialize();

    /**
     * @brief Reset variables for a new deal
     */
    void NewDeal();

    /**
     * @brief Calculate
     *
     * Call this methdo at the end of a deal to calculate the score
     *
     * @param info
     */
    void Calculate(Game &info);

    // Mutateurs
    void SetHandle(Deck &handle, Team team);
    void SetPoints(const Game &infos);
    void SetScore(const Score &score);
    void SetDog(Deck &dog);
    void SetTrick(Deck &trick, int turn);
    void SetDogOwner(Team team);

    // Accesseurs
    Deck GetTrick(int turn);
    int  GetTotalPoints(Place p);
    Score  &GetScore();
    QList<Place> GetPodium();

private:

#ifndef QT_NO_DEBUG
    void GenerateEndDealLog();
#endif

    void AnalyzeGame(Game &info);
    void CalculateScore(Game &info);

    int GetHandlePoints(Handle h);

    // We store played cards to count points
    Deck dogDeck;
    Deck attackHandleDeck;
    Deck defenseHandleDeck;
    Deck tricks[24];    // 24 tricks max with 3 players

    // Bonus: little oudler (1 of trump) played at the last trick
    bool littleEndianOudler;     // true if the little oudler has been played at the last trick
    Team littleEndianOwner;    // who has won the bonus

    // Bonus: Chelem
    bool slamDone;  // true if the slam has been successfully done
    Team slamOwner; // the defense can also perform a slam if everything goes wrong

    // Informations calculées à partir du jeu
    Score  score;

    // scores of previous deals
    int dealCounter;
    int scores[MAX_ROUNDS][5];   // score of each turn players, 5 players max
};


#endif // _DEAL_H

//=============================================================================
// End of file Deal.h
//=============================================================================
