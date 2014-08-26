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

#ifndef DECK_H
#define DECK_H

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

    /**
     * @brief The Comparator class
     *
     * Custom comparator helper class to allow various forms of sorting
     */
    class Sorter
    {
    public:
        Sorter(const std::string &sorting)
        {
            std::string order;
            if (sorting.size() == 5)
            {
                order = sorting;
            }
            else
            {
                order = "TCSDH";
            }

            // Generate a weight for each suit
            for (int i = 0; i < 5; i++)
            {
                std::string letter;
                letter.push_back(order[4 - i]);
                mWeight[Card::ToSuit(letter)] = 100U * i;
            }
        }

        bool operator() (Card *c1, Card *c2)
        {
            bool result;

            std::uint16_t id1 = GetWeight(c1);
            std::uint16_t id2 = GetWeight(c2);
            result = id1 > id2;

            return result;
        }

        std::uint16_t GetWeight(Card *c)
        {
            return(mWeight[c->GetSuit()] + c->GetValue());
        }

    private:
        std::uint16_t mWeight[5];
    };

    /**
     * @brief The Statistics class
     *
     * Helper class to store various deck statistics
     */
    class Statistics
    {
    public:
        Statistics()
        {
            Reset();
        }

        std::uint8_t   nbCards;

        std::uint8_t   trumps;      ///< Total of trumps, including oudlers
        std::uint8_t   oudlers;     ///< 0, 1, 2 or 3
        std::uint8_t   majorTrumps; ///< trumps >= 15

        std::uint8_t   kings;
        std::uint8_t   queens;
        std::uint8_t   knights;
        std::uint8_t   jacks;

        std::uint8_t   weddings;    ///< Number of weddings (king + queen)
        std::uint8_t   longSuits;   ///< Suit with more than 5 cards
        std::uint8_t   cuts;        ///< No cards in a suit
        std::uint8_t   singletons;  ///< Only one card in a suit
        std::uint8_t   sequences;   ///< At least 5 cards in a row

        std::uint8_t   suits[4];    ///< Number of cards in each suit

        bool  littleTrump;
        bool  bigTrump;
        bool  fool;

        float points;

        void Reset();
    };

    Deck();
    Deck(const std::string &cards);

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
    inline std::uint32_t Size() const
    {
        return mDeck.size();
    }
    inline void Clear()
    {
        mDeck.clear();
    }
    inline void Append(Card *c)
    {
        mDeck.push_back(c);
    }
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
    void Sort(const std::string &order);
    bool HasOneOfTrump() const;
    bool HasFool() const;
    Card *HighestTrump() const;
    Card *HighestSuit() const;
    void CreateTarotDeck();
    std::uint32_t RemoveDuplicates(const Deck &deck);

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

    Deck &operator = (const Deck &d)
    {
        mDeck = d.mDeck;
        mOwner = d.mOwner;
        return *this;
    }

    Deck operator += (const Deck &d)
    {
        this->Append(d);
        return *this;
    }

    Deck operator + (Deck &d) const
    {
        Deck deck;
        deck.Append(*this);
        deck.Append(d);
        return deck;
    }

private:
    /**
     * @brief This variable can be use to store a deck owner
     * information, tricks won for example
     */
    Team mOwner;
    std::list<Card *> mDeck; //!< Container to store the cards
};

#endif // DECK_H

//=============================================================================
// End of file Deck.h
//=============================================================================
