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

#include <sstream>
#include "JsonReader.h"
#include "JsonWriter.h"
#include "ServerConfig.h"
#include "Log.h"

static const std::uint16_t DEFAULT_DELAY        = 500U;     // in ms
static const std::uint16_t DEFAULT_TCP_PORT     = 4269U;
static const std::string SERVER_CONFIG_VERSION  = "2.0";
static const std::string SERVER_CONFIG_FILE     = "tcd.json";


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
    bool ret = true;

    JsonReader json;

    ret = json.Open(Config::HomePath + SERVER_CONFIG_FILE);
    if (ret)
    {
        std::string value;
        if (json.GetValue("", "version", value))
        {
            if (value == SERVER_CONFIG_VERSION)
            {

            }
            else
            {
                ret = false;
            }
        }
        else
        {
            ret = false;
        }
    }


/*
    QFile f(QString(Config::HomePath.data()) + SERVER_CONFIG_FILE);
    QString txt;
    int val;


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
                                        options.bots[pos].name = txt.toStdString();

                                    }
                                    else if (xml.name() == "quote")
                                    {
                                        options.bots[pos].quote = xml.readElementText().toStdString();

                                    }
                                    else if (xml.name() == "sex")
                                    {
                                        options.bots[pos].sex = (Identity::Gender)xml.readElementText().toInt();

                                    }
                                    else if (xml.name() == "avatar")
                                    {
                                        options.bots[pos].avatar = xml.readElementText().toStdString();
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
*/
    return ret;
}
/*****************************************************************************/
bool ServerConfig::Save()
{
    bool ret = true;

    JsonWriter json;

    json.CreateValuePair("version", SERVER_CONFIG_VERSION);
    json.CreateValuePair("generator", "Generated by TarotClub " TAROT_VERSION);
    json.CreateValuePair("delay", options.timer);
    json.CreateValuePair("tcp_port", options.tcp_port);

    for (std::uint32_t i = 1U; i < 4U; i++)
    {
        Place bot(i);
        JsonObject *obj = json.CreateObjectPair(bot.ToString());
        obj->CreateValuePair("name", options.bots[i].name);
        obj->CreateValuePair("avatar", options.bots[i].avatar);
        std::string text;
        if (options.bots[i].gender == Identity::MALE)
        {
            text = "male";
        }
        else
        {
            text = "female";
        }

        obj->CreateValuePair("gender", text);
        obj->CreateValuePair("quote", options.bots[i].quote);
    }

    if (!json.SaveToFile(Config::HomePath + SERVER_CONFIG_FILE))
    {
        ret = false;
        TLogError("Saving server's configuration failed.");
    }
    return ret;
}
/*****************************************************************************/
void ServerConfig::SetDefault(ServerOptions &opt)
{
    opt.timer       = DEFAULT_DELAY;
    opt.tcp_port    = DEFAULT_TCP_PORT;

    opt.bots[Place::WEST].name     = "Leela";
    opt.bots[Place::WEST].avatar   = ":/images/avatars/leela.png";
    opt.bots[Place::WEST].quote    = "No, this isn't mutant language. We use a lot more profanity.";
    opt.bots[Place::WEST].gender   = Identity::FEMALE;

    opt.bots[Place::NORTH].name    = "Bender";
    opt.bots[Place::NORTH].avatar  = ":/images/avatars/bender.png";
    opt.bots[Place::NORTH].quote   = "Afterlife? If I'd thought I had to go through a whole another life, I'd kill myself right now.";
    opt.bots[Place::NORTH].gender  = Identity::MALE;

    opt.bots[Place::EAST].name     = "Zoidberg";
    opt.bots[Place::EAST].avatar   = ":/images/avatars/zoidberg.png";
    opt.bots[Place::EAST].quote    = "A buffet... oh if only I'd brought my wallet with me...";
    opt.bots[Place::EAST].gender   = Identity::MALE;
}

//=============================================================================
// End of file ServerConfig.cpp
//=============================================================================
