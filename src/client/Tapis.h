/*=============================================================================
 * TarotClub - Tapis.h
 *=============================================================================
 * visual game contents
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

#ifndef TAPIS_H
#define TAPIS_H

// Qt includes
#include <QtSvg>
#include <QtGui>
#include <QVector>
#include <QList>
#include <QTemporaryFile>

// Game includes
#include "../defines.h"
#include "../Card.h"
#include "TextBox.h"
#include "ClientConfig.h"

enum CardStatus { CARD_NORMAL, CARD_SELECTED };

/*****************************************************************************/
class GfxCard : public QGraphicsSvgItem
{
private:
   CardStatus status;
public:
   GfxCard ( const QString & fileName, QGraphicsItem * parent = 0 );

   enum { Type = UserType + 1 };
   int type() const;
   CardStatus  getStatus();
   void setStatus(CardStatus s);
   void toggleStatus();
};

/*****************************************************************************/
class Tapis : public QGraphicsView
{

   Q_OBJECT

private:
   Filter filter;
   QVector<GfxCard *> cardsPics;
   QVector<CardShadow *> cardShadows;
   QGraphicsScene scene;

   // Graphiques
   QMap<Place, PlayerBox *> playerBox;
   QMap<Place, TextBox *>  textBox;

   QGroupBox      *groupBoutons;
   QPushButton    *boutonPasse;
   QPushButton    *boutonPrise;
   QPushButton    *boutonGarde;
   QPushButton    *boutonGardeSans;
   QPushButton    *boutonGardeContre;
   QCheckBox      *chelem;
   QPushButton    *boutonAccepterChien;
   QPushButton    *boutonPresenterPoignee;

   QPointF CalculateCardPosition(Place p);
   void DrawCardShadows();

protected:
   void  mousePressEvent( QMouseEvent *e );
   void  mouseMoveEvent( QMouseEvent * e );
   void  resizeEvent( QResizeEvent * );

public:
   Tapis(QWidget *parent);

   void setCursorType( CursorType t );
   void setText(Place p, const QString &txt);
   void setAvatar(Place p, const QString &file);
   void setFilter( Filter );
   void setBackground(const QString &fichier);
   void setAccepterChienVisible(bool v);
   void setBoutonPoigneeVisible(bool v);
   void setCardScale(float factor);

   GfxCard *getGfxCard(int i);
   Card *getObjectCard(GfxCard *gc);

   bool loadCards(ClientOptions *opt);
   void colorisePreneur( Place  preneur );
   void setPlayerNames( QList<Identity> &players, Place p );
   void afficheSelection( Place );
   void DrawCard(GfxCard *c, Place p);
   void cacheEncheres();
   void cacheBoutons();
   void showAvatars(bool b);
   void razTapis(bool shadow = false);
   void resetCards();

   /**
    * Utility methods
    */
   static Place SwapPlace(Place origin, Place absolute);

public slots:
   void slotBoutton1();
   void slotBoutton2();
   void slotBoutton3();
   void slotBoutton4();
   void slotBoutton5();
   void slotAccepteChien();
   void slotAfficheBoutons( Contrat contrat );
   void slotAfficheEnchere(Place enchereur,Contrat cont);
   void slotPresenterPoignee();

signals:
   void sgnlViewportClicked();
   void sgnlClickCard( GfxCard * );
   void sgnlMoveCursor( GfxCard * );
   void sgnlContrat(Contrat c);
   void sgnlAccepteChien();
   void sgnlPresenterPoignee();
};

#endif // TAPIS_H

//=============================================================================
// End of file Tapis.h
//=============================================================================
