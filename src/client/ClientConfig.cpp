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
#include <QtXml>
#include <QString>
#include <QDesktopServices>


/*****************************************************************************/
ClientConfig::ClientConfig()
{
    SetDefault(options);
}
/*****************************************************************************/
ClientOptions &ClientConfig::GetOptions()
{
    return options;
}
/*****************************************************************************/
void ClientConfig::SetOptions(ClientOptions &newOptions)
{
    options = newOptions;
}
/*****************************************************************************/
bool ClientConfig::Load()
{
    QDomDocument doc;
    QFile f(Config::HomePath + CLIENT_CONFIG_FILE);
    QString txt;
    int val;

    // Fichier non trouvé, on en crée un par défaut et on sort
    if (f.open(QIODevice::ReadOnly) == false)
    {
        return Save();
    }
    doc.setContent(&f);
    f.close();

    // On teste le tag racine "TarotClub"
    QDomElement root = doc.documentElement();
    if (root.tagName() != "tarotclub")
    {
        return (false);
    }

    if (root.attribute("version", "0") != QString(CLIENT_XML_VERSION))
    {
        return (false);
    }

    // On parse les données
    QDomElement child = root.firstChild().toElement();
    while (!child.isNull())
    {

        if (child.tagName() == "show_avatars")
        {
            val = child.text().toInt();
            if (val == 1)
            {
                options.showAvatars = true;
            }
            else
            {
                options.showAvatars = false;
            }

        }
        else if (child.tagName() == "background_color")
        {
            options.backgroundColor = child.text();
        }
        else if (child.tagName() == "language")
        {
            options.language = child.text().toInt();
            if (options.language >= MAX_LANGUAGES)
            {
                options.language = 0;
            }

        }
        else if (child.tagName() == "delay_before_cleaning")
        {
            options.delayBeforeCleaning = child.text().toInt();
            if (options.delayBeforeCleaning > 5000)
            {
                options.delayBeforeCleaning = 5000;
            }

        }
        else if (child.tagName() == "click_to_clean")
        {
            val = child.text().toInt();
            if (val == 1)
            {
                options.enableDelayBeforeCleaning = true;
            }
            else
            {
                options.enableDelayBeforeCleaning = false;
            }
        }
        else if (child.tagName() == "identity")
        {
            QDomElement lastchild = child.firstChild().toElement();
            while (!lastchild.isNull())
            {
                if (lastchild.tagName() == "name")
                {
                    txt = lastchild.text();
                    if (txt.isEmpty())
                    {
                        txt = "Unknown";
                    }
                    options.identity.name = txt;

                }
                else if (lastchild.tagName() == "quote")
                {
                    options.identity.quote = lastchild.text();

                }
                else if (lastchild.tagName() == "sex")
                {
                    options.identity.sex = (Identity::Gender)lastchild.text().toInt();

                }
                else if (lastchild.tagName() == "avatar")
                {
                    options.identity.avatar = lastchild.text();
                }
                lastchild = lastchild.nextSibling().toElement();
            }
        }
        child = child.nextSibling().toElement();

    }
    return true;
}
/*****************************************************************************/
bool ClientConfig::Save()
{
    // On crée le document
    QDomDocument doc("tarotclub");
    QDomElement identityNode;
    QDomElement nameNode;
    QDomElement avatarNode;
    QDomElement sexNode;
    QDomElement quoteNode;

    doc.appendChild(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));
    doc.appendChild(doc.createTextNode("\n"));

    doc.appendChild(doc.createComment(QString("Generated by ") + QString("TarotClub ") + QString(TAROT_VERSION)));
    doc.appendChild(doc.createTextNode("\n"));

    // root node
    QDomElement rootNode = doc.createElement("tarotclub");
    rootNode.setAttribute("version", QString(CLIENT_XML_VERSION));
    doc.appendChild(rootNode);

    // Show avatar on the board or not?
    QDomElement affAvatarNode = doc.createElement("show_avatars");
    affAvatarNode.appendChild(doc.createTextNode(QString().setNum(options.showAvatars == true ? 1 : 0)));
    rootNode.appendChild(affAvatarNode);

    // Background board color
    QDomElement tapisNode = doc.createElement("background_color");
    tapisNode.appendChild(doc.createTextNode(options.backgroundColor));
    rootNode.appendChild(tapisNode);

    // Language
    QDomElement langueNode = doc.createElement("language");
    langueNode.appendChild(doc.createTextNode(QString().setNum(options.language)));
    rootNode.appendChild(langueNode);

    // Delay before cleaning
    QDomElement delayeNode = doc.createElement("delay_before_cleaning");
    delayeNode.appendChild(doc.createTextNode(QString().setNum(options.delayBeforeCleaning)));
    rootNode.appendChild(delayeNode);

    // click to clean cards after one turn
    QDomElement clickNode = doc.createElement("click_to_clean");
    clickNode.appendChild(doc.createTextNode(QString().setNum(options.enableDelayBeforeCleaning == true ? 1 : 0)));
    rootNode.appendChild(clickNode);

    // Player's indentity
    identityNode = doc.createElement("identity");
    rootNode.appendChild(identityNode);

    // name
    nameNode = doc.createElement("name");
    nameNode.appendChild(doc.createTextNode(options.identity.name));
    identityNode.appendChild(nameNode);

    // avatar
    avatarNode = doc.createElement("avatar");
    avatarNode.appendChild(doc.createTextNode(options.identity.avatar));
    identityNode.appendChild(avatarNode);

    // sex
    sexNode = doc.createElement("sex");
    sexNode.appendChild(doc.createTextNode(QString().setNum(options.identity.sex)));
    identityNode.appendChild(sexNode);

    // quote
    quoteNode = doc.createElement("quote");
    quoteNode.appendChild(doc.createTextNode(options.identity.quote));
    identityNode.appendChild(quoteNode);

    // Save DOM XML into file
    QFile f(Config::HomePath + CLIENT_CONFIG_FILE);
    if (!f.open(QIODevice::WriteOnly))
    {
        qDebug("Saving client's configuration failed.");
        return false;
    }

    QString out = doc.toString();

    QTextStream fout(&f);
    fout << out;
    f.close();

    return true;
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
    opt.identity.quote = QString::fromUtf8("L'inventeur de l'escalier habitait sûrement au premier étage.");
    opt.identity.sex = Identity::MALE;
}

//=============================================================================
// End of file ClientConfig.cpp
//=============================================================================
