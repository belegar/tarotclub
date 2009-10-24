/*=============================================================================
 * Tarot Club - ConfigFile.h
 *=============================================================================
 * Classe de gestion du fichier d'options en XML
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
#ifndef _CONFIGFILE_H
#define _CONFIGFILE_H

// Qt includes
#include <QString>
#include <QFile>
#include <QStringList>

// Game includes
#include "defines.h"


#define XML_VERSION     "1.1"

// Valeurs par défaut
#define NBPLAYERS_DEF   4
#define TIMER1_DEF      500
#define TIMER2_DEF      1500
#define PORT_DEF        DEFAULTPORT
#define AVATARS_DEF     true

/*****************************************************************************/
class ConfigFile
{

private:
   QString path; // répertoire racine du jeu
   GameOptions options;

public:
   ConfigFile( const QString &gamePath );

   bool     load( const QString &fileName );
   bool     save( const QString &fileName );

   GameOptions *getGameOptions();
   QString     getPath();

   void        setGameOptions( GameOptions *newOptions );
   static void setDefault( GameOptions *opt );
};

#endif // _CONFIGFILE_H

//=============================================================================
// End of file ConfigFile.h
//=============================================================================
