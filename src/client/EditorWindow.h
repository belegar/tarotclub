/*=============================================================================
 * TarotClub - EditorWindow.h
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

#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <QDialog>
#include <QListWidgetItem>
#include "../defines.h"
#include "../Card.h"
#include <ui_DealEditorUI.h>

/*****************************************************************************/
class CardListItem : public QListWidgetItem
{
    Card *c;

public:
    CardListItem(Card *ref) : QListWidgetItem()
    {
        c = ref;
        setText(c->GetName());
    }

    Card *GetCard()
    {
        return c;
    }
};


/*****************************************************************************/
class EditorWindow : public QDialog
{
    Q_OBJECT

private:
    Ui::DealEditorUI ui;

public:
    EditorWindow(QWidget *parent = 0);

    void Initialize();
    void AddToList(int id, QListWidget *list, QListWidgetItem *item);

public slots:
    void slotOpenDeal();
    void slotSaveDeal();

    void slotToSouth();
    void slotToNorth();
    void slotToWest();
    void slotToEast();

    void slotRemoveSouthCard(QListWidgetItem *item);
    void slotRemoveNorthCard(QListWidgetItem *item);
    void slotRemoveWestCard(QListWidgetItem *item);
    void slotRemoveEastCard(QListWidgetItem *item);

};

#endif // EDITORWINDOW_H

//=============================================================================
// Fin du fichier EditorWindow.h
//=============================================================================


