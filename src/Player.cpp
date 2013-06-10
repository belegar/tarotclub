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

#ifndef QT_NO_DEBUG
#include <iostream>
#include <fstream>
using namespace std;
#endif // QT_NO_DEBUG


/*****************************************************************************/
Player::Player()
{

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
bool Player::CanPlayCard(Card *cVerif, Deck &trick, Game &info)
{
    Card::Suit   suit; // required suit

    // player's cards in hand
    bool hasSuit = false;           // true if the player has the requested color
    bool hasTrump = false;          // true if the player has some trumps
    int  highestTrumpValue = 0;     // value of the maximum trump in hand

    bool previousTrump = false;       // true if there is previous trump played
    int  maxPreviousTrump = 0;      // maximum value of the previous trump played

    Card *c = NULL;
    int i;

    // The player is the first of the trick, he can play all the cards
    if (info.position == 0)
    {
        return true;
    }

    // Simple use case, the excuse can always be played
    if (cVerif->IsFool())
    {
        return true;
    }

    // We retreive the requested suit by looking at the first card played
    c = trick.at(0);

    if (c->IsFool())
    {
        // The first card is a Excuse...
        if (info.position == 1)
        {
            // ...the player can play everything he wants
            return true;
        }
        // The requested suit is the second card
        c = trick.at(1);
    }
    suit = c->GetSuit();

    // Some indications about previous played cards
    for (i = 0; i < trick.size(); i++)
    {
        c = trick.at(i);
        if (c->GetSuit() == Card::TRUMPS)
        {
            previousTrump = true;
            if (c->GetValue() > maxPreviousTrump)
            {
                maxPreviousTrump = c->GetValue();
            }
        }
    }

    // Some indications on the player cards in hand
    for (i = 0; i < myDeck.count(); i++)
    {
        c = myDeck.at(i);
        if (c->GetSuit() == Card::TRUMPS)
        {
            hasTrump = true;
            if (c->GetValue() > highestTrumpValue)
            {
                highestTrumpValue = c->GetValue();
            }
        }
        else
        {
            if (c->GetSuit() == suit)
            {
                hasSuit = true;
            }
        }
    }

    // Card type requested is a trump
    if (suit == Card::TRUMPS)
    {
        if (cVerif->GetSuit() == Card::TRUMPS)
        {
            // If we have trumps, we must play higher than the highest previous played trump,
            // if we have the card, only. Otherwise, every trump value is ok.
            if (cVerif->GetValue() > maxPreviousTrump)
            {
                return true;
            }
            else
            {
                if (highestTrumpValue > maxPreviousTrump)
                {
                    return false;  // we must play higher trump we have
                }
                else
                {
                    return true;   // any card is ok
                }
            }
        }
        else
        {
            if (hasTrump == true)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
    }
    // Card type requested is a standard card
    else
    {
        // The card is the required suit
        if (cVerif->GetSuit() == suit)
        {
            return true;
        }
        else if (hasSuit == true)
        {
            // not the required card, but he has the suit in hands
            // he must play the required suit
            return false;
        }
        else
        {
            // We are here if the player has not the requested suit
            // He must play a trump if he has some, or any other cards in other case

            if (cVerif->GetSuit() == Card::TRUMPS)
            {
                // He may have to play a higher trump
                if (previousTrump == true)
                {
                    if (cVerif->GetValue() > maxPreviousTrump)
                    {
                        // higher card, ok!
                        return true;
                    }
                    else
                    {
                        // does he have a higher trump in hands?
                        if (highestTrumpValue > maxPreviousTrump)
                        {
                            return false; // yes, he must play it
                        }
                        else
                        {
                            return true;
                        }
                    }
                }
            }
            else
            {
                // a-t-il un atout pour couper ?
                if (hasTrump == true)
                {
                    return false; // he must play a trump
                }
                else
                {
                    return true; // he can play any card
                }
            }
        }
    }
    return true;
}
/*****************************************************************************/
void Player::SetIdentity(const Identity &ident)
{
    identity = ident;
}
/*****************************************************************************/
void Player::SetPlace(Place p)
{
    place = p;
}
/*****************************************************************************/
Identity &Player::GetIdentity()
{
    return identity;
}
/*****************************************************************************/
Place Player::GetPlace()
{
    return place;
}
/*****************************************************************************/
Deck &Player::GetDeck()
{
    return myDeck;
}
/*****************************************************************************/
/*              *           *           *           *           *            */
/*****************************************************************************/
NetPlayer::NetPlayer()
    : socket(NULL)
    , freePlace(true)
{

}
/*****************************************************************************/
bool NetPlayer::IsFree()
{
    return freePlace;
}
/*****************************************************************************/
void NetPlayer::Close()
{
    QObject::connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
    socket->close();
    socket->waitForDisconnected();
    socket = NULL;
    freePlace = true;
}
/*****************************************************************************/
Identity &NetPlayer::GetIdentity()
{
    return player.GetIdentity();
}
/*****************************************************************************/
void NetPlayer::SetConnection(QTcpSocket *s, Place p)
{
    socket = s;
    freePlace = false;
    player.SetPlace(p);
    connect(socket, SIGNAL(disconnected()), this, SLOT(slotClientClosed()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(slotReadData()));
}
/*****************************************************************************/
void NetPlayer::SetIdentity(const Identity &ident)
{
    player.SetIdentity(ident);
}
/*****************************************************************************/
void NetPlayer::SendData(QByteArray &data)
{
    if (socket != NULL)
    {
        if (socket->isValid())
        {
            socket->write(data);
            socket->flush();
        }
        else
        {
            qDebug() << "Send data to a non-connected socket" << endl;
        }
    }
    else
    {
        qDebug() << "Send data to a non-valid socket" << endl;
    }
}
/*****************************************************************************/
QByteArray NetPlayer::GetData()
{
    QByteArray data;
    if (socket != NULL)
    {
        if (socket->isValid())
        {
            data = socket->readAll();
        }
    }
    return data;
}
/*****************************************************************************/
void NetPlayer::slotClientClosed()
{
    emit sigDisconnected(player.GetPlace());
}
/*****************************************************************************/
void NetPlayer::slotReadData()
{
    emit sigReadyRead(player.GetPlace());
}


//=============================================================================
// End of file Player.cpp
//=============================================================================
