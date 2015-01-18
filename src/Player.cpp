/*=============================================================================
 * TarotClub - Player.cpp
 *=============================================================================
 * Basic modelization of a tarot player
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

#include "Player.h"
#include "Protocol.h"

/*****************************************************************************/
Player::Player()
    : mUuid(Protocol::INVALID_UID)
    , mAck(false)
{

}
/*****************************************************************************/
std::uint32_t Player::GetUuid() const
{
    return mUuid;
}
/*****************************************************************************/
void Player::SetUuid(std::uint32_t value)
{
    mUuid = value;
}
/*****************************************************************************/
Deck Player::AutoDiscard(const Deck &dog, std::uint8_t nbPlayers)
{
    Deck discard;

    // We add all the dog cards to the player's deck
    Append(dog);

    // We're looking valid discard cards to put in the discard
    for (Deck::ConstIterator iter = Begin(); iter != End(); ++iter)
    {
        Card c = (*iter);
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
bool Player::CanPlayCard(const Card &card, Deck &trick)
{
    std::uint8_t   suit; // required suit
    bool ret = false;
    PlayerStats stats;

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
    Card c = *(trick.Begin());

    if (c.IsFool())
    {
        // The first card is a Excuse...
        if (trick.Size() == 1)
        {
            // ...the player can play everything he wants
            return true;
        }
        // The requested suit is the second card
        c = *(++trick.Begin());
    }
    suit = c.GetSuit();

    // Some indications about previous played cards
    for (Deck::ConstIterator it = trick.Begin(); it != trick.End(); ++it)
    {
        c = *(it);
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
    for (Deck::ConstIterator it = Begin(); it != End(); ++it)
    {
        c = *(it);
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
bool Player::TestPlayTrump(const Card &card, const PlayerStats &stats)
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
bool Player::TestHandle(const Deck &handle)
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
    for (Deck::ConstIterator it = handle.Begin(); it != handle.End(); ++it)
    {
        Card c = (*it);
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
bool Player::TestDiscard(const Deck &discard, const Deck &dog, std::uint8_t numberOfPlayers)
{
    bool valid = true;

    if (discard.Size() == Tarot::NumberOfDogCards(numberOfPlayers))
    {
        for (Deck::ConstIterator i = discard.Begin(); i != discard.End(); ++i)
        {
            Card c = (*i);

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
void Player::RemoveDuplicates(const Deck &cards)
{
    // remove cards in the player's deck that are similar to the discard
    for (Deck::ConstIterator it = cards.Begin(); it != cards.End(); ++it)
    {
        Card c = (*it);
        if (HasCard(c))
        {
            Remove(c);
        }
    }
}
/*****************************************************************************/
bool Player::IsFree()
{
    if (mUuid == 0U)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//=============================================================================
// End of file Player.cpp
//=============================================================================
