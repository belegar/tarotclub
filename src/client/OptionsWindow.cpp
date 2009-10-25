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

/*****************************************************************************/
OptionsWindow::OptionsWindow( QWidget* parent, Qt::WFlags fl )
   : QDialog( parent, fl )
{
    ui.setupUi(this);

   connect( ui.btnDefaut, SIGNAL(clicked()), this, SLOT(slotBtnDefaut()) );
   connect( ui.btnOk, SIGNAL(clicked()), this, SLOT(slotBtnOk()) );

   // DÃ©filement des cartes
   connect( ui.slider1, SIGNAL(valueChanged(int)), this, SLOT(slider1Changed(int)) );
   connect( ui.slider2, SIGNAL(valueChanged(int)), this, SLOT(slider2Changed(int)) );

   // Boutons choix de l'avatar
   connect( ui.btnPixSud, SIGNAL(clicked()), this, SLOT(slotBtnPixSud()) );
   connect( ui.btnPixEst, SIGNAL(clicked()), this, SLOT(slotBtnPixEst()) );
   connect( ui.btnPixNord, SIGNAL(clicked()), this, SLOT(slotBtnPixNord()) );
   connect( ui.btnPixOuest, SIGNAL(clicked()), this, SLOT(slotBtnPixOuest()) );
   connect( ui.btnPixNordOuest, SIGNAL(clicked()), this, SLOT(slotBtnPixNordOuest()) );

   // Choix du tapis de jeux
   connect( ui.scrollBarTapis, SIGNAL(valueChanged(int)), this, SLOT(slotScrollBarTapis(int)) );

   QStringList listeNiveaux;
   listeNiveaux.append("Amibe");

   ui.niveauEst->addItems( listeNiveaux );
   ui.niveauNord->addItems( listeNiveaux );
   ui.niveauOuest->addItems( listeNiveaux );
   ui.niveauNordOuest->addItems( listeNiveaux );

}
/*****************************************************************************/
void OptionsWindow::setPath( const QString &game_path )
{
   QDir d(game_path+"/data/");
   int max;

   path = game_path;

   // On cherche les tapis disponibles
   d.setFilter( QDir::Files );
   QFileInfoList list = d.entryInfoList();

   for (int i = 0; i < list.size(); ++i) {
      QFileInfo fi = list.at(i);
      if( fi.suffix() == "png" ) {
         tapisList += fi.fileName();
      }
   }
   ui.scrollBarTapis->setMinimum(0);
   max = tapisList.count();
   if( max ) {
      ui.scrollBarTapis->setMaximum( tapisList.count()-1 );
   } else {
      ui.scrollBarTapis->setMaximum(0);
   }
}
/*****************************************************************************/
void OptionsWindow::setOptions( GameOptions *opt )
{
   options = *opt;
   refresh();
}
/*****************************************************************************/
void OptionsWindow::slotScrollBarTapis( int value )
{
   QPixmap im;

   if( value < (int)tapisList.count() ) {
      if( im.load( path+"/data/"+tapisList[value] ) == true ) {
         ui.imageTapis->setPixmap( im );
      }
   }  
}
/*****************************************************************************/
void OptionsWindow::slotBtnOk()
{
   options.identities[0].name = ui.nomJoueurSud->text();
   options.identities[1].name = ui.nomJoueurEst->text();
   options.identities[2].name = ui.nomJoueurNord->text();
   options.identities[3].name = ui.nomJoueurOuest->text();
   options.identities[4].name = ui.nomJoueurNordOuest->text();

   if( ui.troisJoueurs->isChecked() ) {
      options.nbPlayers = 3;
   } else if( ui.cinqJoueurs->isChecked() ) {
      options.nbPlayers = 5;
   } else {
      options.nbPlayers = 4;
   }

   options.timer1 = ui.slider1->value();
   options.timer2 = ui.slider2->value();

   options.identities[0].quote = ui.citationSud->text();

   if( ui.sexeM->isChecked() ) {
      options.identities[0].sex = MALE;
   } else {
      options.identities[0].sex = FEMALE;
   }

   if( ui.afficheAvatars->isChecked() ) {
      options.showAvatars = true;
   } else {
      options.showAvatars = false;
   }

   options.port = QString( ui.portReseau->text()).toInt();
   options.langue = ui.langList->currentIndex();
   if( indexLangue != options.langue ) {
     QMessageBox::information( this, tr("Information"),
                    tr("Vous devez redÃ©marrer le jeu pour que le changement de langue soit actif.\n\n") );
   }
   options.tapis = tapisList[ui.scrollBarTapis->value()];
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
   ui.temps1->setText( tr("%1 secondes").arg((float)(value/100)/10) );
}
/*****************************************************************************/
void OptionsWindow::slider2Changed( int value )
{
   ui.temps2->setText( tr("%1 secondes").arg((float)(value/100)/10) );
}
/*****************************************************************************/
/**
 * Affiche la boite de dialogue de choix d'une image
 */
QString OptionsWindow::choixImage()
{
   QFileInfo fi;

   AvatarsWindow *fd = new AvatarsWindow( this );
   fd->setAttribute(Qt::WA_ShowModal, true);
   fd->listAvatars(path + "/data/avatars");

   if( fd->exec() == QDialog::Rejected ) {
      return("");
   }

   // On retourne le nom de l'image
   return(path + "/data/avatars/" + fd->selectedFile());
}
/*****************************************************************************/
void OptionsWindow::slotBtnPixSud()
{
   QString s;
   QPixmap im;

   s = choixImage();
   if( im.load( s ) == false ) {
      return;
   }
   options.identities[0].avatar = s;
   ui.pixSud->setPixmap( im );
}
/*****************************************************************************/
void OptionsWindow::slotBtnPixEst()
{
   QString s;
   QPixmap im;

   s = choixImage();
   if( im.load( s ) == false ) {
      return;
   }
   options.identities[1].avatar = s;
   ui.pixEst->setPixmap( im );
}
/*****************************************************************************/
void OptionsWindow::slotBtnPixNord()
{
   QString s;
   QPixmap im;

   s = choixImage();
   if( im.load( s ) == false ) {
      return;
   }
   options.identities[2].avatar = s;
   ui.pixNord->setPixmap( im );
}
/*****************************************************************************/
void OptionsWindow::slotBtnPixOuest()
{
   QString s;
   QPixmap im;

   s = choixImage();
   if( im.load( s ) == false ) {
      return;
   }
   options.identities[3].avatar = s;
   ui.pixOuest->setPixmap( im );
}
/*****************************************************************************/
void OptionsWindow::slotBtnPixNordOuest()
{
   QString s;
   QPixmap im;

   s = choixImage();
   if( im.load( s ) == false ) {
      return;
   }
   options.identities[4].avatar = s;
   ui.pixNordOuest->setPixmap( im );
}
/*****************************************************************************/
/**
 * Rafraichi les Widgets graphiques
 */
void OptionsWindow::refresh()
{
   QPixmap im;

   ui.nomJoueurSud->setText( options.identities[0].name );
   ui.nomJoueurEst->setText( options.identities[1].name );
   ui.nomJoueurNord->setText( options.identities[2].name );
   ui.nomJoueurOuest->setText( options.identities[3].name );
   ui.nomJoueurNordOuest->setText( options.identities[4].name );

   ui.quatreJoueurs->setChecked( true );

   ui.slider1->setValue( options.timer1 );
   ui.slider2->setValue( options.timer2 );
   ui.citationSud->setText( options.identities[0].quote );

   if( options.identities[0].sex == MALE ) {
      ui.sexeM->setChecked( true );
   } else {
      ui.sexeF->setChecked( true );
   }

   ui.afficheAvatars->setChecked( options.showAvatars );

   ui.langList->setCurrentIndex( options.langue );
   indexLangue = options.langue;

   if( im.load( options.identities[0].avatar ) == true ) {
      ui.pixSud->setPixmap( im );
   }
   if( im.load( options.identities[1].avatar ) == true ) {
      ui.pixEst->setPixmap( im );
   }
   if( im.load( options.identities[2].avatar ) == true ) {
      ui.pixNord->setPixmap( im );
   }
   if( im.load( options.identities[3].avatar ) == true ) {
      ui.pixOuest->setPixmap( im );
   }
   if( im.load( options.identities[4].avatar ) == true ){
      ui.pixNordOuest->setPixmap( im );
   }

   ui.niveauEst->setCurrentIndex(  0  );
   ui.niveauNord->setCurrentIndex( 0 );
   ui.niveauOuest->setCurrentIndex( 0 );
   ui.niveauNordOuest->setCurrentIndex( 0 );

   ui.portReseau->setValue( options.port );

   for( int i=0; i<tapisList.count(); i++ ) {
      if( tapisList[i] == options.tapis ) {
         ui.scrollBarTapis->setValue(i);
         if( im.load( path+"/data/"+tapisList[i] ) == true ) {
            ui.imageTapis->setPixmap( im );
         }
      }
   }
}

//=============================================================================
// Fin du fichier OptionsWindow.cpp
//=============================================================================
