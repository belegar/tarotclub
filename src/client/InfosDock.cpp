/*=============================================================================
 * TarotClub - InfosDock.h
 *=============================================================================
 * Fenêtre "dockable" où sont stockées quelques informations du jeu en cours.
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

// Includes Qt
#include <QDockWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>

// Includes locales
#include "InfosDock.h"
#include "textes.h"

/*****************************************************************************/
InfosDock::InfosDock( QWidget *parent )
    : QDockWidget( trUtf8("Informations"), parent )
{
   ui.setupUi(this);
}
/*****************************************************************************/
void InfosDock::clear()
{
   ui.donneVar->setText("");
   ui.preneurVar->setText("");
   ui.contratVar->setText("");
}
/*****************************************************************************/
void InfosDock::setDonne( int n )
{
   ui.donneVar->setText( QString("<b>") + QString::number(n) + QString("</b>") );
}
/*****************************************************************************/
void InfosDock::setPreneur( QString preneur )
{
   ui.preneurVar->setText( "<b>" + preneur + "</b>" );
}
/*****************************************************************************/
void InfosDock::setContrat( Contrat cont )
{
   QString mot;

   if( cont == GARDE_CONTRE ) {
      mot = STR_GARDE_CONTRE;
   } else if( cont == GARDE_SANS ) {
      mot = STR_GARDE_SANS;
   } else if( cont == GARDE ) {
      mot = STR_GARDE;
   } else if( cont == PRISE ) {
      mot = STR_PRISE;
   } else {
      mot = STR_PASSE;
   }
   ui.contratVar->setText( "<b>" + mot + "</b>" );
}

//=============================================================================
// Fin du fichier InfosDock.cpp
//=============================================================================
