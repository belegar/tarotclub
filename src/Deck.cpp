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

const std::string Deck::Sorter::cDefault = "TCSDH";

/*****************************************************************************/
Deck::Deck()
{

}
/*****************************************************************************/
Deck::Deck(const std::string &cards)
{
    SetCards(cards);
}
/*****************************************************************************/
void Deck::CreateTarotDeck()
{
    Clear();

    // The 4 suits
    for (std::uint8_t i = 0U; i < 4U; i++)
    {
        // From ace to the king, 14 cards
        for (std::uint8_t j = 1; j <= 14; j++)
        {
            mDeck.push_back(Card(j, i));
        }
    }

    // The 22 trumps, including the fool
    for (std::uint8_t i = 0U; i <= 21U; i++)
    {
        mDeck.push_back(Card(i, Card::TRUMPS));
    }
}
/*****************************************************************************/
/**
 * @brief Deck::RemoveDuplicates
 *
 * This method removes all the cards of the current deck that are common
 * with the deck passed in parameters.
 *
 * @param deck
 * @return The number of duplicate cards removed
 */
std::uint32_t Deck::RemoveDuplicates(const Deck &deck)
{
    std::uint32_t cardsRemoved = 0U;

    for (const auto &c : deck)
    {
        if (HasCard(c))
        {
            Remove(c);
            cardsRemoved++;
        }
    }
    return cardsRemoved;
}
/*****************************************************************************/
void Deck::Append(const Deck &deck)
{
    for (const auto &c : deck)
    {
        Append(c);
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

    for (const auto &c : mDeck)
    {
        if ((counter >= from_pos) &&
                (counter < to_pos))
        {
            deck.Append(c);
        }
        counter++;
    }
    return deck;
}
/*****************************************************************************/
Card Deck::At(uint32_t pos)
{
    Card card;
    std::uint32_t counter = 0U;

    for (const auto &c : mDeck)
    {
        if (pos == counter)
        {
            card = c;
            break;
        }
        counter++;
    }
    return card;
}
/*****************************************************************************/
/**
 * @brief Deck::Remove
 * @param c
 * @return The number of cards removed from the deck
 */
std::uint32_t Deck::Remove(const Card &card)
{
    std::uint32_t counter = Count(card);

    mDeck.erase(std::remove(mDeck.begin(), mDeck.end(), card), mDeck.end());
    return counter;
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
std::uint32_t Deck::Count(const Card &card) const
{
    std::uint32_t counter = 0U;

    for (const auto &c : mDeck)
    {
        if (c == card)
        {
            counter++;
        }
    }
    return counter;
}
/*****************************************************************************/
std::string Deck::ToString() const
{
    std::string list;
    bool first = true;

    for (const auto &c : mDeck)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            list += ";";
        }
        list += c.ToString();
    }
    return list;
}
/*****************************************************************************/
void Deck::Shuffle(int seed)
{
    std::default_random_engine generator(seed);

    // Since the STL random does not work on lists, we have to copy the data
    // into a vector, shuffle the vector, and copy it back into a list.
    std::vector<Card> myVector(Size());
    std::copy(begin(), end(), myVector.begin());

    // Actually shuffle the cards
    std::shuffle(myVector.begin(), myVector.end(), generator);

    // Copy back the intermediate working vector into the Deck
    mDeck.assign(myVector.begin(), myVector.end());
}
/*****************************************************************************/
Card Deck::GetCard(const std::string &i_name)
{
    Card card;
    for (const auto &c : mDeck)
    {
        if (c.ToString() == i_name)
        {
            card = c;
        }
    }
    return card;
}
/*****************************************************************************/
bool Deck::HasCard(const Card &card) const
{
    bool ret = false;
    for (const auto &c : mDeck)
    {
        if (card == c)
        {
            ret = true;
        }
    }
    return ret;
}
/*****************************************************************************/
bool Deck::HasOneOfTrump() const
{
    bool ret = false;
    for (const auto &c : mDeck)
    {
        if ((c.GetSuit() == Card::TRUMPS) &&
            (c.GetValue() == 1U))
        {
            ret = true;
            break;
        }
    }
    return ret;
}
/*****************************************************************************/
bool Deck::HasOnlyOneOfTrump() const
{
    bool ret = false;
    if (HighestTrump().GetValue() == 1U)
    {
        if (!HasFool())
        {
            ret = true;
        }
    }
    return ret;
}
/*****************************************************************************/
bool Deck::HasFool() const
{
    bool ret = false;
    for (const auto &c : mDeck)
    {
        if ((c.GetSuit() == Card::TRUMPS) &&
            (c.GetValue() == 0U))
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
Card Deck::HighestTrump() const
{
    Card card;
    std::uint32_t value = 0U;

    for (const auto &c : mDeck)
    {
        if ((c.GetSuit() == Card::TRUMPS) &&
            (c.GetValue() > value))
        {
            value = c.GetValue();
            card = c;
        }
    }
    return card;
}
/*****************************************************************************/
/**
 * @brief Deck::HighestSuit
 *
 * This algorithm take into account that the first card played lead the color
 *
 * @return the card leader, null if not found
 */
Card Deck::HighestSuit() const
{
    Card card;
    std::uint32_t value = 0U;
    std::uint8_t suit; // leading suit
    bool hasLead = false;

    for (const auto &c : mDeck)
    {
        if ((c.GetSuit() != Card::TRUMPS) &&
            (c.GetValue() > value))
        {
            if (!hasLead)
            {
                hasLead = true;
                suit = c.GetSuit();
                value = c.GetValue();
                card = c;
            }
            else if (c.GetSuit() == suit)
            {
                value = c.GetValue();
                card = c;
            }
        }
    }
    return card;
}
/*****************************************************************************/
Deck Deck::AutoDiscard(const Deck &dog, std::uint8_t nbPlayers)
{
    Deck discard;

    // We add all the dog cards to the player's deck
    Append(dog);

    // We're looking valid discard cards to put in the discard
    for (const auto &c : mDeck)
    {
        if ((c.GetSuit() != Card::TRUMPS) && (c.GetValue() != 14U))
        {
            discard.Append(c);

            if (discard.Size() == Tarot::NumberOfDogCards(nbPlayers))
            {
                // enough cards!
                break;
            }
        }
    }

    return discard;
}
/*****************************************************************************/
/**
 * @brief Player::canPlayCard
 *
 * Test if the card cVerif can be played depending of the already played cards
 * and the cards in the player's hand.
 *
 * @param mainDeck
 * @param cVerif
 * @param gameCounter
 * @param nbPlayers
 * @return true if the card can be played
 */
bool Deck::CanPlayCard(const Card &card, Deck &trick)
{
    std::uint8_t   suit; // required suit
    bool ret = false;
    Stats stats;

    // Check if the player has the card in hand
    if (!HasCard(card))
    {
        return false;
    }

    // The player is the first of the trick, he can play any card
    if (trick.Size() == 0)
    {
        return true;
    }

    // Simple use case, the excuse can always be played
    if (card.IsFool())
    {
        return true;
    }

    // We retreive the requested suit by looking at the first card played
    Card c = trick.At(0);

    if (c.IsFool())
    {
        // The first card is a Excuse...
        if (trick.Size() == 1)
        {
            // ...the player can play everything he wants
            return true;
        }
        // If we are here, it means than we have two or more cards in the trick
        // The requested suit is the second card
        c = trick.At(1);
    }
    suit = c.GetSuit();

    // Some indications about previous played cards
    for (const auto &c : trick)
    {
        if (c.GetSuit() == Card::TRUMPS)
        {
            stats.previousTrump = true;
            if (c.GetValue() > stats.maxPreviousTrump)
            {
                stats.maxPreviousTrump = c.GetValue();
            }
        }
    }

    // Some indications on the player cards in hand
    for (const auto &c : mDeck)
    {
        if (c.GetSuit() == Card::TRUMPS)
        {
            stats.hasTrump = true;
            if (c.GetValue() > stats.highestTrumpValue)
            {
                stats.highestTrumpValue = c.GetValue();
            }
        }
        else
        {
            if (c.GetSuit() == suit)
            {
                stats.hasSuit = true;
            }
        }
    }

    // Card type requested is a trump
    if (suit == Card::TRUMPS)
    {
        ret = TestPlayTrump(card, stats);
    }
    // Card type requested is a standard card
    else
    {
        // The card is the required suit
        if (card.GetSuit() == suit)
        {
            ret = true;
        }
        else if (stats.hasSuit == true)
        {
            // not the required card, but he has the suit in hands
            // he must play the required suit
            ret = false;
        }
        else
        {
            // We are here if the player has not the requested suit
            ret = TestPlayTrump(card, stats);
        }
    }
    return ret;
}
/*****************************************************************************/
/**
 * @brief Player::TestPlayTrump
 *
 * This method test if the player can play a trump
 *
 * @param cVerif
 * @param hasTrump
 * @param maxPreviousTrump
 * @return
 */
bool Deck::TestPlayTrump(const Card &card, const Stats &stats)
{
    bool ret = false;

    // He must play a trump if he has some, higher than the highest previous played trump,
    // or any other cards in other case
    if (card.GetSuit() == Card::TRUMPS)
    {
        // He may have to play a higher trump
        if (stats.previousTrump == true)
        {
            if (card.GetValue() > stats.maxPreviousTrump)
            {
                // higher card, ok!
                ret = true;
            }
            else
            {
                // does he have a higher trump in hands?
                if (stats.highestTrumpValue > stats.maxPreviousTrump)
                {
                    ret = false; // yes, he must play it
                }
                else
                {
                    ret = true;
                }
            }
        }
        else
        {
            // No any previous trump played, so he can play any value
            ret = true;
        }
    }
    else
    {
        // Does he have a trump?
        if (stats.hasTrump == true)
        {
            // If he has only one trump and this trump is the fool, then he can play any card
            if (stats.highestTrumpValue == 0)
            {
                ret = true;
            }
            else
            {
                ret = false; // he must play a trump
            }
        }
        else
        {
            ret = true; // he can play any card
        }
    }

    return ret;
}
/*****************************************************************************/
bool Deck::TestHandle(const Deck &handle)
{
    bool ret = true;
    Deck::Statistics stats;

    // Check if the handle size is correct
    if (Tarot::GetHandleType(handle.Size()) == Tarot::NO_HANDLE)
    {
        ret = false;
    }

    // Test if the handle contains only trumps
    stats.Reset();
    handle.AnalyzeTrumps(stats);

    if (handle.Size() != stats.trumps)
    {
        ret = false;
    }

    // Test if the player has all the cards of the declared handle
    for (const auto &c : handle)
    {
        if (!HasCard(c))
        {
            ret = false;
        }
    }

    // If the fool is shown, then it indicates that there is no more any trumps in the player's hand
    stats.Reset();
    AnalyzeTrumps(stats);
    if ((handle.HasFool() == true) && (stats.trumps > handle.Size()))
    {
        ret = false;
    }
    return ret;
}
/*****************************************************************************/
bool Deck::TestDiscard(const Deck &discard, const Deck &dog, std::uint8_t numberOfPlayers)
{
    bool valid = true;

    if (discard.Size() == Tarot::NumberOfDogCards(numberOfPlayers))
    {
        for (const auto &c : discard)
        {
            // Look if the card belongs to the dog or the player's deck
            if (HasCard(c) || dog.HasCard(c))
            {
                // Look the card value against the Tarot rules
                if ((c.GetSuit() == Card::TRUMPS) ||
                   ((c.GetSuit() != Card::TRUMPS) && (c.GetValue() == 14U)))
                {
                    valid = false;
                }

                // Look if this card is unique
                if (discard.Count(c) != 1U)
                {
                    valid = false;
                }
            }
            else
            {
                valid = false;
            }
        }
    }
    else
    {
        valid = false;
    }

    return valid;
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
    if (Size() != 0)
    {
        Sorter sorter(order);
        std::sort(mDeck.begin(), mDeck.end(), sorter);
    }
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
	std::size_t pos = 0;

    // Clear this deck before setting new cards
    Clear();

    do
    {
		std::size_t size;
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
        count++;
        Card card(cardName);
        if (card.IsValid())
        {
            mDeck.push_back(card);
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
Card Deck::Last()
{
    return mDeck.back();
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
    stats.nbCards = Size();

    // looking for trumps
    for (const auto &c : mDeck)
    {
        if (c.GetSuit() == Card::TRUMPS)
        {
            stats.trumps++;
            val = c.GetValue();
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
        stats.points += c.GetPoints();
    }
}
/*****************************************************************************/
void Deck::AnalyzeSuits(Statistics &stats)
{
    std::uint8_t k;

    // true if the card is available in the deck
    std::array<bool, 14U> distr;

    // Normal suits
    for (std::uint8_t suit = 0U; suit < 4U; suit++)
    {
        std::uint8_t count = 0U; // Generic purpose counter
        distr.fill(false);

        for (const auto &c : mDeck)
        {
            if (c.GetSuit() == suit)
            {
                count++;
                std::uint8_t val = c.GetValue();
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
