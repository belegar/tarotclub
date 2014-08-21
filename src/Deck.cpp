/*=============================================================================
 * TarotClub - Deck.cpp
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

#include <algorithm>
#include <random>
#include <array>
#include "Deck.h"


/*****************************************************************************/
Deck::Deck()
    : mOwner(NO_TEAM)
{

}
/*****************************************************************************/
Deck::Deck(const std::string &cards)
    : mOwner(NO_TEAM)
{
    SetCards(cards);
}
/*****************************************************************************/
void Deck::Append(const Deck &deck)
{
    for (Deck::ConstIterator i = deck.Begin(); i != deck.End(); ++i)
    {
        Append((*i));
    }
}
/*****************************************************************************/
/**
 * @brief Deck::Mid
 * Creates a temporary deck contains all the cards from pos to the end of the
 * deck.
 *
 * @param from_pos Starting position
 * @return the new deck
 */
Deck Deck::Mid(std::uint32_t from_pos)
{
    return Mid(from_pos, Size() - from_pos);
}
/*****************************************************************************/
/**
 * @brief Deck::Mid
 *
 * Creates a temporary deck contains "size" cards from a starting position
 *
 * @param from_pos Starting position
 * @param size The number of elements to get
 * @return the new deck
 */
Deck Deck::Mid(std::uint32_t from_pos, std::uint32_t size)
{
    Deck deck;
    std::uint32_t counter = 0U;

    // Protection regarding the starting position
    if (from_pos >= Size())
    {
        from_pos = 0U;
    }

    // Protection regarding the number of elements
    if (size > Size())
    {
        size = Size() - from_pos;
    }

    // Calculate the last position
    std::uint32_t to_pos = from_pos + size;

    for (Deck::ConstIterator i = Begin(); i != End(); ++i)
    {
        if ((counter >= from_pos) &&
                (counter < to_pos))
        {
            deck.Append((*i));
        }
        counter++;
    }
    return deck;
}
/*****************************************************************************/
void Deck::Remove(Card *c)
{
    mDeck.remove(c);
}
/*****************************************************************************/
/**
 * @brief Deck::Count
 *
 * Counts the number of the same cards in the deck
 *
 * @param c
 * @return
 */
std::uint32_t Deck::Count(Card *c) const
{
    std::uint32_t counter = 0U;

    for (Deck::ConstIterator i = Begin(); i != End(); ++i)
    {
        if (c == (*i))
        {
            counter++;
        }
    }
    return counter;
}
/*****************************************************************************/
std::string Deck::GetCardList() const
{
    std::string list;

    for (Deck::ConstIterator i = Begin(); i != End(); ++i)
    {
        if (i != Begin())
        {
            list += ";";
        }
        list += (*i)->GetName();
    }
    return list;
}
/*****************************************************************************/
void Deck::Shuffle(int seed)
{
    std::default_random_engine generator(seed);

    // Since the STL random does not work on lists, we have to copy the data
    // into a vector, shuffle the vector, and copy it back into a list.
    std::vector<Card *> myVector(Size());
    std::copy(Begin(), End(), myVector.begin());

    // Actually shuffle the cards
    std::shuffle(myVector.begin(), myVector.end(), generator);

    // Copy back the intermediate working vector into the Deck
    mDeck.assign(myVector.begin(), myVector.end());
}
/*****************************************************************************/
Card *Deck::GetCardByName(const std::string &i_name)
{
    for (Deck::ConstIterator i = Begin(); i != End(); ++i)
    {
        if ((*i)->GetName() == i_name)
        {
            return (*i);
        }
    }
    return NULL;
}
/*****************************************************************************/
bool Deck::HasCard(Card *c) const
{
    bool ret = false;
    if (std::count(Begin(), End(), c) > 0)
    {
        ret = true;
    }
    return ret;
}
/*****************************************************************************/
bool Deck::HasOneOfTrump() const
{
    bool ret = false;
    for (Deck::ConstIterator i = Begin(); i != End(); ++i)
    {
        if (((*i)->GetSuit() == Card::TRUMPS) &&
                ((*i)->GetValue() == 1))
        {
            ret = true;
            break;
        }
    }
    return ret;
}
/*****************************************************************************/
bool Deck::HasFool() const
{
    bool ret = false;
    for (Deck::ConstIterator i = Begin(); i != End(); ++i)
    {
        if (((*i)->GetSuit() == Card::TRUMPS) &&
                ((*i)->GetValue() == 0))
        {
            ret = true;
            break;
        }
    }
    return ret;
}
/*****************************************************************************/
/**
 * @brief Deck::HighestTrump
 *
 * This algorithm voluntary eliminates the fool, which as a value of zero.
 * It is not considered as the highest trump, even if it is alone in the deck.
 *
 * @return The highest trump in the deck
 */
Card *Deck::HighestTrump() const
{
    Card *c = NULL;
    int value = 0;

    for (Deck::ConstIterator i = Begin(); i != End(); ++i)
    {
        if (((*i)->GetSuit() == Card::TRUMPS) &&
                ((*i)->GetValue() > value))
        {
            value = (*i)->GetValue();
            c = (*i);
        }
    }
    return c;
}
/*****************************************************************************/
/**
 * @brief Deck::HighestSuit
 *
 * This algorithm take into account that the first card played lead the color
 *
 * @return the card leader, null if not found
 */
Card *Deck::HighestSuit() const
{
    Card *c = NULL;
    int value = 0;
    Card::Suit suit; // leading suit
    bool hasLead = false;

    for (Deck::ConstIterator i = Begin(); i != End(); ++i)
    {
        if (((*i)->GetSuit() != Card::TRUMPS) &&
                ((*i)->GetValue() > value))
        {
            if (!hasLead)
            {
                hasLead = true;
                suit = (*i)->GetSuit();
                value = (*i)->GetValue();
                c = (*i);
            }
            else if ((*i)->GetSuit() == suit)
            {
                value = (*i)->GetValue();
                c = (*i);
            }
        }
    }
    return c;
}
/*****************************************************************************/
/**
 * @brief Sort the deck of card, optional order can be given
 * The sorting order is given in parameter as a string format, one letter
 * per suit.
 * Example:
 * "THSDC" will sort cards in the following format:
 * Trumps, Hearts, Spades, Diamonds, Clubs
 *
 * @param order String containing the order of suits
 */
void Deck::Sort(const std::string &order)
{
    std::uint16_t weight[5];

    if (order.size() == 5)
    {
        // Generate a weight for each suit
        for (int i = 0; i < 5; i++)
        {
            std::string letter;
            letter.push_back(order[4 - i]);
            weight[Card::ToSuit(letter)] = 100 * i;
        }

        // Depending of the sorting contents, give a weight to each card
        for (Deck::ConstIterator i = Begin(); i != End(); ++i)
        {
            Card *c = (*i);

            std::uint16_t id = weight[c->GetSuit()] + c->GetValue();
            c->SetId(id);
        }
    }

    if (Size() != 0)
    {
        mDeck.sort(&Deck::LessThanCards);
    }
}
/*****************************************************************************/
bool Deck::LessThanCards(Card *c1, Card *c2)
{
    return c1->GetId() > c2->GetId();
}
/*****************************************************************************/
void Deck::SetOwner(Team team)
{
    mOwner = team;
}
/*****************************************************************************/
std::uint8_t Deck::SetCards(const std::string &cards)
{
    std::uint8_t count = 0U;
    std::size_t found = std::string::npos;
    int pos = 0;

    // Clear this deck before setting new cards
    Clear();

    do
    {
        int size;
        found = cards.find(';', pos);
        if (found != std::string::npos)
        {
            // calculate size of the string between the delimiters
            size = found - pos;
        }
        else
        {
            // last card: get remaining characters
            size = cards.size() - pos;
        }

        std::string cardName = cards.substr(pos, size);
        pos = found + 1;
        Card *c = TarotDeck::GetCard(cardName);
        if (c != NULL)
        {
            count++;
            mDeck.push_back(c);
        }
    }
    while (found != std::string::npos);
    return count;
}
/*****************************************************************************/
Team Deck::GetOwner()
{
    return mOwner;
}
/*****************************************************************************/
void Deck::Statistics::Reset()
{
    nbCards     = 0U;
    oudlers     = 0U;
    trumps      = 0U;
    majorTrumps = 0U;
    kings       = 0U;
    queens      = 0U;
    knights     = 0U;
    jacks       = 0U;
    weddings    = 0U;
    longSuits   = 0U;
    cuts        = 0U;
    singletons  = 0U;
    sequences   = 0U;

    for (std::uint8_t i = 0U; i < 4U; i++)
    {
        suits[i] = 0U;
    }

    littleTrump = false;
    bigTrump    = false;
    fool        = false;
    points      = 0.0F;
}
/*****************************************************************************/
void Deck::AnalyzeTrumps(Statistics &stats) const
{
    int val;
    Card *c;

    stats.nbCards = Size();

    // looking for trumps
    for (Deck::ConstIterator i = Begin(); i != End(); ++i)
    {
        c = (*i);
        if (c->GetSuit() == Card::TRUMPS)
        {
            stats.trumps++;
            val = c->GetValue();
            if (val >= 15)
            {
                stats.majorTrumps++;
            }
            if (val == 21)
            {
                stats.bigTrump = true;
                stats.oudlers++;
            }
            if (val == 1)
            {
                stats.littleTrump = true;
                stats.oudlers++;
            }
            if (val == 0)
            {
                stats.fool = true;
                stats.oudlers++;
            }
        }
        stats.points += c->GetPoints();
    }
}
/*****************************************************************************/
void Deck::AnalyzeSuits(Statistics &stats)
{
    Card *c;
    std::uint8_t k;

    // true if the card is available in the deck
    std::array<bool, 14U> distr;

    // Normal suits
    for (std::uint8_t suit = 0U; suit < 4U; suit++)
    {
        std::uint8_t count = 0U; // Generic purpose counter
        distr.fill(false);

        for (Deck::ConstIterator iter = Begin(); iter != End(); ++iter)
        {
            c = (*iter);
            if (c != NULL)
            {
                if (c->GetSuit() == suit)
                {
                    count++;
                    std::uint8_t val = c->GetValue();
                    distr[val - 1U] = true;
                    if (val == 11U)
                    {
                        stats.jacks++;
                    }
                    if (val == 12U)
                    {
                        stats.knights++;
                    }
                    if (val == 13U)
                    {
                        stats.queens++;
                    }
                    if (val == 14U)
                    {
                        stats.kings++;
                    }
                }
            }
        }

        stats.suits[suit] = count;

        if (count >= 5U)
        {
            stats.longSuits++;
        }
        if (count == 1U)
        {
            stats.singletons++;
        }
        if (count == 0U)
        {
            stats.cuts++;
        }
        if (distr[13] && distr[12])
        {
            stats.weddings++; // king + queen
        }

        // Sequence detection
        count = 0U; // sequence length
        bool detected = false; // sequence detected
        for (k = 0U; k < 14U; k++)
        {
            if (distr[k])
            {
                count++;
                if (!detected)
                {
                    if (count >= 5U)
                    {
                        // Ok, found sequence, enough for it
                        stats.sequences++;
                        detected = true;
                    }
                }
            }
            else
            {
                count = 0U;
                detected = false;
            }
        }
    }
}

//=============================================================================
// End of file Deck.cpp
//=============================================================================
