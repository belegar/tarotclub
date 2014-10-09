#ifndef MINIDECK_H
#define MINIDECK_H

#include <QtCore>
#include <QtXml/QDomDocument>
#include "Deck.h"

class MiniDeck
{
public:
    MiniDeck();

    QString BuildDeck();

    QString GetDeck();

private:
    QDomElement CreateCard(const Card &card, QDomDocument &doc);
};

#endif // MINIDECK_H
