/*=============================================================================
 * TarotClub - ServerConfig.h
 *=============================================================================
 * Classe de gestion du fichier d'options en XML (serveur)
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
#include "../Identity.h"

#define XML_VERSION           "1.4"
#define SERVER_CONFIG_FILE    "tarotclubd.xml"
#define SERVER_MAX_TABLES     10

// default values
#define TIMER1_DEF      500
#define DEFAULT_PORT    4269

typedef struct {
   int      timer;
   int      port;
   Identity bots[3];
} ServerOptions;

/*****************************************************************************/
class ServerConfig
{

private:
   QString path; // répertoire racine du jeu
   ServerOptions options;

public:
   ServerConfig();

   bool     load();
   bool     save();

   ServerOptions *getOptions();

   void        setOptions( ServerOptions *newOptions );
   static void setDefault( ServerOptions *opt );
};

#endif // _CONFIGFILE_H

//=============================================================================
// End of file ServerConfig.h
//=============================================================================
