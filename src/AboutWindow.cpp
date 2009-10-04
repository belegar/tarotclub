/*=============================================================================
 * Tarot Club - AboutWindow.cpp
 *=============================================================================
 * Custom "about" window
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

#include "AboutWindow.h"
#include "defines.h"

/*****************************************************************************/
AboutWindow::AboutWindow( QWidget* parent  )
	: QDialog( parent )
{
    ui.setupUi(this);

	ui.version->setText("Tarot Club " TAROT_VERSION);
    ui.nom->setText("Nom de code <" TAROT_VNAME ">");

}

//=============================================================================
// End of file AboutWindow.cpp
//=============================================================================
