/*=============================================================================
 * Tarot Club - OptionsWindow.h
 *=============================================================================
 * OptionsWindow : fenêtre d'options
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

#ifndef _OPTIONSWINDOW_H
#define _OPTIONSWINDOW_H

// Includes Qt
#include <QDialog>

// Includes locales
#include "ui/ui_OptionsUI.h"
#include "../defines.h"
#include "../ConfigFile.h"
#include "AvatarsWindow.h"

/*****************************************************************************/
class OptionsWindow : public QDialog
{
   Q_OBJECT

private:
   Ui::OptionsUI  ui;
   QString        path;
   GameOptions    options;
   QStringList    tapisList;
   int            indexLangue;

   QString choixImage();


public:
   OptionsWindow( QWidget* parent = 0, Qt::WFlags fl = 0 );

   void           setPath( const QString &game_path );
   void           setOptions( GameOptions *opt );
   GameOptions    *getOptions();
   void           refresh();

public slots:
   
   void  slotBtnOk();
   void  slotBtnDefaut();

   // Widgets
   void  slider1Changed( int );
   void  slider2Changed( int );
   void  slotBtnPixSud();
   void  slotBtnPixEst();
   void  slotBtnPixNord();
   void  slotBtnPixOuest();
   void  slotBtnPixNordOuest();
   void  slotScrollBarTapis( int value );
      
};

#endif // _OPTIONSWINDOW_H

//=============================================================================
// Fin du fichier OptionsWindow.h
//=============================================================================
