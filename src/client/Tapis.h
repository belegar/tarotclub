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

/*****************************************************************************/
class GfxCard : public QGraphicsSvgItem
{
public:
    enum Status
    {
        NORMAL,
        SELECTED
    };

    GfxCard(const QString &fileName, QGraphicsItem *parent = 0);

    enum { Type = UserType + 1 };
    int type() const;
    Status GetStatus();
    void SetStatus(Status s);
    void ToggleStatus();

private:
    Status status;
};

/*****************************************************************************/
class Tapis : public QGraphicsView
{

    Q_OBJECT

public:
    enum Filter
    {
        AUCUN,
        JEU
    };

    enum CursorType
    {
        FORBIDDEN,
        ARROW
    };

    Tapis(QWidget *parent);

    // Helpers
    bool loadCards(ClientOptions &opt);
    void colorisePreneur(Place  preneur);
    void setPlayerNames(QMap<Place, Identity> &players, Place p);
    void afficheSelection(Place);
    void DrawCard(GfxCard *c, Place p);
    void ShowBidsChoice(Contract contrat);
    void ShowBid(Place p, Contract cont);
    void cacheEncheres();
    void HideBidsChoice();
    void showAvatars(bool b);
    void razTapis(bool shadow = false);
    void resetCards();
    Place SwapPlace(Place my_place, Place absolute);

    // Getters
    GfxCard *getGfxCard(int i);
    Card *getObjectCard(GfxCard *gc);

    // Setters
    void setCursorType(CursorType t);
    void setText(Place p, const QString &txt);
    void setAvatar(Place p, const QString &file);
    void setFilter(Filter);
    void setBackground(const QString &fichier);
    void setAccepterChienVisible(bool v);
    void setBoutonPoigneeVisible(bool v);
    void setCardScale(float factor);

public slots:
    void slotBoutton1();
    void slotBoutton2();
    void slotBoutton3();
    void slotBoutton4();
    void slotBoutton5();
    void slotAccepteChien();
    void slotPresenterPoignee();

signals:
    void sigViewportClicked();
    void sigClickCard(GfxCard *);
    void sigMoveCursor(GfxCard *);
    void sigContract(Contract c);
    void sigAccepteChien();
    void sigPresenterPoignee();

protected:
    void  mousePressEvent(QMouseEvent *e);
    void  mouseMoveEvent(QMouseEvent *e);
    void  resizeEvent(QResizeEvent *);

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
};

#endif // TAPIS_H

//=============================================================================
// End of file Tapis.h
//=============================================================================
