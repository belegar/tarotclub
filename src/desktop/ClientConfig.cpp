/*=============================================================================
 * TarotClub - ClientConfig.cpp
 *=============================================================================
 * Option file parameters management for the client configuration
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

#include "ClientConfig.h"
#include <QtCore>
#include "Log.h"


/*****************************************************************************/
ClientConfig::ClientConfig()
{
    // Initialize supported languages
    lang << "fr" << "en";
    loaded = false;
    SetDefault(options);
}
/*****************************************************************************/
ClientOptions &ClientConfig::GetOptions()
{
    if (!loaded)
    {
        Load();
    }
    return options;
}
/*****************************************************************************/
QString ClientConfig::GetLocale()
{
    if (!loaded)
    {
        Load();
    }
    if ((options.language >= lang.size()) ||
            (options.language < 0))
    {
        options.language = 0;
    }
    return lang[options.language];
}
/*****************************************************************************/
void ClientConfig::SetOptions(ClientOptions &newOptions)
{
    options = newOptions;
}
/*****************************************************************************/
bool ClientConfig::Load()
{
    QFile f(QString(Config::HomePath.data()) + CLIENT_CONFIG_FILE);
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
                        if (attributes.value("version").toString() != QString(CLIENT_XML_VERSION))
                        {
                            ret = false;
                        }
                    }
                    else
                    {
                        ret = false;
                    }
                }

                if (xml.name() == "show_avatars")
                {
                    val = xml.readElementText().toInt();
                    if (val == 1)
                    {
                        options.showAvatars = true;
                    }
                    else
                    {
                        options.showAvatars = false;
                    }

                }
                else if (xml.name() == "background_color")
                {
                    options.backgroundColor = xml.readElementText();
                }
                else if (xml.name() == "language")
                {
                    options.language = xml.readElementText().toInt();
                    if ((options.language >= lang.size()) ||
                        (options.language < 0))
                    {
                        options.language = 0;
                    }
                }
                else if (xml.name() == "delay_before_cleaning")
                {
                    options.delayBeforeCleaning = xml.readElementText().toInt();
                    if (options.delayBeforeCleaning > 5000)
                    {
                        options.delayBeforeCleaning = 5000;
                    }

                }
                else if (xml.name() == "click_to_clean")
                {
                    val = xml.readElementText().toInt();
                    if (val == 1)
                    {
                        options.enableDelayBeforeCleaning = true;
                    }
                    else
                    {
                        options.enableDelayBeforeCleaning = false;
                    }
                }
                else if (xml.name() == "identity")
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
                                options.identity.name = txt.toStdString();

                            }
                            else if (xml.name() == "quote")
                            {
                                options.identity.quote = xml.readElementText().toStdString();

                            }
                            else if (xml.name() == "sex")
                            {
                                options.identity.gender = (Identity::Gender)xml.readElementText().toInt();

                            }
                            else if (xml.name() == "avatar")
                            {
                                options.identity.avatar = xml.readElementText().toStdString();
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        ret = false;
    }

    if (!ret)
    {
        // Overwrite old file with default value
        SetDefault(options);
        ret = Save();
    }

    loaded = true;
    return ret;
}
/*****************************************************************************/
bool ClientConfig::Save()
{
    bool ret = false;

    // Open configuration file for writing configuration
    QFile f(QString(Config::HomePath.data()) + CLIENT_CONFIG_FILE);
    if (f.open(QIODevice::WriteOnly))
    {
        QXmlStreamWriter stream(&f);
        stream.setAutoFormatting(true);
        stream.writeStartDocument();

        stream.writeComment(QString("Generated by ") + QString("TarotClub ") + QString(TAROT_VERSION));

        stream.writeStartElement("tarotclub");
        stream.writeAttribute("version", QString(CLIENT_XML_VERSION));

        // Show avatar on the board or not?
        stream.writeTextElement("show_avatars", QString().setNum(options.showAvatars == true ? 1 : 0));
        // Background board color
        stream.writeTextElement("background_color", options.backgroundColor);
        // Language
        stream.writeTextElement("language", QString().setNum(options.language));
        // Delay before cleaning
        stream.writeTextElement("delay_before_cleaning", QString().setNum(options.delayBeforeCleaning));
        // click to clean cards after one turn
        stream.writeTextElement("click_to_clean", QString().setNum(options.enableDelayBeforeCleaning == true ? 1 : 0));

        // Player's indentity
        stream.writeStartElement("identity");

        // name
        stream.writeTextElement("name", options.identity.name.data());
        // avatar
        stream.writeTextElement("avatar", options.identity.avatar.data());
        // sex
        stream.writeTextElement("sex", QString().setNum(options.identity.gender));
        // quote
        stream.writeTextElement("quote", options.identity.quote.data());

        stream.writeEndElement(); // identity
        stream.writeEndElement(); // tarotclub
        stream.writeEndDocument();
        f.close();
        ret = true;
    }
    else
    {
        TLogError("Saving client's configuration failed.");
    }

    return ret;
}
/*****************************************************************************/
void ClientConfig::SetDefault(ClientOptions &opt)
{
    opt.showAvatars = AVATARS_DEF;
    opt.backgroundColor = "#008000";
    opt.language = 0;
    opt.deckFilePath = "default";

    opt.delayBeforeCleaning = CLIENT_TIMER_DEF;
    opt.enableDelayBeforeCleaning = true;

    opt.identity.name = "Moi";
    opt.identity.avatar = ":/images/avatars/inconnu.png";
    opt.identity.quote = "L'inventeur de l'escalier habitait sûrement au premier étage.";
    opt.identity.gender = Identity::MALE;
}

//=============================================================================
// End of file ClientConfig.cpp
//=============================================================================
