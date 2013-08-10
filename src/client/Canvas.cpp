/*=============================================================================
 * TarotClub - Canvas.cpp
 *=============================================================================
 * Visual game contents
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

#include "Canvas.h"
#include <QtCore>
#include <QResizeEvent>
#include <QMouseEvent>
#include "../Tools.h"
#include "../TarotDeck.h"

#define NORTH_BOX_POS_X   350
#define NORTH_BOX_POS_Y   10


static const QPointF coordPlayerBox[5] =
{
    QPointF(NORTH_BOX_POS_X, NORTH_BOX_POS_Y + 452),    // SOUTH
    QPointF(NORTH_BOX_POS_X + 230, NORTH_BOX_POS_Y + 103), // EAST
    QPointF(NORTH_BOX_POS_X, NORTH_BOX_POS_Y),          // NORTH
    QPointF(NORTH_BOX_POS_X - 230, NORTH_BOX_POS_Y + 103), // WEST
    QPointF(0, 0)                                       // NORTH-WEST
};

static const QPointF coordTextBox[5] =
{
    QPointF(NORTH_BOX_POS_X, NORTH_BOX_POS_Y + 412),    // SOUTH
    QPointF(NORTH_BOX_POS_X + 230, NORTH_BOX_POS_Y + 143), // EAST
    QPointF(NORTH_BOX_POS_X, NORTH_BOX_POS_Y + 40),     // NORTH
    QPointF(NORTH_BOX_POS_X - 230, NORTH_BOX_POS_Y + 143), // WEST
    QPointF(0, 0)                                       // NORTH-WEST
};

/*****************************************************************************/
Canvas::Canvas(QWidget *parent)
    : QGraphicsView(parent)
    , cardsPics(0)
{
    setScene(&scene);

    // ==============================================================
    // BIDS BUTTONS
    // ==============================================================
    groupBoutons = new QGroupBox(trUtf8("Bids"), this);
    groupBoutons->hide();

    boutonPasse = new QPushButton(STR_PASSE);
    boutonPrise = new QPushButton(STR_PRISE);
    boutonGarde = new QPushButton(STR_GARDE);
    boutonGardeSans = new QPushButton(STR_GARDE_SANS);
    boutonGardeContre = new QPushButton(STR_GARDE_CONTRE);
    chelem = new QCheckBox("Slam");

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(boutonPasse);
    vbox->addWidget(boutonPrise);
    vbox->addWidget(boutonGarde);
    vbox->addWidget(boutonGardeSans);
    vbox->addWidget(boutonGardeContre);
    vbox->addWidget(chelem);
    groupBoutons->setLayout(vbox);

    groupBoutons->move(10, 300);

    //==============================================================

    boutonAccepterChien = new QPushButton(trUtf8("Accept the discard"), this);
    boutonAccepterChien->move(800, 462);
    boutonAccepterChien->setMinimumSize(boutonAccepterChien->sizeHint());
    boutonAccepterChien->hide();

    //==============================================================

    boutonPresenterPoignee = new QPushButton(trUtf8("Declare a handle"), this);
    boutonPresenterPoignee->move(800, 462);
    boutonPresenterPoignee->setMinimumSize(boutonPresenterPoignee->sizeHint());
    boutonPresenterPoignee->hide();

    //==============================================================
    //       CANVAS ELEMENTS
    //==============================================================

    // 4 players by default
    for (int i = 0; i < 4; i++)
    {
        PlayerBox *pb = new PlayerBox(coordPlayerBox[i], &scene);
        pb->show();
        TextBox *tb = new TextBox(coordTextBox[i], &scene);
        tb->hide();

        playerBox.insert((Place)i, pb);
        textBox.insert((Place)i, tb);
    }

    connect(boutonPasse, SIGNAL(clicked()), this, SLOT(slotBoutton1()));
    connect(boutonPrise, SIGNAL(clicked()), this, SLOT(slotBoutton2()));
    connect(boutonGarde, SIGNAL(clicked()), this, SLOT(slotBoutton3()));
    connect(boutonGardeSans, SIGNAL(clicked()), this, SLOT(slotBoutton4()));
    connect(boutonGardeContre, SIGNAL(clicked()), this, SLOT(slotBoutton5()));
    connect(boutonAccepterChien, SIGNAL(clicked()), this, SLOT(slotAccepteChien()));
    connect(boutonPresenterPoignee, SIGNAL(clicked()), this, SLOT(slotPresenterPoignee()));

    // call mouseEvent as soon as the mouse is moving, without any click buttons
    viewport()->setMouseTracking(true);
    filter = BLOCK_ALL;
}
/*****************************************************************************/
void Canvas::setBackground(const QString &fichier)
{
    QColor color(fichier);
    if (color.isValid())
    {
        scene.setBackgroundBrush(color);
    }
}
/*****************************************************************************/
GfxCard *Canvas::getGfxCard(int i)
{
    return cardsPics.at(i);
}
/*****************************************************************************/
Card *Canvas::getObjectCard(GfxCard *gc)
{
    Card *c = NULL;
    for (int i = 0; i < cardsPics.size(); i++)
    {
        if (cardsPics.at(i) == gc)
        {
            c = TarotDeck::GetCard(i);
            break;
        }
    }
    return(c);
}
/*****************************************************************************/
void Canvas::setAccepterChienVisible(bool v)
{
    if (v == true)
    {
        boutonAccepterChien->show();
    }
    else
    {
        boutonAccepterChien->hide();
    }
}
/*****************************************************************************/
void Canvas::setBoutonPoigneeVisible(bool v)
{
    if (v == true)
    {
        boutonPresenterPoignee->show();
    }
    else
    {
        boutonPresenterPoignee->hide();
    }
}
/*****************************************************************************/
bool Canvas::loadCards(ClientOptions &opt)
{
    int i, j;
    QString varImg;
    QString image;
    QString path;

#ifndef QT_NO_DEBUG
    // Debug, the binary is inside the build directory
    path = qApp->applicationDirPath() + "/../../src/data/cards/default/";
#else
    // Release
    path = qApp->applicationDirPath() +  "/" + opt.deckFilePath + "/";
#endif

    cardsPics.clear();

    //----- 4 couleurs
    for (i = 0; i < 4; i++)
    {
        if (i == 0)
        {
            varImg = "pique";
        }
        else if (i == 1)
        {
            varImg = "coeur";
        }
        else if (i == 2)
        {
            varImg = "trefle";
        }
        else
        {
            varImg = "carreau";
        }

        // de l'as au roi (14 cartes)
        for (j = 0; j < 14; j++)
        {
          //  n = i * 14 + j;
            image = path + varImg + QString("-") + QString().sprintf("%02d.svg", j + 1);

            // Test if file exists
            QFile fileTest(image);
            if (fileTest.exists())
            {
                GfxCard *item = new GfxCard(image);
                item->hide();
                cardsPics.append(item);
                scene.addItem(item);
            }
            else
            {
                return false;
            }
        }
    }

    //----- L'excuse
    image = path + "excuse.svg";

    // Test if file exists
    QFile fileTest(image);
    if (fileTest.exists())
    {
        GfxCard *item = new GfxCard(image);
        item->hide();
        cardsPics.append(item);
        scene.addItem(item);
    }
    else
    {
        return false;
    }

    //----- 21 atouts
    for (i = 57; i < 78; i++)
    {
        image = path + "atout-" + QString().sprintf("%02d.svg", i - 56);

        // Test if file exists
        QFile fileTest(image);
        if (fileTest.exists())
        {
            GfxCard *item = new GfxCard(image);
            item->hide();
            cardsPics.append(item);
            scene.addItem(item);
        }
        else
        {
            return false;
        }
    }

    setCardScale(1.5);

    // Draw shadows under cards after SVG graphics are initialized to get the size
    DrawCardShadows();
    return true;
}
/*****************************************************************************/
void Canvas::resizeEvent(QResizeEvent *e)
{
    QSize s;
    s = e->size();
    setSceneRect(0, 0, s.width(), s.height());
}
/*****************************************************************************/
void Canvas::setCardScale(float factor)
{
    for (int i = 0; i < cardsPics.size(); i++)
    {
        cardsPics.at(i)->setScale(factor);
    }
}
/*****************************************************************************/
void Canvas::mousePressEvent(QMouseEvent *e)
{
    QList<QGraphicsItem *> list;

    emit sigViewportClicked();

    if (filter == BLOCK_ALL)
    {
        return;
    }
    if (filter == GAME_ONLY)
    {
        list = scene.items(e->pos());
        if (!list.isEmpty())
        {
            if (list.first()->type() == GfxCard::Type)
            {
                GfxCard *c = (GfxCard *)list.first();
                emit sigClickCard(c);
            }
        }
    }
}
/*****************************************************************************/
/*
 * Emit a signal each time the cursor goes over a card
 */
void Canvas::mouseMoveEvent(QMouseEvent *e)
{
    QList<QGraphicsItem *> list;

    if (filter == BLOCK_ALL)
    {
        return;
    }

    list = scene.items(e->pos());

    if (!list.isEmpty())
    {
        // Si c'EAST une carte, retourne l'obet, sinon 0
        if (list.first()->type() == GfxCard::Type)
        {
            GfxCard *c = (GfxCard *)list.first();
            emit sigMoveCursor(c);
        }
    }
    else
    {
        setCursorType(ARROW);
    }
}
/*****************************************************************************/
/**
 * n==0 : norma icon with an arrow
 * n==1 : forbidden icon
 */
void Canvas::setCursorType(CursorType t)
{
    if (t == ARROW)
    {
        QCursor cu(Qt::ArrowCursor);
        setCursor(cu);
    }
    else
    {
        QCursor cu(Qt::ForbiddenCursor);
        setCursor(cu);
    }
}
/*****************************************************************************/
void Canvas::colorisePreneur(Place preneur)
{
    playerBox.value(preneur)->highlightPlayer(true);
}
/*****************************************************************************/
void Canvas::setText(Place p, const QString &txt)
{
    playerBox.value(p)->setText(txt);
}
/*****************************************************************************/
void Canvas::setAvatar(Place p, const QString &file)
{
    playerBox.value(p)->setAvatar(file);
}
/*****************************************************************************/
/**
 * @brief Canvas::SwapPlace
 *
 * Change an absolute place, from the server point of view (always SOUTH), in
 * a relative place depending of the player position.
 *
 * @param origin
 * @param absolute
 * @return
 */
Place Canvas::SwapPlace(Place my_place, Place absolute)
{
    // FIXME: generalize this algorithm with the number of players from GameInfo
    Place pl = (Place)((absolute+4-my_place)%4);

    return pl;
}
/*****************************************************************************/
/**
 * Show names on the board, bottom player is always south
 */
void Canvas::setPlayerNames(QMap<Place, Identity> &players, Place p)
{
    QMapIterator<Place, Identity> i(players);
    while (i.hasNext())
    {
        i.next();
        Place rel = SwapPlace(p, i.key());  // relative place

        playerBox.value(rel)->setText(i.value().name);
        playerBox.value(rel)->setAvatar(":/images/avatars/" + i.value().avatar);
    }
}
/*****************************************************************************/
/**
 * @brief Draw a card at a player's position
 *
 * @arg[in] c The graphics card to show
 * @arg[in] p = NORTH, WEST, SOUTH, EAST
 */
void Canvas::DrawCard(GfxCard *c, Place p)
{
    QPointF pos = CalculateCardPosition(p);

    if (c)
    {
        c->setPos(pos);
        c->setZValue(1);
        c->show();
    }
    else
    {
        qFatal("Card is null, cannot display it!");
    }
}
/*****************************************************************************/
/**
 * @brief Calculate the card position, used to draw it
 *
 * @arg[in] p = NORTH, WEST, SOUTH, EAST
 */
QPointF Canvas::CalculateCardPosition(Place p)
{
    qreal x, y, height, width;
    QRectF rect = cardsPics.at(0)->boundingRect();
    height = rect.height();
    width = rect.width();

    x = playerBox.value(p)->rect().x();
    y = playerBox.value(p)->rect().y();

    x = x + (TEXT_BOX_WIDTH - width) / 4;
    if (p == SOUTH)
    {
        y = y - height - TEXT_BOX_HEIGHT - 40;
    }
    else
    {
        y = y + TEXT_BOX_HEIGHT + 10;
    }

    return QPointF(x, y);
}
/*****************************************************************************/
/**
 * @brief Draw a card at a player's position
 *
 * @arg[in] c The graphics card to show
 * @arg[in] p = NORTH, WEST, SOUTH, EAST
 */
void Canvas::DrawCardShadows()
{
    for (int i = 0; i < 4; i++)
    {
        QRectF rect = cardsPics.at(0)->boundingRect();
        QPointF pos = CalculateCardPosition((Place)i);
        CardShadow *shadow = new CardShadow(rect, &scene);

        shadow->hide();
        shadow->setScale(1.5);
        shadow->setPos(pos);
        shadow->setZValue(0);
        cardShadows.insert((Place)i, shadow);
    }
}
/*****************************************************************************/
void Canvas::afficheSelection(Place p)
{
    QMapIterator<Place, PlayerBox *> i(playerBox);
    while (i.hasNext())
    {
        i.next();
        if (i.key() == p)
        {
            i.value()->selectPlayer(true);
        }
        else
        {
            i.value()->selectPlayer(false);
        }
    }
}
/*****************************************************************************/
void Canvas::ShowBid(Place p, Contract cont)
{
    QString txt;

    if (cont == GUARD_AGAINST)
    {
        txt = STR_GARDE_CONTRE;
    }
    else if (cont == GUARD_WITHOUT)
    {
        txt = STR_GARDE_SANS;
    }
    else if (cont == GUARD)
    {
        txt = STR_GARDE;
    }
    else if (cont == TAKE)
    {
        txt = STR_PRISE;
    }
    else
    {
        txt = STR_PASSE;
    }

    textBox.value(p)->setText(txt);
    textBox.value(p)->show();
}
/*****************************************************************************/
void Canvas::cacheEncheres()
{
    QMapIterator<Place, TextBox *> i(textBox);
    while (i.hasNext())
    {
        i.next();
        i.value()->hide();
    }
}
/*****************************************************************************/
void Canvas::ShowBidsChoice(Contract contrat)
{
    boutonPrise->show();
    boutonGarde->show();
    boutonGardeSans->show();
    boutonGardeContre->show();

    if (contrat >= TAKE)
    {
        boutonPrise->hide();
    }
    if (contrat >= GUARD)
    {
        boutonGarde->hide();
    }
    if (contrat >= GUARD_WITHOUT)
    {
        boutonGardeSans->hide();
    }
    if (contrat >= GUARD_AGAINST)
    {
        boutonGardeContre->hide();
    }
    groupBoutons->show();
}
/*****************************************************************************/
void Canvas::HideBidsChoice()
{
    groupBoutons->hide();
}
/*****************************************************************************/
void Canvas::showAvatars(bool b)
{
    QMapIterator<Place, PlayerBox *> i(playerBox);
    while (i.hasNext())
    {
        i.next();
        i.value()->enableAvatar(b);
    }
}
/*****************************************************************************/
void Canvas::razTapis(bool shadow)
{
    cacheEncheres();
    groupBoutons->hide();

    QMapIterator<Place, PlayerBox *> i(playerBox);
    while (i.hasNext())
    {
        i.next();
        i.value()->highlightPlayer(false);
        i.value()->selectPlayer(false);
    }

    for (int c = 0; c < cardShadows.size(); c++)
    {
        if (shadow == true)
        {
            cardShadows.at(c)->show();
        }
        else
        {
            cardShadows.at(c)->hide();
        }

    }
}
/*****************************************************************************/
void Canvas::resetCards()
{
    for (int i = 0; i < cardsPics.size(); i++)
    {
        cardsPics.at(i)->hide();
        cardsPics.at(i)->SetStatus(GfxCard::NORMAL);
    }
}
/*****************************************************************************/
void Canvas::slotBoutton1()
{
    emit sigContract(PASS);
}
/*****************************************************************************/
void Canvas::slotBoutton2()
{
    emit sigContract(TAKE);
}
/*****************************************************************************/
void Canvas::slotBoutton3()
{
    emit sigContract(GUARD);
}
/*****************************************************************************/
void Canvas::slotBoutton4()
{
    emit sigContract(GUARD_WITHOUT);
}
/*****************************************************************************/
void Canvas::slotBoutton5()
{
    emit sigContract(GUARD_AGAINST);
}
/*****************************************************************************/
void Canvas::slotAccepteChien()
{
    emit sigAccepteChien();
}
/*****************************************************************************/
void Canvas::slotPresenterPoignee()
{
    emit sigPresenterPoignee();
}
/*****************************************************************************/
void Canvas::setFilter(Filter f)
{
    filter = f;
}

//=============================================================================
// End of file Tapis.cpp
//=============================================================================
