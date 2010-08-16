/*=============================================================================
 * TarotClub - ServerConfig.cpp
 *=============================================================================
 * Classe de gestion du fichier d'options en XML (serveur)
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * This file must be used under the terms of the CeCILL.
 * This source file is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at
 * http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
 *
 *=============================================================================
 */

#include "ServerConfig.h"
#include <QtXml>
#include <QString>

/*****************************************************************************/
ServerConfig::ServerConfig()
{
   setDefault( &options );
}
/*****************************************************************************/
ServerOptions *ServerConfig::getOptions()
{
   return(&options);
}
/*****************************************************************************/
void ServerConfig::setOptions( ServerOptions *newOptions )
{
   options = *newOptions;
}
/*****************************************************************************/
bool ServerConfig::load()
{
   QDomDocument doc;
   QFile f( QCoreApplication::applicationDirPath () + "/" + SERVER_CONFIG_FILE );
   QString txt;
   int val;

   // Fichier non trouvé, on en crée un par défaut et on sort
   if( f.open(QIODevice::ReadOnly) == false ) {
      save();
      return(true);
   }
   doc.setContent(&f);
   f.close();

   // On teste le tag racine "TarotClubd"
   QDomElement root=doc.documentElement();
   if(root.tagName()!="TarotClubd") {
      return (false);
   }

   if(root.attribute("version","0") != QString(XML_VERSION)) {
      return (false);
   }

   // On parse les données
   QDomElement child=root.firstChild().toElement();
   while(!child.isNull()) {

      if(child.tagName() == "pause") {
         val = child.text().toInt();
         if( val<0 || val>9000 ) {
            val = TIMER1_DEF;
         }
         options.timer = val;

      } else if(child.tagName() == "port") {
         options.port = child.text().toInt();

      } else if(child.tagName() == "identity") {
         int pos;
         pos = child.attribute("pos","-1").toInt();
         if (pos<0 || pos>2)
            return false;

         QDomElement lastchild = child.firstChild().toElement();
         while(!lastchild.isNull()) {
            if(lastchild.tagName() == "name") {
               txt = lastchild.text();
               if( txt.isEmpty() ) {
                  txt = "Sans nom";
               }
               options.bots[pos].name = txt;

            } else if(lastchild.tagName() == "quote") {
               options.bots[pos].quote = lastchild.text();

            } else if(lastchild.tagName() == "sex") {
               options.bots[pos].sex = (SexType)lastchild.text().toInt();

            } else if(lastchild.tagName() == "avatar") {
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
bool ServerConfig::save()
{
   // On crée le document
   QDomDocument doc("TarotClub");
   QDomElement identityNode;
   QDomElement nameNode;
   QDomElement avatarNode;
   QDomElement sexNode;
   QDomElement quoteNode;

   doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );
   doc.appendChild(doc.createTextNode("\n"));

   doc.appendChild(doc.createComment( QString("Généré par ")+QString("TarotClub ")+QString(TAROT_VERSION) ) );
   doc.appendChild(doc.createTextNode("\n"));

   // root node
   QDomElement rootNode = doc.createElement("TarotClubd");
   rootNode.setAttribute("version", QString(XML_VERSION));
   doc.appendChild(rootNode);

   // Réglage du timing entre chaque joueur
   QDomElement temps1Node = doc.createElement("pause");
   temps1Node.appendChild(doc.createTextNode( QString().setNum( options.timer ) ));
   rootNode.appendChild(temps1Node);

   // Port réseau
   QDomElement portNode = doc.createElement("port");
   portNode.appendChild(doc.createTextNode( QString().setNum( options.port ) ));
   rootNode.appendChild(portNode);

   // Paramètres pour chaque bot
   for( int i=0; i<3; i++ )
   {
      identityNode = doc.createElement("identity");
      rootNode.appendChild(identityNode);
      identityNode.setAttribute("pos", QString().setNum(i));

         // name
         nameNode = doc.createElement("name");
         nameNode.appendChild(doc.createTextNode( options.bots[i].name ));
         identityNode.appendChild(nameNode);

         // avatar
         avatarNode = doc.createElement("avatar");
         avatarNode.appendChild(doc.createTextNode( options.bots[i].avatar ));
         identityNode.appendChild(avatarNode);

         // sex
         sexNode = doc.createElement("sex");
         sexNode.appendChild(doc.createTextNode( QString().setNum( options.bots[i].sex ) ));
         identityNode.appendChild(sexNode);

         // quote
         quoteNode = doc.createElement("quote");
         quoteNode.appendChild(doc.createTextNode( options.bots[i].quote ));
         identityNode.appendChild(quoteNode);
   }

   // Sauvegarde du document DOM en mémoire
   QFile f( QCoreApplication::applicationDirPath () + "/" + SERVER_CONFIG_FILE );
   if(!f.open(QIODevice::WriteOnly)) {
      return (false); // problème de création du fichier
   }

   QString out = doc.toString();

   QTextStream fout(&f);
   fout << out;
   f.close();

   return (true);
}
/*****************************************************************************/
void ServerConfig::setDefault( ServerOptions *opt )
{
   opt->timer = TIMER1_DEF;
   opt->port = DEFAULT_PORT;

   opt->bots[BOT_WEST].name = "Ouest";
   opt->bots[BOT_WEST].avatar = ":/images/avatars/robot.png";
   opt->bots[BOT_WEST].quote = QString::fromUtf8("Quand on mettra les cons sur orbite, t'as pas fini de tourner.");
   opt->bots[BOT_WEST].sex = MALE;

   opt->bots[BOT_NORTH].name = "Nord";
   opt->bots[BOT_NORTH].avatar = ":/images/avatars/robot.png";
   opt->bots[BOT_NORTH].quote = QString::fromUtf8("J'ai fait un test de QI, les resultats étaient negatifs.");
   opt->bots[BOT_NORTH].sex = MALE;

   opt->bots[BOT_EAST].name = "Est";
   opt->bots[BOT_EAST].avatar = ":/images/avatars/robot.png";
   opt->bots[BOT_EAST].quote = "Plus je grossis, plus je m'aigris.";
   opt->bots[BOT_EAST].sex = MALE;
}

//=============================================================================
// End of file ServerConfig.cpp
//=============================================================================
