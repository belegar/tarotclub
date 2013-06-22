/*=============================================================================
 * TarotClub - Tapis.cpp
 *=============================================================================
 * visual game contents
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

#include "Tapis.h"
#include <QtCore>
#include <QResizeEvent>
#include <QMouseEvent>
#include "textes.h"
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
GfxCard::GfxCard(const QString &fileName, QGraphicsItem *parent) : QGraphicsSvgItem(fileName, parent)

{
    status = NORMAL;
}
/*****************************************************************************/
int GfxCard::type() const
{
    // Enable the use of qgraphicsitem_cast with this item.
    return Type;
}
/*****************************************************************************/
GfxCard::Status GfxCard::GetStatus()
{
    return status;
}
/*****************************************************************************/
void GfxCard::SetStatus(GfxCard::Status s)
{
    status = s;
}
/*****************************************************************************/
void GfxCard::ToggleStatus()
{
    if (status == NORMAL)
    {
        status = SELECTED;
        this->moveBy(0.0, -20.0);
    }
    else
    {
        status = NORMAL;
        this->moveBy(0.0, 20.0);
    }
}


/*****************************************************************************/
//          *                          *                        *
/*****************************************************************************/


Tapis::Tapis(QWidget *parent)
    : QGraphicsView(parent)
    , cardsPics(0)
{
    setScene(&scene);

    //==============================================================
    //       BOUTONS ENCHERES
    //==============================================================
    groupBoutons = new QGroupBox(trUtf8("Enchères"), this);
    groupBoutons->hide();

    boutonPasse = new QPushButton(STR_PASSE);
    boutonPrise = new QPushButton(STR_PRISE);
    boutonGarde = new QPushButton(STR_GARDE);
    boutonGardeSans = new QPushButton(STR_GARDE_SANS);
    boutonGardeContre = new QPushButton(STR_GARDE_CONTRE);
    chelem = new QCheckBox("Chelem");

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

    boutonAccepterChien = new QPushButton(trUtf8("Accepter le chien"), this);
    boutonAccepterChien->move(800, 462);
    boutonAccepterChien->setMinimumSize(boutonAccepterChien->sizeHint());
    boutonAccepterChien->hide();

    //==============================================================

    boutonPresenterPoignee = new QPushButton(trUtf8("PrÃ©senter poignÃ©e"), this);
    boutonPresenterPoignee->move(800, 462);
    boutonPresenterPoignee->setMinimumSize(boutonPresenterPoignee->sizeHint());
    boutonPresenterPoignee->hide();

    //==============================================================
    //       ELEMENTS DU CANVAS
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
    filter = AUCUN;
}
/*****************************************************************************/
void Tapis::setBackground(const QString &fichier)
{
    QColor color(fichier);
    if (color.isValid())
    {
        scene.setBackgroundBrush(color);
    }
}
/*****************************************************************************/
GfxCard *Tapis::getGfxCard(int i)
{
    return cardsPics.at(i);
}
/*****************************************************************************/
Card *Tapis::getObjectCard(GfxCard *gc)
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
void Tapis::setAccepterChienVisible(bool v)
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
void Tapis::setBoutonPoigneeVisible(bool v)
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
bool Tapis::loadCards(ClientOptions &opt)
{
    int i, j, n;
    QString varImg;
    QString image;
    QString path = QCoreApplication::applicationDirPath() + "/" + opt.deckFilePath + "/";

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
            n = i * 14 + j;
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
void Tapis::resizeEvent(QResizeEvent *e)
{
    QSize s;
    s = e->size();
    setSceneRect(0, 0, s.width(), s.height());
}
/*****************************************************************************/
void Tapis::setCardScale(float factor)
{
    for (int i = 0; i < cardsPics.size(); i++)
    {
        cardsPics.at(i)->setScale(factor);
    }
}
/*****************************************************************************/
void Tapis::mousePressEvent(QMouseEvent *e)
{
    QList<QGraphicsItem *> list;

    emit sigViewportClicked();

    if (filter == AUCUN)
    {
        return;
    }
    if (filter == JEU)
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
void Tapis::mouseMoveEvent(QMouseEvent *e)
{
    QList<QGraphicsItem *> list;

    if (filter == AUCUN)
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
void Tapis::setCursorType(CursorType t)
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
void Tapis::colorisePreneur(Place preneur)
{
    playerBox.value(preneur)->highlightPlayer(true);
}
/*****************************************************************************/
void Tapis::setText(Place p, const QString &txt)
{
    playerBox.value(p)->setText(txt);
}
/*****************************************************************************/
void Tapis::setAvatar(Place p, const QString &file)
{
    playerBox.value(p)->setAvatar(file);
}
/*****************************************************************************/
/**
 * @brief Tapis::SwapPlace
 *
 * Change an absolute place, from the server point of view (always SOUTH), in
 * a relative place depending of the player position.
 *
 * @param origin
 * @param absolute
 * @return
 */
Place Tapis::SwapPlace(Place origin, Place absolute)
{
    Place pl = SOUTH;

    if (origin == WEST)
    {
        Place tab[] = {EAST, NORTH, WEST, SOUTH};
        pl = tab[absolute];
    }
    else if (origin == NORTH)
    {
        Place tab[] = {NORTH, WEST, SOUTH, EAST};
        pl = tab[absolute];
    }
    else if (origin == EAST)
    {
        Place tab[] = {WEST, SOUTH, EAST, NORTH};
        pl = tab[absolute];
    }
    else
    {
        Place tab[] = {SOUTH, EAST, NORTH, WEST};
        pl = tab[absolute];
    }

    return (pl);
}
/*****************************************************************************/
/**
 * Show names on the board, bottom player is always south
 */
void Tapis::setPlayerNames(QMap<Place, Identity> &players, Place p)
{
    p = SwapPlace(p, SOUTH);   // always south

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
void Tapis::DrawCard(GfxCard *c, Place p)
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
QPointF Tapis::CalculateCardPosition(Place p)
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
void Tapis::DrawCardShadows()
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
void Tapis::afficheSelection(Place p)
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
void Tapis::ShowBid(Place p, Contract cont)
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
void Tapis::cacheEncheres()
{
    QMapIterator<Place, TextBox *> i(textBox);
    while (i.hasNext())
    {
        i.next();
        i.value()->hide();
    }
}
/*****************************************************************************/
void Tapis::ShowBidsChoice(Contract contrat)
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
void Tapis::HideBidsChoice()
{
    groupBoutons->hide();
}
/*****************************************************************************/
void Tapis::showAvatars(bool b)
{
    QMapIterator<Place, PlayerBox *> i(playerBox);
    while (i.hasNext())
    {
        i.next();
        i.value()->enableAvatar(b);
    }
}
/*****************************************************************************/
void Tapis::razTapis(bool shadow)
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
void Tapis::resetCards()
{
    for (int i = 0; i < cardsPics.size(); i++)
    {
        cardsPics.at(i)->hide();
        cardsPics.at(i)->SetStatus(GfxCard::NORMAL);
    }
}
/*****************************************************************************/
void Tapis::slotBoutton1()
{
    emit sigContract(PASS);
}
/*****************************************************************************/
void Tapis::slotBoutton2()
{
    emit sigContract(TAKE);
}
/*****************************************************************************/
void Tapis::slotBoutton3()
{
    emit sigContract(GUARD);
}
/*****************************************************************************/
void Tapis::slotBoutton4()
{
    emit sigContract(GUARD_WITHOUT);
}
/*****************************************************************************/
void Tapis::slotBoutton5()
{
    emit sigContract(GUARD_AGAINST);
}
/*****************************************************************************/
void Tapis::slotAccepteChien()
{
    emit sigAccepteChien();
}
/*****************************************************************************/
void Tapis::slotPresenterPoignee()
{
    emit sigPresenterPoignee();
}
/*****************************************************************************/
void Tapis::setFilter(Filter f)
{
    filter = f;
}

//=============================================================================
// End of file Tapis.cpp
//=============================================================================
