/*=============================================================================
 * TarotClub - DealEditorFile.cpp
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

#include "DealEditorFile.h"
#include "defines.h"
#include "Jeu.h"
#include <QtXml>

#define DEAL_XML_VERSION  "1.0"

/*****************************************************************************/
DealEditorFile::DealEditorFile()
{

}
/*****************************************************************************/
void DealEditorFile::loadFile(QString fileName)
{
   QDomDocument doc;
   QFile f(fileName);

   // Fichier non trouvé, on sort
   if( f.open(QIODevice::ReadOnly) == false ) {
      return;
   }
   doc.setContent(&f);
   f.close();

   chienDeck.clear();
   southDeck.clear();
   northDeck.clear();
   westDeck.clear();
   eastDeck.clear();

   // On teste le tag racine "TarotClub"
   QDomElement root=doc.documentElement();
   if(root.tagName()!="CustomDeal") {
      return;
   }

   if(root.attribute("version","0") != QString(DEAL_XML_VERSION)) {
      return;
   }

   // On parse les données
   QDomElement child=root.firstChild().toElement();
   while(!child.isNull()) {
      // Section "chien"
      if(child.tagName() == "chien") {
         QDomElement subchild = child.firstChild().toElement();
         while(!subchild.isNull()) {
            int val;
            if(subchild.tagName() == "card") {
               val = subchild.text().toInt();
               if( val<0 || val>77 ) {
                  return;
               }
               chienDeck.append(Jeu::getCard(val));
            }
            subchild = subchild.nextSibling().toElement();
         }
      // Section "south"
      } else if( child.tagName() == "south" ) {
         QDomElement subchild = child.firstChild().toElement();
         while(!subchild.isNull()) {
            int val;
            if(subchild.tagName() == "card") {
               val = subchild.text().toInt();
               if( val<0 || val>77 ) {
                  return;
               }
               southDeck.append(Jeu::getCard(val));
            }
            subchild = subchild.nextSibling().toElement();
         }
      // Section "north"
      } else if( child.tagName() == "north" ) {
         QDomElement subchild = child.firstChild().toElement();
         while(!subchild.isNull()) {
            int val;
            if(subchild.tagName() == "card") {
               val = subchild.text().toInt();
               if( val<0 || val>77 ) {
                  return;
               }
               northDeck.append(Jeu::getCard(val));
            }
            subchild = subchild.nextSibling().toElement();
         }
      // Section "west"
      } else if( child.tagName() == "west" ) {
         QDomElement subchild = child.firstChild().toElement();
         while(!subchild.isNull()) {
            int val;
            if(subchild.tagName() == "card") {
               val = subchild.text().toInt();
               if( val<0 || val>77 ) {
                  return;
               }
               westDeck.append(Jeu::getCard(val));
            }
            subchild = subchild.nextSibling().toElement();
         }
      // Section "east"
      } else if( child.tagName() == "east" ) {
         QDomElement subchild = child.firstChild().toElement();
         while(!subchild.isNull()) {
            int val;
            if(subchild.tagName() == "card") {
               val = subchild.text().toInt();
               if( val<0 || val>77 ) {
                  return;
               }
               eastDeck.append(Jeu::getCard(val));
            }
            subchild = subchild.nextSibling().toElement();
         }
      }
      child = child.nextSibling().toElement();
   }
}
/*****************************************************************************/
void DealEditorFile::saveFile(QString fileName)
{
   // On crée le document
   QDomDocument doc("CustomDeal");

   doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );
   doc.appendChild(doc.createTextNode("\n"));

   doc.appendChild(doc.createComment( QString("Generated by ")+QString("TarotClub ")+QString(TAROT_VERSION) ) );
   doc.appendChild(doc.createTextNode("\n"));

   // root node
   QDomElement rootNode = doc.createElement("CustomDeal");
   rootNode.setAttribute("version", QString(DEAL_XML_VERSION));
   doc.appendChild(rootNode);

   // Cartes du Chien
   QDomElement chienNode = doc.createElement("chien");
   rootNode.appendChild(chienNode);
   for (int i=0; i<chienDeck.count(); i++) {
      QDomElement node = doc.createElement("card");
      node.appendChild(doc.createTextNode( QString().setNum(chienDeck.at(i)->getId()) ));
      chienNode.appendChild(node);
   }

   // Cartes Sud
   QDomElement southNode = doc.createElement("south");
   rootNode.appendChild(southNode);
   for (int i=0; i<southDeck.count(); i++) {
      QDomElement node = doc.createElement("card");
      node.appendChild(doc.createTextNode( QString().setNum(southDeck.at(i)->getId()) ));
      southNode.appendChild(node);
   }

   // Cartes Ouest
   QDomElement westNode = doc.createElement("west");
   rootNode.appendChild(westNode);
   for (int i=0; i<westDeck.count(); i++) {
      QDomElement node = doc.createElement("card");
      node.appendChild(doc.createTextNode( QString().setNum(westDeck.at(i)->getId()) ));
      westNode.appendChild(node);
   }

   // Cartes Nord
   QDomElement northNode = doc.createElement("north");
   rootNode.appendChild(northNode);
   for (int i=0; i<northDeck.count(); i++) {
      QDomElement node = doc.createElement("card");
      node.appendChild(doc.createTextNode( QString().setNum(northDeck.at(i)->getId()) ));
      northNode.appendChild(node);
   }

   // Cartes Est
   QDomElement eastNode = doc.createElement("east");
   rootNode.appendChild(eastNode);
   for (int i=0; i<eastDeck.count(); i++) {
      QDomElement node = doc.createElement("card");
      node.appendChild(doc.createTextNode( QString().setNum(eastDeck.at(i)->getId()) ));
      eastNode.appendChild(node);
   }

   // Sauvegarde du document DOM en mémoire
   QFile f(fileName);
   if(!f.open(QIODevice::WriteOnly)) {
      return; // problème de création du fichier
   }

   QString out = doc.toString();

   QTextStream fout(&f);
   fout << out;
   f.close();
}

//=============================================================================
// End of file DealEditorFile.cpp
//=============================================================================
