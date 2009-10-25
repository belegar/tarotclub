/*=============================================================================
 * TarotClub - AvatarsWindow.h
 *=============================================================================
 * AvatarsWindow : choix d'un avatar
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

#ifndef _AVATARSWINDOW_H
#define _AVATARSWINDOW_H

// Includes Qt
#include <QDialog>

// Includes locales
#include <ui_AvatarsUI.h>

/*****************************************************************************/
class AvatarsWindow : public QDialog
{
   Q_OBJECT

private:
    Ui::Avatars ui;
    QString path;

public:
   AvatarsWindow( QWidget* parent = 0, Qt::WFlags fl = 0 );

   void	listAvatars( const QString &avatarsPath );
    QString selectedFile();

public slots:
    void slotShowImage(QListWidgetItem *item, QListWidgetItem *previous);
};

#endif // _AVATARSWINDOW_H

//=============================================================================
// Fin du fichier AvatarsWindow.h
//=============================================================================
