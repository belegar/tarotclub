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
void OptionsWindow::setClientOptions(ClientOptions *cfg)
{
   clientOptions = *cfg;
   refresh();
}
/*****************************************************************************/
void OptionsWindow::setServerOptions(ServerOptions *cfg)
{
   serverOptions = *cfg;
   refresh();
}
/*****************************************************************************/
ClientOptions *OptionsWindow::getClientOptions()
{
   return &clientOptions;
}
/*****************************************************************************/
ServerOptions *OptionsWindow::getServerOptions()
{
   return &serverOptions;
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
void OptionsWindow::slotBtnOk()
{
   clientOptions.identity.name = ui.nomJoueurSud->text();
   clientOptions.identity.quote = ui.citationSud->text();
   if( ui.sexeM->isChecked() ) {
      clientOptions.identity.sex = MALE;
   } else {
      clientOptions.identity.sex = FEMALE;
   }
   if( ui.afficheAvatars->isChecked() ) {
      clientOptions.showAvatars = true;
   } else {
      clientOptions.showAvatars = false;
   }
   clientOptions.langue = ui.langList->currentIndex();
   if( indexLangue != clientOptions.langue ) {
     QMessageBox::information( this, trUtf8("Information"),
                    trUtf8("Vous devez redémarrer le jeu pour que le changement de langue soit actif.\n\n") );
   }
   clientOptions.tapis = colorName;

   // Server stuff
   serverOptions.timer = ui.slider1->value();
   serverOptions.port = QString( ui.portReseau->text()).toInt();

   serverOptions.bots[BOT_EAST].name = ui.nomJoueurEst->text();
   serverOptions.bots[BOT_NORTH].name = ui.nomJoueurNord->text();
   serverOptions.bots[BOT_WEST].name = ui.nomJoueurOuest->text();

   accept();
}
/*****************************************************************************/
void OptionsWindow::slotBtnDefaut()
{
   ClientConfig::setDefault( &clientOptions );
   ServerConfig::setDefault( &serverOptions );
   refresh();
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

   s = choixAvatar(clientOptions.identity.avatar);
   if( im.load( s ) == false ) {
      return;
   }
   clientOptions.identity.avatar = s;
   ui.pixSud->setPixmap( im );
}
/*****************************************************************************/
void OptionsWindow::slotBtnPixEst()
{
   QString s;
   QPixmap im;

   s = choixAvatar(serverOptions.bots[BOT_EAST].avatar);
   if( im.load( s ) == false ) {
      return;
   }
   serverOptions.bots[BOT_EAST].avatar = s;
   ui.pixEst->setPixmap( im );
}
/*****************************************************************************/
void OptionsWindow::slotBtnPixNord()
{
   QString s;
   QPixmap im;

   s = choixAvatar(serverOptions.bots[BOT_NORTH].avatar);
   if( im.load( s ) == false ) {
      return;
   }
   serverOptions.bots[BOT_NORTH].avatar = s;
   ui.pixNord->setPixmap( im );
}
/*****************************************************************************/
void OptionsWindow::slotBtnPixOuest()
{
   QString s;
   QPixmap im;

   s = choixAvatar(serverOptions.bots[BOT_WEST].avatar);
   if( im.load( s ) == false ) {
      return;
   }
   serverOptions.bots[BOT_WEST].avatar = s;
   ui.pixOuest->setPixmap( im );
}
/*****************************************************************************/
/**
 * Rafraichi les Widgets graphiques
 */
void OptionsWindow::refresh()
{
   QPixmap im;

   ui.nomJoueurSud->setText( clientOptions.identity.name );
   ui.citationSud->setText( clientOptions.identity.quote );
   if( clientOptions.identity.sex == MALE ) {
      ui.sexeM->setChecked( true );
   } else {
      ui.sexeF->setChecked( true );
   }
   ui.afficheAvatars->setChecked( clientOptions.showAvatars );
   ui.langList->setCurrentIndex( clientOptions.langue );
   indexLangue = clientOptions.langue;
   if( im.load( clientOptions.identity.avatar ) == true ) {
      ui.pixSud->setPixmap( im );
   }
   QColor color(clientOptions.tapis);
   if (color.isValid()) {
       colorName = color.name();
       ui.tapisColor->setPalette(QPalette(color));
       ui.tapisColor->setAutoFillBackground(true);
   }

   // server stuff
   ui.slider1->setValue( serverOptions.timer );
   ui.portReseau->setValue( serverOptions.port );
   ui.nomJoueurEst->setText( serverOptions.bots[BOT_EAST].name );
   ui.nomJoueurNord->setText( serverOptions.bots[BOT_NORTH].name );
   ui.nomJoueurOuest->setText( serverOptions.bots[BOT_WEST].name );

   if( im.load( serverOptions.bots[BOT_EAST].avatar ) == true ) {
      ui.pixEst->setPixmap( im );
   }
   if( im.load( serverOptions.bots[BOT_NORTH].avatar ) == true ) {
      ui.pixNord->setPixmap( im );
   }
   if( im.load( serverOptions.bots[BOT_WEST].avatar ) == true ) {
      ui.pixOuest->setPixmap( im );
   }

   ui.niveauEst->setCurrentIndex(  0  );
   ui.niveauNord->setCurrentIndex( 0 );
   ui.niveauOuest->setCurrentIndex( 0 );
}

//=============================================================================
// Fin du fichier OptionsWindow.cpp
//=============================================================================
