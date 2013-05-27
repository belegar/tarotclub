/*=============================================================================
 * TarotClub - Bot.h
 *=============================================================================
 * Modélise un robot joueur. Interface vers les scripts d'IA
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

#ifndef _BOT_H
#define _BOT_H

#include "Client.h"
#include <QScriptEngine>
#include <QtScriptTools>

class StatsWrapper: public QObject
{
    Q_OBJECT
public:

    StatsWrapper(DeckStats &stats)
        : mDeck(stats)
    {

    }

    DeckStats &mDeck;

public slots:
    int getNumberOfCards()
    {
        return mDeck.nbCards;
    }

    int getNumberOfAtouts()
    {
        return mDeck.atouts;
    }

    int getNumberOfBouts()
    {
        return mDeck.bouts;   // 0, 1, 2 ou 3
    }

    int getNumberOfAtoutsMajeurs()
    {
        return mDeck.atoutsMajeurs; // atouts >= 15
    }

    int getNumberOfRois()
    {
        return mDeck.rois;
    }

    int getNumberOfDames()
    {
        return mDeck.dames;
    }

    int getNumberOfCavaliers()
    {
        return mDeck.cavaliers;
    }

    int getNumberOfValets()
    {
        return mDeck.valets;
    }

    int getNumberOfMariages()
    {
        return mDeck.mariages;   // nombre de mariages dans la main
    }

    int getNumberOfLongues()
    {
        return mDeck.longues;
    }

    int getNumberOfCoupes()
    {
        return mDeck.coupes;     // aucune carte dans une couleur
    }

    int getNumberOfSingletons()
    {
        return mDeck.singletons; // une seule carte dans une couleur
    }

    int getNumberOfSequences()
    {
        return mDeck.sequences;  // cartes qui se suivent (au moins 5 cartes pour être comptées)
    }

    int getNumberOfTrefles()
    {
        return mDeck.trefles;
    }

    int getNumberOfPics()
    {
        return mDeck.pics;
    }

    int getNumberOfCoeurs()
    {
        return mDeck.coeurs;
    }

    int getNumberOfCarreaux()
    {
        return mDeck.carreaux;
    }

    bool hasPetits()
    {
        return mDeck.petit;
    }

    bool hasVingtEtUn()
    {
        return mDeck.vingtEtUn;
    }

    bool hasExcuse()
    {
        return mDeck.excuse;
    }

};
/*****************************************************************************/
class DeckWrapper: public QObject
{
    Q_OBJECT
public:

    DeckWrapper(Deck &i_botDeck, Deck &i_mainDeck)
        :   mBotDeck(i_botDeck)
        ,   mMainDeck(i_mainDeck)
    {

    }

    Deck &mBotDeck;
    Deck &mMainDeck;    // cards played in previous and current turns

public slots:
    QString GetBotCards()
    {
        return mBotDeck.GetCardList();
    }

    QString GetMainCards()
    {
        return mMainDeck.GetCardList();
    }
};
/*****************************************************************************/
class Bot : public Client
{
    Q_OBJECT

private:
    QTimer  timeBeforeSend;
    QScriptEngine botEngine;

#ifndef QT_NO_DEBUG
    QScriptEngineDebugger debugger;
#endif

    // Exposed object to the Javascript
    StatsWrapper statsObj;
    DeckWrapper deckObj;

    bool initializeScriptContext();

public:
    Bot();
    ~Bot();

    void setTimeBeforeSend(int t);

    // Private slots are not visible in a QtScript
private slots:
    void slotTimeBeforeSend();

    // client events
    void slotMessage(const QString &text);
    void slotReceptionCartes();
    void slotAfficheSelection(Place p);
    void slotChoixEnchere(Contrat highestBid);
    void slotAfficheEnchere(Place place, Contrat contract);
    void slotRedist();
    void slotAfficheChien();
    void slotPrepareChien();
    void slotDepartDonne(Place i_taker, Contrat i_contract);
    void slotJoueCarte();
    void slotAfficheCarte(int id, Place p);
    void slotFinDonne(Place, float, bool lastDeal);
    void slotWaitPli(Place p, float points);


};

#endif // _BOT_H

//=============================================================================
// End of file Bot.h
//=============================================================================
