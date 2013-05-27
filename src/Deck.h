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

// Qt includes
#include <QList>

// Game includes
#include "Card.h"
#include "defines.h"

/*****************************************************************************/
class DeckStats
{

public:
    DeckStats() {}

    int   nbCards;

    int   atouts;  // nombres d'atouts , en comptant les bouts et l'excuse
    int   bouts;   // 0, 1, 2 ou 3
    int   atoutsMajeurs; // atouts >= 15

    int   rois;
    int   dames;
    int   cavaliers;
    int   valets;

    int   mariages;   // nombre de mariages dans la main
    int   longues;
    int   coupes;     // aucune carte dans une couleur
    int   singletons; // une seule carte dans une couleur
    int   sequences;  // cartes qui se suivent (au moins 5 cartes pour être comptées)

    int   trefles;
    int   pics;
    int   coeurs;
    int   carreaux;

    bool  petit;
    bool  vingtEtUn;
    bool  excuse;
};

/*****************************************************************************/
class Deck : public QList<Card *>
{

public:
    Deck();

    void Shuffle(int seed);
    Card *GetCardById(int);               // Renvoi un pointeur vers une carte
    void Sort();
    QString GetCardList();

private:
    static bool LessThanCards(Card *carte1, Card *carte2);
};

#endif // _DECK_H

//=============================================================================
// End of file Deck.h
//=============================================================================
