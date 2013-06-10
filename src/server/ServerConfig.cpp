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
#include <QtXml>
#include <QString>
#include <QDesktopServices>

/*****************************************************************************/
ServerConfig::ServerConfig()
{
    SetDefault(&options);
}
/*****************************************************************************/
ServerOptions &ServerConfig::GetOptions()
{
    return options;
}
/*****************************************************************************/
void ServerConfig::SetOptions(ServerOptions &newOptions)
{
    options = newOptions;
}
/*****************************************************************************/
bool ServerConfig::Load()
{
    QDomDocument doc;
    QFile f(Config::path + SERVER_CONFIG_FILE);
    QString txt;
    int val;

    // File not found, we create default one and quit loading
    if (f.open(QIODevice::ReadOnly) == false)
    {
        return save();
    }
    doc.setContent(&f);
    f.close();

    // test of root element "tarotclubd"
    QDomElement root = doc.documentElement();
    if (root.tagName() != "tarotclubd")
    {
        return (false);
    }

    if (root.attribute("version", "0") != QString(SERVER_XML_VERSION))
    {
        return (false);
    }

    // Parsing data
    QDomElement child = root.firstChild().toElement();
    while (!child.isNull())
    {

        if (child.tagName() == "pause")
        {
            val = child.text().toInt();
            if (val < 0 || val > 9000)
            {
                val = TIMER1_DEF;
            }
            options.timer = val;

        }
        else if (child.tagName() == "port")
        {
            options.port = child.text().toInt();

        }
        else if (child.tagName() == "identity")
        {
            int pos;
            pos = child.attribute("pos", "-1").toInt();
            if (pos < 0 || pos > 2)
            {
                return false;
            }

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
                    options.bots[pos].name = txt;

                }
                else if (lastchild.tagName() == "quote")
                {
                    options.bots[pos].quote = lastchild.text();

                }
                else if (lastchild.tagName() == "sex")
                {
                    options.bots[pos].sex = (SexType)lastchild.text().toInt();

                }
                else if (lastchild.tagName() == "avatar")
                {
                    options.bots[pos].avatar = lastchild.text();
                }
                lastchild = lastchild.nextSibling().toElement();
            }
        }
        child = child.nextSibling().toElement();
    }
    return true;
}
/*****************************************************************************/
bool ServerConfig::Save()
{
    // Document creation
    QDomDocument doc("tarotclubd");
    QDomElement identityNode;
    QDomElement nameNode;
    QDomElement avatarNode;
    QDomElement sexNode;
    QDomElement quoteNode;

    doc.appendChild(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));
    doc.appendChild(doc.createTextNode("\n"));

    doc.appendChild(doc.createComment(QString("Généré par ") + QString("TarotClub ") + QString(TAROT_VERSION)));
    doc.appendChild(doc.createTextNode("\n"));

    // root node
    QDomElement rootNode = doc.createElement("tarotclubd");
    rootNode.setAttribute("version", QString(SERVER_XML_VERSION));
    doc.appendChild(rootNode);

    // Réglage du timing entre chaque joueur
    QDomElement temps1Node = doc.createElement("pause");
    temps1Node.appendChild(doc.createTextNode(QString().setNum(options.timer)));
    rootNode.appendChild(temps1Node);

    // Port réseau
    QDomElement portNode = doc.createElement("port");
    portNode.appendChild(doc.createTextNode(QString().setNum(options.port)));
    rootNode.appendChild(portNode);

    // Parameter of each bot
    for (int i = 0; i < 3; i++)
    {
        identityNode = doc.createElement("identity");
        rootNode.appendChild(identityNode);
        identityNode.setAttribute("pos", QString().setNum(i));

        // name
        nameNode = doc.createElement("name");
        nameNode.appendChild(doc.createTextNode(options.bots[i].name));
        identityNode.appendChild(nameNode);

        // avatar
        avatarNode = doc.createElement("avatar");
        avatarNode.appendChild(doc.createTextNode(options.bots[i].avatar));
        identityNode.appendChild(avatarNode);

        // sex
        sexNode = doc.createElement("sex");
        sexNode.appendChild(doc.createTextNode(QString().setNum(options.bots[i].sex)));
        identityNode.appendChild(sexNode);

        // quote
        quoteNode = doc.createElement("quote");
        quoteNode.appendChild(doc.createTextNode(options.bots[i].quote));
        identityNode.appendChild(quoteNode);
    }

    // Save DOM XML tree into file
    QFile f(Config::path + SERVER_CONFIG_FILE);
    if (!f.open(QIODevice::WriteOnly))
    {
        qDebug("Saving server's configuration failed.");
        return false;
    }

    QString out = doc.toString();

    QTextStream fout(&f);
    fout << out;
    f.close();

    return true;
}
/*****************************************************************************/
void ServerConfig::SetDefault(ServerOptions &opt)
{
    opt.timer = TIMER1_DEF;
    opt.port = DEFAULT_PORT;

    opt.bots[BOT_WEST].name = "Ouest";
    opt.bots[BOT_WEST].avatar = ":/images/avatars/robot.png";
    opt.bots[BOT_WEST].quote = QString::fromUtf8("Quand on mettra les cons sur orbite, t'as pas fini de tourner.");
    opt.bots[BOT_WEST].sex = MALE;

    opt.bots[BOT_NORTH].name = "Nord";
    opt.bots[BOT_NORTH].avatar = ":/images/avatars/robot.png";
    opt.bots[BOT_NORTH].quote = QString::fromUtf8("J'ai fait un test de QI, les resultats étaient negatifs.");
    opt.bots[BOT_NORTH].sex = MALE;

    opt.bots[BOT_EAST].name = "Est";
    opt.bots[BOT_EAST].avatar = ":/images/avatars/robot.png";
    opt.bots[BOT_EAST].quote = "Plus je grossis, plus je m'aigris.";
    opt.bots[BOT_EAST].sex = MALE;
}

//=============================================================================
// End of file ServerConfig.cpp
//=============================================================================
