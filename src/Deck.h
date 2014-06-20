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

#include <list>

// Game includes
#include "Card.h"
#include "Common.h"
#include "TarotDeck.h"
#include "ByteStreamReader.h"
#include "ByteStreamWriter.h"

/*****************************************************************************/
class Deck
{
public:
    class Statistics
    {
    public:
        Statistics() {}

        std::uint32_t   nbCards;

        std::uint32_t   trumps;  // nombres d'atouts , en comptant les bouts et l'excuse
        std::uint32_t   oudlers;   // 0, 1, 2 ou 3
        std::uint32_t   majorTrumps; // atouts >= 15

        std::uint32_t   kings;
        std::uint32_t   queens;
        std::uint32_t   knights;
        std::uint32_t   jacks;

        std::uint32_t   weddings;   // nombre de mariages dans la main
        std::uint32_t   longSuits;
        std::uint32_t   cuts;     // aucune carte dans une couleur
        std::uint32_t   singletons; // une seule carte dans une couleur
        std::uint32_t   sequences;  // cartes qui se suivent (au moins 5 cartes pour être comptées)

        std::uint32_t   clubs;
        std::uint32_t   spades;
        std::uint32_t   hearts;
        std::uint32_t   diamonds;

        bool  littleTrump;
        bool  bigTrump;
        bool  fool;

        float points;

        void Reset();
    };

    Deck();

    // STL-compatible iterator types
    typedef std::list<Card *>::iterator Iterator;
    typedef std::list<Card *>::const_iterator ConstIterator;

    // STL-compatible begin/end functions for iterating over the deck cards
    inline Iterator Begin()
    {
        return mDeck.begin();
    }
    inline ConstIterator Begin() const
    {
        return mDeck.begin();
    }
    inline Iterator End()
    {
        return mDeck.end();
    }
    inline ConstIterator End() const
    {
        return mDeck.end();
    }

    // Raw deck management
    inline std::uint32_t Size() const { return mDeck.size(); }
    inline void Clear() { mDeck.clear(); }
    inline void Append(Card *c) { mDeck.push_back(c); }
    void Append(const Deck &deck);
    Deck Mid(std::uint32_t from_pos);
    Deck Mid(std::uint32_t from_pos, std::uint32_t size);
    void Remove(Card *c);
    std::uint32_t Count(Card *c) const;
    bool HasCard(Card *c) const;

    // Helpers
    void AnalyzeTrumps(Statistics &stats) const;
    void AnalyzeSuits(Statistics &stats);
    void Shuffle(int seed);
    void Sort(const std::string &order = "TCHDS");
    bool HasOneOfTrump() const;
    bool HasFool() const;
    Card *HighestTrump() const;
    Card *HighestSuit() const;

    // Getters
    Card *GetCardByName(const std::string &i_name);
    std::string GetCardList() const;
    Team GetOwner();

    // Setters
    void SetOwner(Team o);
    std::uint8_t SetCards(const std::string &cards);

    friend ByteStreamWriter &operator<<(ByteStreamWriter &out, const Deck &deck)
    {
        std::string cards = deck.GetCardList();
        out << cards;
        return out;
    }

    friend ByteStreamReader &operator>>(ByteStreamReader &s, Deck &deck)
    {
        std::string cards;
        s >> cards;
        deck.SetCards(cards);
        return s;
    }

    Deck operator = (const Deck &l)
    {
        mDeck = l.mDeck;
        return *this;
    }

    Deck operator += (const Deck &l)
    {
        this->Append(l);
        return *this;
    }

    Deck operator + (Deck &l) const
    {
        Deck d;
        d.Append(*this);
        d.Append(l);
        return d;
    }

private:
    static bool LessThanCards(Card *carte1, Card *carte2);

    /**
     * @brief This variable can be use to store a deck owner
     * information, tricks won for example
     */
    Team owner;
    std::list<Card *> mDeck; //!< Container to store the cards
};

#endif // _DECK_H

//=============================================================================
// End of file Deck.h
//=============================================================================
