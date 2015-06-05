/*=============================================================================
 * TarotClub - NewTurnWindow.cpp
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
#include "NewTurnWindow.h"
#include "Common.h"
#include <QFileDialog>
#include <QMessageBox>

/*****************************************************************************/
NewTurnWindow::NewTurnWindow(QWidget *parent, const QStringList &dealTypes)
    : QDialog(parent)
{
    ui.setupUi(this);
    ui.comboDealType->addItems(dealTypes);

    connect (ui.buttonBrowse, &QPushButton::clicked, this, &NewTurnWindow::slotBrowse);
    connect (ui.comboDealType, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, NewTurnWindow::slotIndexChanged);
    connect (ui.buttonBox, &QDialogButtonBox::accepted, this, &NewTurnWindow::slotOk);
    connect (ui.buttonBox, &QDialogButtonBox::rejected, this, &NewTurnWindow::reject);
}
/*****************************************************************************/
void NewTurnWindow::SetFilePath(const QString &filePath)
{
    ui.lineEditDealPath->setText(filePath);
}
/*****************************************************************************/
void NewTurnWindow::slotIndexChanged(int index)
{
    if (index == 0)
    {
        // Random deal
    }
    else if (index == 1)
    {
        // Numbered deal
    }
    else
    {
        // Custom deal
    }
}
/*****************************************************************************/
void NewTurnWindow::slotOk()
{
 /*   {
        (void) QMessageBox::information(this, tr("TarotClub"),
                                    tr("The avatar cannot be fetched from\n"
                                       "the specified url or path."),
                                    QMessageBox::Ok);
    }
    ui.okButton->setEnabled(true);
    */

    accept();
}
/*****************************************************************************/
void NewTurnWindow::slotBrowse()
{
    ui.lineEditDealPath->setText(QFileDialog::getOpenFileName(this));
}

//=============================================================================
// End of file NewTurnWindow.cpp
//=============================================================================
