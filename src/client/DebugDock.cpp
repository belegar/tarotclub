/*=============================================================================
 * TarotClub - DebugDock.cpp
 *=============================================================================
 * General purpose dock window
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

// Includes Qt
#include <QDockWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>

// Includes locales
#include "DebugDock.h"

DebugDock *DebugDock::instance = NULL;

/*****************************************************************************/
DebugDock::DebugDock( QWidget *parent )
    : QDockWidget( trUtf8("Debug"), parent )
{        
    logWindow = new QTextBrowser();
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

    connect(clearButton, SIGNAL(clicked()), this, SLOT(clear()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(save()));

    instance = this;
}
/*****************************************************************************/
void DebugDock::closeEvent( QCloseEvent * e )
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
void DebugDock::message(const QString &message)
{
    logWindow->append( message );
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

    if(saveFileName.isEmpty()) {
        return;
    }

    QFile file(saveFileName);
    if(!file.open(QIODevice::WriteOnly)) {
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
