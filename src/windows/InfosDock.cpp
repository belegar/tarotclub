/*=============================================================================
 * Tarot Club - InfosDock.h
 *=============================================================================
 * Fenêtre "dockable" où sont stockées quelques informations du jeu en cours.
 *=============================================================================
 * Tarot Club est un jeu de Tarot français
 * Copyright (C) 2003-2005  Anthony Rabine
 * anthony@ooso.org
 * http://tarotclub.ooso.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *=============================================================================
 */

// Includes Qt
#include <QDockWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>

// Includes locales
#include "InfosDock.h"
#include "../textes.h"

/*****************************************************************************/
InfosDock::InfosDock( QWidget *parent )
    : QDockWidget( tr("Informations"), parent )
{
   // numéro de la donne
   donneTxt = new QLabel(tr("Donne numéro "));
   donneVar = new QLabel("");

   // preneur
   preneurTxt = new QLabel(tr("Preneur "));
   preneurVar = new QLabel("");

   // contrat
   contratTxt = new QLabel(tr("Contrat "));
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
