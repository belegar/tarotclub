/*=============================================================================
 * TarotClub - DealFile.cpp
 *=============================================================================
 * Manage saving and loading XML files of custom deals
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

#include <QtCore>
#include "DealFile.h"
#include "defines.h"
#include "TarotDeck.h"

#define DEAL_XML_VERSION  "1.0"

/*****************************************************************************/
DealFile::DealFile()
{

}
/*****************************************************************************/
bool DealFile::LoadFile(const std::string &fileName)
{
    bool ret = true;
    QFile f(fileName.data());

    if (f.open(QIODevice::ReadOnly) == true)
    {
        QXmlStreamReader xml(&f);
        dogDeck.clear();
        southDeck.clear();
        northDeck.clear();
        westDeck.clear();
        eastDeck.clear();

        while (!xml.atEnd() && ret)
        {
            QXmlStreamReader::TokenType token = xml.readNext();

            // If token is just StartDocument, we'll go to next
            if (token == QXmlStreamReader::StartDocument)
            {
                continue;
            }
            // If token is StartElement, we'll see if we can read it
            if (token == QXmlStreamReader::StartElement)
            {
                if (xml.name() == "custom_deal")
                {
                    // Let's get the attributes
                    QXmlStreamAttributes attributes = xml.attributes();
                    // Let's check the version number
                    if (attributes.hasAttribute("version"))
                    {
                        if (attributes.value("version").toString() != QString(DEAL_XML_VERSION))
                        {
                            ret = false;
                        }
                    }
                    else
                    {
                        ret = false;
                    }
                }

                // Section "Dog"
                if (xml.name() == "dog")
                {
                    if (FillDeck(dogDeck, xml) == false)
                    {
                        ret = false;
                    }
                }
                else if (xml.name() == "south")
                {
                    if (FillDeck(southDeck, xml) == false)
                    {
                        ret = false;
                    }
                }
                else if (xml.name() == "north")
                {
                    if (FillDeck(northDeck, xml) == false)
                    {
                        ret = false;
                    }
                }
                else if (xml.name() == "west")
                {
                    if (FillDeck(westDeck, xml) == false)
                    {
                        ret = false;
                    }
                }
                else if (xml.name() == "east")
                {
                    if (FillDeck(eastDeck, xml) == false)
                    {
                        ret = false;
                    }
                }
            }
        }
        if (xml.hasError())
        {
            ret = false;
        }

        f.close();
    }
    else
    {
        ret = false;
    }

    return ret;
}
/*****************************************************************************/
bool DealFile::FillDeck(Deck &deck, QXmlStreamReader &xml)
{
    QString val;

    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() != "card"))
    {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartElement)
        {
            if (xml.name() == "card")
            {
                val = xml.readElementText();
                deck.append(TarotDeck::GetCard(val.toStdString()));
            }
        }
    }
    return true;
}
/*****************************************************************************/
void DealFile::SaveFile(const std::string &fileName)
{
    // Open a file where to stream out the XML
    QFile f(fileName.data());
    if (!f.open(QIODevice::WriteOnly))
    {
        return;
    }

    QXmlStreamWriter stream(&f);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();

    stream.writeComment(QString("Generated by ") + QString("TarotClub ") + QString(TAROT_VERSION));

    stream.writeStartElement("custom_deal");
    stream.writeAttribute("version", QString(DEAL_XML_VERSION));

    // Dog cards
    stream.writeStartElement("dog");
    for (int i = 0; i < dogDeck.count(); i++)
    {
        stream.writeTextElement("card", dogDeck.at(i)->GetName().data());
    }
    stream.writeEndElement(); // dog

    // South cards
    stream.writeStartElement("south");
    for (int i = 0; i < southDeck.count(); i++)
    {
        stream.writeTextElement("card", southDeck.at(i)->GetName().data());
    }
    stream.writeEndElement(); // south

    // West cards
    stream.writeStartElement("west");
    for (int i = 0; i < westDeck.count(); i++)
    {
        stream.writeTextElement("card", westDeck.at(i)->GetName().data());
    }
    stream.writeEndElement(); // west

    // North cards
    stream.writeStartElement("north");
    for (int i = 0; i < northDeck.count(); i++)
    {
        stream.writeTextElement("card", northDeck.at(i)->GetName().data());
    }
    stream.writeEndElement(); // north

    // East cards
    stream.writeStartElement("east");
    for (int i = 0; i < eastDeck.count(); i++)
    {
        stream.writeTextElement("card", eastDeck.at(i)->GetName().data());
    }
    stream.writeEndElement(); // east

    stream.writeEndElement(); // custom_deal
    stream.writeEndDocument();
    f.close();
}

//=============================================================================
// End of file DealFile.cpp
//=============================================================================
