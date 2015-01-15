/*=============================================================================
 * TarotClub - DebugDock.cpp
 *=============================================================================
 * General purpose dock window
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

// Includes Qt
#include <QDockWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

// Includes locales
#include "DebugDock.h"

/*****************************************************************************/
DebugDock::DebugDock(QWidget *parent)
    : QDockWidget(trUtf8("Debug output window"), parent)
{
    logWindow = new QPlainTextEdit();
    logWindow->setReadOnly(true);
    clearButton = new QPushButton(trUtf8("Clear"));
    saveButton = new QPushButton(trUtf8("Save"));

    QHBoxLayout *lay1 = new QHBoxLayout();
    lay1->addStretch();
    lay1->addWidget(clearButton);
    lay1->addWidget(saveButton);

    QVBoxLayout *principal = new QVBoxLayout();
    principal->addWidget(logWindow);
    principal->addLayout(lay1);

    QFrame *frame = new QFrame(this);
    frame->setLayout(principal);
    setWidget(frame);

    connect(this, &DebugDock::sigMessage, this, &DebugDock::slotMessage, Qt::QueuedConnection);

    connect(clearButton, SIGNAL(clicked()), this, SLOT(clear()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(save()));
}
/*****************************************************************************/
void DebugDock::closeEvent(QCloseEvent *e)
{
    Q_UNUSED(e);

    // TODO: automatically save log
}
/*****************************************************************************/
void DebugDock::clear()
{
    logWindow->clear();
}
/*****************************************************************************/
void DebugDock::slotMessage(QString message)
{
    logWindow->appendPlainText(message);
}
/*****************************************************************************/
void DebugDock::save()
{
    QString saveFileName = QFileDialog::getSaveFileName(
                               this,
                               tr("Save Log Output"),
                               tr("%1/logfile.txt").arg(QDir::homePath()),
                               tr("Text Files (*.txt);;All Files (*)")
                           );

    if (saveFileName.isEmpty())
    {
        return;
    }

    QFile file(saveFileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(
            this,
            tr("Error"),
            QString(tr("<nobr>File '%1'<br/>cannot be opened for writing.<br/><br/>"
                       "The log output could <b>not</b> be saved!</nobr>"))
            .arg(saveFileName));
        return;
    }

    QTextStream stream(&file);
    stream << logWindow->toPlainText();
    file.close();
}

//=============================================================================
// Fin du fichier DebugDock.cpp
//=============================================================================
