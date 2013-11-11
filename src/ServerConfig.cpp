/*=============================================================================
 * TarotClub - ServerConfig.cpp
 *=============================================================================
 * Classe de gestion du fichier d'options en XML (serveur)
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

#include "ServerConfig.h"
#include <QString>
#include <QDesktopServices>

/*****************************************************************************/
ServerConfig::ServerConfig()
{
    SetDefault(options);
}
/*****************************************************************************/
ServerOptions &ServerConfig::GetOptions()
{
    return options;
}
/*****************************************************************************/
void ServerConfig::SetOptions(const ServerOptions &newOptions)
{
    options = newOptions;
}
/*****************************************************************************/
bool ServerConfig::Load()
{
    QFile f(Config::HomePath + SERVER_CONFIG_FILE);
    QString txt;
    int val;
    bool ret = true;

    if (f.open(QIODevice::ReadOnly))
    {
        QXmlStreamReader xml(&f);

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
                if (xml.name() == "tarotclubd")
                {
                    // Let's get the attributes
                    QXmlStreamAttributes attributes = xml.attributes();
                    // Let's check the version number
                    if (attributes.hasAttribute("version"))
                    {
                        if (attributes.value("version").toString() != QString(SERVER_XML_VERSION))
                        {
                            ret = false;
                        }
                    }
                    else
                    {
                        ret = false;
                    }
                }
                if (xml.name() == "pause")
                {
                    val = xml.readElementText().toInt();
                    if ((val < 0) || (val > 9000))
                    {
                        val = TIMER1_DEF;
                    }
                    options.timer = val;

                }
                else if (xml.name() == "port")
                {
                    options.port = xml.readElementText().toInt();

                }
                else if (xml.name() == "identity")
                {
                    QXmlStreamAttributes attributes = xml.attributes();
                    if (attributes.hasAttribute("pos"))
                    {
                        int pos;
                        pos = attributes.value("pos").toInt();
                        if ((pos < 0) || (pos > 2))
                        {
                            ret = false;
                        }
                        else
                        {
                            while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                                     xml.name() == "identity"))
                            {
                                QXmlStreamReader::TokenType ident_token = xml.readNext();
                                if (ident_token == QXmlStreamReader::StartElement)
                                {
                                    if (xml.name() == "name")
                                    {
                                        txt = xml.readElementText();
                                        if (txt.isEmpty())
                                        {
                                            txt = "Unknown";
                                        }
                                        options.bots[pos].name = txt;

                                    }
                                    else if (xml.name() == "quote")
                                    {
                                        options.bots[pos].quote = xml.readElementText();

                                    }
                                    else if (xml.name() == "sex")
                                    {
                                        options.bots[pos].sex = (Identity::Gender)xml.readElementText().toInt();

                                    }
                                    else if (xml.name() == "avatar")
                                    {
                                        options.bots[pos].avatar = xml.readElementText();
                                    }

                                }
                            }
                        }
                    }
                    else
                    {
                        ret = false;
                    }
                }
            }
        }
    }
    else
    {
        ret = false;
    }

    return ret;
}
/*****************************************************************************/
bool ServerConfig::Save()
{
    bool ret = false;

    // Open configuration file for writing configuration
    QFile f(Config::HomePath + SERVER_CONFIG_FILE);
    if (f.open(QIODevice::WriteOnly))
    {
        QXmlStreamWriter stream(&f);
        stream.setAutoFormatting(true);
        stream.writeStartDocument();

        stream.writeComment(QString("Generated by ") + QString("TarotClub ") + QString(TAROT_VERSION));

        stream.writeStartElement("tarotclubd");
        stream.writeAttribute("version", QString(SERVER_XML_VERSION));

        // Timing between two players
        stream.writeTextElement("pause", QString().setNum(options.timer));

        // TCP/IP port
        stream.writeTextElement("port", QString().setNum(options.port));

        // Parameter of each bot
        for (int i = 0; i < 3; i++)
        {
            stream.writeStartElement("identity");
            stream.writeAttribute("pos", QString().setNum(i));

            // name
            stream.writeTextElement("name", options.bots[i].name);
            // avatar
            stream.writeTextElement("avatar", options.bots[i].avatar);
            // sex
            stream.writeTextElement("sex", QString().setNum(options.bots[i].sex));
            // quote
            stream.writeTextElement("quote", options.bots[i].quote);

            stream.writeEndElement(); // identity
        }

        stream.writeEndElement(); // tarotclubd
        stream.writeEndDocument();
        f.close();
        ret = true;
    }
    else
    {
        qDebug("Saving server's configuration failed.");
    }

    return ret;
}
/*****************************************************************************/
void ServerConfig::SetDefault(ServerOptions &opt)
{
    opt.timer = TIMER1_DEF;
    opt.port = DEFAULT_PORT;

    opt.bots[BOT_WEST].name = "Ouest";
    opt.bots[BOT_WEST].avatar = ":/images/avatars/robot.png";
    opt.bots[BOT_WEST].quote = QString::fromUtf8("Quand on mettra les cons sur orbite, t'as pas fini de tourner.");
    opt.bots[BOT_WEST].sex = Identity::MALE;

    opt.bots[BOT_NORTH].name = "Nord";
    opt.bots[BOT_NORTH].avatar = ":/images/avatars/robot.png";
    opt.bots[BOT_NORTH].quote = QString::fromUtf8("J'ai fait un test de QI, les resultats étaient negatifs.");
    opt.bots[BOT_NORTH].sex = Identity::MALE;

    opt.bots[BOT_EAST].name = "Est";
    opt.bots[BOT_EAST].avatar = ":/images/avatars/robot.png";
    opt.bots[BOT_EAST].quote = "Plus je grossis, plus je m'aigris.";
    opt.bots[BOT_EAST].sex = Identity::MALE;
}

//=============================================================================
// End of file ServerConfig.cpp
//=============================================================================
