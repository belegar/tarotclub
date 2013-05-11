/*=============================================================================
 * TarotClub - OptionsWindow.h
 *=============================================================================
 * OptionsWindow : fenêtre d'options
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

#ifndef _OPTIONSWINDOW_H
#define _OPTIONSWINDOW_H

// Includes Qt
#include <QDialog>

// Includes locales
#include <ui_OptionsUI.h>
#include <ui_AvatarsUI.h>
#include "../defines.h"
#include "ClientConfig.h"
#include "ServerConfig.h"

/*****************************************************************************/
class OptionsWindow : public QDialog
{
   Q_OBJECT

private:
   Ui::OptionsUI  ui;
   ClientOptions    clientOptions;
   ServerOptions    serverOptions;
   unsigned int     indexLangue; // permet de détecter si la langue a été modifiée (warning de reboot)
   QString colorName;

   QString choixAvatar(QString defaultAvatar);
   void    refresh();

public:
   OptionsWindow( QWidget* parent = 0);

   void setClientOptions(ClientOptions *opt);
   void setServerOptions(ServerOptions *opt);

   ClientOptions *getClientOptions();
   ServerOptions *getServerOptions();

public slots:
   void slotBtnOk();
   void slotBtnDefaut();

   void slotClickOptionChanged(int state);

   // Widgets
   void slider1Changed( int );
   void slider2Changed( int );
   void slotBtnPixSud();
   void slotBtnPixEst();
   void slotBtnPixNord();
   void slotBtnPixOuest();
   void slotColorPicker();
};

#endif // _OPTIONSWINDOW_H

//=============================================================================
// Fin du fichier OptionsWindow.h
//=============================================================================
