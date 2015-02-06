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

#ifndef OPTIONS_WINDOW_H
#define OPTIONS_WINDOW_H

// Includes Qt
#include <QDialog>
#include <QFrame>

// Includes locales
#include <ui_OptionsUI.h>
#include <ui_AvatarsUI.h>
#include "Common.h"
#include "ClientConfig.h"
#include "ServerConfig.h"
#include "Card.h"
#include "ImportAvatarWindow.h"

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
        std::uint8_t suit;
    };

    DragIcon mIcons[5];
};
/*****************************************************************************/
// Specialized label to store file name of displayed image
class AvatarLabel : public QLabel
{
    Q_OBJECT
public:
    AvatarLabel(int id, const QString &fileName) : QLabel()
    {
        mId = id;
        mFileName = fileName;
        setTextInteractionFlags(Qt::TextSelectableByMouse);
    }

    void mousePressEvent(QMouseEvent * event)
    {
        (void) event;
        emit sigClicked(mId);
    }

    QString GetFileName() { return mFileName; }

signals:
    void sigClicked(int id);

private:
    int mId;
    QString mFileName;
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
    void SetClientOptions(const ClientOptions &opt);
    void SetServerOptions(const ServerOptions &opt);

    void Refresh();

private slots:
    void slotBtnOk();
    void slotBtnDefaut();
    void slotAvatarSelected(int id);
    void slotClickOptionChanged(int state);

    // Widgets
    void slider1Changed(int);
    void slider2Changed(int);
    void slotBtnPixSud();
    void slotButtonBotAvatar();
    void slotColorPicker();
    void slotBotSelected(int currentRow);
    void slotButtonToggled(int id, bool checked);
    void slotImportAvatar();
    void slotChooseScriptPath();

    // Network tab widgets
    void slotAddServer();
    void slotRemoveServer();

private:
    Ui::OptionsUI  ui;
    Ui::Avatars mAvatarsUi;
    QDialog *mAvatarsDiag;
    ClientOptions    clientOptions;
    ServerOptions    serverOptions;
    int     indexLangue; // Detect any language change to inform that a reboot is needed
    QString colorName;
    DragWidget *dragWidget;
    QMap<int, AvatarLabel *> mAvatarFiles;
    int mSelectedAvatar;
    int mPreviousSelectedBot;
    QButtonGroup mRadioGroup;
    ImportAvatarWindow mImportAvatarWindow;

    QString ChooseAvatar(const QString &defaultAvatar);
    void UpdateServersList();

};

#endif // OPTIONS_WINDOW_H

//=============================================================================
// Fin du fichier OptionsWindow.h
//=============================================================================
