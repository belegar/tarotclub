/*=============================================================================
 * TarotClub - ConfigFile.cpp
 *=============================================================================
 * Classe de gestion du fichier d'options en XML
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

#include "ConfigFile.h"
#include <QtXml>
#include <QString>

/*****************************************************************************/
ConfigFile::ConfigFile( const QString &gamePath )
{
   path = gamePath;
   setDefault( &options );
}
/*****************************************************************************/
GameOptions *ConfigFile::getGameOptions()
{
   return(&options);
}
/*****************************************************************************/
QString ConfigFile::getPath()
{
   return(path);
}
/*****************************************************************************/
void ConfigFile::setGameOptions( GameOptions *newOptions )
{
   options = *newOptions;
}
/*****************************************************************************/
bool ConfigFile::load( const QString &fileName )
{
   QDomDocument doc;
   QFile f( path + "/" + fileName );
   QString txt;
   int val;

   // Fichier non trouvé, on en crée un par défaut et on sort
   if( f.open(QIODevice::ReadOnly) == false ) {
      save(fileName);
      return(true);
   }
   doc.setContent(&f);
   f.close();

   // On teste le tag racine "TarotClub"
   QDomElement root=doc.documentElement();
   if(root.tagName()!="TarotClub") {
      return (false);
   }

   if(root.attribute("version","0") != QString(XML_VERSION)) {
      return (false);
   }

   // On parse les données
   QDomElement child=root.firstChild().toElement();
   while(!child.isNull()) {
      //-------------------
      // Section "client"
      //-------------------
      if(child.tagName() == "client") {
         QDomElement subchild = child.firstChild().toElement();
         while(!subchild.isNull()) {

            if(subchild.tagName() == "avatars") {
               val = subchild.text().toInt();
               if( val == 1 ) {
                  options.showAvatars = true;
               } else {
                  options.showAvatars = false;
               }

            } else if(subchild.tagName() == "tapis") {
               options.tapis = subchild.text();

            } else if(subchild.tagName() == "langue") {
               options.langue = subchild.text().toInt();
               if( options.langue >= NB_LANGUES ) {
                  options.langue = 0;
               }

            } else if(subchild.tagName() == "identity") {
               QDomElement lastchild = subchild.firstChild().toElement();
               while(!lastchild.isNull()) {
                  if(lastchild.tagName() == "name") {
                     txt = lastchild.text();
                     if( txt.isEmpty() ) {
                        txt = "Sans nom";
                     }
                     options.client.name = txt;

                  } else if(lastchild.tagName() == "quote") {
                     options.client.quote = lastchild.text();

                  } else if(lastchild.tagName() == "sex") {
                     options.client.sex = (SexType)lastchild.text().toInt();

                  } else if(lastchild.tagName() == "avatar") {
                     options.client.avatar = lastchild.text();
                  }
                  lastchild = lastchild.nextSibling().toElement();
               }
            }
            subchild = subchild.nextSibling().toElement();
         }

      //-------------------
      // Section "server"
      //-------------------
      } else if( child.tagName() == "server" ) {
         QDomElement subchild = child.firstChild().toElement();
         while(!subchild.isNull()) {

            if(subchild.tagName() == "pause") {
               val = subchild.text().toInt();
               if( val<0 || val>9000 ) {
                  val = TIMER1_DEF;
               }
               options.timer = val;

            } else if(subchild.tagName() == "port") {
               options.port = subchild.text().toInt();

            } else if(subchild.tagName() == "identity") {
               int pos;
               pos = subchild.attribute("pos","-1").toInt();
               if (pos<0 || pos>2)
                  return false;

               QDomElement lastchild = subchild.firstChild().toElement();
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
            subchild = subchild.nextSibling().toElement();
         }
      }
      child = child.nextSibling().toElement();
   }
   return true;
}
/*****************************************************************************/
bool ConfigFile::save( const QString &fileName )
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
   QDomElement rootNode = doc.createElement("TarotClub");
   rootNode.setAttribute("version", QString(XML_VERSION));
   doc.appendChild(rootNode);

   //------------- Paramètres du client --------------------------------
   QDomElement clientNode = doc.createElement("client");
   rootNode.appendChild(clientNode);

      // affichage ou non des avatars
      QDomElement affAvatarNode = doc.createElement("avatars");
      affAvatarNode.appendChild(doc.createTextNode( QString().setNum( options.showAvatars==true?1:0 ) ));
      clientNode.appendChild(affAvatarNode);

      // Tapis de jeu
      QDomElement tapisNode = doc.createElement("tapis");
      tapisNode.appendChild(doc.createTextNode( options.tapis ));
      clientNode.appendChild(tapisNode);

      // Langue
      QDomElement langueNode = doc.createElement("langue");
      langueNode.appendChild(doc.createTextNode( QString().setNum(options.langue) ));
      clientNode.appendChild(langueNode);

      // Identité du joueur
      identityNode = doc.createElement("identity");
      clientNode.appendChild(identityNode);

         // name
         nameNode = doc.createElement("name");
         nameNode.appendChild(doc.createTextNode( options.client.name ));
         identityNode.appendChild(nameNode);

         // avatar
         avatarNode = doc.createElement("avatar");
         avatarNode.appendChild(doc.createTextNode( options.client.avatar ));
         identityNode.appendChild(avatarNode);

         // sex
         sexNode = doc.createElement("sex");
         sexNode.appendChild(doc.createTextNode( QString().setNum( options.client.sex ) ));
         identityNode.appendChild(sexNode);

         // quote
         quoteNode = doc.createElement("quote");
         quoteNode.appendChild(doc.createTextNode( options.client.quote ));
         identityNode.appendChild(quoteNode);


   //------------- Paramètres du serveur --------------------------------
   QDomElement serverNode = doc.createElement("server");
   rootNode.appendChild(serverNode);

      // Réglage du timing entre chaque joueur
      QDomElement temps1Node = doc.createElement("pause");
      temps1Node.appendChild(doc.createTextNode( QString().setNum( options.timer ) ));
      serverNode.appendChild(temps1Node);

      // Port réseau
      QDomElement portNode = doc.createElement("port");
      portNode.appendChild(doc.createTextNode( QString().setNum( options.port ) ));
      serverNode.appendChild(portNode);

      // Paramètres pour chaque bot
      for( int i=0; i<3; i++ )
      {
         identityNode = doc.createElement("identity");
         serverNode.appendChild(identityNode);
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
   QFile f( path + "/" + fileName );
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
void ConfigFile::setDefault( GameOptions *opt )
{

   opt->showAvatars = AVATARS_DEF;
   opt->tapis = "#008000";
   opt->langue = 0;
   opt->deckFilePath = "./default";

   opt->client.name = "Moi";
   opt->client.avatar = ":/images/avatars/inconnu.png";
   opt->client.quote = QString::fromUtf8("L'inventeur de l'escalier habitait sûrement au premier étage.");
   opt->client.sex = MALE;

   // ----- Server
   opt->timer = TIMER1_DEF;
   opt->port = PORT_DEF;

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
// End of file ConfigFile.cpp
//=============================================================================
