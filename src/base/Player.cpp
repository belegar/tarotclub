/*=============================================================================
 * Tarot Club - Player.cpp
 *=============================================================================
 * Base class for all players.
 *=============================================================================
 * Tarot Club est un jeu de Tarot fran�ais
 * Copyright (C) 2003-2005  Anthony Rabine
 * anthony@ooso.org
 * http://tarotclub.ooso.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *=============================================================================
 */

#include "Player.h"

#ifndef QT_NO_DEBUG
   #include <iostream>
   #include <fstream>
using namespace std;
#endif // _DEBUG


/*****************************************************************************/
Player::Player()
{

}
/*****************************************************************************/
void Player::addCard( Card *newCard )
{
   myDeck.append( newCard );
}
/*****************************************************************************/
void Player::removeCard( Card *c )
{
   myDeck.removeAll( c );
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
Card *Player::getCard( int i )
{
   return (myDeck.at(i));
}
/*****************************************************************************/
Card *Player::getCardById( int i )
{
   return (myDeck.getCardById(i));
}
/*****************************************************************************/
/**
 * Retourne true si la carte est pr�sente dans les main du joueur, sinon false
 */
bool Player::cardExists( Card *c )
{
   if( myDeck.indexOf( c ) == -1 ) {
      return (false);
   } else {
      return (true);
   }
}
/*****************************************************************************/
/**
 * Teste si la carte cVerif peut �tre jou�e : cela d�pend des cartes d�j� 
 * jou�es sur le tapis et des cartes dans la main du joueur
 */
bool Player::canPlayCard( Deck *mainDeck, Card *cVerif , int gameCounter, int nbPlayers )
{
   int debut;
   int rang;
   debut = (gameCounter-1)/nbPlayers;
   debut = debut*nbPlayers;

   rang = gameCounter - debut;

   // Le joueur entame le tour, il peut commencer comme il veut
   if( rang == 1 ) {
      return true;
   }

   // on traite un cas simple
   if( cVerif->getType() == EXCUSE ) {
      return true;
   }

   // premi�re carte (couleur demand�e)
   CardType type;
   CardColor   coul;
   int         val;
   //////////////

   bool possedeCouleur=false; // vrai si le joueur posse�de la couleur demandee
   bool possedeAtout=false;   // vrai si le joueur poss�de un atout
   bool precedentAtout=false; // vrai si un atout max pr�c�dent
   int  precAtoutMax=0;
   int  atoutMax=0; // atout maxi poss�d�
   bool ret=true;
   Card *c;
   int i,n;

   // on cherche la couleur demand�e
   c = mainDeck->at( debut ); // premi�re carte jou�e � ce tour

   type = c->getType();
   coul = c->getColor();
   val  = c->getValue();
   if( type == EXCUSE ) { // a�e, le joueur a commenc� avec une excuse
      // le joueur peut jouer n'importe quelle carte apr�s l'excuse, c'est lui qui d�cide alors de la couleur
      if( rang == 2 ) {
         return true;
      }
      c = mainDeck->at( debut + 1 ); // la couleur demand�e est donc la seconde carte
      type = c->getType();
      coul = c->getColor();
      val  = c->getValue();
   }

   if( type == CARTE && cVerif->getColor() == coul ) {
      return true;
   }

   // Quelques indications sur les plis pr�c�dents
   // On regarde s'il y a un atout
   for( i=0; i<rang-1; i++ ) {
      c = mainDeck->at( debut + i );
      if( c->getType() == ATOUT ) {
         precedentAtout = true;
         if( c->getValue() > precAtoutMax ) {
            precAtoutMax = c->getValue();
         }
      }
   }

   // Quelques indications sur les cartes du joueur
   n = myDeck.count();

   for( i=0; i<n; i++ ) {
      c = myDeck.at( i );
      
      if( c->getType() == ATOUT ) {
         possedeAtout=true;
         if( c->getValue() > atoutMax ) {
            atoutMax = c->getValue();
         }
      }
      else if( c->getType() == CARTE ) {
         if( c->getColor() == coul ) {
            possedeCouleur = true;
         }
      }
   }

   // cas 1 : le type demand� est ATOUT
   if( type == ATOUT ) {
      if( cVerif->getType() == ATOUT ) {
         if( cVerif->getValue() > precAtoutMax ) {
            return true;
         } else {
            if( atoutMax > precAtoutMax ) {
               return false;  // doit surcouper !
            } else {
               return true;   // sinon tant pis, on doit quand m�me jouer la couleur demand�e
            }
         }
      } else {
         if( possedeAtout == true ) {
            return false;
         } else {
            return true;      
         }
      }
   } else {// cas 2 : le type demand� est CARTE
      // le joueur poss�de la couleur demand�e, il doit donc la jouer cela
      if( possedeCouleur == true ) {
         return false;
      } else { // pas la couleur demand�e
         if( cVerif->getType() == ATOUT ) {
            // doit-il surcouper ?
            if( precedentAtout==true ) {
               if( cVerif->getValue() > precAtoutMax ) { // carte de surcoupe
                  return true;
               } else {
                  // a-t-il alors un atout plus fort ?
                  if( atoutMax > precAtoutMax ) {
                     return false; // oui, il doit donc surcouper
                  } else {
                     return true;         
                  }
               }
            }
         } else {
            // a-t-il un atout pour couper ?
            if( possedeAtout == true ) {
               return false; // oui, il DOIT couper
            } else {
               return true; // non, il peut se d�fausser
            }
         }
      }
   }
   return ret;
}
/*****************************************************************************/
void Player::setName( const QString &n )
{
   identity.name = n;
}
/*****************************************************************************/
void Player::setQuote( const QString &q )
{
   identity.quote = q;
}
/*****************************************************************************/
void Player::setSex( SexType s )
{
   identity.sex = s;
}
/*****************************************************************************/
void Player::setAvatar( const QString &a )
{
   identity.avatar = a;
}
/*****************************************************************************/
void Player::setIdentity( Identity *ident )
{
   identity = *ident;
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


//=============================================================================
// End of file Player.cpp
//=============================================================================
