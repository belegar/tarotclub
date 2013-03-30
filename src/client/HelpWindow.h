/*=============================================================================
 * TarotClub - HelpWindow.h
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

// Qt includes
#include <QLabel>
#include <QDialog>
#include <QTextBrowser>
#include <QtHelp>

// Game includes
#include <ui_HelpUI.h>

/*****************************************************************************/
class QHelpEngine;

class HelpBrowser : public QTextBrowser
{
public:
    HelpBrowser(QHelpEngine *help, QWidget *parent = 0);
    QVariant loadResource(int type, const QUrl &url);

private:
    QHelpEngine *helpEngine;
};

/*****************************************************************************/

class HelpWindow : public QDialog
{
       Q_OBJECT

private:
    Ui::HelpUI ui;
    QHelpEngine *helpEngine;
    HelpBrowser *browser;

public:
    HelpWindow( QWidget* parent = 0 );

public slots:
    void DisplayUrl(const QUrl &url);

};

//=============================================================================
// End of file HelpWindow.h
//=============================================================================
