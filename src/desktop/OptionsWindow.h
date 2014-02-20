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
#include <QFrame>

// Includes locales
#include <ui_OptionsUI.h>
#include <ui_AvatarsUI.h>
#include "defines.h"
#include "ClientConfig.h"
#include "ServerConfig.h"
#include "Card.h"

class QDragEnterEvent;
class QDropEvent;
/*****************************************************************************/
class DragWidget : public QFrame
{
public:
    DragWidget(QWidget *parent = 0);

    std::string GetOrder();
    void SetOrder(const std::string &order);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    int DetectLabel(int x);

    struct DragIcon
    {
        QLabel *label;
        QString icon;
        Card::Suit suit;
    };

    DragIcon mIcons[5];
};

/*****************************************************************************/
class OptionsWindow : public QDialog
{
    Q_OBJECT

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

private:
    Ui::OptionsUI  ui;
    ClientOptions    clientOptions;
    ServerOptions    serverOptions;
    int     indexLangue; // Detect any language change to inform that a reboot is needed
    QString colorName;
    DragWidget *dragWidget;

    QString choixAvatar(QString defaultAvatar);
    void    refresh();

};

#endif // _OPTIONSWINDOW_H

//=============================================================================
// Fin du fichier OptionsWindow.h
//=============================================================================
