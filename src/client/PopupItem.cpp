#include "PopupItem.h"
#include <QBrush>
#include <QPainter>
#include <QtGlobal>
#include <QPointF>

static const qreal STEP_X = 40.0f;

PopupItem::PopupItem()
{
    setBrush(QColor("#808080"));
    setPen(QPen(Qt::white));
    setZValue(3.0);
}

void PopupItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Paint with specified color and pen
    painter->setRenderHint(QPainter::Antialiasing);

    painter->setPen(pen());
    painter->setBrush(brush());
    painter->drawRoundRect(rect(), (int)(25 * rect().height()
                                         / rect().width()), 25);
}

void PopupItem::DrawItems(const QList<QGraphicsItem *> &items)
{
    QRectF rec = rect();
    qreal cardPosX = rec.x() + 10.0f;
    qreal cardPosY = rec.y() + 10.0f;
    mItems = items;

    for (int i = 0; i < items.size(); i++)
    {
        items.at(i)->setPos(cardPosX + i*STEP_X, cardPosY);
        items.at(i)->setZValue(4 + i);
        items.at(i)->show();
    }
    update();
}

void PopupItem::HideAll()
{
    for (int i = 0; i < mItems.size(); i++)
    {
        mItems.at(i)->hide();
    }
    hide();
}
