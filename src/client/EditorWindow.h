/*=============================================================================
 * TarotClub - EditorWindow.h
 *=============================================================================
 * Edit custom deal for each player
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * This file must be used under the terms of the CeCILL.
 * This source file is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at
 * http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
 *
 *=============================================================================
 */

#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

// Includes Qt
#include <QDialog>
#include <QListWidgetItem>

// Includes locales
#include "../defines.h"
#include "../Jeu.h"
#include <ui_DealEditorUI.h>

/*****************************************************************************/
class CardListItem : public QListWidgetItem
{
   Card *c;

public:
   CardListItem(Card *ref) : QListWidgetItem() {
      c = ref;
      setText(c->getCardName());
   }

   Card *getCard() { return c; }
};


/*****************************************************************************/
class EditorWindow : public QDialog
{
	Q_OBJECT

private:
   Ui::DealEditorUI ui;

public:
   EditorWindow( QWidget* parent = 0, Qt::WFlags fl = 0 );

   void init();
   void addToList(int id, QListWidget *list, QListWidgetItem *item);

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


