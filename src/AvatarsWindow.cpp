/*=============================================================================
 * Tarot Club - AvatarsWindow.cpp
 *=============================================================================
 * AvatarsWindow : choix d'un avatar
 *=============================================================================
 * Tarot Club est un jeu de Tarot français
 * Copyright (C) 2003-2005  Anthony Rabine
 * anthony@ooso.org
 * http://tarotclub.ooso.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
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
