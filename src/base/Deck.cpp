/*=============================================================================
 * Tarot Club - Deck.cpp
 *=============================================================================
 * A deck composed of a (dynamic) list of cards.
 *=============================================================================
 * Tarot Club est un jeu de Tarot français
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

#include "Deck.h"
// Standard includes
#ifndef QT_NO_DEBUG
   #include <iostream>
   #include <fstream>
#endif // _DEBUG

#include <QtAlgorithms>

#include <cstdlib>
#include <ctime>
using namespace std;

/*****************************************************************************/
Deck::Deck()
{

}
/*****************************************************************************/
/**
 * Méthode "Quick and dirty" pour mélanger les cartes du jeu
 * Principe : on assigne à chaque carte un nombre au hasard entre 0 et 10000,
 * puis on range les cartes dans l'ordre croissant
 * si hadard == true, la graine est choisie entre 0 et 65000, sinon on 
 * utilise la graine passée en paramètre
 */
unsigned int Deck::shuffle( bool hasard, unsigned int seed )
{
   int i, ok, temp;
   int nbCards;
   int *cartesId;
   unsigned int graine;

   if( hasard == true ){
      graine = qrand()%65000;
   } else {
      graine = seed;
      srand( seed );
   }

   nbCards = this->count();
   cartesId = new int[nbCards];

#ifndef QT_NO_DEBUG
   // Affichage avant le mélange
   ofstream f("cartes_avant.txt");
   
   f << "Nombre de cartes : " << nbCards << endl << endl;


   for( i=0; i<nbCards; i++) {
      Card *c;
      c = this->at(i);
      if (f.is_open()) {
         f << "Couleur : " << c->getColor() << endl;
         f << "Type : " << c->getType() << endl;
         f << "Valeur : " << c->getValue() << endl;
         f << "Id : " << c->getId() << endl;
         f << endl;
      }
   }
   f.close();
#endif // _DEBUG

   // On assigne à chaque carte un nombre aléatoire
   for( i=0; i<nbCards; i++) {
      cartesId[i] = qrand()%10000;
   }

   // On range ensuite les cartes dans l'ordre croissant de ces nombres aléatoires
   // Tri à bulle classique
   do {
      ok = 1;
      for( i=1; i<nbCards; i++) {
         if ( cartesId[i-1]>cartesId[i] ) {
            ok = 0;
            // on échange le tableau de valeurs
            temp = cartesId[i-1];
            cartesId[i-1] = cartesId[i];
            cartesId[i] = temp;
            // et les cartes correspondantes
            this->swap( i-1, i );
         }
      }
   }
   while(!ok);

#ifndef QT_NO_DEBUG
   // on affiche les cartes mélangées
   
   f.open("cartes_apres.txt");
   f << "Nombre de cartes : " << nbCards << endl << endl;

   for( i=0; i<nbCards; i++) {
      Card *c;
      c = this->at(i);
      if (f.is_open()) {
         f << "Couleur : " << c->getColor() << endl;
         f << "Type : " << c->getType() << endl;
         f << "Valeur : " << c->getValue() << endl;
         f << "Tri : " << cartesId[i] << endl;
         f << "Id : " << c->getId() << endl;
         f << endl;
      }
   }
   f.close();
#endif // _DEBUG

   delete[] cartesId;
   return graine;
}
/*****************************************************************************/
Card *Deck::getCardById( int id )
{
   for( int i = 0; i<this->size(); ++i) {
     if( this->at(i)->getId() == id )
         return this->at(i);
   }
   return (NULL);
}
/*****************************************************************************/
void Deck::sort()
{
   qSort( this->begin(), this->end(), lessThanCards );
}
/*****************************************************************************/
bool Deck::lessThanCards(Card *carte1, Card *carte2 )
{
   return carte1->getId() > carte2->getId();
}
//=============================================================================
// End of file Deck.cpp
//=============================================================================
