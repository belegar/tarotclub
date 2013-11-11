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
#include <QDir>
#include <QMessageBox>
#include <QColorDialog>
#include <QString>

/*****************************************************************************/
OptionsWindow::OptionsWindow(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    connect(ui.btnDefaut, SIGNAL(clicked()), this, SLOT(slotBtnDefaut()));
    connect(ui.btnOk, SIGNAL(clicked()), this, SLOT(slotBtnOk()));

    // Sliders slots
    connect(ui.slider1, SIGNAL(valueChanged(int)), this, SLOT(slider1Changed(int)));
    connect(ui.slider2, SIGNAL(valueChanged(int)), this, SLOT(slider2Changed(int)));

    // Avatar choice
    connect(ui.btnPixSud, SIGNAL(clicked()), this, SLOT(slotBtnPixSud()));
    connect(ui.btnPixEst, SIGNAL(clicked()), this, SLOT(slotBtnPixEst()));
    connect(ui.btnPixNord, SIGNAL(clicked()), this, SLOT(slotBtnPixNord()));
    connect(ui.btnPixOuest, SIGNAL(clicked()), this, SLOT(slotBtnPixOuest()));

    // Background color choice
    connect(ui.tapisColor, SIGNAL(clicked()), this, SLOT(slotColorPicker()));

    // Disable the delay when the click option has been choosen
    connect(ui.clic, SIGNAL(stateChanged(int)), this, SLOT(slotClickOptionChanged(int)));

    QStringList listeNiveaux;
    listeNiveaux.append("Amibe");

    ui.niveauEst->addItems(listeNiveaux);
    ui.niveauNord->addItems(listeNiveaux);
    ui.niveauOuest->addItems(listeNiveaux);

}
/*****************************************************************************/
void OptionsWindow::SetClientOptions(ClientOptions &cfg)
{
    clientOptions = cfg;
    refresh();
}
/*****************************************************************************/
void OptionsWindow::SetServerOptions(ServerOptions &cfg)
{
    serverOptions = cfg;
    refresh();
}
/*****************************************************************************/
ClientOptions &OptionsWindow::GetClientOptions()
{
    return clientOptions;
}
/*****************************************************************************/
ServerOptions &OptionsWindow::GetServerOptions()
{
    return serverOptions;
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
void OptionsWindow::slotBtnOk()
{
    clientOptions.identity.name = ui.nomJoueurSud->text();
    clientOptions.identity.quote = ui.citationSud->text();
    if (ui.sexeM->isChecked())
    {
        clientOptions.identity.sex = Identity::MALE;
    }
    else
    {
        clientOptions.identity.sex = Identity::FEMALE;
    }

    if (ui.afficheAvatars->isChecked())
    {
        clientOptions.showAvatars = true;
    }
    else
    {
        clientOptions.showAvatars = false;
    }
    clientOptions.language = ui.langList->currentIndex();

    if (indexLangue != clientOptions.language)
    {
        QMessageBox::information(this, tr("Information"),
                                 tr("You must restart TarotClub to enable the new language.") + "\n\n");
    }
    clientOptions.backgroundColor = colorName;
    clientOptions.delayBeforeCleaning = ui.slider2->value();

    if (ui.clic->isChecked())
    {
        clientOptions.enableDelayBeforeCleaning = true;
    }
    else
    {
        clientOptions.enableDelayBeforeCleaning = false;
    }

    // Server stuff
    serverOptions.timer = ui.slider1->value();
    serverOptions.port = QString(ui.portReseau->text()).toInt();

    serverOptions.bots[ServerConfig::BOT_EAST].name = ui.nomJoueurEst->text();
    serverOptions.bots[ServerConfig::BOT_NORTH].name = ui.nomJoueurNord->text();
    serverOptions.bots[ServerConfig::BOT_WEST].name = ui.nomJoueurOuest->text();

    accept();
}
/*****************************************************************************/
void OptionsWindow::slotBtnDefaut()
{
    ClientConfig::SetDefault(clientOptions);
    ServerConfig::SetDefault(serverOptions);
    refresh();
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
QString OptionsWindow::choixAvatar(QString defaultAvatar)
{
    Ui::Avatars ui;
    QDialog *diag = new QDialog(this);
    ui.setupUi(diag);

    // populate the list with internal avatar
    ui.avatarsList->clear();
    QDir dir(":/images/avatars");
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList list = dir.entryInfoList();

    // On affiche la liste des avatars
    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
        QListWidgetItem *item = new QListWidgetItem(ui.avatarsList);
        item->setText(fileInfo.baseName());
        item->setIcon(QIcon(fileInfo.absoluteFilePath()));
        item->setData(Qt::UserRole, fileInfo.absoluteFilePath());
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }

    if (diag->exec() == QDialog::Rejected)
    {
        return (defaultAvatar);
    }
    else if (ui.avatarsList->currentItem() != NULL)
    {
        // On retourne le nom de l'image
        return (ui.avatarsList->currentItem()->data(Qt::UserRole).toString());
    }
    else
    {
        return (defaultAvatar);
    }
}
/*****************************************************************************/
void OptionsWindow::slotBtnPixSud()
{
    QString s;
    QPixmap im;

    s = choixAvatar(clientOptions.identity.avatar);
    if (im.load(s) == false)
    {
        return;
    }
    clientOptions.identity.avatar = s;
    ui.pixSud->setPixmap(im);
}
/*****************************************************************************/
void OptionsWindow::slotBtnPixEst()
{
    QString s;
    QPixmap im;

    s = choixAvatar(serverOptions.bots[ServerConfig::BOT_EAST].avatar);
    if (im.load(s) == false)
    {
        return;
    }
    serverOptions.bots[ServerConfig::BOT_EAST].avatar = s;
    ui.pixEst->setPixmap(im);
}
/*****************************************************************************/
void OptionsWindow::slotBtnPixNord()
{
    QString s;
    QPixmap im;

    s = choixAvatar(serverOptions.bots[ServerConfig::BOT_NORTH].avatar);
    if (im.load(s) == false)
    {
        return;
    }
    serverOptions.bots[ServerConfig::BOT_NORTH].avatar = s;
    ui.pixNord->setPixmap(im);
}
/*****************************************************************************/
void OptionsWindow::slotBtnPixOuest()
{
    QString s;
    QPixmap im;

    s = choixAvatar(serverOptions.bots[ServerConfig::BOT_WEST].avatar);
    if (im.load(s) == false)
    {
        return;
    }
    serverOptions.bots[ServerConfig::BOT_WEST].avatar = s;
    ui.pixOuest->setPixmap(im);
}
/*****************************************************************************/
/**
 * @brief Refresh widgets with current configuration
 */
void OptionsWindow::refresh()
{
    QPixmap im;

    ui.nomJoueurSud->setText(clientOptions.identity.name);
    ui.citationSud->setText(clientOptions.identity.quote);
    if (clientOptions.identity.sex == Identity::MALE)
    {
        ui.sexeM->setChecked(true);
    }
    else
    {
        ui.sexeF->setChecked(true);
    }
    ui.afficheAvatars->setChecked(clientOptions.showAvatars);
    ui.langList->setCurrentIndex(clientOptions.language);
    indexLangue = clientOptions.language;
    if (im.load(clientOptions.identity.avatar) == true)
    {
        ui.pixSud->setPixmap(im);
    }
    QColor color(clientOptions.backgroundColor);
    if (color.isValid())
    {
        colorName = color.name();
        ui.tapisColor->setPalette(QPalette(color));
        ui.tapisColor->setAutoFillBackground(true);
    }
    ui.slider2->setValue(clientOptions.delayBeforeCleaning);

    if (clientOptions.enableDelayBeforeCleaning == true)
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

    // server stuff
    ui.slider1->setValue(serverOptions.timer);
    ui.portReseau->setValue(serverOptions.port);
    ui.nomJoueurEst->setText(serverOptions.bots[ServerConfig::BOT_EAST].name);
    ui.nomJoueurNord->setText(serverOptions.bots[ServerConfig::BOT_NORTH].name);
    ui.nomJoueurOuest->setText(serverOptions.bots[ServerConfig::BOT_WEST].name);

    if (im.load(serverOptions.bots[ServerConfig::BOT_EAST].avatar) == true)
    {
        ui.pixEst->setPixmap(im);
    }
    if (im.load(serverOptions.bots[ServerConfig::BOT_NORTH].avatar) == true)
    {
        ui.pixNord->setPixmap(im);
    }
    if (im.load(serverOptions.bots[ServerConfig::BOT_WEST].avatar) == true)
    {
        ui.pixOuest->setPixmap(im);
    }

    ui.niveauEst->setCurrentIndex(0);
    ui.niveauNord->setCurrentIndex(0);
    ui.niveauOuest->setCurrentIndex(0);
}

//=============================================================================
// Fin du fichier OptionsWindow.cpp
//=============================================================================
