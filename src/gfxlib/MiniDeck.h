#ifndef MINIDECK_H
#define MINIDECK_H

#include <QtCore>
#include <QtXml/QDomDocument>
#include "Deck.h"

class MiniDeck
{
public:
    MiniDeck();

    void BuildDeck();

    QString GetDeck();

private:
    QString mSvgString;
    QDomElement CreateCard(const Card &card, QDomDocument &doc);
};

#endif // MINIDECK_H
