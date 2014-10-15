#ifndef DEALSWINDOW_H
#define DEALSWINDOW_H

#include <QDialog>
#include <QPushButton>
#include <QComboBox>
#include <QLayout>
#include <QGraphicsView>
#include <QtSvg>

#include "Deal.h"

class DealsWindow : public QDialog
{
    Q_OBJECT

public:
    DealsWindow(QWidget *parent = 0);
    ~DealsWindow();

    void Initialize();

    void RefreshDeals();

public slots:
    void slotActivated(const QString & text);

private:
    QVBoxLayout *mLayout;
    QComboBox *mComboBox;
    QPushButton *mOkButton;
    QGraphicsView *mView;
    QGraphicsScene mScene;
    QSvgRenderer mDeck;
    QVector<QGraphicsSvgItem *> mCardsPics;
    Deal mDeal;

};

#endif // DEALSWINDOW_H
