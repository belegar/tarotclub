/*=============================================================================
 * TarotClub - EditorWindow.cpp
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

#include "EditorWindow.h"
#include <QObject>
#include <QFileDialog>
#include "../DealEditorFile.h"

/*****************************************************************************/
EditorWindow::EditorWindow(QWidget* parent)
    : QDialog( parent)
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
   connect(ui.southList, SIGNAL(itemDoubleClicked (QListWidgetItem *)), this, SLOT(slotRemoveSouthCard(QListWidgetItem *)));
   connect(ui.northList, SIGNAL(itemDoubleClicked (QListWidgetItem *)), this, SLOT(slotRemoveNorthCard(QListWidgetItem *)));
   connect(ui.westList, SIGNAL(itemDoubleClicked (QListWidgetItem *)), this, SLOT(slotRemoveWestCard(QListWidgetItem *)));
   connect(ui.eastList, SIGNAL(itemDoubleClicked (QListWidgetItem *)), this, SLOT(slotRemoveEastCard(QListWidgetItem *)));
}
/*****************************************************************************/
void EditorWindow::init()
{
   ui.mainCardList->clear();
   ui.eastList->clear();
   ui.northList->clear();
   ui.southList->clear();
   ui.westList->clear();

   for(int i=0; i<78; i++) {
      ui.mainCardList->addItem(new CardListItem(Jeu::getCard(i)));
   }
}
/*****************************************************************************/
void EditorWindow::addToList(int id, QListWidget *list, QListWidgetItem *item)
{
   int i;

   // find the best row to insert it
   for (i=0; i<list->count(); i++) {
      CardListItem *el = (CardListItem *)list->item(i);
      if (id < el->getCard()->getId()) {
         break;
      }
   }
   list->insertItem(i,item);
}
/*****************************************************************************/
void EditorWindow::slotToSouth()
{
   if(ui.southList->count() == 18)
      return;

   int row = ui.mainCardList->currentRow();
   QListWidgetItem *element = ui.mainCardList->takeItem(row);
   Card *c = ((CardListItem *)(element))->getCard();
   addToList(c->getId(), ui.southList, element);
}
/*****************************************************************************/
void EditorWindow::slotToNorth()
{
   if(ui.northList->count() == 18)
      return;

   int row = ui.mainCardList->currentRow();
   QListWidgetItem *element = ui.mainCardList->takeItem(row);
   Card *c = ((CardListItem *)(element))->getCard();
   addToList(c->getId(), ui.northList, element);
}
/*****************************************************************************/
void EditorWindow::slotToWest()
{
   if(ui.westList->count() == 18)
      return;

   int row = ui.mainCardList->currentRow();
   QListWidgetItem *element = ui.mainCardList->takeItem(row);
   Card *c = ((CardListItem *)(element))->getCard();
   addToList(c->getId(), ui.westList, element);
}
/*****************************************************************************/
void EditorWindow::slotToEast()
{
   if(ui.eastList->count() == 18)
      return;

   int row = ui.mainCardList->currentRow();
   QListWidgetItem *element = ui.mainCardList->takeItem(row);
   Card *c = ((CardListItem *)(element))->getCard();
   addToList(c->getId(), ui.eastList, element);
}
/*****************************************************************************/
void EditorWindow::slotRemoveSouthCard(QListWidgetItem *item)
{
   Q_UNUSED(item);

   int row = ui.southList->currentRow();
   QListWidgetItem *element = ui.southList->takeItem(row);
   Card *c = ((CardListItem *)(element))->getCard();

   addToList(c->getId(), ui.mainCardList, element);
}
/*****************************************************************************/
void EditorWindow::slotRemoveNorthCard(QListWidgetItem *item)
{
   Q_UNUSED(item);

   int row = ui.northList->currentRow();
   QListWidgetItem *element = ui.northList->takeItem(row);
   Card *c = ((CardListItem *)(element))->getCard();

   addToList(c->getId(), ui.mainCardList, element);
}
/*****************************************************************************/
void EditorWindow::slotRemoveWestCard(QListWidgetItem *item)
{
   Q_UNUSED(item);

   int row = ui.westList->currentRow();
   QListWidgetItem *element = ui.westList->takeItem(row);
   Card *c = ((CardListItem *)(element))->getCard();

   addToList(c->getId(), ui.mainCardList, element);
}
/*****************************************************************************/
void EditorWindow::slotRemoveEastCard(QListWidgetItem *item)
{
   Q_UNUSED(item);

   int row = ui.eastList->currentRow();
   QListWidgetItem *element = ui.eastList->takeItem(row);
   Card *c = ((CardListItem *)(element))->getCard();

   addToList(c->getId(), ui.mainCardList, element);
}
/*****************************************************************************/
void EditorWindow::slotOpenDeal()
{

}
/*****************************************************************************/
void EditorWindow::slotSaveDeal()
{
   if(ui.southList->count() != 18 ||
      ui.northList->count() != 18 ||
      ui.westList->count() != 18 ||
      ui.eastList->count() != 18 )
      return;

   QString fileName = QFileDialog::getSaveFileName(this);
   if (fileName.isEmpty())
      return;

   DealEditorFile editor;
   editor.chienDeck.clear();
   editor.eastDeck.clear();
   editor.northDeck.clear();
   editor.southDeck.clear();
   editor.westDeck.clear();

   // Chien
   for (int i=0; i<ui.mainCardList->count(); i++) {
      CardListItem *el = (CardListItem *)ui.mainCardList->item(i);
      editor.chienDeck.append( ((CardListItem *)el)->getCard() );
   }
   // East
   for (int i=0; i<ui.eastList->count(); i++) {
      CardListItem *el = (CardListItem *)ui.eastList->item(i);
      editor.eastDeck.append( ((CardListItem *)el)->getCard() );
   }
   // West
   for (int i=0; i<ui.westList->count(); i++) {
      CardListItem *el = (CardListItem *)ui.westList->item(i);
      editor.westDeck.append( ((CardListItem *)el)->getCard() );
   }
   // South
   for (int i=0; i<ui.southList->count(); i++) {
      CardListItem *el = (CardListItem *)ui.southList->item(i);
      editor.southDeck.append( ((CardListItem *)el)->getCard() );
   }
   // North
   for (int i=0; i<ui.northList->count(); i++) {
      CardListItem *el = (CardListItem *)ui.northList->item(i);
      editor.northDeck.append( ((CardListItem *)el)->getCard() );
   }

   editor.saveFile(fileName);
}

//=============================================================================
// Fin du fichier EditorWindow.cpp
//=============================================================================
