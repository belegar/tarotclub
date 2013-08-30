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

static const QRectF border(10, 10, 925, 700);

#define BORDER_WIDTH        3
#define NORTH_BOX_POS_X     350
#define NORTH_BOX_POS_Y     20
#define SOUTH_CARDS_POS     522

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

class BorderLine : public QGraphicsItem
{
public:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

QRectF BorderLine::boundingRect() const
{
    // Leave a transparent border between the white border and the window, it's nicer!
    return QRectF(border.x()-10, border.y()-10, border.width()+20, border.height()+20);
}

void BorderLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(Qt::white, BORDER_WIDTH, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
    painter->setBrush(Qt::NoBrush);

    QPainterPath path;
    path.addRect(border);

    painter->drawPath(path);
}

/*****************************************************************************/
Canvas::Canvas(QWidget *parent)
    : QGraphicsView(parent)
    , cardsPics(0)
{
    setScene(&scene);

    BorderLine *line = new BorderLine();
    scene.addItem(line);

    setRenderHint(QPainter::Antialiasing);
    setCacheMode(QGraphicsView::CacheBackground);

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

    groupBoutons->move(20, 300);

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

    // ==============================================================
    //       CANVAS ELEMENTS
    // ==============================================================

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
void Canvas::SetBackground(const QString &code)
{
    QColor color(code);
    if (color.isValid())
    {
        scene.setBackgroundBrush(color);
    }
}
/*****************************************************************************/
GfxCard *Canvas::GetGfxCard(int i)
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
bool Canvas::LoadCards(ClientOptions &opt)
{
    int i, j;
    QString varImg;
    QString image;
    QString path;

#ifdef QT_DEBUG
    // Debug, the binary is inside the build directory
    path = qApp->applicationDirPath() + "/../../src/data/cards/default/";
    Q_UNUSED(opt);
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
void Canvas::resizeEvent(QResizeEvent *event)
{
    // ------------------------------------------------------------
    // Use the following code to fit the view to the window size
    // ------------------------------------------------------------
    QGraphicsView::resizeEvent(event);
    fitInView(this->sceneRect(), Qt::KeepAspectRatio);

    // ------------------------------------------------------------
    // Use the following code for absolute graphic items creation
    // ------------------------------------------------------------

/*
    QSize s;
    s = event->size();
    setSceneRect(0, 0, s.width(), s.height());
    */
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
        list = scene.items(mapToScene(e->pos()));
        if (!list.isEmpty())
        {
            if (list.first()->type() == GfxCard::Type)
            {
                GfxCard *c = (GfxCard *)list.first();
                emit sigClickCard(c);
            }
        }
    }

#ifdef QT_DEBUG
    if (e->button() == Qt::RightButton)
    {
        qDebug() << "x=" << e->pos().x() << ", y=" << e->pos().y();
    }
#endif
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

    list = scene.items( mapToScene(e->pos()) );
    if (!list.isEmpty())
    {
        // If it is a card, return the object, otherwise NULL
        if (list.first()->type() == GfxCard::Type)
        {
            GfxCard *c = (GfxCard *)list.first();
            emit sigMoveCursor(c);
        }
    }
    else
    {
        SetCursorType(ARROW);
    }
}
/*****************************************************************************/
/**
 * n==0 : norma icon with an arrow
 * n==1 : forbidden icon
 */
void Canvas::SetCursorType(CursorType t)
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
void Canvas::ShowTaker(Place taker, Place myPlace)
{
    Place rel = SwapPlace(myPlace, taker);  // relative place
    playerBox.value(rel)->highlightPlayer(true);
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
void Canvas::SetPlayerNames(QMap<Place, Identity> &players, Place myPlace)
{
    QMapIterator<Place, Identity> i(players);
    while (i.hasNext())
    {
        i.next();
        Place rel = SwapPlace(myPlace, i.key());  // relative place

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
void Canvas::DrawCard(GfxCard *c, Place p, Place myPlace)
{
    Place rel = SwapPlace(myPlace, p);  // relative place
    QPointF pos = CalculateCardPosition(rel);

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
void Canvas::DrawSouthCards(const Deck &cards)
{
    qreal x = 20.0;
    qreal y = SOUTH_CARDS_POS;
    GfxCard *cgfx;

    // Calculate the step needed between each card.
    // It depends on the number of the cards to be displayed within the border line
    qreal width = border.width() - 20; // leave a 10px space on left and right
    qreal number = cards.size();
    qreal card_width = cardsPics.at(0)->boundingRect().width();
    qreal step = (width- card_width - 20 - 2*BORDER_WIDTH)/(number-1);

    for (int i = 0; i < cards.size(); i++)
    {
        cgfx = GetGfxCard(cards.at(i)->GetId());
        cgfx->setPos(x, y);
        cgfx->setZValue(i);
        cgfx->show();
        x = x + step;
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
void Canvas::ShowSelection(Place p, Place myPlace)
{
    QMapIterator<Place, PlayerBox *> i(playerBox);
    while (i.hasNext())
    {
        i.next();
        Place rel = SwapPlace(myPlace, p);  // relative place

        if (i.key() == rel)
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
void Canvas::ShowBid(Place p, Contract cont, Place myPlace)
{
    QString txt;

    Place rel = SwapPlace(myPlace, p);  // relative place

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

    textBox.value(rel)->setText(txt);
    textBox.value(rel)->show();
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
