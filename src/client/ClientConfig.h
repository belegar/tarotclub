/*=============================================================================
 * TarotClub - ClientConfig.h
 *=============================================================================
 * Classe de gestion du fichier d'options en XML (client graphique)
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
#ifndef _CLIENTCONFIG_H
#define _CLIENTCONFIG_H

// Qt includes
#include <QString>
#include <QFile>
#include <QStringList>

// Game includes
#include "../defines.h"
#include "../Identity.h"

#define CLIENT_XML_VERSION    "1.5"
#define CLIENT_CONFIG_FILE    "tarotclub.xml"

// default values
#define AVATARS_DEF         true
#define CLIENT_TIMER_DEF    1500

typedef struct {
   QString          deckFilePath;
   bool             showAvatars;
   unsigned int     language;
   QString          backgroundColor;
   Identity         identity;
   unsigned int     delayBeforeCleaning;  // in milliseconds
   bool             enableDelayBeforeCleaning;
} ClientOptions;


/*****************************************************************************/
class ClientConfig
{

private:
   ClientOptions options;

public:
   ClientConfig();

   bool     load();
   bool     save();

   ClientOptions *getOptions();

   void        setOptions( ClientOptions *newOptions );
   static void setDefault( ClientOptions *opt );
};

#endif // _CLIENTCONFIG_H

//=============================================================================
// End of file ClientConfig.h
//=============================================================================
