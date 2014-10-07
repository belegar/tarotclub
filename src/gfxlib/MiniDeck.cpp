#include "MiniDeck.h"

MiniDeck::MiniDeck()
{
}

void MiniDeck::BuildDeck()
{
    QDomDocument doc;
    QFile file(":cards/minideck.svg");
    if (!file.open(QIODevice::ReadOnly))
    {
        return;
    }
    if (!doc.setContent(&file))
    {
        file.close();
        return;
    }


    Deck deck;
    deck.CreateTarotDeck();

    QDomElement root = doc.documentElement();
    for (Deck::ConstIterator iter = deck.Begin(); iter != deck.End(); ++iter)
    {
        QDomElement node = CreateCard((*iter), doc);
        root.appendChild(node);
    }

    std::cout << doc.toString().toStdString();

}

/*
 *  <g id="c01-S" transform="matrix(1,0,0,1,0,0)">
 *      <use xlink:href="#background"></use>
 *      <use xlink:href="#spade" transform="matrix(0.15,0,0,0.15,4.5,34.5)"></use>
 *      <text x="5" y="30" class="valueText">1</text>
 * </g>
 */
QDomElement MiniDeck::CreateCard(const Card &card, QDomDocument &doc)
{
    QDomElement node = doc.createElement("g");
    node.setAttribute("id", QString(card.GetName().c_str()));

    QDomElement background = doc.createElement("use");
    background.setAttribute("xlink:href", "#background");

    QDomElement icon = doc.createElement("use");;

    QString suitIcon;
    QString scale = "0.15";
    if (card.GetSuit() == Card::CLUBS)
    {
        suitIcon = "#club";
    }
    else if (card.GetSuit() == Card::DIAMONDS)
    {
        suitIcon = "#diamond";
    }
    else if (card.GetSuit() == Card::HEARTS)
    {
        suitIcon = "#heart";
    }
    else if (card.GetSuit() == Card::SPADES)
    {
        suitIcon = "#spade";
    }
    else
    {
        suitIcon = "#trump";
        scale = "0.25";
    }
    icon.setAttribute("xlink:href", suitIcon);
    icon.setAttribute("transform", "scale(" + scale + ") translate(30, 230)");

    QDomElement value = doc.createElement("text");;
    value.setAttribute("x", "5");
    value.setAttribute("y", "30");
    value.setAttribute("class", "valueText");
    value.setNodeValue(QString().setNum((int)card.GetValue()));

    node.appendChild(background);
    node.appendChild(icon);
    node.appendChild(value);

    return node;
}


