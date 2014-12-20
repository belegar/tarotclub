/*=============================================================================
 * TarotClub - ImportAvatarWindow.h
 *=============================================================================
 * Dialog to fetch a local or network avatar image file.
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

// Qt includes
#include <QLabel>
#include <QDialog>

// Game includes
#include <ui_ImportAvatar.h>
#include "Avatar.h"

/*****************************************************************************/
class ImportAvatarWindow : public QDialog
{
public:
    ImportAvatarWindow(QWidget *parent = 0);

    QPixmap GetPixmap() { return mAvatar.GetPixmap(); }
    QString GetFilePath() { return mAvatar.GetFilePath(); }
    void SetFilePath(const QString &filePath);

private:
    Ui::importAvatarUI ui;
    Avatar mAvatar;

private slots:
    void slotBrowse();
    void slotOk();
};

//=============================================================================
// End of file ImportAvatarWindow.h
//=============================================================================
