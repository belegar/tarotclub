#include "DealsWindow.h"
#include "Deck.h"
#include "System.h"
#include "time.h"

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

DealsWindow::~DealsWindow()
{

}

void DealsWindow::slotActivated(const QString &text)
{
    mDeal.LoadGameDealLog(System::GamePath() + "/deal_result_" + text.toStdString() + ".json");

}

void DealsWindow::Initialize()
{
    mDeck.load(QString(":cards/minideck.svg"));
    Deck deck;
    deck.CreateTarotDeck();
    mCardsPics.clear();

    qreal x = 0;
    qreal y = 0;
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
