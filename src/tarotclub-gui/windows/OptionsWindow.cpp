/*=============================================================================
 * TarotClub - OptionsWindow.cpp
 *=============================================================================
 * OptionsWindow : fenêtre d'options
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

#include "OptionsWindow.h"
#include "Translations.h"
#include <QDir>
#include <QString>
#include <QtWidgets>
#include "ui_NewServerUI.h"

static const QPoint labelPos[5] =
{
    {10, 2},
    {60, 2},
    {110, 2},
    {160, 2},
    {210, 2}
};

static const QString iconName[5] =
{
    ":/icons/spade.svg",
    ":/icons/heart.svg",
    ":/icons/diamond.svg",
    ":/icons/club.svg",
    ":/icons/trump.svg"
};

static const std::uint8_t suitList[5] =
{
    Card::SPADES,
    Card::HEARTS,
    Card::DIAMONDS,
    Card::CLUBS,
    Card::TRUMPS
};

static const QSize labelSize(40, 40);

static const int cLocalRadioButtonId = 1U;
static const int cPredefinedRadioButtonId = 2U;

/*****************************************************************************/
DragWidget::DragWidget(QWidget *parent)
    : QFrame(parent)
{
    setMinimumSize(260, 44);
    setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
    setAcceptDrops(true);

    for (int i = 0; i < 5; i++)
    {
        mIcons[i].label = new QLabel(this);
        mIcons[i].label->setPixmap(QPixmap(iconName[i]));
        mIcons[i].label->move(labelPos[i]);
        mIcons[i].label->resize(labelSize);
        mIcons[i].label->setScaledContents(true);
        mIcons[i].label->show();

        mIcons[i].icon = iconName[i];
        mIcons[i].suit = suitList[i];
    }
}
/*****************************************************************************/
std::string DragWidget::GetOrder()
{
    std::string order;
    for (int i = 0; i < 5; i++)
    {
        order += Card::SuitName(mIcons[i].suit);
    }
    return order;
}
/*****************************************************************************/
void DragWidget::SetOrder(const std::string &order)
{
    if (order.size() >= 5)
    {
        for (int i = 0; i < 5; i++)
        {
            std::string letter;
            letter.push_back(order[i]);
            std::uint8_t suit = Card::SuitFromName(letter);
            mIcons[i].icon = iconName[suit];
            mIcons[i].suit = suit;

            // update Pixmap
            mIcons[i].label->setPixmap(mIcons[i].icon);
        }
    }
}
/*****************************************************************************/
void DragWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata"))
    {
        if (event->source() == this)
        {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        }
        else
        {
            event->acceptProposedAction();
        }
    }
    else
    {
        event->ignore();
    }
}
/*****************************************************************************/
void DragWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata"))
    {
        if (event->source() == this)
        {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        }
        else
        {
            event->acceptProposedAction();
        }
    }
    else
    {
        event->ignore();
    }
}
/*****************************************************************************/
void DragWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata"))
    {
        // Look for the selected suit
        int destination = DetectLabel(event->position().toPoint().x());
        if ((destination >= 0) && (destination < 5))
        {
            QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
            QDataStream dataStream(&itemData, QIODevice::ReadOnly);

            int source = -1;
            dataStream >> source;

            if ((source >= 0) && (source < 5))
            {
                // Swap icons
                DragIcon temp = mIcons[destination];
                mIcons[destination].icon = mIcons[source].icon;
                mIcons[destination].suit = mIcons[source].suit;

                mIcons[source].icon = temp.icon;
                mIcons[source].suit = temp.suit;

                // update Pixmap
                mIcons[source].label->setPixmap(mIcons[source].icon);
                mIcons[destination].label->setPixmap(mIcons[destination].icon);

                if (event->source() == this)
                {
                    event->setDropAction(Qt::MoveAction);
                    event->accept();
                }
                else
                {
                    event->acceptProposedAction();
                }
            }
            else
            {
                event->ignore();
            }
        }
        else
        {
            event->ignore();
        }
    }
    else
    {
        event->ignore();
    }
}
/*****************************************************************************/
void DragWidget::mousePressEvent(QMouseEvent *event)
{
    QLabel *child = static_cast<QLabel *>(childAt(event->pos()));
    if (!child)
    {
        return;
    }

    QPixmap pixmap = child->pixmap().scaled(40, 40);

    // Look for the selected suit
    int selected = DetectLabel(child->pos().x());
    if (selected < 0)
    {
        return;
    }

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << selected;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-dnditemdata", itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);
    drag->setHotSpot(event->pos() - child->pos());

    (void)drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);
}
/*****************************************************************************/
int DragWidget::DetectLabel(int x)
{
    int selected = -1;
    for (int i = 4; i >= 0; i--)
    {
        if ((x >= labelPos[i].x()) &&
                (x < (labelPos[i].x() + labelSize.width())))
        {
            selected = i;
        }
    }
    return selected;
}
/*****************************************************************************/
OptionsWindow::OptionsWindow(QWidget *parent)
    : QDialog(parent)
    , mLanguageIndex(0U)
    , mPreviousSelectedBot(-1)
    , mImportAvatarWindow(this)
{
    ui.setupUi(this);

    // Build the Avatar window
    mAvatarsDiag = new QDialog(this);
    mAvatarsUi.setupUi(mAvatarsDiag);

    // Setup the tournament ui table

    ui.tournamentList->horizontalHeader()->setVisible(true);
    ui.tournamentList->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.tournamentList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.tournamentList->setHorizontalHeaderLabels(QStringList()
                                                 << tr("Turn")
                                                 << tr("Deal type")
                                                 << tr("Deal number")
                                                 << tr("Deal file")
                                                 );

    mDealTypes << tr("Random deal") << tr("Numbered deal") << tr("Custom deal");

    // Create the scroll area widget
    QWidget *contents = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout();
    contents->setLayout(layout);

    mAvatarsUi.avatarsList->setContentsMargins(0, 0, 0, 0);
    mAvatarsUi.avatarsList->setAlignment(Qt::AlignCenter);
    mAvatarsUi.avatarsList->setFrameStyle(0);

    // Show the avatars
    mSelectedAvatar = -1;
    QDir dir(":/avatars");
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size();)
    {
        QHBoxLayout *hLayout = new QHBoxLayout();
        layout->addLayout(hLayout);

        for (int j = 0; j < 4; j++)
        {
            if (i < list.size())
            {
                QFileInfo fileInfo = list.at(i);
                QString fileName = fileInfo.absoluteFilePath();

                AvatarLabel *item = new AvatarLabel(i, fileName);
                mAvatarFiles[i] = item;
                QImage image(fileName);
                item->setPixmap(QPixmap::fromImage(image.scaled(160, 160)));
                item->setScaledContents(false);
                connect(item, &AvatarLabel::sigClicked, this, &OptionsWindow::slotAvatarSelected);

                hLayout->addWidget(item);
                i++;
            }
        }
    }

    // We must call the set widget _after_ adding elements
    mAvatarsUi.avatarsList->setWidget(contents);
    mAvatarsUi.avatarsList->show();

    connect(ui.btnDefaut, SIGNAL(clicked()), this, SLOT(slotBtnDefaut()));
    connect(ui.btnOk, SIGNAL(clicked()), this, SLOT(slotBtnOk()));

    // Sliders slots
    connect(ui.slider1, SIGNAL(valueChanged(int)), this, SLOT(slider1Changed(int)));
    connect(ui.slider2, SIGNAL(valueChanged(int)), this, SLOT(slider2Changed(int)));

    // Avatar choice
    connect(ui.btnPixSud, SIGNAL(clicked()), this, SLOT(slotBtnPixSud()));
    connect(ui.buttonBotAvatar, SIGNAL(clicked()), this, SLOT(slotButtonBotAvatar()));
    connect(ui.buttonImport, &QPushButton::clicked, this, &OptionsWindow::slotImportAvatar);
    connect(ui.buttonScriptPath, &QPushButton::clicked, this, &OptionsWindow::slotChooseScriptPath);

    // Background color choice
    connect(ui.tapisColor, SIGNAL(clicked()), this, SLOT(slotColorPicker()));

    // Disable the delay when the click option has been choosen
    connect(ui.clic, SIGNAL(stateChanged(int)), this, SLOT(slotClickOptionChanged(int)));

    // Enable/disable the custom avatar URL
    mRadioGroup.addButton(ui.radioLocal, cLocalRadioButtonId);
    mRadioGroup.addButton(ui.radioPredefined, cPredefinedRadioButtonId);
    connect (&mRadioGroup, &QButtonGroup::idToggled, this, &OptionsWindow::slotButtonToggled);

    // Server tab
    connect(ui.addServerButton, &QPushButton::clicked, this, &OptionsWindow::slotAddServer);
    connect(ui.removeServerButton, &QPushButton::clicked, this, &OptionsWindow::slotRemoveServer);

    // Tournament tab
    connect(ui.btnAddTurn, &QPushButton::clicked, this, &OptionsWindow::slotAddTurn);
    connect(ui.btnRemoveTurn, &QPushButton::clicked, this, &OptionsWindow::slotRemoveTurn);

    connect(ui.botsList, &QListWidget::currentRowChanged, this, &OptionsWindow::slotBotSelected);
    ui.botsList->addItem(PlaceToString(Place(Place::EAST)));
    ui.botsList->addItem(PlaceToString(Place(Place::NORTH)));
    ui.botsList->addItem(PlaceToString(Place(Place::WEST)));

    dragWidget = new DragWidget();
    ui.iconsLayout->addWidget(dragWidget);
}
/*****************************************************************************/
void OptionsWindow::SetClientOptions(const ClientOptions &opt)
{
    mClientOptions = opt;
    ui.botsList->setCurrentRow(0);
}
/*****************************************************************************/
void OptionsWindow::SetServerOptions(const ServerOptions &opt)
{
    mServerOptions = opt;
}
/*****************************************************************************/
void OptionsWindow::SetTournamentOptions(const TournamentOptions &opt)
{
    mTournamentOptions = opt;
}
/*****************************************************************************/
ClientOptions &OptionsWindow::GetClientOptions()
{
    return mClientOptions;
}
/*****************************************************************************/
ServerOptions &OptionsWindow::GetServerOptions()
{
    return mServerOptions;
}
/*****************************************************************************/
TournamentOptions &OptionsWindow::GetTournamentOptions()
{
    return mTournamentOptions;
}
/*****************************************************************************/
void OptionsWindow::slotColorPicker()
{
    QColor color = QColorDialog::getColor(Qt::darkGreen, this);
    if (color.isValid())
    {
        colorName = color.name();
        ui.tapisColor->setPalette(QPalette(color));
        ui.tapisColor->setAutoFillBackground(true);
    }
}
/*****************************************************************************/
void OptionsWindow::slotBotSelected(int currentRow)
{
    if (currentRow >= 0)
    {
        if (currentRow != mPreviousSelectedBot)
        {
            if (mPreviousSelectedBot >= 0)
            {
                Place place(mPreviousSelectedBot + 1U);
                // Save previous bot configuration
                if (ui.botMale->isChecked())
                {
                    mClientOptions.bots[place].identity.gender = Identity::cGenderMale;
                }
                else
                {
                    mClientOptions.bots[place].identity.gender = Identity::cGenderFemale;
                }
                mClientOptions.bots[place].identity.nickname = ui.botName->text().toStdString();

                // other elements are saved on widget action (AI path, avatar ...)
            }

            mPreviousSelectedBot = currentRow;

            RefreshBots();
        }
    }
}
/*****************************************************************************/
void OptionsWindow::slotButtonToggled(int id, bool checked)
{
    if (id == cLocalRadioButtonId)
    {
        if (checked == true)
        {
            ui.buttonImport->setEnabled(true);
            ui.btnPixSud->setEnabled(false);
        }
    }
    else
    {
        ui.buttonImport->setEnabled(false);
        ui.btnPixSud->setEnabled(true);
    }
}
/*****************************************************************************/
void OptionsWindow::slotBtnOk()
{
    bool valid = true;

    mClientOptions.identity.nickname = ui.nomJoueurSud->text().toStdString();
    if (ui.sexeM->isChecked())
    {
        mClientOptions.identity.gender = Identity::cGenderMale;
    }
    else
    {
        mClientOptions.identity.gender = Identity::cGenderFemale;
    }

    if (ui.afficheAvatars->isChecked())
    {
        mClientOptions.showAvatars = true;
    }
    else
    {
        mClientOptions.showAvatars = false;
    }
    mClientOptions.language = ui.langList->currentIndex();
    if (mLanguageIndex != mClientOptions.language)
    {
        QMessageBox::information(this, tr("Information"),
                                 tr("You must restart TarotClub to enable the new language.") + "\n\n");
    }
    mClientOptions.backgroundColor = colorName.toStdString();
    mClientOptions.timer = ui.slider1->value();
    mClientOptions.delayBeforeCleaning = ui.slider2->value();

    if (ui.clic->isChecked())
    {
        mClientOptions.clickToClean = true;
    }
    else
    {
        mClientOptions.clickToClean = false;
    }
    mClientOptions.cardsOrder = dragWidget->GetOrder();


    // -------------  TOURNAMENT TAB --------------

    if (ui.tournamentList->rowCount() == 0)
    {
        valid = false;
        (void) QMessageBox::information(this, tr("TarotClub"),
                                    tr("Please define at least one tournament turn"),
                                    QMessageBox::Ok);
    }

    mTournamentOptions.turns.clear();
    for (int i = 0; i < ui.tournamentList->rowCount(); i++)
    {
        QString dealType = ui.tournamentList->item(i, 1)->text();
        QString seed = ui.tournamentList->item(i, 2)->text();
        QString path = ui.tournamentList->item(i, 3)->text();
        Tarot::Distribution deal;

        deal.mFile = path.toStdString();
        deal.mSeed = seed.toInt();
        deal.mType = static_cast<std::uint8_t>(dealType.toInt());

        mTournamentOptions.turns.push_back(deal);
    }

    if (valid)
    {
        accept();
    }
}
/*****************************************************************************/
void OptionsWindow::slotBtnDefaut()
{
    mClientOptions = ClientConfig::GetDefault();
    mServerOptions = ServerConfig::GetDefault();

    mPreviousSelectedBot = -1;
    ui.botsList->setCurrentRow(0);
    Refresh();
}
/*****************************************************************************/
void OptionsWindow::slider1Changed(int value)
{
    ui.temps1->setText(tr("%1 seconds").arg((float)(value / 100) / 10));
}
/*****************************************************************************/
void OptionsWindow::slider2Changed(int value)
{
    ui.temps2->setText(tr("%1 seconds").arg((float)(value / 100) / 10));
}
/*****************************************************************************/
void OptionsWindow::slotClickOptionChanged(int state)
{
    if (state == Qt::Checked)
    {
        ui.temps2->setEnabled(true);
        ui.slider2->setEnabled(true);
        ui.label2->setEnabled(true);
    }
    else
    {
        ui.temps2->setEnabled(false);
        ui.slider2->setEnabled(false);
        ui.label2->setEnabled(false);
    }
}
/*****************************************************************************/
QString OptionsWindow::ChooseAvatar(const QString &defaultAvatar)
{
    mSelectedAvatar = -1;
    QString avatar = defaultAvatar;
    if (mAvatarsDiag->exec() == QDialog::Accepted)
    {
        if (mAvatarFiles.contains(mSelectedAvatar))
        {
            AvatarLabel *label = mAvatarFiles.value(mSelectedAvatar);
            if (label != NULL)
            {
                avatar = label->GetFileName();
            }
        }
    }
    return (avatar);
}
/*****************************************************************************/
void OptionsWindow::slotAvatarSelected(int id)
{
    mSelectedAvatar  = id;

    for (int i = 0; i < mAvatarFiles.size(); i++)
    {
        if (mAvatarFiles.contains(i))
        {
            AvatarLabel *label = mAvatarFiles.value(i);
            if (label != NULL)
            {
                if (i == id)
                {
                    // Select it
                    label->setFrameStyle(QFrame::Box);
                    label->setLineWidth(4);
                }
                else
                {
                    // deselect it
                    label->setFrameStyle(QFrame::NoFrame);
                }
            }
        }
    }
}
/*****************************************************************************/
void OptionsWindow::slotBtnPixSud()
{
    QString s;
    QPixmap im;

    s = ChooseAvatar(QString(mClientOptions.identity.avatar.data()));
    if (im.load(s) == false)
    {
        return;
    }
    mClientOptions.identity.avatar = s.toStdString();
    ui.pixSud->setPixmap(im);
}
/*****************************************************************************/
void OptionsWindow::slotButtonBotAvatar()
{
    QString s;
    QPixmap im;

    Place place(ui.botsList->currentRow() + 1U);

    s = ChooseAvatar(QString(mClientOptions.bots[place].identity.avatar.data()));
    if (im.load(s) == false)
    {
        return;
    }
    mClientOptions.bots[place].identity.avatar = s.toStdString();
    ui.botAvatar->setPixmap(im);
}
/*****************************************************************************/
void OptionsWindow::slotImportAvatar()
{
    mImportAvatarWindow.SetFilePath(QString(mClientOptions.identity.avatar.c_str()));
    if (mImportAvatarWindow.exec() == QDialog::Accepted)
    {
        mClientOptions.identity.avatar = mImportAvatarWindow.GetFilePath().toStdString();
        ui.pixSud->setPixmap(mImportAvatarWindow.GetPixmap());
    }
}
/*****************************************************************************/


void OptionsWindow::slotChooseScriptPath()
{

    QString path = QFileDialog::getExistingDirectory(this, tr("Choose a script directory"),
                                                    "",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if (QFileInfo(path).exists() ||
        QDir(path).exists())
    {
        Place place(ui.botsList->currentRow() + 1U);

        mClientOptions.bots[place].scriptFilePath = path.toStdString();
        ui.scriptPath->setText(path);
    }
    else
    {
        (void) QMessageBox::critical(this, tr("TarotClub"),
                                     tr("No file or directory."),
                                     QMessageBox::Ok );
    }

}
/*****************************************************************************/
void OptionsWindow::slotAddServer()
{
    QDialog *dialog = new QDialog(this);
    Ui::newServerUI srvUi;

    srvUi.setupUi(dialog);

    if (dialog->exec() == QDialog::Accepted)
    {
        ServerInfo server;
        server.address = srvUi.address->text().toStdString();
        server.game_tcp_port = srvUi.gamePort->value();

        mClientOptions.serverList.push_back(server);
        UpdateServersList();
    }
}
/*****************************************************************************/
void OptionsWindow::slotRemoveServer()
{
    std::uint32_t item = ui.serverList->currentRow();
    if (item < mClientOptions.serverList.size())
    {
        mClientOptions.serverList.erase(mClientOptions.serverList.begin() + item);
        UpdateServersList();
    }
}
/*****************************************************************************/
void OptionsWindow::slotAddTurn()
{
    NewTurnWindow *newTurn = new NewTurnWindow(this, mDealTypes);

    if (newTurn->exec() == QDialog::Accepted)
    {
        int row = ui.tournamentList->rowCount() + 1;
        ui.tournamentList->setRowCount(row);
        for (int col = 0; col < 4; col++)
        {
            QTableWidgetItem *item = new QTableWidgetItem();
            if (col == 0)
            {
                item->setText(QString().setNum(row));
            }
            else if (col == 1)
            {
                item->setText(mDealTypes[newTurn->GetDealType()]);
            }
            else if (col == 2)
            {
                item->setText(QString().setNum(newTurn->GetDealNumber()));
            }
            else
            {
                item->setText(newTurn->GetDealFilePath());
            }
            ui.tournamentList->setItem(row-1, col, item);
        }
    }
}
/*****************************************************************************/
void OptionsWindow::slotRemoveTurn()
{
    int row = ui.tournamentList->rowCount();
    if (row > 0)
    {
        ui.tournamentList->setRowCount(row - 1);
    }
}
/*****************************************************************************/
void OptionsWindow::UpdateServersList()
{
    ui.serverList->clear();
    for (std::vector<ServerInfo>::iterator iter = mClientOptions.serverList.begin(); iter != mClientOptions.serverList.end(); ++iter)
    {
        QString server = QString(iter->address.c_str()) + QString(" (%1)").arg(iter->game_tcp_port);
        ui.serverList->addItem(server);
    }

    if (mClientOptions.serverList.size() > 0)
    {
        // Select first element
        ui.serverList->setCurrentRow(0);
    }
}
/*****************************************************************************/
/**
 * @brief Refresh widgets with current configuration
 */
void OptionsWindow::Refresh()
{
    ui.nomJoueurSud->setText(QString::fromStdString(mClientOptions.identity.nickname));
    if (mClientOptions.identity.gender == Identity::cGenderMale)
    {
        ui.sexeM->setChecked(true);
    }
    else
    {
        ui.sexeF->setChecked(true);
    }
    ui.afficheAvatars->setChecked(mClientOptions.showAvatars);
    ui.langList->setCurrentIndex(mClientOptions.language);
  //  ui.turns->setValue(static_cast<int>(serverOptions.tournamentTurns));
    mLanguageIndex = mClientOptions.language;

    Avatar avatar(QString(mClientOptions.identity.avatar.c_str()));

    if (avatar.IsPredefined())
    {
        ui.pixSud->setPixmap(avatar.GetFilePath());
        ui.radioPredefined->setChecked(true);
        ui.buttonImport->setEnabled(false);
        ui.btnPixSud->setEnabled(true);
    }
    else
    {
        if (avatar.ExistsInAvatarDirectory())
        {
            ui.pixSud->setPixmap(avatar.GetLocalPath());
        }

        ui.radioLocal->setChecked(true);
        ui.buttonImport->setEnabled(true);
        ui.btnPixSud->setEnabled(false);
    }

    QColor color(mClientOptions.backgroundColor.c_str());
    if (color.isValid())
    {
        colorName = color.name();
        ui.tapisColor->setPalette(QPalette(color));
        ui.tapisColor->setAutoFillBackground(true);
    }
    ui.slider1->setValue(mClientOptions.timer);
    ui.slider2->setValue(mClientOptions.delayBeforeCleaning);

    if (mClientOptions.clickToClean == true)
    {
        ui.clic->setChecked(true);
        // Force refresh
        slotClickOptionChanged(Qt::Checked);
    }
    else
    {
        ui.clic->setChecked(false);
        // Force refresh
        slotClickOptionChanged(Qt::Unchecked);
    }
    dragWidget->SetOrder(mClientOptions.cardsOrder);

    // -------------  BOTS TAB --------------
    RefreshBots();

    // -------------  SERVERS LIST TAB --------------
    UpdateServersList();

    // -------------  TOURNAMENT TAB --------------
    std::vector<Tarot::Distribution>::iterator iter;

    ui.tournamentList->setRowCount(0);
    int row;
    for (iter = mTournamentOptions.turns.begin();
         iter != mTournamentOptions.turns.end();
         ++iter)
    {
        row = ui.tournamentList->rowCount();
        ui.tournamentList->setRowCount(row+1);

        for (int col = 0; col < 4; col++)
        {
            QTableWidgetItem *item = new QTableWidgetItem();
            if (col == 0)
            {
                item->setText(QString().setNum(row+1));
            }
            else if (col == 1)
            {
                item->setText(mDealTypes[iter->mType]);
            }
            else if (col == 2)
            {
                item->setText(QString().setNum(iter->mSeed));
            }
            else
            {
                item->setText(QString(iter->mFile.c_str()));
            }
            ui.tournamentList->setItem(row, col, item);
        }
    }
}
/*****************************************************************************/
void OptionsWindow::RefreshBots()
{
    // Get the place of the selected bot
    Place place(ui.botsList->currentRow() + 1U);

    ui.botName->setText(QString::fromStdString(mClientOptions.bots[place].identity.nickname));
    ui.scriptPath->setText(QString::fromStdString(mClientOptions.bots[place].scriptFilePath));

    if (mClientOptions.bots[place].identity.gender == Identity::cGenderMale)
    {
        ui.botMale->setChecked(true);
    }
    else
    {
        ui.botFemale->setChecked(true);
    }

    QPixmap im;
    if (im.load(QString(mClientOptions.bots[place].identity.avatar.data())) == true)
    {
        ui.botAvatar->setPixmap(im);
    }

}

//=============================================================================
// Fin du fichier OptionsWindow.cpp
//=============================================================================
