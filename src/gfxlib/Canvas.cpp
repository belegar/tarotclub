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

// Qt includes
#include "Canvas.h"
#include <QtCore>
#include <QResizeEvent>
#include <QMouseEvent>

// Game includes
#include "Common.h"
#include "TarotDeck.h"
#include "Log.h"
#include "System.h"
#include "Translations.h"

static const QRectF border(10, 10, 925, 700);

#define BORDER_WIDTH        3
#define SOUTH_CARDS_POS     522

#define MENU_POS_X  30
#define MENU_POS_Y  350

static const qreal SCALE_FACTOR = 1.5f;

static const QPointF coordPlayerBox[5] =
{
    QPointF(300, 280),  // SOUTH
    QPointF(570, 100),  // EAST
    QPointF(300, 20),   // NORTH
    QPointF(30, 100),   // WEST
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
    , mShowAvatars(true)
    , mMenuItem(this)
{
    setScene(&scene);

    // Ok, we lose the pointer but we don't care as it is only decorative
    BorderLine *line = new BorderLine();
    scene.addItem(line);

    setRenderHint(QPainter::Antialiasing);
    setCacheMode(QGraphicsView::CacheBackground);

    mMenuItem.setPos(MENU_POS_X, MENU_POS_Y);
    mMenuItem.show();
    scene.addItem(&mMenuItem);

    popupItem.hide();
    scene.addItem(&popupItem);

    mMsgBoxItem.hide();
    scene.addItem(&mMsgBoxItem);
}
/*****************************************************************************/
bool Canvas::Initialize()
{
    bool ret = true;
    QString image;
    QString path(System::DeckPath().c_str());

    mMenuItem.Initialize();
    cardsPics.clear();

    for (std::uint8_t i = 0U; i < 78U; i++)
    {
        image = path + TarotDeck::GetCard(i)->GetName().c_str() + ".svg";

        // Test if file exists
        QFile fileTest(image);
        if (fileTest.exists())
        {
            GfxCard *item = new GfxCard(image, this, i);
            item->hide();
            item->setScale(SCALE_FACTOR);
            cardsPics.append(item);
            scene.addItem(item);
        }
        else
        {
            TLogError("Cannot find image file: " + image.toStdString());
            ret = false;
        }
    }

    if (ret)
    {
        QRectF cardSize = cardsPics.at(0)->boundingRect();
        cardSize.setWidth(cardSize.width() * SCALE_FACTOR);
        cardSize.setHeight(cardSize.height() * SCALE_FACTOR);

        // 4 players by default
        for (std::uint32_t i = 0U; i < 4U; i++)
        {
            PlayerBox *pb = new PlayerBox(cardSize);
            pb->setPos(coordPlayerBox[i]);
            pb->show();
            playerBox.insert((Place)i, pb);
            scene.addItem(pb);
        }

        // Give canvas element sizes to the popup to allow dynamic resizing
        popupItem.SetSizes(border, cardSize);
        mMsgBoxItem.SetBorder(border);
    }

    return ret;
}
/*****************************************************************************/
void Canvas::SetBackground(const std::string &code)
{
    QColor color(code.c_str());

    QRectF size = scene.sceneRect();

    QRadialGradient gradient(size.width() / 2, size.height() / 2, size.width() * 1.5);

    if (color.isValid())
    {
        gradient.setColorAt(0, color);
        gradient.setColorAt(1, Qt::black);
        scene.setBackgroundBrush(gradient);
    }
}
/*****************************************************************************/
void Canvas::HideCard(Card *c)
{
    std::uint8_t index = TarotDeck::GetIndex(c->GetName());
    if (index < cardsPics.size())
    {
        cardsPics.at(index)->setVisible(false);
    }
}
/*****************************************************************************/
void Canvas::ToggleCardSelection(std::uint8_t index)
{
    if (index < cardsPics.size())
    {
        cardsPics.at(index)->ToggleSelection();
    }
}
/*****************************************************************************/
bool Canvas::GetSlamOption()
{
    return mMenuItem.GetSlamOption();
}
/*****************************************************************************/
void Canvas::DisplayDiscardMenu(bool visible)
{
    if (visible)
    {
        mMenuItem.DisplayMenu(MenuItem::DISCARD_MENU);
    }
    else
    {
        mMenuItem.DisplayMenu(MenuItem::NO_MENU);
    }
}
/*****************************************************************************/
void Canvas::DisplayHandleMenu(bool visible)
{
    if (visible)
    {
        mMenuItem.DisplayMenu(MenuItem::HANDLE_MENU);
    }
    else
    {
        mMenuItem.DisplayMenu(MenuItem::NO_MENU);
    }
}
/*****************************************************************************/
void Canvas::DisplayMainMenu(bool visible)
{
    if (visible)
    {
        mMenuItem.DisplayMenu(MenuItem::MAIN_MENU);
    }
    else
    {
        mMenuItem.DisplayMenu(MenuItem::NO_MENU);
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
    if (TestFilter(BOARD))
    {
        // We catch the event for us and forbid any sub item to receive it
        emit sigViewportClicked();
    }
    else if (mFilter != BLOCK_ALL)
    {
        // Broadcast the event for all the canvas items
        QGraphicsView::mousePressEvent(e);
    }
}
/*****************************************************************************/
void Canvas::ButtonClicked(std::uint8_t id, std::uint8_t menu)
{
    if (mFilter == BLOCK_ALL)
    {
        return;
    }

    if (menu == MenuItem::BIDS_MENU)
    {
        emit sigContract(Contract(id));
    }
    else if (menu == MenuItem::HANDLE_MENU)
    {
        emit sigAcceptHandle();
    }
    else if (menu == MenuItem::DISCARD_MENU)
    {
        emit sigAcceptDiscard();
    }
    else if (menu == MenuItem::MAIN_MENU)
    {
        emit sigStartGame();
    }
}
/*****************************************************************************/
void Canvas::CardClicked(std::uint8_t card, bool selected)
{
    emit sigClickCard(card, selected);
}
/*****************************************************************************/
void Canvas::CardHoverEnter(std::uint8_t card)
{
    emit sigCursorOverCard(card);
}
/*****************************************************************************/
void Canvas::CardHoverLeave(std::uint8_t card)
{
    (void)card;
    SetCursorType(ARROW);
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
    Place pl = (std::uint8_t)((absolute.Value() + 4 - my_place.Value()) % 4);

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

        playerBox.value(rel)->SetPlayerName(i.value().name.data());
        playerBox.value(rel)->SetAvatar(QString(i.value().avatar.data()));
    }
}
/*****************************************************************************/
/**
 * @brief Draw a card at a player's position
 *
 * @arg[in] c The graphics card to show
 * @arg[in] p = NORTH, WEST, SOUTH, EAST
 */
void Canvas::DrawCard(std::uint8_t index, Place p, Place myPlace)
{
    if (index < cardsPics.size())
    {
        GfxCard *c = cardsPics.at(index);
        Place rel = SwapPlace(myPlace, p);  // relative place
        playerBox.value(rel)->DrawCard(c);
    }
}
/*****************************************************************************/
void Canvas::DrawSouthCards(const Deck &cards)
{
    qreal x = 20.0;
    qreal y = SOUTH_CARDS_POS;

    // Calculate the step needed between each card.
    // It depends on the number of the cards to be displayed within the border line
    qreal max_width = border.width() - 20; // leave a 10px space on left and right
    qreal card_width = cardsPics.at(0)->boundingRect().width();

    // Try the fixed step, cards are centered
    qreal step = 40.0;
    qreal width = (step * (cards.Size() - 1)) + card_width + 20 + 2 * BORDER_WIDTH;

    if (width > border.width())
    {
        // dynamic step
        step = (max_width - card_width - 20 - 2 * BORDER_WIDTH) / (cards.Size() - 1);
    }
    else
    {
        // Fixed step, move X coordinate to center cards
        x = (border.width() - width) / 2;
    }

    qreal z = 0.0;
    for (Deck::ConstIterator i = cards.Begin(); i != cards.End(); ++i)
    {
        std::string name = (*i)->GetName();
        //        std::cout << name << ", ";
        GfxCard *cgfx = cardsPics.at(TarotDeck::GetIndex(name));
        cgfx->setPos(x, y);
        cgfx->setZValue(z++);
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
        items.append(cardsPics.at(TarotDeck::GetIndex(cards.at(i)->GetName())));
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
void Canvas::HideMessageBox()
{
    mMsgBoxItem.hide();
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
    playerBox.value(rel)->SetBidText(ContractToString(contract));
}
/*****************************************************************************/
void Canvas::ShowBidsChoice(Contract contract)
{
    mMenuItem.ClearSlamOption();
    mMenuItem.DisplayMenu(contract);
}
/*****************************************************************************/
void Canvas::HideBidsChoice()
{
    mMenuItem.DisplayMenu(MenuItem::NO_MENU);
}
/*****************************************************************************/
void Canvas::ShowAvatars(bool b)
{
    mShowAvatars = b;
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
        i.value()->EnableAvatar(mShowAvatars);
        i.value()->SetBidText("");
    }

    HidePopup();
    HideBidsChoice();
    HideMessageBox();
}
/*****************************************************************************/
void Canvas::ResetCards()
{
    for (int i = 0; i < cardsPics.size(); i++)
    {
        cardsPics.at(i)->hide();
        cardsPics.at(i)->setParentItem(0); // top level item
        cardsPics.at(i)->SetSelected(false);
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
/*****************************************************************************/
void Canvas::SetResult(const Score &score, const Tarot::Bid &bid)
{
    QString result_str = "<html><head></head><body>";

    if (score.Winner() == ATTACK)
    {
        result_str = QString("<h2><center><font color=\"darkgreen\">") + tr("Contract succeded by ");
    }
    else
    {
        result_str = QString("<h2><center><font color=\"darkred\">") + tr("Contract failed by ");
    }

    result_str += QString().setNum(abs(score.difference)) + QString(" ") + tr("points") + QString("</font></center></h2><hr />");
    result_str += "<table><tr>";

    // Deal caracteristics
    result_str += "<td width=50%><table>";
    result_str += QString("<tr><td colspan=\"2\" align=\"center\"><b>") + tr("Summary") + QString("</b><td /></tr>");
    result_str += QString("<tr><td>") + tr("Taker") + QString("</td><td>&nbsp;&nbsp;&nbsp;") + PlaceToString(bid.taker) + QString("</td></tr>");
    result_str += QString("<tr><td>") + tr("Contract") + QString("</td><td>&nbsp;&nbsp;&nbsp;") + ContractToString(bid.contract) + QString("</td>");
    result_str += QString("<tr><td>") + tr("Number of oudlers") + QString("</td><td>&nbsp;&nbsp;&nbsp;") + QString().setNum(score.GetNumberOfOudlers()) + QString("</td></tr>");
    result_str += QString("<tr><td>") + tr("Points") + QString("</td><td>&nbsp;&nbsp;&nbsp;") + QString().setNum((int)score.pointsAttack) + QString("</td></tr>");
    result_str += QString("<tr><td>") + tr("Points to do") + QString("</td><td>&nbsp;&nbsp;&nbsp;") + QString().setNum((int)score.pointsToDo) + QString("</td></tr>");
    result_str += "</table></td>";

    result_str += "<td width=50%><table>";
    result_str += QString("<tr><td colspan=\"2\" align=\"center\"><b>") + tr("Calculation") + QString("</b><td /></tr>");
    result_str += QString("<tr><td>") + tr("Contract") + QString("</td><td>&nbsp;&nbsp;&nbsp;25</td></tr>");
    result_str += QString("<tr><td>") + tr("Earn / loss") + QString("</td><td>&nbsp;&nbsp;&nbsp;") + QString().setNum(abs(score.difference)) + QString("</td>");
    result_str += QString("<tr><td>") + tr("Little endian bonus") + QString("</td><td>&nbsp;&nbsp;&nbsp;") + QString().setNum(score.littleEndianPoints) + QString("</td></tr>");
    result_str += QString("<tr><td>") + tr("Contract multiplier") + QString("</td><td>&nbsp;&nbsp;&nbsp;") + QString().setNum(score.multiplier) + QString("</td></tr>");
    result_str += QString("<tr><td>") + tr("Handle") + QString("</td><td>&nbsp;&nbsp;&nbsp;") + QString().setNum(score.handlePoints) + QString("</td></tr>");
    result_str += QString("<tr><td>") + tr("Slam") + QString("</td><td>&nbsp;&nbsp;&nbsp;") + QString().setNum(score.slamPoints) + QString("</td></tr>");
    result_str += "</table></td>";

    result_str += "</tr></table><hr />";


    result_str += tr("Total defense") + QString("&nbsp;&nbsp;&nbsp;") + QString().setNum(score.GetDefenseScore()) + QString(" ")  + tr("points") + "<br />";
    result_str += tr("Total attack") + QString("&nbsp;&nbsp;&nbsp;")  + QString().setNum(score.GetAttackScore()) + QString(" ")  + tr("points") + "<br />";


    result_str += "</body></html>";

    mMsgBoxItem.SetText(result_str);
    mMsgBoxItem.show();
}

//=============================================================================
// End of file Canvas.cpp
//=============================================================================
