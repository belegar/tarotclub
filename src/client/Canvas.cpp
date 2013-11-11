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
#define SOUTH_CARDS_POS     522

#define MENU_POS_X  30
#define MENU_POS_Y  350

static const qreal SCALE_FACTOR = 1.5f;

static const QPointF coordPlayerBox[5] =
{
    QPointF(300, 280),  // SOUTH
    QPointF(570, 100),   // EAST
    QPointF(300, 20),   // NORTH
    QPointF(30, 100),  // WEST
    QPointF(0, 0)       // NORTH-WEST
};

/*****************************************************************************/
class BorderLine : public QGraphicsItem
{
public:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

QRectF BorderLine::boundingRect() const
{
    // Leave a transparent border between the white border and the window, it's nicer!
    return QRectF(border.x() - 10, border.y() - 10, border.width() + 20, border.height() + 20);
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

    menuItem.setPos(MENU_POS_X, MENU_POS_Y);
    menuItem.show();
    scene.addItem(&menuItem);

    popupItem.hide();
    scene.addItem(&popupItem);

    // call mouseEvent as soon as the mouse is moving, without any click buttons
    viewport()->setMouseTracking(true);
    mFilter = BLOCK_ALL;
}
/*****************************************************************************/
bool Canvas::Initialize(ClientOptions &opt)
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
                AddGfxCard(image);
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
        AddGfxCard(image);
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
            AddGfxCard(image);
        }
        else
        {
            return false;
        }
    }

    QRectF cardSize = cardsPics.at(0)->boundingRect();
    cardSize.setWidth(cardSize.width() * SCALE_FACTOR);
    cardSize.setHeight(cardSize.height() * SCALE_FACTOR);

    // 4 players by default
    for (int i = 0; i < 4; i++)
    {
        PlayerBox *pb = new PlayerBox(cardSize);
        pb->setPos(coordPlayerBox[i]);
        pb->show();
        playerBox.insert((Place)i, pb);
        scene.addItem(pb);
    }

    // Give canvas element sizes to the popup to allow dynamic resizing
    popupItem.SetSizes(border, cardSize);

    return true;
}
/*****************************************************************************/
void Canvas::AddGfxCard(const QString &filename)
{
    GfxCard *item = new GfxCard(filename);
    item->hide();
    item->setScale(SCALE_FACTOR);
    cardsPics.append(item);
    scene.addItem(item);
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
Card *Canvas::GetObjectCard(GfxCard *gc)
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
    return (c);
}
/*****************************************************************************/
bool Canvas::GetSlamOption()
{
    return menuItem.GetSlamOption();
}
/*****************************************************************************/
void Canvas::DisplayDiscardMenu(bool visible)
{
    if (visible)
    {
        menuItem.DisplayMenu(MenuItem::DISCARD_MENU);
    }
    else
    {
        menuItem.DisplayMenu(MenuItem::NO_MENU);
    }
}
/*****************************************************************************/
void Canvas::DisplayHandleMenu(bool visible)
{
    if (visible)
    {
        menuItem.DisplayMenu(MenuItem::HANDLE_MENU);
    }
    else
    {
        menuItem.DisplayMenu(MenuItem::NO_MENU);
    }
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
void Canvas::mousePressEvent(QMouseEvent *e)
{
    QList<QGraphicsItem *> list;

    emit sigViewportClicked();

#ifdef QT_DEBUG
    if (e->button() == Qt::RightButton)
    {
        qDebug() << "x=" << e->pos().x() << ", y=" << e->pos().y();
    }
#endif

    if (mFilter == BLOCK_ALL)
    {
        return;
    }

    if (e->button() == Qt::LeftButton)
    {
        if (TestFilter(MENU))
        {
            const MenuItem::MenuButton *ret = menuItem.Refresh(mapToScene(e->pos()), true);
            if (ret != NULL)
            {
                if (ret->menu == MenuItem::BIDS_MENU)
                {
                    Contract contract;
                    if (ret->widget == MenuItem::TAKE_BUTTON)
                    {
                        contract = TAKE;
                    }
                    else if (ret->widget == MenuItem::GUARD_BUTTON)
                    {
                        contract = GUARD;
                    }
                    else if (ret->widget == MenuItem::GUARD_WITHOUT_BUTTON)
                    {
                        contract = GUARD_WITHOUT;
                    }
                    else if (ret->widget == MenuItem::GUARD_AGAINST_BUTTON)
                    {
                        contract = GUARD_AGAINST;
                    }
                    else
                    {
                        contract = PASS;
                    }
                    emit sigContract(contract);
                }
                else if (ret->menu == MenuItem::HANDLE_MENU)
                {
                    emit sigAcceptHandle();
                }
                else if (ret->menu == MenuItem::DISCARD_MENU)
                {
                    emit sigAcceptDiscard();
                }
            }
        }

        if (TestFilter(CARDS))
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
    }
}
/*****************************************************************************/
/*
 * Emit a signal each time the cursor goes over a card
 */
void Canvas::mouseMoveEvent(QMouseEvent *e)
{
    QList<QGraphicsItem *> list;

    if (mFilter == BLOCK_ALL)
    {
        return;
    }

    if (TestFilter(MENU))
    {
        (void)menuItem.Refresh(mapToScene(e->pos()), false);
    }

    if (TestFilter(CARDS))
    {
        list = scene.items(mapToScene(e->pos()));
        if (!list.isEmpty())
        {
            // If it is a card, return the object, otherwise NULL
            if (list.first()->type() == GfxCard::Type)
            {
                GfxCard *c = (GfxCard *)list.first();
                emit sigMoveCursor(c);
            }
            else
            {
                SetCursorType(ARROW);
            }
        }
        else
        {
            SetCursorType(ARROW);
        }
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
    playerBox.value(rel)->HighlightPlayer(true);
}
/*****************************************************************************/
void Canvas::SetAvatar(Place p, const QString &file)
{
    playerBox.value(p)->SetAvatar(file);
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
 * @return The place to display elements on the board
 */
Place Canvas::SwapPlace(Place my_place, Place absolute)
{
    // FIXME: generalize this algorithm with the number of players from GameInfo
    Place pl = (Place)((absolute + 4 - my_place) % 4);

    return pl;
}
/*****************************************************************************/
/**
 * Show names on the board, bottom player is always south
 */
void Canvas::SetPlayerIdentity(QMap<Place, Identity> &players, Place myPlace)
{
    QMapIterator<Place, Identity> i(players);
    while (i.hasNext())
    {
        i.next();
        Place rel = SwapPlace(myPlace, i.key());  // relative place

        playerBox.value(rel)->SetPlayerName(i.value().name);
        playerBox.value(rel)->SetAvatar(":/images/avatars/" + i.value().avatar);
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
    playerBox.value(rel)->DrawCard(c);
}
/*****************************************************************************/
void Canvas::DrawSouthCards(const Deck &cards)
{
    qreal x = 20.0;
    qreal y = SOUTH_CARDS_POS;
    GfxCard *cgfx;

    // Calculate the step needed between each card.
    // It depends on the number of the cards to be displayed within the border line
    qreal max_width = border.width() - 20; // leave a 10px space on left and right
    qreal card_width = cardsPics.at(0)->boundingRect().width();

    // Try the fixed step, cards are centered
    qreal step = 40.0;
    qreal width = (step * (cards.size() - 1)) + card_width + 20 + 2 * BORDER_WIDTH;

    if (width > border.width())
    {
        // dynamic step
        step = (max_width - card_width - 20 - 2 * BORDER_WIDTH) / (cards.size() - 1);
    }
    else
    {
        // Fixed step, move X coordinate to center cards
        x = (border.width() - width) / 2;
    }

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
void Canvas::DrawCardsInPopup(const QList<Card *> &cards)
{
    QList<QGraphicsItem *> items;

    for (int i = 0; i < cards.size(); i++)
    {
        items.append(GetGfxCard(cards.at(i)->GetId()));
    }

    popupItem.DrawItems(items);
    popupItem.show();
}
/*****************************************************************************/
void Canvas::HidePopup()
{
    popupItem.HideAll();
    popupItem.hide();
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
            i.value()->SelectPlayer(true);
        }
        else
        {
            i.value()->SelectPlayer(false);
        }
    }
}
/*****************************************************************************/
void Canvas::ShowBid(Place p, Contract contract, Place myPlace)
{
    Place rel = SwapPlace(myPlace, p);  // relative place
    playerBox.value(rel)->SetBidText(Util::ToString(contract));
}
/*****************************************************************************/
void Canvas::ShowBidsChoice(Contract contract)
{
    menuItem.DisplayMenu(contract);
}
/*****************************************************************************/
void Canvas::HideBidsChoice()
{
    menuItem.DisplayMenu(MenuItem::NO_MENU);
}
/*****************************************************************************/
void Canvas::ShowAvatars(bool b)
{
    QMapIterator<Place, PlayerBox *> i(playerBox);
    while (i.hasNext())
    {
        i.next();
        i.value()->EnableAvatar(b);
    }
}
/*****************************************************************************/
void Canvas::InitBoard()
{
    QMapIterator<Place, PlayerBox *> i(playerBox);
    while (i.hasNext())
    {
        i.next();
        i.value()->HighlightPlayer(false);
        i.value()->SelectPlayer(false);
        i.value()->SetBidText("");
    }
}
/*****************************************************************************/
void Canvas::ResetCards()
{
    for (int i = 0; i < cardsPics.size(); i++)
    {
        cardsPics.at(i)->hide();
        cardsPics.at(i)->setParentItem(0); // top level item
        cardsPics.at(i)->SetStatus(GfxCard::NORMAL);
    }
}
/*****************************************************************************/
void Canvas::SetFilter(quint8 f)
{
    mFilter = f;
}
/*****************************************************************************/
bool Canvas::TestFilter(quint8 mask)
{
    bool ret = false;

    if (mFilter & mask)
    {
        ret = true;
    }
    return ret;
}

//=============================================================================
// End of file Canvas.cpp
//=============================================================================
