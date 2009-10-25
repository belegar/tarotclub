/*=============================================================================
 * TarotClub - AboutWindow.h
 *=============================================================================
 * Custom "about" window
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

// Game includes
#include <ui_AboutUI.h>

/*****************************************************************************/
class AboutWindow : public QDialog
{
private:
    Ui::AboutUI ui;
    
public:
	AboutWindow( QWidget* parent = 0 );

};

//=============================================================================
// End of file AboutWindow.h
//=============================================================================
