/*=============================================================================
 * TarotClub - DealsWindow.h
 *=============================================================================
 * Display previously played deals
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

#ifndef DEALS_WINDOW_H
#define DEALS_WINDOW_H

#include <QDialog>
#include <QPushButton>
#include <QComboBox>
#include <QLayout>
#include <QGraphicsView>
#include <QtSvg>

#include "Deal.h"
#include "DummyRemoteDb.h"

/*****************************************************************************/
class DealsWindow : public QDialog
{
    Q_OBJECT

public:
    DealsWindow(QWidget *parent = 0);
    ~DealsWindow();

    void Initialize();

    void RefreshDeals();

public slots:
    void slotActivated(int index);

private:
    QVBoxLayout *mLayout;
    QComboBox *mComboBox;
    QPushButton *mOkButton;
    QGraphicsView *mView;
    QGraphicsScene mScene;
    QSvgRenderer mDeck;
    QGraphicsSimpleTextItem *mText;
    QVector<QGraphicsSvgItem *> mCardsPics;

    DummyRemoteDb mRemoteDb;
    Deal mDeal;
    QFileInfoList mList;

    void DisplayDeck(const Deck &deck, qreal x, qreal y);
};

#endif // DEALS_WINDOW_H

//=============================================================================
// Fin du fichier DealsWindow.h
//=============================================================================
