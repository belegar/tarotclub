/*=============================================================================
 * TarotClub - OptionsWindow.h
 *=============================================================================
 * OptionsWindow : fenêtre d'options
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * TarotClub is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotClub is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
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
#include "defines.h"
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
    int     indexLangue; // Detect any language change to inform that a reboot is needed
    QString colorName;

    QString choixAvatar(QString defaultAvatar);
    void    refresh();

public:
    OptionsWindow(QWidget *parent = 0);

    // Getters
    ClientOptions &GetClientOptions();
    ServerOptions &GetServerOptions();

    // Setters
    void SetClientOptions(ClientOptions &opt);
    void SetServerOptions(ServerOptions &opt);

public slots:
    void slotBtnOk();
    void slotBtnDefaut();

    void slotClickOptionChanged(int state);

    // Widgets
    void slider1Changed(int);
    void slider2Changed(int);
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
