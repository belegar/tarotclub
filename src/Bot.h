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

    StatsWrapper(Deck::Statistics &stats)
        : mDeck(stats)
    {

    }

    Deck::Statistics &mDeck;

public slots:
    int getNumberOfCards()
    {
        return mDeck.nbCards;
    }

    int getNumberOfAtouts()
    {
        return mDeck.trumps;
    }

    int getNumberOfBouts()
    {
        return mDeck.oudlers;   // 0, 1, 2 ou 3
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
class Bot : public Client
{
    Q_OBJECT

public:
    Bot();
    ~Bot();

    void SetTimeBeforeSend(int t);

private:
    QTimer  timeBeforeSend;
    QScriptEngine botEngine;
    // Exposed object to the Javascript
    StatsWrapper statsObj;

#ifndef QT_NO_DEBUG
    QScriptEngineDebugger debugger;
#endif

    QScriptValue CallScript(const QString &function, QScriptValueList &args);
    bool InitializeScriptContext();

private slots:
    void slotTimeBeforeSend();

    // client events
    void slotMessage(const QString &message);
    void slotAssignedPlace(Place p);
    void slotPlayersList(QList<Identity> &players);
    void slotReceiveCards();
    void slotSelectPlayer(Place p);
    void slotRequestBid(Contract highestBid);
    void slotShowBid(Place, Contract contract);
    void slotStartDeal(Place, Contract contract);
    void slotShowDog();
    void slotBuildDiscard();
    void slotDealAgain();
    void slotPlayCard();
    void slotShowCard(int id, Place p);
    void slotWaitTrick(Place winner);
    void slotEndOfDeal();
    void slotEndOfGame();


};

#endif // _BOT_H

//=============================================================================
// End of file Bot.h
//=============================================================================
