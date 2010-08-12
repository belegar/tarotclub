/*=============================================================================
 * TarotClub - OptionsWindow.cpp
 *=============================================================================
 * OptionsWindow : fenêtre d'options
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

#include "OptionsWindow.h"
#include <QDir>
#include <QMessageBox>
#include <QColorDialog>
#include <QString>

/*****************************************************************************/
OptionsWindow::OptionsWindow( QWidget* parent, Qt::WFlags fl )
   : QDialog( parent, fl )
{
    ui.setupUi(this);

   connect( ui.btnDefaut, SIGNAL(clicked()), this, SLOT(slotBtnDefaut()) );
   connect( ui.btnOk, SIGNAL(clicked()), this, SLOT(slotBtnOk()) );

   // Défilement des cartes
   connect( ui.slider1, SIGNAL(valueChanged(int)), this, SLOT(slider1Changed(int)) );

   // Boutons choix de l'avatar
   connect( ui.btnPixSud, SIGNAL(clicked()), this, SLOT(slotBtnPixSud()) );
   connect( ui.btnPixEst, SIGNAL(clicked()), this, SLOT(slotBtnPixEst()) );
   connect( ui.btnPixNord, SIGNAL(clicked()), this, SLOT(slotBtnPixNord()) );
   connect( ui.btnPixOuest, SIGNAL(clicked()), this, SLOT(slotBtnPixOuest()) );

   // Choix de la couleur du tapis
   connect( ui.tapisColor, SIGNAL(clicked()), this, SLOT(slotColorPicker()) );

   QStringList listeNiveaux;
   listeNiveaux.append("Amibe");

   ui.niveauEst->addItems( listeNiveaux );
   ui.niveauNord->addItems( listeNiveaux );
   ui.niveauOuest->addItems( listeNiveaux );

}
/*****************************************************************************/
void OptionsWindow::slotColorPicker()
{
   QColor color = QColorDialog::getColor(Qt::darkGreen, this);
   if (color.isValid()) {
       colorName = color.name();
       ui.tapisColor->setPalette(QPalette(color));
       ui.tapisColor->setAutoFillBackground(true);
   }
}
/*****************************************************************************/
void OptionsWindow::setOptions( GameOptions *opt )
{
   options = *opt;
   refresh();
}
/*****************************************************************************/
void OptionsWindow::slotBtnOk()
{
   options.client.name = ui.nomJoueurSud->text();
   options.client.quote = ui.citationSud->text();
   if( ui.sexeM->isChecked() ) {
      options.client.sex = MALE;
   } else {
      options.client.sex = FEMALE;
   }
   if( ui.afficheAvatars->isChecked() ) {
      options.showAvatars = true;
   } else {
      options.showAvatars = false;
   }
   options.langue = ui.langList->currentIndex();
   if( indexLangue != options.langue ) {
     QMessageBox::information( this, trUtf8("Information"),
                    trUtf8("Vous devez redémarrer le jeu pour que le changement de langue soit actif.\n\n") );
   }
   options.tapis = colorName;

   // Server stuff
   options.timer = ui.slider1->value();
   options.port = QString( ui.portReseau->text()).toInt();

   options.bots[BOT_EAST].name = ui.nomJoueurEst->text();
   options.bots[BOT_NORTH].name = ui.nomJoueurNord->text();
   options.bots[BOT_WEST].name = ui.nomJoueurOuest->text();

   accept();
}
/*****************************************************************************/
void OptionsWindow::slotBtnDefaut()
{
   ConfigFile::setDefault( &options );
   refresh();
}
/*****************************************************************************/
GameOptions *OptionsWindow::getOptions()
{
   return(&options);
}
/*****************************************************************************/
void OptionsWindow::slider1Changed( int value )
{
   ui.temps1->setText( trUtf8("%1 secondes").arg((float)(value/100)/10) );
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
   for(int i = 0; i < list.size(); ++i) {
       QFileInfo fileInfo = list.at(i);
       QListWidgetItem *item = new QListWidgetItem(ui.avatarsList);
       item->setText(fileInfo.baseName());
       item->setIcon(QIcon(fileInfo.absoluteFilePath()));
       item->setData(Qt::UserRole, fileInfo.absoluteFilePath());
       item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
   }

   if( diag->exec() == QDialog::Rejected ) {
      return(defaultAvatar);
   } else if (ui.avatarsList->currentItem() != NULL) {
      // On retourne le nom de l'image
      return(ui.avatarsList->currentItem()->data(Qt::UserRole).toString());
   } else {
      return(defaultAvatar);
   }
}
/*****************************************************************************/
void OptionsWindow::slotBtnPixSud()
{
   QString s;
   QPixmap im;

   s = choixAvatar(options.client.avatar);
   if( im.load( s ) == false ) {
      return;
   }
   options.client.avatar = s;
   ui.pixSud->setPixmap( im );
}
/*****************************************************************************/
void OptionsWindow::slotBtnPixEst()
{
   QString s;
   QPixmap im;

   s = choixAvatar(options.bots[BOT_EAST].avatar);
   if( im.load( s ) == false ) {
      return;
   }
   options.bots[BOT_EAST].avatar = s;
   ui.pixEst->setPixmap( im );
}
/*****************************************************************************/
void OptionsWindow::slotBtnPixNord()
{
   QString s;
   QPixmap im;

   s = choixAvatar(options.bots[BOT_NORTH].avatar);
   if( im.load( s ) == false ) {
      return;
   }
   options.bots[BOT_NORTH].avatar = s;
   ui.pixNord->setPixmap( im );
}
/*****************************************************************************/
void OptionsWindow::slotBtnPixOuest()
{
   QString s;
   QPixmap im;

   s = choixAvatar(options.bots[BOT_WEST].avatar);
   if( im.load( s ) == false ) {
      return;
   }
   options.bots[BOT_WEST].avatar = s;
   ui.pixOuest->setPixmap( im );
}
/*****************************************************************************/
/**
 * Rafraichi les Widgets graphiques
 */
void OptionsWindow::refresh()
{
   QPixmap im;

   ui.nomJoueurSud->setText( options.client.name );
   ui.citationSud->setText( options.client.quote );
   if( options.client.sex == MALE ) {
      ui.sexeM->setChecked( true );
   } else {
      ui.sexeF->setChecked( true );
   }
   ui.afficheAvatars->setChecked( options.showAvatars );
   ui.langList->setCurrentIndex( options.langue );
   indexLangue = options.langue;
   if( im.load( options.client.avatar ) == true ) {
      ui.pixSud->setPixmap( im );
   }

   // server stuff
   ui.slider1->setValue( options.timer );
   ui.portReseau->setValue( options.port );
   ui.nomJoueurEst->setText( options.bots[BOT_EAST].name );
   ui.nomJoueurNord->setText( options.bots[BOT_NORTH].name );
   ui.nomJoueurOuest->setText( options.bots[BOT_WEST].name );

   if( im.load( options.bots[BOT_EAST].avatar ) == true ) {
      ui.pixEst->setPixmap( im );
   }
   if( im.load( options.bots[BOT_NORTH].avatar ) == true ) {
      ui.pixNord->setPixmap( im );
   }
   if( im.load( options.bots[BOT_WEST].avatar ) == true ) {
      ui.pixOuest->setPixmap( im );
   }

   ui.niveauEst->setCurrentIndex(  0  );
   ui.niveauNord->setCurrentIndex( 0 );
   ui.niveauOuest->setCurrentIndex( 0 );

   QColor color(options.tapis);
   if (color.isValid()) {
       colorName = color.name();
       ui.tapisColor->setPalette(QPalette(color));
       ui.tapisColor->setAutoFillBackground(true);
   }
}

//=============================================================================
// Fin du fichier OptionsWindow.cpp
//=============================================================================
