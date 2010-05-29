/*=============================================================================
 * TarotClub - Player.cpp
 *=============================================================================
 * Base class for all players.
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * This file must be used under the terms of the CeCILL.
 * This source file is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at
 * http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
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
 * Retourne true si la carte est présente dans les main du joueur, sinon false
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
 * Teste si la carte cVerif peut être jouée : cela dépend des cartes déjà
 * jouées sur le tapis et des cartes dans la main du joueur
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

   // première carte (couleur demandée)
   CardType type;
   CardColor   coul;
   int         val;
   //////////////

   bool possedeCouleur=false; // vrai si le joueur posseède la couleur demandee
   bool possedeAtout=false;   // vrai si le joueur possède un atout
   bool precedentAtout=false; // vrai si un atout max précédent
   int  precAtoutMax=0;
   int  atoutMax=0; // atout maxi possédé
   bool ret=true;
   Card *c;
   int i,n;

   // on cherche la couleur demandée
   c = mainDeck->at( debut ); // première carte jouée à ce tour

   type = c->getType();
   coul = c->getColor();
   val  = c->getValue();
   if( type == EXCUSE ) { // aïe, le joueur a commencé avec une excuse
      // le joueur peut jouer n'importe quelle carte après l'excuse, c'est lui qui décide alors de la couleur
      if( rang == 2 ) {
         return true;
      }
      c = mainDeck->at( debut + 1 ); // la couleur demandée est donc la seconde carte
      type = c->getType();
      coul = c->getColor();
      val  = c->getValue();
   }

   if( type == CARTE && cVerif->getColor() == coul ) {
      return true;
   }

   // Quelques indications sur les plis précédents
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

   // cas 1 : le type demandé est ATOUT
   if( type == ATOUT ) {
      if( cVerif->getType() == ATOUT ) {
         if( cVerif->getValue() > precAtoutMax ) {
            return true;
         } else {
            if( atoutMax > precAtoutMax ) {
               return false;  // doit surcouper !
            } else {
               return true;   // sinon tant pis, on doit quand même jouer la couleur demandée
            }
         }
      } else {
         if( possedeAtout == true ) {
            return false;
         } else {
            return true;
         }
      }
   } else {// cas 2 : le type demandé est CARTE
      // le joueur possède la couleur demandée, il doit donc la jouer cela
      if( possedeCouleur == true ) {
         return false;
      } else { // pas la couleur demandée
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
               return true; // non, il peut se défausser
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
void Player::setPlace(Place p)
{
   place = p;
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
   return place;
}

//=============================================================================
// End of file Player.cpp
//=============================================================================
