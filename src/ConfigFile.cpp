/*=============================================================================
 * Tarot Club - ConfigFile.cpp
 *=============================================================================
 * Classe de gestion du fichier d'options en XML
 *=============================================================================
 * Tarot Club est un jeu de Tarot fran�ais
 * Copyright (C) 2003-2005  Anthony Rabine
 * anthony@ooso.org
 * http://tarotclub.ooso.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *=============================================================================
 */

#include "ConfigFile.h"
#include <QtXml>

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

   // Fichier non trouv�, on en cr�e un par d�faut et on sort
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

   // On parse les donn�es
   QDomElement child=root.firstChild().toElement();
   while(!child.isNull()) {
      //-------------------
      // Section "general"
      //-------------------
      if(child.tagName() == "general") {
         QDomElement subchild = child.firstChild().toElement();
         while(!subchild.isNull()) {
            int val;

            if(subchild.tagName() == "players") {
               val = subchild.text().toInt();
               if( val<3 || val>5 ) {
                  val = NBPLAYERS_DEF;
               }
               options.nbPlayers = val;

            } else if(subchild.tagName() == "pause1") {
               val = subchild.text().toInt();
               if( val<0 || val>9000 ) {
                  val = TIMER1_DEF;
               }
               options.timer1 = val;

            } else if(subchild.tagName() == "pause2") {
               val = subchild.text().toInt();
               if( val<0 || val>9000 ) {
                  val = TIMER2_DEF;
               }
               options.timer2 = val;

            } else if(subchild.tagName() == "port") {
               options.port = subchild.text().toInt();

            } else if(subchild.tagName() == "avatars") {
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
            }
            subchild = subchild.nextSibling().toElement();
         }
      
      //-------------------
      // Section "player"
      //-------------------
      } else if( child.tagName() == "player" ) {
         int pos;
         pos = child.attribute("pos","0").toInt();

         QDomElement subchild = child.firstChild().toElement();
         while(!subchild.isNull()) {            
            QString txt;

            if(subchild.tagName() == "name") {
               txt = subchild.text();
               if( txt.isEmpty() ) {
                  txt = "Sans nom";
               }
               options.identities[pos].name = txt;

            } else if(subchild.tagName() == "quote") {
               options.identities[pos].quote = subchild.text();

            } else if(subchild.tagName() == "sex") {
               options.identities[pos].sex = (SexType)subchild.text().toInt();

            } else if(subchild.tagName() == "avatar") {
               options.identities[pos].avatar = subchild.text();
            }        
            subchild = subchild.nextSibling().toElement();
         }
      }
      child = child.nextSibling().toElement();
   }
   return (true);
}
/*****************************************************************************/
bool ConfigFile::save( const QString &fileName )
{
   // On cr�e le document
   QDomDocument doc("TarotClub");

   doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"ISO-8859-1\"" ) );
   doc.appendChild(doc.createTextNode("\n"));

   doc.appendChild(doc.createComment( QString("G�n�r� par ")+QString("Tarot Club ")+QString(TAROT_VERSION) ) );
   doc.appendChild(doc.createTextNode("\n"));

   // root node
   QDomElement rootNode = doc.createElement("TarotClub");
   rootNode.setAttribute("version", QString(XML_VERSION));
   doc.appendChild(rootNode);

   // Param�tres g�n�raux
   QDomElement generalNode = doc.createElement("general");
   rootNode.appendChild(generalNode);

      // Nombre de joueurs
      QDomElement joueursNode = doc.createElement("player");
      joueursNode.appendChild(doc.createTextNode( QString().setNum( options.nbPlayers ) ));
      generalNode.appendChild(joueursNode);

      // R�glage du timing entre chaque joueur
      QDomElement temps1Node = doc.createElement("pause1");
      temps1Node.appendChild(doc.createTextNode( QString().setNum( options.timer1 ) ));
      generalNode.appendChild(temps1Node);

      // R�glage du timing entre chaque joueur
      QDomElement temps2Node = doc.createElement("pause2");
      temps2Node.appendChild(doc.createTextNode( QString().setNum( options.timer2 ) ));
      generalNode.appendChild(temps2Node);

      // affichage ou non des avatars
      QDomElement affAvatarNode = doc.createElement("avatars");
      affAvatarNode.appendChild(doc.createTextNode( QString().setNum( options.showAvatars==true?1:0 ) ));
      generalNode.appendChild(affAvatarNode);

      // Port r�seau
      QDomElement portNode = doc.createElement("port");
      portNode.appendChild(doc.createTextNode( QString().setNum( options.port ) ));
      generalNode.appendChild(portNode);

      // Tapis de jeu
      QDomElement tapisNode = doc.createElement("tapis");
      tapisNode.appendChild(doc.createTextNode( options.tapis ));
      generalNode.appendChild(tapisNode);

        // Langue
      QDomElement langueNode = doc.createElement("langue");
      langueNode.appendChild(doc.createTextNode( QString().setNum(options.langue) ));
      generalNode.appendChild(langueNode);

   // Param�tres pour chaque joueur
   QDomElement playerNode;
   QDomElement nameNode;
   QDomElement avatarNode;
   QDomElement sexNode;
   QDomElement quoteNode;

   for( int i=0; i<5; i++ )
   {
      playerNode = doc.createElement("player");
      playerNode.setAttribute("pos", QString().setNum(i));

      // name
      nameNode = doc.createElement("name");
      nameNode.appendChild(doc.createTextNode( options.identities[i].name ));
      playerNode.appendChild(nameNode);

      // avatar
      avatarNode = doc.createElement("avatar");
      avatarNode.appendChild(doc.createTextNode( options.identities[i].avatar ));
      playerNode.appendChild(avatarNode);
      
      // sex
      sexNode = doc.createElement("sex");
      sexNode.appendChild(doc.createTextNode( QString().setNum( options.identities[i].sex ) ));
      playerNode.appendChild(sexNode);

      // quote
      quoteNode = doc.createElement("quote");
      quoteNode.appendChild(doc.createTextNode( options.identities[i].quote ));
      playerNode.appendChild(quoteNode);

      rootNode.appendChild(playerNode);
   }

   // Sauvegarde du document DOM en m�moire
   QFile f( path + "/" + fileName );
   if(!f.open(QIODevice::WriteOnly)) {
      return (false); // probl�me de cr�ation du fichier
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
   opt->nbPlayers = NBPLAYERS_DEF;
   opt->timer1 = TIMER1_DEF;
   opt->timer2 = TIMER2_DEF;
   opt->port = PORT_DEF;
   opt->showAvatars = AVATARS_DEF;
   opt->tapis = "tapis0.png";
   opt->langue = 0;
   
   opt->identities[0].name = "Moi";
   opt->identities[0].avatar = ":/images/inconnu.png";
   opt->identities[0].quote = "L'inventeur de l'escalier habitait s�rement au premier �tage.";
   opt->identities[0].sex = MALE;

   opt->identities[1].name = "Est";
   opt->identities[1].avatar = ":/images/robot.png";
   opt->identities[1].quote = "Plus je grossis, plus je m'aigris.";
   opt->identities[1].sex = MALE;

   opt->identities[2].name = "Nord";
   opt->identities[2].avatar = ":/images/robot.png";
   opt->identities[2].quote = "J'ai fait un test de QI, les resultats �taient negatifs.";
   opt->identities[2].sex = MALE;

   opt->identities[3].name = "Ouest";
   opt->identities[3].avatar = ":/images/robot.png";
   opt->identities[3].quote = "Quand on mettra les cons sur orbite, t'as pas fini de tourner.";
   opt->identities[3].sex = MALE;

   opt->identities[4].name = "Nord-Ouest";
   opt->identities[4].avatar = ":/images/robot.png";
   opt->identities[4].quote = "Dans le pass�, il y avait plus de futur que maintenant.";
   opt->identities[4].sex = MALE;

}

//=============================================================================
// End of file ConfigFile.cpp
//=============================================================================
