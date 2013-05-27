/*=============================================================================
 * TarotClub - Player.cpp
 *=============================================================================
 * Base class for all players.
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
void Player::addCard(Card *newCard)
{
    myDeck.append(newCard);
}
/*****************************************************************************/
void Player::removeCard(Card *c)
{
    myDeck.removeAll(c);
}
/*****************************************************************************/
void Player::emptyDeck()
{
    myDeck.clear();
}
/*****************************************************************************/
void Player::tidyDeck()
{
    myDeck.sort();
}
/*****************************************************************************/
int Player::getCardNumber()
{
    return myDeck.count();
}
/*****************************************************************************/
Card *Player::getCard(int i)
{
    return (myDeck.at(i));
}
/*****************************************************************************/
Card *Player::getCardById(int i)
{
    return (myDeck.getCardById(i));
}
/*****************************************************************************/
Card *Player::GetCardByName(const QString &i_name)
{
    Card *c = NULL;

    for (int i = 0; i < myDeck.size(); i++)
    {
        c = myDeck.at(i);
        if (c->getCardName() == i_name)
        {
            return c;
        }
    }
    return c;
}
/*****************************************************************************/
/**
 * Retourne true si la carte est présente dans les main du joueur, sinon false
 */
bool Player::HasCard(Card *c)
{
    if (myDeck.indexOf(c) == -1)
    {
        return (false);
    }
    else
    {
        return (true);
    }
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
bool Player::CanPlayCard(Deck *mainDeck, Card *cVerif, GameState &info)
{
    CardType    type; // required card
    CardColor   coul; // required color

    bool hasColor = false; // true if the player has the requested color
    bool possedeAtout = false;   // vrai si le joueur possède un atout
    bool precedentAtout = false; // vrai si un atout max précédent
    int  precAtoutMax = 0;
    int  atoutMax = 0; // atout maxi possédé
    bool ret = true;
    Card *c = NULL;
    int i, n;

    // The player is the first of the trick, he can play all the cards
    if (info.trickCounter == 0)
    {
        return true;
    }

    // Simple use case, the excuse can always be played
    if (cVerif->getType() == EXCUSE)
    {
        return true;
    }

    // We retreive the requested color by looking at the first card played
    c = mainDeck->at(debut);

    type = c->getType();
    coul = c->getColor();
    if (type == EXCUSE)
    {
        // The first card is a Excuse
        if (rang == 2)
        {
            // The player can play everything he wants
            return true;
        }
        c = mainDeck->at(debut + 1);   // la couleur demandée est donc la seconde carte
        type = c->getType();
        coul = c->getColor();
    }

    if (type == CARTE && cVerif->getColor() == coul)
    {
        return true;
    }

    // Quelques indications sur les plis précédents
    // On regarde s'il y a un atout
    for (i = 0; i < rang - 1; i++)
    {
        c = mainDeck->at(debut + i);
        if (c->getType() == ATOUT)
        {
            precedentAtout = true;
            if (c->getValue() > precAtoutMax)
            {
                precAtoutMax = c->getValue();
            }
        }
    }

    // Quelques indications sur les cartes du joueur
    n = myDeck.count();

    for (i = 0; i < n; i++)
    {
        c = myDeck.at(i);

        if (c->getType() == ATOUT)
        {
            possedeAtout = true;
            if (c->getValue() > atoutMax)
            {
                atoutMax = c->getValue();
            }
        }
        else if (c->getType() == CARTE)
        {
            if (c->getColor() == coul)
            {
                hasColor = true;
            }
        }
    }

    // Card type requested is a trump
    if (type == ATOUT)
    {
        if (cVerif->getType() == ATOUT)
        {
            if (cVerif->getValue() > precAtoutMax)
            {
                return true;
            }
            else
            {
                if (atoutMax > precAtoutMax)
                {
                    return false;  // doit surcouper !
                }
                else
                {
                    return true;   // sinon tant pis, on doit quand même jouer la couleur demandée
                }
            }
        }
        else
        {
            if (possedeAtout == true)
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
        // le joueur possède la couleur demandée, il doit donc la jouer cela
        if (hasColor == true)
        {
            return false;
        }
        else
        {
            // pas la couleur demandée
            if (cVerif->getType() == ATOUT)
            {
                // doit-il surcouper ?
                if (precedentAtout == true)
                {
                    if (cVerif->getValue() > precAtoutMax)
                    {
                        // carte de surcoupe
                        return true;
                    }
                    else
                    {
                        // a-t-il alors un atout plus fort ?
                        if (atoutMax > precAtoutMax)
                        {
                            return false; // oui, il doit donc surcouper
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
                if (possedeAtout == true)
                {
                    return false; // oui, il DOIT couper
                }
                else
                {
                    return true; // non, il peut se défausser
                }
            }
        }
    }
    return ret;
}
/*****************************************************************************/
void Player::setName(const QString &n)
{
    identity.name = n;
}
/*****************************************************************************/
void Player::setQuote(const QString &q)
{
    identity.quote = q;
}
/*****************************************************************************/
void Player::setSex(SexType s)
{
    identity.sex = s;
}
/*****************************************************************************/
void Player::setAvatar(const QString &a)
{
    identity.avatar = a;
}
/*****************************************************************************/
void Player::setIdentity(Identity &ident)
{
    identity = ident;
}
/*****************************************************************************/
void Player::setPlace(Place p)
{
    identity.place = p;
}
/*****************************************************************************/
QString Player::getName()
{
    return (identity.name);
}
/*****************************************************************************/
QString Player::getQuote()
{
    return (identity.quote);
}
/*****************************************************************************/
SexType  Player::getSex()
{
    return (identity.sex);
}
/*****************************************************************************/
QString  Player::getAvatar()
{
    return (identity.avatar);
}
/*****************************************************************************/
Identity *Player::getIdentity()
{
    return (&identity);
}
/*****************************************************************************/
Place Player::getPlace()
{
    return identity.place;
}

//=============================================================================
// End of file Player.cpp
//=============================================================================
