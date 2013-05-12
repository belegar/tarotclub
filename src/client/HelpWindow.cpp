/*=============================================================================
 * TarotClub - HelpWindow.cpp
 *=============================================================================
 * Display a simple help window
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
    {
        return QVariant(helpEngine->fileData(url));
    }
    else
    {
        return QTextBrowser::loadResource(type, url);
    }
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
    browser = new HelpBrowser(helpEngine, this);

    ui.splitter->insertWidget(0, helpEngine->contentWidget());
    ui.splitter->insertWidget(1, browser);
    ui.splitter->setStretchFactor(1, 2);

 //   connect(helpEngine->contentWidget(), SIGNAL(linkActivated(const QUrl &)),
 //               helpBrowser, SLOT(setSource(const QUrl &)));

    connect(helpEngine->contentWidget(), SIGNAL(linkActivated(const QUrl &)),
                    this, SLOT(DisplayUrl(const QUrl &)));

    DisplayUrl(QUrl("qthelp://tarotclub.fr/help/index.html"));
}
/*****************************************************************************/
void HelpWindow::DisplayUrl(const QUrl &url)
{
//    qDebug(url.toDisplayString().toLatin1().constData());

    browser->setSource(url);

}

//=============================================================================
// End of file HelpWindow.cpp
//=============================================================================
