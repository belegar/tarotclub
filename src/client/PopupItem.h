#ifndef POPUPITEM_H
#define POPUPITEM_H

#include <QGraphicsRectItem>

class PopupItem : public QGraphicsRectItem
{
public:
    PopupItem();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    void DrawItems(const QList<QGraphicsItem *> &items);
    void HideAll();

private:
    QList<QGraphicsItem *> mItems;
};

#endif // POPUPITEM_H
