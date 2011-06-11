/*=============================================================================
 * TarotClub - ClientConfig.cpp
 *=============================================================================
 * Classe de gestion du fichier d'options en XML (client graphique)
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

#include "ClientConfig.h"
#include <QtXml>
#include <QString>
#include <QDesktopServices>

QString ClientConfig::configPath = QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + "/.tarotclub/";

/*****************************************************************************/
ClientConfig::ClientConfig()
{
   setDefault( &options );
}
/*****************************************************************************/
ClientOptions *ClientConfig::getOptions()
{
   return(&options);
}
/*****************************************************************************/
void ClientConfig::setOptions( ClientOptions *newOptions )
{
   options = *newOptions;
}
/*****************************************************************************/
bool ClientConfig::load()
{
   QDomDocument doc;
   QFile f( configPath + CLIENT_CONFIG_FILE );
   QString txt;
   int val;

   // Fichier non trouvé, on en crée un par défaut et on sort
   if( f.open(QIODevice::ReadOnly) == false ) {
      save();
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

      if(child.tagName() == "avatars") {
         val = child.text().toInt();
         if( val == 1 ) {
            options.showAvatars = true;
         } else {
            options.showAvatars = false;
         }

      } else if(child.tagName() == "tapis") {
         options.tapis = child.text();

      } else if(child.tagName() == "langue") {
         options.langue = child.text().toInt();
         if( options.langue >= NB_LANGUES ) {
            options.langue = 0;
         }

      } else if(child.tagName() == "identity") {
         QDomElement lastchild = child.firstChild().toElement();
         while(!lastchild.isNull()) {
            if(lastchild.tagName() == "name") {
               txt = lastchild.text();
               if( txt.isEmpty() ) {
                  txt = "Sans nom";
               }
               options.identity.name = txt;

            } else if(lastchild.tagName() == "quote") {
               options.identity.quote = lastchild.text();

            } else if(lastchild.tagName() == "sex") {
               options.identity.sex = (SexType)lastchild.text().toInt();

            } else if(lastchild.tagName() == "avatar") {
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
bool ClientConfig::save()
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

   // affichage ou non des avatars
   QDomElement affAvatarNode = doc.createElement("avatars");
   affAvatarNode.appendChild(doc.createTextNode( QString().setNum( options.showAvatars==true?1:0 ) ));
   rootNode.appendChild(affAvatarNode);

   // Tapis de jeu
   QDomElement tapisNode = doc.createElement("tapis");
   tapisNode.appendChild(doc.createTextNode( options.tapis ));
   rootNode.appendChild(tapisNode);

   // Langue
   QDomElement langueNode = doc.createElement("langue");
   langueNode.appendChild(doc.createTextNode( QString().setNum(options.langue) ));
   rootNode.appendChild(langueNode);

   // Identité du joueur
   identityNode = doc.createElement("identity");
   rootNode.appendChild(identityNode);

      // name
      nameNode = doc.createElement("name");
      nameNode.appendChild(doc.createTextNode( options.identity.name ));
      identityNode.appendChild(nameNode);

      // avatar
      avatarNode = doc.createElement("avatar");
      avatarNode.appendChild(doc.createTextNode( options.identity.avatar ));
      identityNode.appendChild(avatarNode);

      // sex
      sexNode = doc.createElement("sex");
      sexNode.appendChild(doc.createTextNode( QString().setNum( options.identity.sex ) ));
      identityNode.appendChild(sexNode);

      // quote
      quoteNode = doc.createElement("quote");
      quoteNode.appendChild(doc.createTextNode( options.identity.quote ));
      identityNode.appendChild(quoteNode);

   // Sauvegarde du document DOM en mémoire
   QFile f( configPath + CLIENT_CONFIG_FILE );
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
void ClientConfig::setDefault( ClientOptions *opt )
{
   opt->showAvatars = AVATARS_DEF;
   opt->tapis = "#008000";
   opt->langue = 0;
   opt->deckFilePath = "default";

   opt->identity.name = "Moi";
   opt->identity.avatar = ":/images/avatars/inconnu.png";
   opt->identity.quote = QString::fromUtf8("L'inventeur de l'escalier habitait sûrement au premier étage.");
   opt->identity.sex = MALE;
}

//=============================================================================
// End of file ClientConfig.cpp
//=============================================================================
