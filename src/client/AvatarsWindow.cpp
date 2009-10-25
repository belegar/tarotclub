/*=============================================================================
 * TarotClub - AvatarsWindow.cpp
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

#include "AvatarsWindow.h"
#include <QDir>

/*****************************************************************************/
AvatarsWindow::AvatarsWindow( QWidget* parent, Qt::WFlags fl )
   : QDialog( parent, fl )
{
    ui.setupUi(this);

    connect( ui.listWidget, SIGNAL(currentItemChanged(QListWidgetItem *,QListWidgetItem *)), this, SLOT(slotShowImage(QListWidgetItem *,QListWidgetItem *)) );
}
/*****************************************************************************/
void AvatarsWindow::slotShowImage(QListWidgetItem *item, QListWidgetItem *previous)
{
    if( item != 0 ) {
        QPixmap im;
      if( im.load(path+"/"+item->text()) == false )
         return;
      ui.avatarImage->setPixmap( im );
    }
}
/*****************************************************************************/
QString AvatarsWindow::selectedFile()
{
    QString img;
    QListWidgetItem *item = ui.listWidget->currentItem();
    if( item == NULL )
        img = ":/images/inconnu.png"; // image par défaut
    else
        img = item->text();

    return(img);
}
/*****************************************************************************/
void AvatarsWindow::listAvatars( const QString &avatarsPath )
{
    path = avatarsPath;
    QDir dir(path);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);

    QFileInfoList list = dir.entryInfoList();

    // On affiche la liste des avatars
    for(int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        if( fileInfo.suffix() == "png" ) {
            // On essaye d'ouvrir cette image
            QImage im(fileInfo.absoluteFilePath());
            if(im.isNull() == false ) {
                // maintenant on teste sa taille (40x40 max)
                if( im.width() == 40 && im.height() == 40 ) {
                    // l'image est valide, on l'ajoute à la liste
                    new QListWidgetItem(fileInfo.fileName(), ui.listWidget);
                }
            }
        }
    }

}
/*****************************************************************************/

//=============================================================================
// Fin du fichier AvatarsWindow.cpp
//=============================================================================
