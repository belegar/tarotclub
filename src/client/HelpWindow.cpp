/*=============================================================================
 * TarotClub - HelpWindow.cpp
 *=============================================================================
 * Display a simple help window
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

#include <QString>
#include "HelpWindow.h"
#include "../defines.h"

/*****************************************************************************/
HelpBrowser::HelpBrowser(QHelpEngine *help, QWidget *parent)
    : QTextBrowser(parent), helpEngine(help)
{

}
/*****************************************************************************/
QVariant HelpBrowser::loadResource(int type, const QUrl &url)
{
    if (url.scheme() == "qthelp")
        return QVariant(helpEngine->fileData(url));
    else
        return QTextBrowser::loadResource(type, url);
}
/*****************************************************************************/
HelpWindow::HelpWindow( QWidget* parent  )
    : QDialog( parent )
{
    ui.setupUi(this);

    helpEngine = new QHelpEngine("tarotclub.qhc", this);
    if (helpEngine->setupData() == false)
    {
        qFatal("Cannot initialize help system.");
    }
    helpBrowser = new HelpBrowser(helpEngine, this);

    ui.splitter->insertWidget(0, helpEngine->contentWidget());
    ui.splitter->insertWidget(1, helpBrowser);
    ui.splitter->setStretchFactor(1, 1);

    connect(helpEngine->contentWidget(), SIGNAL(linkActivated(const QUrl &)),
                helpBrowser, SLOT(setSource(const QUrl &)));

 //   connect(helpEngine->contentWidget(), SIGNAL(linkActivated(const QUrl &)),
  //                  this, SLOT(DisplayUrl(const QUrl &)));

}
/*****************************************************************************/
void HelpWindow::DisplayUrl(const QUrl &url)
{
    qDebug(url.toDisplayString().toLatin1().constData());
}

//=============================================================================
// End of file HelpWindow.cpp
//=============================================================================
