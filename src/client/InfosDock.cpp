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
   // numéro de la donne
   donneTxt = new QLabel(trUtf8("Donne numéro "));
   donneVar = new QLabel("");

   // preneur
   preneurTxt = new QLabel(trUtf8("Preneur "));
   preneurVar = new QLabel("");

   // contrat
   contratTxt = new QLabel(trUtf8("Contrat "));
   contratVar = new QLabel("");

   // On place les éléments sur le widget
   QHBoxLayout *lay1 = new QHBoxLayout();
   lay1->addWidget(donneTxt);
   lay1->addWidget(donneVar);
   QHBoxLayout *lay2 = new QHBoxLayout();
   lay2->addWidget(preneurTxt);
   lay2->addWidget(preneurVar);
   QHBoxLayout *lay3 = new QHBoxLayout();
   lay3->addWidget(contratTxt);
   lay3->addWidget(contratVar);

   QVBoxLayout *principal = new QVBoxLayout();
   principal->addLayout(lay1);
   principal->addLayout(lay2);
   principal->addLayout(lay3);
   principal->addStretch();

   QFrame *fr = new QFrame(this);
   fr->setLayout(principal);
   setWidget(fr);
}
/*****************************************************************************/
void InfosDock::clear()
{
   donneVar->setText("");
   preneurVar->setText("");
   contratVar->setText("");
}
/*****************************************************************************/
void InfosDock::setDonne( int n )
{
   donneVar->setText( QString("<b>") + QString::number(n) + QString("</b>") );
}
/*****************************************************************************/
void InfosDock::setPreneur( QString preneur )
{
   preneurVar->setText( "<b>" + preneur + "</b>" );
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
   contratVar->setText( "<b>" + mot + "</b>" );
}

//=============================================================================
// Fin du fichier InfosDock.cpp
//=============================================================================
