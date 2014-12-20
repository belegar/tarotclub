/*=============================================================================
 * TarotClub - ImportAvatarWindow.cpp
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

#include <QString>
#include "ImportAvatarWindow.h"
#include "Common.h"
#include <QFileDialog>
#include <QMessageBox>

/*****************************************************************************/
ImportAvatarWindow::ImportAvatarWindow(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    connect (ui.buttonBrowse, &QPushButton::clicked, this, &ImportAvatarWindow::slotBrowse);
    connect (ui.okButton, &QPushButton::clicked, this, &ImportAvatarWindow::slotOk);
}
/*****************************************************************************/
void ImportAvatarWindow::SetFilePath(const QString &filePath)
{
    ui.lineEdit->setText(filePath);
}
/*****************************************************************************/
void ImportAvatarWindow::slotOk()
{
    mAvatar.SetFilePath(ui.lineEdit->text());
    QCursor currentCursor = cursor();
    setCursor(QCursor(Qt::WaitCursor));
    ui.okButton->setEnabled(false);
    if (mAvatar.LoadFile())
    {
        if (mAvatar.IsLocal())
        {
            setCursor(currentCursor);
            accept();
        }
        else
        {
            setCursor(QCursor(Qt::WaitCursor));
            if (mAvatar.SaveToLocalDirectory())
            {
                (void) QMessageBox::information(this, tr("TarotClub"),
                                            tr("Import success."),
                                            QMessageBox::Ok);
                setCursor(currentCursor);
                accept();
            }
            else
            {
                (void) QMessageBox::information(this, tr("TarotClub"),
                                            tr("Saving to local directory failed."),
                                            QMessageBox::Ok);
            }
        }
    }
    else
    {
        (void) QMessageBox::information(this, tr("TarotClub"),
                                    tr("The avatar cannot be fetched from\n"
                                       "the specified url or path."),
                                    QMessageBox::Ok);
    }
    ui.okButton->setEnabled(true);
}
/*****************************************************************************/
void ImportAvatarWindow::slotBrowse()
{
    ui.lineEdit->setText(QFileDialog::getOpenFileName(this));
}

//=============================================================================
// End of file ImportAvatarWindow.cpp
//=============================================================================
