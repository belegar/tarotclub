/*=============================================================================
 * TarotClub - EditorWindow.cpp
 *=============================================================================
 * Edit custom deal for each player
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

#include "EditorWindow.h"
#include <QObject>
#include <QFileDialog>
#include "DealFile.h"
#include "TarotDeck.h"

/*****************************************************************************/
EditorWindow::EditorWindow(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    connect(ui.openButton, SIGNAL(clicked()), this, SLOT(slotOpenDeal()));
    connect(ui.saveButton, SIGNAL(clicked()), this, SLOT(slotSaveDeal()));

    // Arrow to dispatch cards
    connect(ui.toSouth, SIGNAL(clicked()), this, SLOT(slotToSouth()));
    connect(ui.toNorth, SIGNAL(clicked()), this, SLOT(slotToNorth()));
    connect(ui.toWest, SIGNAL(clicked()), this, SLOT(slotToWest()));
    connect(ui.toEast, SIGNAL(clicked()), this, SLOT(slotToEast()));

    // Double click to return the card into the main list
    connect(ui.southList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(slotRemoveSouthCard(QListWidgetItem *)));
    connect(ui.northList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(slotRemoveNorthCard(QListWidgetItem *)));
    connect(ui.westList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(slotRemoveWestCard(QListWidgetItem *)));
    connect(ui.eastList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(slotRemoveEastCard(QListWidgetItem *)));
}
/*****************************************************************************/
void EditorWindow::Initialize()
{
    ui.mainCardList->clear();
    ui.eastList->clear();
    ui.northList->clear();
    ui.southList->clear();
    ui.westList->clear();

    Deck deck;
    deck.CreateTarotDeck();
    for (Deck::ConstIterator it = deck.Begin(); it != deck.End(); ++it)
    {
        ui.mainCardList->addItem(new QListWidgetItem((*it).GetName().data()));
    }
}
/*****************************************************************************/
void EditorWindow::slotToSouth()
{
    if (ui.southList->count() == 18)
    {
        return;
    }

    int row = ui.mainCardList->currentRow();
    QListWidgetItem *element = ui.mainCardList->takeItem(row);
    ui.southList->addItem(element);
}
/*****************************************************************************/
void EditorWindow::slotToNorth()
{
    if (ui.northList->count() == 18)
    {
        return;
    }

    int row = ui.mainCardList->currentRow();
    QListWidgetItem *element = ui.mainCardList->takeItem(row);
    ui.northList->addItem(element);
}
/*****************************************************************************/
void EditorWindow::slotToWest()
{
    if (ui.westList->count() == 18)
    {
        return;
    }

    int row = ui.mainCardList->currentRow();
    QListWidgetItem *element = ui.mainCardList->takeItem(row);
    ui.westList->addItem(element);
}
/*****************************************************************************/
void EditorWindow::slotToEast()
{
    if (ui.eastList->count() == 18)
    {
        return;
    }

    int row = ui.mainCardList->currentRow();
    QListWidgetItem *element = ui.mainCardList->takeItem(row);
    ui.eastList->addItem(element);
}
/*****************************************************************************/
void EditorWindow::slotRemoveSouthCard(QListWidgetItem *item)
{
    Q_UNUSED(item);

    int row = ui.southList->currentRow();
    QListWidgetItem *element = ui.southList->takeItem(row);
    ui.mainCardList->addItem(element);
}
/*****************************************************************************/
void EditorWindow::slotRemoveNorthCard(QListWidgetItem *item)
{
    Q_UNUSED(item);

    int row = ui.northList->currentRow();
    QListWidgetItem *element = ui.northList->takeItem(row);
    ui.mainCardList->addItem(element);
}
/*****************************************************************************/
void EditorWindow::slotRemoveWestCard(QListWidgetItem *item)
{
    Q_UNUSED(item);

    int row = ui.westList->currentRow();
    QListWidgetItem *element = ui.westList->takeItem(row);
    ui.mainCardList->addItem(element);
}
/*****************************************************************************/
void EditorWindow::slotRemoveEastCard(QListWidgetItem *item)
{
    Q_UNUSED(item);

    int row = ui.eastList->currentRow();
    QListWidgetItem *element = ui.eastList->takeItem(row);
    ui.mainCardList->addItem(element);
}
/*****************************************************************************/
void EditorWindow::slotOpenDeal()
{
    // FIXME: add the loading mechanism
}
/*****************************************************************************/
void EditorWindow::slotSaveDeal()
{
    if (ui.southList->count() != 18 ||
            ui.northList->count() != 18 ||
            ui.westList->count() != 18 ||
            ui.eastList->count() != 18)
    {
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save deal"),
                       "tarot.json",
                       tr("JSON (*json)"));
    if (fileName.isEmpty())
    {
        return;
    }

    DealFile editor;
    editor.Clear();

    // Chien
    for (int i = 0; i < ui.mainCardList->count(); i++)
    {
        QListWidgetItem *item = ui.mainCardList->item(i);
        if (item != NULL)
        {
            editor.GetDogDeck().Append(Card(item->text().toStdString()));
        }
    }
    // East
    for (int i = 0; i < ui.eastList->count(); i++)
    {
        QListWidgetItem *item = ui.eastList->item(i);
        if (item != NULL)
        {
            editor.GetEastDeck().Append(Card(item->text().toStdString()));
        }
    }
    // West
    for (int i = 0; i < ui.westList->count(); i++)
    {
        QListWidgetItem *item = ui.westList->item(i);
        if (item != NULL)
        {
            editor.GetWestDeck().Append(Card(item->text().toStdString()));
        }
    }
    // South
    for (int i = 0; i < ui.southList->count(); i++)
    {
        QListWidgetItem *item = ui.southList->item(i);
        if (item != NULL)
        {
            editor.GetSouthDeck().Append(Card(item->text().toStdString()));
        }
    }
    // North
    for (int i = 0; i < ui.northList->count(); i++)
    {
        QListWidgetItem *item = ui.northList->item(i);
        if (item != NULL)
        {
            editor.GetNorthDeck().Append(Card(item->text().toStdString()));
        }
    }

    editor.SaveFile(fileName.toStdString());
}

//=============================================================================
// Fin du fichier EditorWindow.cpp
//=============================================================================
