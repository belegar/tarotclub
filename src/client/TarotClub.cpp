/*=============================================================================
 * TarotClub - TarotClub.cpp
 *=============================================================================
 * Graphical TarotClub client class, contains an embedded server.
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

#include <QStatusBar>
#include <QInputDialog>
#include <QMessageBox>
#include "TarotClub.h"
#include "../TarotDeck.h"
#include "ui_NumberedDealUI.h"
#include "ui_WinUI.h"
#include "../Tools.h"

#define SOUTH_CARDS_POS     522

/*****************************************************************************/
TarotClub::TarotClub() : MainWindow()
{
    // Board click events
    connect(tapis, &Canvas::sigViewportClicked, this, &TarotClub::slotClickTapis);
    connect(tapis, &Canvas::sigClickCard, this, &TarotClub::slotClickCard);
    connect(tapis, &Canvas::sigMoveCursor, this, &TarotClub::slotMoveCursor);
    connect(tapis, &Canvas::sigContract, this, &TarotClub::slotSetEnchere);
    connect(tapis, &Canvas::sigAccepteChien, this, &TarotClub::slotAccepteChien);
    connect(tapis, &Canvas::sigPresenterPoignee, this, &TarotClub::slotPresenterPoignee);

    // FIXME show server log
//    connect(&table.GetServer(), &Server::sigServerMessage, serverUI.textBrowser, &QTextBrowser::append);

    // Client events connection
    connect(&client, &Client::sigPlayersList, this, &TarotClub::slotPlayersList);
    connect(&client, &Client::sigMessage, this, &TarotClub::slotMessage);
    connect(&client, &Client::sigAssignedPlace, this, &TarotClub::slotAssignedPlace);
    connect(&client, &Client::sigReceiveCards, this, &TarotClub::slotReceiveCards);
    connect(&client, &Client::sigSelectPlayer, this, &TarotClub::slotSelectPlayer);
    connect(&client, &Client::sigRequestBid, this, &TarotClub::slotRequestBid);
    connect(&client, &Client::sigShowBid, this, &TarotClub::slotShowBid);
    connect(&client, &Client::sigStartDeal, this, &TarotClub::slotStartDeal);
    connect(&client, &Client::sigShowDog, this, &TarotClub::slotShowDog);
    connect(&client, &Client::sigBuildDiscard, this, &TarotClub::slotBuildDiscard);
    connect(&client, &Client::sigDealAgain, this, &TarotClub::slotDealAgain);
    connect(&client, &Client::sigPlayCard, this, &TarotClub::slotPlayCard);
    connect(&client, &Client::sigShowCard, this, &TarotClub::slotShowCard);
    connect(&client, &Client::sigShowHandle, this, &TarotClub::slotShowHandle);
    connect(&client, &Client::sigWaitTrick, this, &TarotClub::slotWaitTrick);
    connect(&client, &Client::sigEndOfDeal, this, &TarotClub::slotEndOfDeal);
    connect(&client, &Client::sigEndOfGame, this, &TarotClub::slotEndOfGame);

    // Game Menu
    connect(newQuickGameAct, SIGNAL(triggered()), this, SLOT(slotNewQuickGame()));
    connect(newTournamentAct, SIGNAL(triggered()), this, SLOT(slotNewTournamentGame()));
    connect(newNumberedDealAct, SIGNAL(triggered()), this, SLOT(slotNewNumberedDeal()));
    connect(newCustomDealAct, SIGNAL(triggered()), this, SLOT(slotNewCustomDeal()));
    connect(netGameClientAct, SIGNAL(triggered()), this, SLOT(slotJoinNetworkGame()));
    connect(netGameServerAct, SIGNAL(triggered()), this, SLOT(slotCreateNetworkGame()));

    //   connect(pliPrecAct, SIGNAL(triggered()), this, SLOT(slotAffichePliPrecedent()));

    // Network chat
    connect(chatDock, &ChatDock::sigEmitMessage, &client, &Client::SendChatMessage);

    // Parameters Menu
    connect(optionsAct, SIGNAL(triggered()), this, SLOT(slotShowOptions()));

    // Exit catching to terminate the game properly
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(slotQuitTarotClub()));
}
/*****************************************************************************/
/**
 * @brief TarotClub::Initialize
 * @return The locale, string format, eg: "fr", "en"
 */
void TarotClub::Initialize()
{
    // Check user's directory and create it if not exists
    if (!QDir(Config::HomePath).exists())
    {
        QDir().mkdir(Config::HomePath);
    }
    if (!QDir(Config::LogPath).exists())
    {
        QDir().mkdir(Config::LogPath);
    }

    clientConfig.Load();

    TarotDeck::Initialize();
    if (tapis->loadCards(clientConfig.GetOptions()) == false)
    {
        qFatal("Cannot load SVG images, exiting...");
    }
    ApplyOptions();

    table.moveToThread(&thread);
    thread.start();
    deal.Initialize();
}
/*****************************************************************************/
/**
 * This method allows a proper cleanup before closing the application
 */
void TarotClub::slotQuitTarotClub()
{
    table.Stop();
}
/*****************************************************************************/
void TarotClub::slotNewTournamentGame()
{
    TarotEngine::Shuffle sh;
    sh.type = TarotEngine::RANDOM_DEAL;
    table.SetShuffle(sh);

    table.CreateGame(Game::TOURNAMENT);
    NewGame("127.0.0.1", DEFAULT_PORT);
    // start game
    table.ConnectBots();
}
/*****************************************************************************/
void TarotClub::slotNewNumberedDeal()
{
    QDialog *widget = new QDialog;
    Ui::NumberedDeal ui;
    ui.setupUi(widget);

    if (widget->exec() == QDialog::Accepted)
    {
        TarotEngine::Shuffle sh;
        sh.type = TarotEngine::NUMBERED_DEAL;
        sh.seed = ui.dealNumber->value();
        table.SetShuffle(sh);

        table.CreateGame(Game::ONE_DEAL);
        NewGame("127.0.0.1", DEFAULT_PORT);
        // start game
        table.ConnectBots();
    }
}
/*****************************************************************************/
void TarotClub::slotNewCustomDeal()
{
    QString fileName = QFileDialog::getOpenFileName(this);

    if (fileName.size() != 0)
    {
        TarotEngine::Shuffle sh;
        sh.type = TarotEngine::CUSTOM_DEAL;
        sh.file = fileName;
        table.SetShuffle(sh);

        table.CreateGame(Game::ONE_DEAL);
        NewGame("127.0.0.1", DEFAULT_PORT);
        // start game
        table.ConnectBots();
    }
}
/*****************************************************************************/
void TarotClub::slotNewQuickGame()
{
    TarotEngine::Shuffle sh;
    sh.type = TarotEngine::RANDOM_DEAL;
    table.SetShuffle(sh);

    table.CreateGame(Game::ONE_DEAL);
    NewGame("127.0.0.1", DEFAULT_PORT);
    // start game
    table.ConnectBots();
}
/*****************************************************************************/
void TarotClub::NewGame(const QString &address, int port)
{
    // GUI initialization
    scoresDock->clear();
    infosDock->Clear();
    tapis->razTapis();
    tapis->resetCards();
    tapis->setFilter(Canvas::BLOCK_ALL);

    // Connect us to the server
    client.Initialize();
    client.Close();
    client.ConnectToHost(address, port);
}
/*****************************************************************************/
void TarotClub::slotJoinNetworkGame()
{
    table.Stop();
    if (joinWizard->exec() == QDialog::Accepted)
    {
        // connect to table
        JoinWizard::Connection cn = joinWizard->GetTableConnection();

        if (cn.isValid)
        {
            NewGame(cn.ip, cn.port);
        }
    }
}
/*****************************************************************************/
void TarotClub::slotCreateNetworkGame()
{
    TarotEngine::Shuffle sh;
    sh.type = TarotEngine::RANDOM_DEAL;
    table.SetShuffle(sh);

    table.CreateGame(Game::ONE_DEAL);
    NewGame("127.0.0.1", DEFAULT_PORT);
}
/*****************************************************************************/
void TarotClub::ApplyOptions()
{
    ClientOptions &options = clientConfig.GetOptions();
    client.SetMyIdentity(options.identity);

    table.LoadConfiguration();

    tapis->showAvatars(options.showAvatars);
    tapis->setBackground(options.backgroundColor);
}
/*****************************************************************************/
void TarotClub::slotShowOptions()
{
    optionsWindow->SetClientOptions(clientConfig.GetOptions());
    optionsWindow->SetServerOptions(table.GetOptions());

    if (optionsWindow->exec() == QDialog::Accepted)
    {
        clientConfig.SetOptions(optionsWindow->GetClientOptions());
        clientConfig.Save();

        table.SaveConfiguration(optionsWindow->GetServerOptions());

        ApplyOptions();
    }
}
/*****************************************************************************/
void TarotClub::showVictoryWindow()
{

    /*
    QGraphicsTextItem *txt;

    QDialog *widget = new QDialog;
    Ui::WinUI ui;
    ui.setupUi(widget);

    QGraphicsScene *scene = new QGraphicsScene();
    QGraphicsSvgItem *victory = new QGraphicsSvgItem(":images/podium.svg");

    ui.tournamentGraph->setScene(scene);
    scene->addItem(victory);
    ui.tournamentGraph->centerOn(victory);
    */
    QList<Place> podium = deal.GetPodium();

    QMessageBox::information(this, trUtf8("Tournament result"),
                             trUtf8("The winner of the tournament is ") + players[podium[0]].name,
                             QMessageBox::Ok);

    /*
    // add the three best players to the image
    txt = scene->addText(players[podium[0]].name);
    txt->setPos(20+150, 450);
    txt = scene->addText(players[podium[1]].name);
    txt->setPos(20, 450);
    txt = scene->addText(players[podium[2]].name);
    txt->setPos(20+300, 450);

    widget->exec();
    */
}
/*****************************************************************************/
void TarotClub::hidePli()
{
    int i;
    Card *c;
    GfxCard *gc;
    Deck &trick = client.GetCurrentTrick();

    for (i = 0; i < trick.size(); i++)
    {
        c = trick.at(i);
        gc = tapis->getGfxCard(c->GetId());
        gc->hide();
    }
}
/*****************************************************************************/
void TarotClub::slotClickTapis()
{
    if (client.GetGameInfo().sequence == Game::SHOW_DOG)
    {
        statusBar()->clearMessage();
        hideChien();
        client.SendSyncDog();
    }
    else if (client.GetGameInfo().sequence == Game::SYNC_TRICK)
    {
        hidePli();
        statusBar()->clearMessage();
        client.SendSyncTrick();
    }
}
/*****************************************************************************/
/**
 * @brief TarotClub::slotMoveCursor
 *
 * This function is called every time the mouse cursor moves
 *
 * @param gc
 */
void TarotClub::slotMoveCursor(GfxCard *gc)
{
    Card *c = tapis->getObjectCard(gc);

    if (client.GetMyDeck().contains(c) == false)
    {
        return;
    }

    if (client.GetGameInfo().sequence == Game::BUILD_DOG)
    {
        if ((c->GetSuit() == Card::TRUMPS) ||
           ((c->GetSuit() != Card::TRUMPS) && (c->GetValue() == 14)))
        {
            tapis->setCursorType(Canvas::FORBIDDEN);
        }
        else
        {
            tapis->setCursorType(Canvas::ARROW);
        }
    }
    else if (client.GetGameInfo().sequence == Game::BUILD_HANDLE)
    {
        if (c->GetSuit() == Card::TRUMPS)
        {
            tapis->setCursorType(Canvas::ARROW);
        }
        else
        {
            tapis->setCursorType(Canvas::FORBIDDEN);
        }
    }
    else if (client.GetGameInfo().sequence == Game::PLAY_TRICK)
    {
        if (client.IsValid(c) == true)
        {
            tapis->setCursorType(Canvas::ARROW);
        }
        else
        {
            tapis->setCursorType(Canvas::FORBIDDEN);
        }
    }
}
/*****************************************************************************/
void TarotClub::slotClickCard(GfxCard *gc)
{
    Card *c = tapis->getObjectCard(gc);

    if (client.GetMyDeck().contains(c) == false)
    {
        return;
    }

    if (client.GetGameInfo().sequence == Game::PLAY_TRICK)
    {
        if (client.IsValid(c) == false)
        {
            return;
        }
        tapis->setFilter(Canvas::BLOCK_ALL);
        statusBar()->clearMessage();

        client.GetMyDeck().removeAll(c);
        client.SendCard(c);

        afficheCartesJoueur(0);

    }
    else if (client.GetGameInfo().sequence == Game::BUILD_DOG)
    {

        if ((c->GetSuit() == Card::TRUMPS) ||
           ((c->GetSuit() != Card::TRUMPS) && (c->GetValue() == 14)))
        {
            return;
        }

        // select one card
        if (gc->GetStatus() == GfxCard::NORMAL)
        {
            if (client.GetDogDeck().size() == 6)
            {
                return;
            }
            client.GetDogDeck().append(c);
            if (client.GetDogDeck().size() == 6)
            {
                tapis->setAccepterChienVisible(true);
            }
        }
        // Un-select card
        else if (gc->GetStatus() == GfxCard::SELECTED)
        {
            if (client.GetDogDeck().size() == 0)
            {
                return;
            }
            client.GetDogDeck().removeAll(c);
            tapis->setAccepterChienVisible(false);
        }
        gc->ToggleStatus();
    }
    else if (client.GetGameInfo().sequence == Game::BUILD_HANDLE)
    {
        if (c->GetSuit() == Card::TRUMPS)
        {
            // select one card
            if (gc->GetStatus() == GfxCard::NORMAL)
            {
                client.GetHandleDeck().append(c);
            }
            else if (gc->GetStatus() == GfxCard::SELECTED)
            {
                client.GetHandleDeck().removeAll(c);
            }
            if ((client.GetHandleDeck().size() == 10) ||
                (client.GetHandleDeck().size() == 13) ||
                (client.GetHandleDeck().size() == 15))
            {
                tapis->setBoutonPoigneeVisible(true);
            }
            else
            {
                tapis->setBoutonPoigneeVisible(false);
            }
            gc->ToggleStatus();
        }
    }
}
/*****************************************************************************/
void TarotClub::slotPlayersList(QMap<Place, Identity> &pl)
{
    players = pl;
    tapis->SetPlayerNames(players, client.GetPlace());
    scoresDock->setPlayers(players);
}
/*****************************************************************************/
void TarotClub::slotReceiveCards()
{
    infosDock->PrintStats(client.GetStatistics());
    tapis->resetCards();
    afficheCartesJoueur(0);
}
/*****************************************************************************/
void TarotClub::slotSelectPlayer(Place p)
{
    tapis->ShowSelection(p, client.GetPlace());
}
/*****************************************************************************/
void TarotClub::slotRequestBid(Contract highestBid)
{
    tapis->ShowBidsChoice(highestBid);
}
/*****************************************************************************/
void TarotClub::slotShowBid(Place p, Contract c)
{
    tapis->ShowBid(p, c, client.GetPlace());
    client.SendSyncBid();
}
/*****************************************************************************/
void TarotClub::slotAccepteChien()
{
    Card *c;
    GfxCard *gc;
    int i;

    for (i = 0; i < client.GetDogDeck().size(); i++)
    {
        c = client.GetDogDeck().at(i);
        client.GetMyDeck().removeAll(c);
        gc = tapis->getGfxCard(c->GetId());
        gc->hide();
    }
    tapis->setAccepterChienVisible(false);
    tapis->setFilter(Canvas::BLOCK_ALL);
    afficheCartesJoueur(0);
    client.SendDog();
}
/*****************************************************************************/
void TarotClub::slotPresenterPoignee()
{
    if (client.TestHandle() == false)
    {
        QMessageBox::information(this, trUtf8("Information"),
                                 trUtf8("Your handle is not valid.\n"
                                        "Showing the fool means that you have no any more trumps in your deck."));
        return;
    }
    tapis->setBoutonPoigneeVisible(false);
    client.SendHandle();
    afficheCartesJoueur(0);
    client.GetGameInfo().sequence = Game::PLAY_TRICK;
    statusBar()->showMessage(trUtf8("Please, play a card, it's your turn!"));
}
/*****************************************************************************/
void TarotClub::slotSetEnchere(Contract cont)
{
    client.SendBid(cont);
    tapis->HideBidsChoice();
}
/*****************************************************************************/
/**
 * @brief TarotClub::afficheCartesJoueur
 *
 * Show current player's cards
 *
 * @param pos = 0 for 18-cards in the hand, otherwise 1 (with cards from the chien)
 */
void TarotClub::afficheCartesJoueur(int pos)
{
    qreal i, x, y, pas;
    Card *c;
    GfxCard *cgfx;

    client.GetMyDeck().Sort();

    if (pos == 1)
    {
        x = 20.0;
        pas = 35.0;
    }
    else
    {
        x = 50.0;
        pas = 40.0;
    }
    y = SOUTH_CARDS_POS;

    for (i = 0; i < client.GetMyDeck().size(); i++)
    {
        c = client.GetMyDeck().at(i);
        cgfx = tapis->getGfxCard(c->GetId());
        cgfx->setPos(x, y);
        cgfx->setZValue(i);
        cgfx->show();
        x = x + pas;
    }
}
/*****************************************************************************/
void TarotClub::slotShowDog()
{
    int i, n, x, y;
    Card *c;
    GfxCard *cgfx;

    n = client.GetDogDeck().size();
    x = 260;
    y = 160;

    for (i = 0; i < n; i++)
    {
        c = client.GetDogDeck().at(i);
        cgfx = tapis->getGfxCard(c->GetId());
        cgfx->setPos(x, y);
        cgfx->setZValue(i);
        cgfx->show();
        x = x + 40;
    }
    statusBar()->showMessage(trUtf8("Click on the board once you have seen the dog."));
}
/*****************************************************************************/
void TarotClub::hideChien()
{
    int i;
    Card *c;
    GfxCard *cgfx;

    for (i = 0; i < client.GetDogDeck().size(); i++)
    {
        c = client.GetDogDeck().at(i);
        cgfx = tapis->getGfxCard(c->GetId());
        cgfx->hide();
    }
}
/*****************************************************************************/
void TarotClub::slotDealAgain()
{
    infosDock->Clear();
    tapis->setFilter(Canvas::BLOCK_ALL);
    tapis->razTapis();

    QMessageBox::information(this, trUtf8("Information"),
                             trUtf8("All the players have passed.\n"
                                    "New deal will begin."));
    client.SendReady();
}
/*****************************************************************************/
void TarotClub::slotBuildDiscard()
{
    Card *c;

    // We add the dog to the player's deck
    for (int i = 0; i < client.GetDogDeck().size(); i++)
    {
        c = client.GetDogDeck().at(i);
        client.GetMyDeck().append(c);
    }
    client.GetDogDeck().clear();
    tapis->setFilter(Canvas::GAME_ONLY);
    // on affiche le deck du joueur + le contenu du chien
    afficheCartesJoueur(1);
    statusBar()->showMessage(trUtf8("Select cards to build your discard."));
}
/*****************************************************************************/
void TarotClub::slotStartDeal(Place p, Contract c)
{
    firstTurn = true;
    infosDock->Clear();
    infosDock->SetContract(c);

    QString name = "ERROR";

    if (players.contains(p))
    {
        name = players.value(p).name;
    }
    infosDock->SetTaker(name, p);
    TarotEngine::Shuffle sh = table.GetShuffle();
    if (sh.type != TarotEngine::CUSTOM_DEAL)
    {
        infosDock->SetDealNumber(sh.seed);
    }
    else
    {
        // Numbered deal
        infosDock->SetDealNumber(-1);
    }
    tapis->setFilter(Canvas::BLOCK_ALL);
    tapis->razTapis(true);
    tapis->ShowTaker(p, client.GetPlace());

    // We are ready, let's inform the server about that
    client.SendSyncStart();
}
/*****************************************************************************/
void TarotClub::slotPlayCard()
{
    tapis->setFilter(Canvas::GAME_ONLY);

    // If we're about to play the first card, the Player is allowed to declare a handle
    if (firstTurn == true)
    {
        firstTurn = false;
        // If a handle exists in the player's deck, we propose to declare it
        if (client.GetStatistics().trumps >= 10)
        {
            int ret = QMessageBox::question(this, trUtf8("Handle"),
                                            trUtf8("You have a handle.\n"
                                                   "Do you want to declare it?"),
                                            QMessageBox::Yes | QMessageBox::No);
            if (ret == QMessageBox::Yes)
            {
                client.GetGameInfo().sequence = Game::BUILD_HANDLE;
                client.GetHandleDeck().clear();
                statusBar()->showMessage(trUtf8("Build your handle."));
            }
        }
    }
    else
    {
        statusBar()->showMessage(trUtf8("It's your turn to play a card."));
    }
}
/*****************************************************************************/
void TarotClub::slotShowCard(int id, Place p)
{
    GfxCard *gc = tapis->getGfxCard(id);
    tapis->DrawCard(gc, p, client.GetPlace());
    infosDock->AddRound(client.GetGameInfo(), p, TarotDeck::GetCard(id)->GetName());

    // We have seen the card, let's inform the server about that
    client.SendSyncCard();
}
/*****************************************************************************/
void TarotClub::slotShowHandle()
{
    // FIXME: show the declared handle on the board

    // We have seen the handle, let's inform the server about that
    client.SendSyncHandle();
}
/*****************************************************************************/
void TarotClub::slotEndOfDeal()
{
    statusBar()->showMessage(trUtf8("End of the deal."));
    tapis->setFilter(Canvas::BLOCK_ALL);
    tapis->razTapis();
    tapis->resetCards();

    resultWindow->SetResult(client.GetScore(), client.GetGameInfo());
    resultWindow->exec();

    /*
     * FIXME:
        - If tournament mode, show the deal winner, then send a sync on window closing
        - If the last turn of a tournament, show the deal result, then sho the podem
        - Otherwise, show the deal winner
     */

    if (client.GetGameInfo().gameMode == Game::TOURNAMENT)
    {
        bool continueGame;

        deal.SetScore(client.GetScore());
        continueGame = deal.AddScore(client.GetGameInfo());
        scoresDock->SetNewScore(deal);

        if (continueGame == true)
        {
            client.SendReady();
        }
        else
        {
            // Continue next deal (FIXME: test if it is the last deal)
            showVictoryWindow();
        }
    }
}
/*****************************************************************************/
/**
 * @brief TarotClub::slotWaitTrick
 *
 * This method is called at the end of each turn, when all the players have
 * played a card.
 */
void TarotClub::slotWaitTrick(Place winner)
{
    infosDock->SelectWinner(client.GetGameInfo(), winner);
    tapis->setFilter(Canvas::BLOCK_ALL);
    statusBar()->showMessage(trUtf8("Click on the board to continue."));

    // launch timer to clean cards, if needed
    if (clientConfig.GetOptions().enableDelayBeforeCleaning == true)
    {
        QTimer::singleShot(clientConfig.GetOptions().delayBeforeCleaning, this, SLOT(slotClickTapis()));
    }
}
/*****************************************************************************/
void TarotClub::slotMessage(const QString &message)
{
    chatDock->message(message);
}
/*****************************************************************************/
void TarotClub::slotAssignedPlace(Place p)
{
    chatDock->message("The server grant you access to the table in place " + Util::ToString(p));
}
/*****************************************************************************/
void TarotClub::slotEndOfGame()
{
}

//=============================================================================
// End of file TarotClub.cpp
//=============================================================================
