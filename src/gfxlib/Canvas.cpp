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
#include "Log.h"
#include "System.h"
#include "Translations.h"

float scaleFactor = 1.25;

static QRectF border(10, 10, 800, 700);

#define BORDER_WIDTH        3
#define SOUTH_CARDS_POS     480

#define MENU_POS_X  300
#define MENU_POS_Y  190

// Cards played in the center of the board
static const QPointF coordCards[5] =
{
    QPointF(370, 300),  // SOUTH
    QPointF(450, 250),  // EAST
    QPointF(370, 200),  // NORTH
    QPointF(300, 250),  // WEST
    QPointF(0, 0)       // NORTH-WEST
};

static QPointF coordPlayerBox[5] =
{
    QPointF(border.topLeft().x() + 20, border.bottomLeft().y() - 10 - PlayerBox::PLAYER_BOX_HEIGHT_HOR),  // SOUTH
    QPointF(border.width() - PlayerBox::PLAYER_BOX_WIDTH_VERT - 10, 150),  // EAST
    QPointF(300, 20),   // NORTH
    QPointF(border.topLeft().x() + 20, 150),   // WEST
    QPointF(0, 0)       // NORTH-WEST
};

void Canvas::MovePlayerBoxes()
{
    QPointF array[4];
    array[0] = QPointF(border.topLeft().x() + 20, border.bottomLeft().y() - 10 - PlayerBox::PLAYER_BOX_HEIGHT_HOR);  // SOUTH
    array[1] = QPointF(border.width() - PlayerBox::PLAYER_BOX_WIDTH_VERT - 10, 150);  // EAST
    array[2] = QPointF(300, 20);   // NORTH
    array[3] = QPointF(border.topLeft().x() + 20, 150);   // WEST

    for (std::uint32_t i = 0U; i < Place::FIFTH; i++)
    {
        mPlayerBox.value(i)->setPos(array[i]);
    }
}


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
    , mFilter(BLOCK_ALL)
    , mShowAvatars(true)
    , mMenuItem(this)
    , mDisplayCard(0.0F)
{
    setScene(&mScene);

    // Ok, we lose the pointer but we don't care as it is only decorative
    BorderLine *line = new BorderLine();
    mScene.addItem(line);

    setRenderHint(QPainter::Antialiasing);
    setCacheMode(QGraphicsView::CacheBackground);

    mMenuItem.setPos(MENU_POS_X, MENU_POS_Y);
    mMenuItem.show();
    mScene.addItem(&mMenuItem);
    mMenuItem.setZValue(100);

    mPopupItem.hide();
    mScene.addItem(&mPopupItem);

    mMsgBoxItem.hide();
    mScene.addItem(&mMsgBoxItem);
}
/*****************************************************************************/
void Canvas::resizeEvent(QResizeEvent *event)
{

   // QSize s = size();
    /*
    QRect rect;
    s = event->size();
    rect.setWidth(s.width());
    rect.setHeight(s.height());
    QPolygonF poly = mapToScene(rect);
*/
    // ------------------------------------------------------------
    // Use the following code to fit the view to the window size
    // ------------------------------------------------------------

  //  border.setHeight(s.height() - 20);
  //  border.setWidth(s.width() - 20);

  //  MovePlayerBoxes();

    QGraphicsView::resizeEvent(event);
    fitInView(sceneRect(), Qt::KeepAspectRatio);


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

#ifdef TAROT_DEBUG
    QString text;
    QPointF point = mMenuItem.pos();
    text = "Menu: x: " + QString().setNum((int)point.x()) + " y: " + QString().setNum((int)point.y());
    std::cout << text.toStdString() << std::endl;
#endif

    // Broadcast the event for all the canvas items
    QGraphicsView::mousePressEvent(e);
}
/*****************************************************************************/
void Canvas::ButtonClicked(std::uint8_t id, std::uint8_t menu)
{
    if (TestFilter(MENU))
    {
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
}
/*****************************************************************************/
void Canvas::CardClicked(std::uint8_t value, std::uint8_t suit, bool selected)
{
    if (TestFilter(CARDS))
    {
        emit sigClickCard(value, suit, selected);
    }
}
/*****************************************************************************/
void Canvas::CardHoverEnter(std::uint8_t value, std::uint8_t suit)
{
    emit sigCursorOverCard(value, suit);
}
/*****************************************************************************/
void Canvas::CardHoverLeave(std::uint8_t value, std::uint8_t suit)
{
    (void)value;
    (void)suit;
    SetCursorType(ARROW);
}
/*****************************************************************************/
bool Canvas::Initialize()
{
    bool ret = true;
    QString image;
    QString path(System::DeckPath().c_str());
    Deck deck;

    deck.CreateTarotDeck();

    mMenuItem.Initialize();
    mCardsPics.clear();

    for (Deck::ConstIterator it = deck.Begin(); it != deck.End(); ++it)
    {
        image = path + (*it).GetName().c_str() + ".svg";

        // Test if file exists
        QFile fileTest(image);
        if (fileTest.exists())
        {
            GfxCard *item = new GfxCard(image, this, *it);
            item->hide();
            mCardsPics.append(item);
            mScene.addItem(item);
        }
        else
        {
            TLogError("Cannot find image file: " + image.toStdString());
            ret = false;
        }
    }

    if (ret)
    {
        // 4 players by default
        for (std::uint32_t i = 0U; i < 4U; i++)
        {
            PlayerBox::Layout layout = PlayerBox::VERTICAL;
            if ((i == Place::SOUTH) || (i == Place::NORTH))
            {
                layout = PlayerBox::HORIZONTAL;
            }

            PlayerBox *pb = new PlayerBox(layout);
            pb->setPos(coordPlayerBox[i]);
            pb->show();
            pb->setZValue(50.0);
            mPlayerBox.insert((Place)i, pb);
            mScene.addItem(pb);
        }

        // Give canvas element sizes to the popup to allow dynamic resizing
        mPopupItem.SetSizes(border, mCardsPics.at(0)->GetRealSize());
        mMsgBoxItem.SetBorder(border);
    }

    return ret;
}
/*****************************************************************************/
void Canvas::SetBackground(const std::string &code)
{
    QColor color(code.c_str());

    QRectF size = mScene.sceneRect();

    QRadialGradient gradient(size.width() / 2, size.height() / 2, size.width() * 1.0);

    if (color.isValid())
    {
        gradient.setColorAt(0, color);
        gradient.setColorAt(1, Qt::black);
        mScene.setBackgroundBrush(gradient);
    }
}
/*****************************************************************************/
void Canvas::HideCard(const Card &c)
{
    GfxCard *gfx = FindGfxCard(c);
    if (gfx != NULL)
    {
        gfx->setRotation(0);
        gfx->setVisible(false);
    }
}
/*****************************************************************************/
GfxCard *Canvas::FindGfxCard(const Card &c)
{
    GfxCard *card = NULL;
    QVector<GfxCard *>::iterator iter;

    for (iter = mCardsPics.begin(); iter != mCardsPics.end(); ++iter)
    {
        if ((*iter)->IsEqual(c))
        {
            card = *iter;
        }
    }
    return card;
}
/*****************************************************************************/
void Canvas::ToggleCardSelection(const Card &c)
{
    GfxCard *gfx = FindGfxCard(c);

    if (gfx != NULL)
    {
        gfx->ToggleSelection();
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
        mMenuItem.show();
    }
    else
    {
        mMenuItem.DisplayMenu(MenuItem::NO_MENU);
        mMenuItem.hide();
    }
}
/*****************************************************************************/
void Canvas::DisplayHandleMenu(bool visible)
{
    if (visible)
    {
        mMenuItem.DisplayMenu(MenuItem::HANDLE_MENU);
        mMenuItem.show();
    }
    else
    {
        mMenuItem.DisplayMenu(MenuItem::NO_MENU);
        mMenuItem.hide();
    }
}
/*****************************************************************************/
void Canvas::DisplayMainMenu(bool visible)
{
    if (visible)
    {
        mMenuItem.DisplayMenu(MenuItem::MAIN_MENU);
        mMenuItem.show();
    }
    else
    {
        mMenuItem.DisplayMenu(MenuItem::NO_MENU);
        mMenuItem.hide();
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
    mPlayerBox.value(rel)->HighlightPlayer(true);
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
void Canvas::SetPlayerIdentity(const QMap<Place, Identity> &players, Place myPlace)
{
    for (QMap<Place, Identity>::const_iterator iter = players.begin(); iter != players.end(); ++iter)
    {
        Place rel = SwapPlace(myPlace, iter.key());  // relative place
        Identity ident = iter.value();
        mPlayerBox.value(rel)->SetPlayerName(ident.nickname.c_str());
        mPlayerBox.value(rel)->SetAvatar(ident.avatar.c_str());
        mPlayerBox.value(rel)->EnableAvatar(mShowAvatars); // forward the config
    }

    for (std::uint32_t i = 0U; i < Place::FIFTH; i++)
    {
        if (players.find(Place(i)) == players.end())
        {
            // Clear it
            Place rel = SwapPlace(myPlace, Place(i));  // relative place
            mPlayerBox.value(rel)->SetPlayerName("");
            mPlayerBox.value(rel)->SetAvatar("");
            mPlayerBox.value(rel)->EnableAvatar(false);
        }
    }
}
/*****************************************************************************/
/**
 * @brief Draw a card at a player's position
 *
 * @arg[in] c The graphics card to show
 * @arg[in] p = NORTH, WEST, SOUTH, EAST
 */
void Canvas::DrawCard(const Card &c, Place p, Place myPlace)
{
    Place rel = SwapPlace(myPlace, p);  // relative place
    GfxCard *gfx = FindGfxCard(c);
    if (gfx != NULL)
    {
        mDisplayCard++;
        gfx->setPos(coordCards[rel.Value()]);
        gfx->setZValue(mDisplayCard);
        gfx->show();
        gfx->setRotation((qrand() % 30) - 15);

        // FIXME: does not work for 3 or 5 players
        if (mDisplayCard >= 4.0)
        {
            mDisplayCard = 0.0;
        }
    }
    else
    {
        TLogError("Card is null, cannot display it!");
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
    qreal card_width = mCardsPics.at(0)->boundingRect().width();

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
    for (Deck::ConstIterator it = cards.Begin(); it != cards.End(); ++it)
    {
        //        std::cout << name << ", ";
        GfxCard *cgfx = FindGfxCard(*it);
        if (cgfx != NULL)
        {
            cgfx->setPos(x, y);
            cgfx->setZValue(z++);
            cgfx->setRotation(0);
            cgfx->show();
            x = x + step;
        }
    }
}
/*****************************************************************************/
void Canvas::DrawCardsInPopup(const Deck &deck)
{
    QList<QGraphicsItem *> items;

    for (Deck::ConstIterator it = deck.Begin(); it != deck.End(); ++it)
    {
        GfxCard *gfx = FindGfxCard(*it);
        if (gfx != NULL)
        {
            items.append(gfx);
        }
    }

    mPopupItem.DrawItems(items);
    mPopupItem.show();
}
/*****************************************************************************/
void Canvas::HidePopup()
{
    mPopupItem.HideAll();
    mPopupItem.hide();
}
/*****************************************************************************/
void Canvas::HideMessageBox()
{
    mMsgBoxItem.hide();
}
/*****************************************************************************/
void Canvas::ShowSelection(Place p, Place myPlace)
{
    Place rel = SwapPlace(myPlace, p);  // relative place

    QMapIterator<Place, PlayerBox *> i(mPlayerBox);
    while (i.hasNext())
    {
        i.next();        
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
    mPlayerBox.value(rel)->SetBidText(ContractToString(contract));
}
/*****************************************************************************/
void Canvas::ShowBidsChoice(Contract contract)
{
    mMenuItem.ClearSlamOption();
    mMenuItem.DisplayMenu(contract);
    mMenuItem.show();
}
/*****************************************************************************/
void Canvas::HideBidsChoice()
{
    mMenuItem.DisplayMenu(MenuItem::NO_MENU);
    mMenuItem.hide();
}
/*****************************************************************************/
void Canvas::ShowAvatars(bool b)
{
    mShowAvatars = b;
    QMapIterator<Place, PlayerBox *> i(mPlayerBox);
    while (i.hasNext())
    {
        i.next();
        i.value()->EnableAvatar(b);
    }
}
/*****************************************************************************/
void Canvas::InitBoard(bool clear)
{
    QMapIterator<Place, PlayerBox *> i(mPlayerBox);
    while (i.hasNext())
    {
        i.next();
        i.value()->HighlightPlayer(false);
        i.value()->SelectPlayer(false);
        i.value()->SetBidText("");

        // Clear process hides everything
        if (clear)
        {
            i.value()->SetPlayerName("");
            i.value()->EnableAvatar(false);
            i.value()->SetAvatar("");
        }
        else
        {
            i.value()->EnableAvatar(mShowAvatars);
        }
    }

    HidePopup();
    HideBidsChoice();
    HideMessageBox();
}
/*****************************************************************************/
void Canvas::ResetCards()
{
    for (int i = 0; i < mCardsPics.size(); i++)
    {
        mCardsPics.at(i)->hide();
        mCardsPics.at(i)->setParentItem(0); // top level item
        mCardsPics.at(i)->SetSelected(false);
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
void Canvas::SetResult(const Points &points, const Tarot::Bid &bid)
{
    QString result_str = "<html><head></head><body>";

    if (points.Winner() == ATTACK)
    {
        result_str = QString("<h2><center><font color=\"darkgreen\">") + tr("Contract succeded by ");
    }
    else
    {
        result_str = QString("<h2><center><font color=\"darkred\">") + tr("Contract failed by ");
    }

    result_str += QString().setNum(abs(points.Difference())) + QString(" ") + tr("points") + QString("</font></center></h2><hr />");
    result_str += "<table><tr>";

    // Deal caracteristics
    result_str += "<td width=50%><table>";
    result_str += QString("<tr><td colspan=\"2\" align=\"center\"><b>") + tr("Summary") + QString("</b><td /></tr>");
    result_str += QString("<tr><td>") + tr("Taker") + QString("</td><td>&nbsp;&nbsp;&nbsp;") + PlaceToString(bid.taker) + QString("</td></tr>");
    result_str += QString("<tr><td>") + tr("Contract") + QString("</td><td>&nbsp;&nbsp;&nbsp;") + ContractToString(bid.contract) + QString("</td>");
    result_str += QString("<tr><td>") + tr("Number of oudlers") + QString("</td><td>&nbsp;&nbsp;&nbsp;") + QString().setNum(points.oudlers) + QString("</td></tr>");
    result_str += QString("<tr><td>") + tr("Points") + QString("</td><td>&nbsp;&nbsp;&nbsp;") + QString().setNum((int)points.pointsAttack) + QString("</td></tr>");
    result_str += QString("<tr><td>") + tr("Points to do") + QString("</td><td>&nbsp;&nbsp;&nbsp;") + QString().setNum(Tarot::PointsToDo(points.oudlers)) + QString("</td></tr>");
    result_str += "</table></td>";

    result_str += "<td width=50%><table>";
    result_str += QString("<tr><td colspan=\"2\" align=\"center\"><b>") + tr("Calculation") + QString("</b><td /></tr>");
    result_str += QString("<tr><td>") + tr("Contract") + QString("</td><td>&nbsp;&nbsp;&nbsp;25</td></tr>");
    result_str += QString("<tr><td>") + tr("Earn / loss") + QString("</td><td>&nbsp;&nbsp;&nbsp;") + QString().setNum(abs(points.Difference())) + QString("</td>");
    result_str += QString("<tr><td>") + tr("Little endian bonus") + QString("</td><td>&nbsp;&nbsp;&nbsp;") + QString().setNum(points.littleEndianPoints) + QString("</td></tr>");
    result_str += QString("<tr><td>") + tr("Contract multiplier") + QString("</td><td>&nbsp;&nbsp;&nbsp;") + QString().setNum(Tarot::GetMultiplier(bid.contract)) + QString("</td></tr>");
    result_str += QString("<tr><td>") + tr("Handle") + QString("</td><td>&nbsp;&nbsp;&nbsp;") + QString().setNum(points.handlePoints) + QString("</td></tr>");
    result_str += QString("<tr><td>") + tr("Slam") + QString("</td><td>&nbsp;&nbsp;&nbsp;") + QString().setNum(points.slamPoints) + QString("</td></tr>");
    result_str += "</table></td>";

    result_str += "</tr></table><hr />";


    result_str += tr("Total defense") + QString("&nbsp;&nbsp;&nbsp;") + QString().setNum(points.GetDefenseScore()) + QString(" ")  + tr("points") + "<br />";
    result_str += tr("Total attack") + QString("&nbsp;&nbsp;&nbsp;")  + QString().setNum(points.GetAttackScore()) + QString(" ")  + tr("points") + "<br />";


    result_str += "</body></html>";

    mMsgBoxItem.SetText(result_str, QSize(480, 300));
    mMsgBoxItem.show();
}

//=============================================================================
// End of file Canvas.cpp
//=============================================================================
