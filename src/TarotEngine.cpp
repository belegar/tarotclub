/*=============================================================================
 * TarotClub - TarotEngine.cpp
 *=============================================================================
 * Moteur de jeu principal + serveur de jeu réseau
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

#include <QtNetwork>
#include <QCoreApplication>
#include "TarotEngine.h"
#include "DealEditorFile.h"
#include "Identity.h"

#ifndef QT_NO_DEBUG
#include <iostream>
#include <fstream>
using namespace std;
#endif // QT_NO_DEBUG

/*****************************************************************************/
TarotEngine::TarotEngine()
{
    dealNumber = 0;
    dealType = RANDOM_DEAL;
    gameMode = LOCAL_ONEDEAL;
}
/*****************************************************************************/
TarotEngine::~TarotEngine()
{

}
/*****************************************************************************/
void TarotEngine::SetGameMode(GameMode mode)
{
    gameMode = mode;
}
/*****************************************************************************/
void TarotEngine::SetDealNumber(int deal)
{
    dealNumber = deal;
}
/*****************************************************************************/
void TarotEngine::SetDealType(DealType type)
{
    dealType = type;
}
/*****************************************************************************/
void TarotEngine::SetDealFile(QString file)
{
    dealFile = file;
}
/*****************************************************************************/
void TarotEngine::StopGame()
{
    gameState.Stop();
}
/*****************************************************************************/
Player &TarotEngine::GetPlayer(Place p)
{
    return players[p];
}
/*****************************************************************************/
Score &TarotEngine::GetScore()
{
    return score;
}
/*****************************************************************************/
int TarotEngine::GetDealNumber()
{
    return dealNumber;
}
/*****************************************************************************/
bool TarotEngine::IsCardValid(Card *c, Place p)
{
    bool ret = false;

    if (sequence == CHIEN)
    {
        if (c->getType() == ATOUT || c->getType() == EXCUSE ||
                (c->getType() == CARTE && c->getValue() == 14))
        {
            ret = false;
        }
        else
        {
            ret = true;
        }
    }
    else if (sequence == GAME)
    {
        ret =  players[p].CanPlayCard(&mainDeck, c, gameState);
    }

    return (ret);
}
/*****************************************************************************/
bool TarotEngine::HasCard(Card *c, Place p)
{
    return players[p].HasCard(c);
}
/*****************************************************************************/
Place TarotEngine::CalculateTrickWinner()
{
    Card *c;

    // First card
    Card::Type type;
    Card::Suit suit;
    Place pl = NORD;

    int i, debut = 0;
    int leader = 0; // 0 est le premier joueur a joué etc. jusqu'à 4
    // par défaut, celui qui entame est le leader (car couleur demandée)

    // étape 1 : on cherche la couleur demandée

    c = mainDeck.at(gameState.FirstCard());   // première carte jouée à ce tour

    type = c->getType();
    color = c->getColor();

    // aïe, le joueur a commencé avec une excuse
    if (type == EXCUSE)
    {
        c = mainDeck.at(infos.gameCounter - NB_PLAYERS + 1);   // la couleur demandée est donc la seconde carte
        debut = 1;
        type = c->getType();
        color = c->getColor();
        leader = 2;
    }

    debut = debut + infos.gameCounter - NB_PLAYERS + 1 ; // le début est décalé si le premier pli est l'excuse

    int  drapeau; // indique une coupe, donc le leader est forcément un atout
    int  valLeader;
    valLeader = c->getValue();

    if (type == ATOUT)
    {
        drapeau = 1;   // on cherche une éventuelle surcoupe
    }
    else
    {
        drapeau = 0;   // on cherche une coupe
    }

    for (i = debut; i < infos.gameCounter; i++)
    {
        c = mainDeck.at(i);

        // coupe !
        if (c->getType() == ATOUT)
        {
            if (c->getValue() > valLeader && drapeau == 1)    // surcoupe ??
            {
                valLeader = c->getValue();
                leader = i - (infos.gameCounter - NB_PLAYERS);
            }
            else if (drapeau == 0)
            {
                valLeader = c->getValue();
                leader = i - (infos.gameCounter - NB_PLAYERS);
                drapeau = 1;
            }

            // pas de coupe détectée
        }
        else if (c->getType() == CARTE && drapeau == 0)
        {
            if (c->getColor() == color)   // ahah, même couleur !
            {
                if (c->getValue() > valLeader)
                {
                    valLeader = c->getValue();
                    leader = i - (infos.gameCounter - NB_PLAYERS);
                }
            }
        }
    }

    // place du leader
    if (tour == SUD)
    {
        Place tab[] = {EST, NORD, OUEST, SUD};
        pl = tab[leader];
    }
    else if (tour == EST)
    {
        Place tab[] = {NORD, OUEST, SUD, EST};
        pl = tab[leader];
    }
    else if (tour == NORD)
    {
        Place tab[] = {OUEST, SUD, EST, NORD};
        pl = tab[leader];
    }
    else
    {
        Place tab[] = {SUD, EST, NORD, OUEST};
        pl = tab[leader];
    }

    return pl;
}
/*****************************************************************************/
void TarotEngine::NewDeal()
{
    if (dealType == CUSTOM_DEAL)
    {
        CustomDeal();
    }
    else
    {
        RandomDeal();
    }
    score.reset();
    donneur = nextPlayer(donneur);
    infos.contrat = PASSE;
    infos.gameCounter = 0;
    tour = nextPlayer(donneur); // Le joueur à la droite du donneur commence les enchères
    selectPlayer(tour);
    askBid(infos.contrat);
}
/*****************************************************************************/
#ifndef QT_NO_DEBUG
/**
 * @brief Generate a file with all played cards of the deal
 *
 * This file also contains useful information such as ower of cards, points ...
 * The log contains 2 parts, the main deck and the dog, each parts has 4 lines
 * line 1: name of cards
 * line 2: points of each card
 * line 3: the original owner of the card
 * line 4: the final owner of the card
 */
void TarotEngine::GenerateEndDealLog()
{
    ofstream f("round_cards.csv");

    QString line1 = "Card;" + mainDeck.GetCardList();
    QString line2 = "Points";
    QString line3 = "Base owner";
    QString line4 = "Final owner";

    // Card type
    for (int j = 0; j < mainDeck.size(); j++)
    {
        QString n;
        Card *c = mainDeck.at(j);
        line2 += ";" + n.setNum(c->getPoints());
        line3 += ";" + n.setNum((int)c->getOwner());
        line4 += ";" + n.setNum(score.getPli(j));
    }

    f << "Main deck" << endl;
    f << line1.toStdString() << "\n" << line2.toStdString() << "\n" << line3.toStdString() << "\n" << line4.toStdString() << "\n\n";

    line1 = line2 = line3 = line4 = "";

    // Card type
    line1 = deckChien.GetCardList();
    for (int j = 0; j < deckChien.size(); j++)
    {
        QString n;
        Card *c = deckChien.at(j);
        line2 += n.setNum(c->getPoints()) + ";";
        line3 += n.setNum((int)c->getOwner()) + ";";
        line4 += n.setNum(score.getPli(j)) + ";";
    }
    f << "Dog deck" << endl;
    f << line1.toStdString() << "\n" << line2.toStdString() << "\n" << line3.toStdString() << "\n" << line4.toStdString() << "\n\n";

    f << "Game infos" << endl;
    f << "Taker;" << infos.preneur << endl << "Bid;" << infos.contrat << endl;

    f.close();
}
#endif // QT_NO_DEBUG
/*****************************************************************************/
void TarotEngine::GameSateMachine()
{
    bool ret;
    float points;
    bool lastDeal;

    if (!(infos.gameCounter % NB_PLAYERS) && infos.gameCounter)
    {
        ret = finLevee(points);

        // end of the deal?
        if (ret == true)
        {
#ifndef QT_NO_DEBUG
            GenerateEndDealLog();
#endif
            dealCounter++;
            // end of deal, send score to all players
            if (dealCounter < MAX_ROUNDS && gameType == LOC_TOURNAMENT)
            {
                lastDeal = false;
                sequence = SEQ_WAIT_PLAYER;
            }
            else
            {
                lastDeal = true;
                gameState.state = GAME_FINISHED;
                sequence = VIDE;
            }

            cptVuDonne = 0;
            sendFinDonne(score.getScoreInfos(), lastDeal, points);
        }
        else
        {
            cptVuPli = 0;
            sequence = SEQ_WAIT_PLI;
            sendWaitPli(points);
        }
    }
    else
    {
        tour = nextPlayer(tour);
        gameState++;
        selectPlayer(tour);
        sendJoueCarte();
    }
}
/*****************************************************************************/
void TarotEngine::BidSequence()
{
    if (tour == donneur)
    {
        if (infos.contrat == PASSE)
        {
            // si tous les joueurs ont passé on s'arrête et on prévient tout le monde
            cptVuDonne = 0;
            sequence = SEQ_WAIT_PLAYER;
            sendRedist();
        }
        else
        {
            montreChien();
        }
    }
    else
    {
        tour = nextPlayer(tour);
        selectPlayer(tour);
        askBid(infos.contrat);
    }
}
/*****************************************************************************/
void TarotEngine::ShowDog()
{
    int i;
    Card *c;

    // On précise quel joueur possède le chien en fonction du contrat
    for (i = 0; i < deckChien.count(); i++)
    {
        c = deckChien.at(i);
        if (infos.contrat != GARDE_CONTRE)
        {
            c->setOwner(infos.preneur);
        }
        else
        {
            // En cas de garde contre, le chien appartient à la défense
            c->setOwner(HYPERSPACE);
        }
    }

    if (infos.contrat == GARDE_SANS || infos.contrat == GARDE_CONTRE)
    {
        // On n'affiche pas le chien et on commence la partie immédiatement
        sendDepartDonne();
        jeu();
        return;
    }

    cptVuChien = 0;
    sequence = WAIT_CHIEN; // et on attend le chien écarté par le preneur
    sendShowChien(); // Prise ou garde, on affiche le chien chez tout le monde
}
/*****************************************************************************/
bool TarotEngine::EndOfTrick(float &points)
{
    int i;
    Card *c;

#ifndef QT_NO_DEBUG
    QFile f("debug.txt");
    QTextStream fout(&f);
    f.open(QIODevice::Append | QIODevice::Text);
    fout << "----- tour avant : " << tour << endl;
#endif

    // On détecte qui a remporté le pli, c'est lui qui entame le prochain tour
    tour = calculGagnantPli();

    points = 0;
    for (i = (infos.gameCounter - NB_PLAYERS); i < infos.gameCounter; i++)
    {
        c = mainDeck.at(i);
        score.setPli(i, tour);
        // calculate points won by the taker, with the special case of the Excuse
        if (tour == infos.preneur)
        {
            // here, the taker won the round but the Excuse stays to the Defenders
            if (c->getType() != EXCUSE)
            {
                points += c->getPoints();
            }
            else
            {
                points += 0.5;
            }
        }
        else
        {
            // here, the taker lost the round, he can keep his Excuse
            if (c->getType() == EXCUSE && c->getOwner() == tour)
            {
                points += c->getPoints() + 0.5;
            }
        }

#ifndef QT_NO_DEBUG
        fout << c->getId() << " i=" << i << ", tour=" << tour << " ";
#endif
    }

#ifndef QT_NO_DEBUG
    fout << endl << "-----" << endl;
    f.close();
#endif

    if (infos.gameCounter < 72)
    {
        return false;
    }
    else     // fin du jeu
    {
        score.calcul(mainDeck, deckChien, infos);
        return true;
    }

}
/*****************************************************************************/
/**
 * Use a custom deal for the game
 */
void TarotEngine::CustomDeal()
{
    int j;
    Card *c;
    quint8 params[NB_HAND_CARDS] = {0};   // cartes du joueur (3 joueurs: 24 cartes, 4j:18, 5j:15)

    DealEditorFile editor;
    editor.loadFile(dealFile);

    QMapIterator<QTcpSocket *, Player *> i(players);
    while (i.hasNext())
    {
        i.next();
        i.value()->emptyDeck();
        Place p = i.value()->getPlace();
        Deck *d;

        if (p == SUD)
        {
            d = &editor.southDeck;
        }
        else if (p == EST)
        {
            d = &editor.eastDeck;
        }
        else if (p == OUEST)
        {
            d = &editor.westDeck;
        }
        else
        {
            d = &editor.northDeck;
        }

        for (j = 0; j < NB_HAND_CARDS; j++)
        {
            c = d->at(j);
            if (c->getType() == EXCUSE)
            {
                score.setExcuse(p);
            }
            c->setOwner(p);
            i.value()->addCard(c);
            params[j] = c->getId();
        }
        sendCards(p, params);
    }

    deckChien = editor.chienDeck;
    mainDeck.clear();
}
/*****************************************************************************/
/**
 * On distribue les cartes entre les joueurs et le chien
 */
void TarotEngine::RandomDeal()
{
    int n, j;
    Card *c;
    int index;
    Place p;
    quint8 params[NB_HAND_CARDS] = {0};   // cartes du joueur (3 joueurs: 24 cartes, 4j:18, 5j:15)


    // on ajoute les cartes dans le deck
    mainDeck.clear();
    for (int i = 0; i < 78; i++)
    {
        mainDeck.append(Jeu::getCard(i));
    }
    if (dealType == RANDOM_DEAL)
    {
        dealNumber = qrand() % RAND_MAX;
    }
    mainDeck.shuffle(dealNumber);
    deckChien.clear();

    n = 0;
    QMapIterator<QTcpSocket *, Player *> i(players);
    while (i.hasNext())
    {
        i.next();
        i.value()->emptyDeck();
        p = i.value()->getPlace();

        for (j = 0; j < NB_HAND_CARDS; j++)
        {
            index = n * NB_HAND_CARDS + j;
            c = mainDeck.at(index);
            if (c->getType() == EXCUSE)
            {
                score.setExcuse(p);
            }
            c->setOwner(p);
            i.value()->addCard(c);
            params[j] = c->getId();
        }
        sendCards(p, params);
        n++;
    }

    // Les cartes restantes vont au chien
    for (j = 78 - NB_CHIEN_CARDS; j < 78; j++)
    {
        c = mainDeck.at(j);
        deckChien.append(c);
    }
    mainDeck.clear();
}


//=============================================================================
// End of file TarotEngine.cpp
//=============================================================================
