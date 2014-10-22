/*=============================================================================
 * TarotClub - DealsWindow.cpp
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

#include "DealsWindow.h"
#include "Deck.h"
#include "System.h"
#include "time.h"

/*****************************************************************************/
DealsWindow::DealsWindow(QWidget *parent)
    : QDialog(parent)
{
    mComboBox = new QComboBox(this);
    mOkButton = new QPushButton(tr("&Close"), this);

    mView = new QGraphicsView(&mScene);

    mLayout = new QVBoxLayout(this);

    QHBoxLayout *layout1 = new QHBoxLayout();
    layout1->addWidget(new QLabel(tr("Choose a previously played deal to display.")));
    layout1->addWidget(mComboBox);
    layout1->addStretch();

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->addStretch();
    layout2->addWidget(mOkButton);

    mLayout->addLayout(layout1);
    mLayout->addWidget(mView);
    mLayout->addLayout(layout2);

    connect(mOkButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(mComboBox, static_cast<void(QComboBox::*)(const QString&)>(&QComboBox::activated), this, &DealsWindow::slotActivated);
}
/*****************************************************************************/
DealsWindow::~DealsWindow()
{

}
/*****************************************************************************/
void DealsWindow::slotActivated(const QString &text)
{
    mDeal.LoadGameDealLog(System::GamePath() + "/deal_result_" + text.toStdString() + ".json");

    qreal x = 0;
    qreal y = 0;
    qreal limit_x = 700;

    // Show discard
    DisplayDeck(mDeal.GetDiscard(), x, y);

    y = 115;
    // Show played tricks
    for (std::uint8_t i = 0U; i < Tarot::NumberOfCardsInHand(4U); i++)
    {
         Deck trick = mDeal.GetTrick(i, 4U);
         DisplayDeck(trick, x, y);

         // Space between horizontal tricks
         x += 60;

         if (x >= limit_x)
         {
             x = 0;
             y += 115;
         }
    }
}
/*****************************************************************************/
void DealsWindow::DisplayDeck(const Deck &deck, qreal x, qreal y)
{
    qreal z = 0;
    for (Deck::ConstIterator iter = deck.Begin(); iter != deck.End(); ++iter)
    {
       std::string name = "c" + iter->GetName();
       for (int j = 0U; j < mCardsPics.size(); j++)
       {
           QGraphicsSvgItem *item = mCardsPics[j];
           if (item->elementId().toStdString() == name)
           {
               item->setPos(x, y);
               item->setZValue(z++);
           }
       }
       x += 35;
    }

}
/*****************************************************************************/
void DealsWindow::Initialize()
{
    mDeck.load(QString(":cards/minideck.svg"));
    Deck deck;
    deck.CreateTarotDeck();
    mCardsPics.clear();

    mScene.setBackgroundBrush(QColor("#004f00"));

    mText = new QGraphicsSimpleTextItem();
    mScene.addItem(mText);
    mText->setX(0);
    mText->setY(0);
    mText->setText("Discard:");

    qreal x = 0;
    qreal y = 115;
    for (Deck::ConstIterator iter = deck.Begin(); iter != deck.End(); ++iter)
    {
        QGraphicsSvgItem *item = new QGraphicsSvgItem();
        item->setElementId("c" + QString(iter->GetName().c_str()));
        item->setSharedRenderer(&mDeck);
        item->setPos(x, y);
        item->show();

        mCardsPics.append(item);
        mScene.addItem(item);

        qreal limit = 14;
        if (iter->GetSuit() == Card::TRUMPS)
        {
            limit = 13;
        }

        if (iter->GetValue() == limit)
        {
            x = 0;
            y += 115;
        }
        else
        {
            x += 65;
        }
    }
}
/*****************************************************************************/
void DealsWindow::RefreshDeals()
{
    QDir dir(QString(System::GamePath().c_str()));
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList list = dir.entryInfoList();

    for (int i = 0; i < list.size(); i++)
    {
        // Filename format is: deal_result_2014-10-12.153826.json
        // We have to extract the date and time "%Y-%m-%d.%H%M%S"

        // 1. get the filename and remove .json
        QString dateTime = list.at(i).fileName();
        dateTime.chop(5);

        // 3. Fill the combo box
        mComboBox->addItem(dateTime.right(17));
    }
}

//=============================================================================
// Fin du fichier DealsWindow.cpp
//=============================================================================
