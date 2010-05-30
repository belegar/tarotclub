/*=============================================================================
 * TarotClub - ConfigFile.h
 *=============================================================================
 * Classe de gestion du fichier d'options en XML
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
#ifndef _CONFIGFILE_H
#define _CONFIGFILE_H

// Qt includes
#include <QString>
#include <QFile>
#include <QStringList>

// Game includes
#include "../defines.h"


#define XML_VERSION     "1.2"

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
