/*=============================================================================
 * TarotClub - Deck.h
 *=============================================================================
 * A deck composed of a (dynamic) list of cards.
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

#ifndef _DECK_H
#define _DECK_H

// Qt includes
#include <QList>

// Game includes
#include "Card.h"
#include "defines.h"
#include "TarotDeck.h"
#include "ByteStreamReader.h"
#include "ByteStreamWriter.h"

/*****************************************************************************/
class Deck : public QList<Card *>
{
public:
    class Statistics
    {

    public:
        Statistics() {}

        int   nbCards;

        int   trumps;  // nombres d'atouts , en comptant les bouts et l'excuse
        int   oudlers;   // 0, 1, 2 ou 3
        int   majorTrumps; // atouts >= 15

        int   kings;
        int   queens;
        int   knights;
        int   jacks;

        int   weddings;   // nombre de mariages dans la main
        int   longSuits;
        int   cuts;     // aucune carte dans une couleur
        int   singletons; // une seule carte dans une couleur
        int   sequences;  // cartes qui se suivent (au moins 5 cartes pour être comptées)

        int   clubs;
        int   spades;
        int   hearts;
        int   diamonds;

        bool  littleTrump;
        bool  bigTrump;
        bool  fool;

        float points;

        void Reset();
    };

    Deck();

    // Helpers
    void AnalyzeTrumps(Statistics &stats);
    void AnalyzeSuits(Statistics &stats);
    void Shuffle(int seed);
    void Sort(const std::string &order = "TCHDS");
    bool HasCard(Card *c);
    bool HasOneOfTrump();
    bool HasFool();
    Card *HighestTrump();
    Card *HighestSuit();

    // Getters
    Card *GetCardByName(const std::string &i_name);
    std::string GetCardList();
    Team GetOwner();

    // Setters
    void SetOwner(Team o);
    int SetCards(const std::string &cards);

    friend ByteStreamWriter &operator<<(ByteStreamWriter &out, const Deck &deck)
    {
        std::string cards = deck.GetCardList();
        out << cards;
        return out;
    }

    friend ByteStreamReader &operator>>(ByteStreamReader &s, Deck &l)
    {
        std::string cards;
        s >> cards;
        l.SetCards(cards);
        return s;
    }

private:
    static bool LessThanCards(Card *carte1, Card *carte2);

    /**
     * @brief This variable can be use to store a deck owner
     * information, tricks won for example
     */
    Team owner;
};

#endif // _DECK_H

//=============================================================================
// End of file Deck.h
//=============================================================================
