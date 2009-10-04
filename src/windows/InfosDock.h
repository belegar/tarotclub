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

#ifndef INFOSDOCK_H
#define INFOSDOCK_H

// Includes Qt
#include <QDockWidget>
#include <QLabel>

// Includes locales
#include "../defines.h"

/*****************************************************************************/
class InfosDock : public QDockWidget
{
private:
   QLabel *donneTxt, *donneVar; // numéro de la donne
   QLabel *preneurTxt, *preneurVar; // preneur
   QLabel *contratTxt, *contratVar; // contrat

public:
   InfosDock( QWidget *parent );

   void clear();
   void setContrat( Contrat c );
   void setPreneur( QString preneur );
   void setDonne( int n );

};

#endif // INFOSDOCK_H

//=============================================================================
// Fin du fichier InfosDock.h
//=============================================================================
