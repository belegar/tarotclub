/*=============================================================================
 * Tarot Club - ResultWindow.cpp
 *=============================================================================
 * ResultWindow : affiche le résultat d'une donne
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

#include "ResultWindow.h"
#include "textes.h"

/*****************************************************************************/
ResultWindow::ResultWindow( QWidget* parent, Qt::WFlags fl )
	: QDialog( parent, fl )
{
    ui.setupUi(this);
}
/*****************************************************************************/
void ResultWindow::setCalcul( ScoreInfos *score, GameInfos *infos )
{
   QString result_str;

	// On affiche les caractéristiques de la partie
   if( infos->preneur == SUD ) {
		ui.lblPreneur->setText(STR_SUD);
   } else if( infos->preneur == EST ) {
		ui.lblPreneur->setText(STR_EST);
   }else if( infos->preneur == NORD ) {
		ui.lblPreneur->setText(STR_NORD);
   } else if( infos->preneur == OUEST ) {
		ui.lblPreneur->setText(STR_OUEST);
   } else {
      ui.lblPreneur->setText(STR_NORD_O);
   } 

   if( infos->contrat == PRISE ) {
		ui.lblContrat->setText(STR_PRISE);
   } else if( infos->contrat == GARDE ) {
		ui.lblContrat->setText(STR_GARDE);
   } else if( infos->contrat == GARDE_SANS ) {
		ui.lblContrat->setText(STR_GARDE_SANS);
   } else {
		ui.lblContrat->setText(STR_GARDE_CONTRE);
   }

   ui.lblNbBouts->setNum( score->bouts );
   ui.lblPoints->setNum( (int)score->attaque );

   if( (int)(score->attaque) >= score->pointsAFaire ) {
      result_str = QString("<font color=\"#0D851B\">") + STR_WIN;
	} else {
      result_str = QString("<font color=\"red\">") + STR_LOSE;
	}
   ui.lblResultat->setText(result_str + QString().setNum( score->difference ) + 
                           STR_POINTS + QString("</font>"));

	ui.lblGains->setNum( score->difference );
   ui.lblPetit->setNum( score->points_petit_au_bout );
   ui.lblMultiple->setNum( score->multiplicateur );
   ui.lblPoignee->setNum( score->points_poignee );
   ui.lblChelem->setNum( score->points_chelem );
   ui.lblTotalDefense->setText( QString().setNum( score->defense ) + STR_POINTS );
	ui.lblTotalAttaque->setText( QString().setNum( score->defense*(-3) ) + STR_POINTS );
}


//=============================================================================
// Fin du fichier ResultWindow.cpp
//=============================================================================
