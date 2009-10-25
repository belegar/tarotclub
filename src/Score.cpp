/*=============================================================================
 * TarotClub - Score.cpp
 *=============================================================================
 * Calcule les scores en fonction des paramètres du jeu
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

#include "Score.h"
#include <stdlib.h>
#include <QFile>
#include <QString>
#include <QTextStream>

/*****************************************************************************/
Score::Score()
{
   init();
}
/*****************************************************************************/
/**
 * RAZ complet des variables
 */
void Score::init()
{
   int i;

   // RAZ des totaux
   for( i=0; i<5; i++ ) {
      scores[i] = 0;
   }
   carteExcuse = HYPERSPACE;
   chelemDeclare = false;
}
/*****************************************************************************/
/**
 * Est appelé juste avant le premier tour
 */
void Score::reset()
{
   for( int i = 0; i<78; i++ ) {
      plis[i] = HYPERSPACE;
   }

   gagne = false;
   
   chelemDeclare = false;
   chelemRealise = false;
   chelemDefense = false;

   petitAuBout = false;
   petitAttaque = false;
   
   poigneeDefense = false;
   poigneeAttaque = false;

   typePoigneeD = P_SANS;
   typePoigneeA = P_SANS;

   score_inf.pointsAFaire = 0;
   score_inf.difference = 0;
   score_inf.points_petit_au_bout = 0;
   score_inf.multiplicateur = 0;
   score_inf.points_poignee = 0;
   score_inf.points_chelem = 0;
   score_inf.points_defense = 0;
   score_inf.attaque = 0.0;
   score_inf.defense = 0.0;
   score_inf.bouts = 0;
}
/*****************************************************************************/
void Score::setPli( int i, Place p )
{
   if( i>=78 || i<0 ) {
      return;
   }

   plis[i] = p;
}
/*****************************************************************************/
void Score::setChelemDeclare( bool c )
{
   chelemDeclare = c;
}
/*****************************************************************************/
void Score::setExcuse( Place p )
{
   carteExcuse = p;
}
/*****************************************************************************/
Place Score::getExcuse()
{
    return(carteExcuse);
}
/*****************************************************************************/
int Score::getScore( int i )
{
   return(scores[i]);
}
/*****************************************************************************/
ScoreInfos *Score::getScoreInfos()
{
   return(&score_inf);
}
/*****************************************************************************/
/**
 * Calcule les points de la défense et de l'attaque
 */
void Score::calcul( Deck &mainDeck, Deck &deckChien, GameInfos *infos )
{
   int i;
   float n;
   Card *c;
   int flag=0;
   
   score_inf.bouts=0;
   score_inf.attaque = 0.0;
   score_inf.defense = 0.0;
   petitAuBout = false;
   petitAttaque = false;

#ifndef QT_NO_DEBUG
   QString filename = "plis_";
   
   char car;
   for( i=0; i<7; i++ ){
      car = qrand()%26 + 0x61;
      filename += car;
   }
   filename += ".txt";
   QFile out_file(filename);
   out_file.open(QIODevice::WriteOnly | QIODevice::Text);
   QTextStream f(&out_file);
#endif

   for( i=0; i<72; i++ ) {
      c = mainDeck.at( i );
      n = c->getPoints();

#ifndef QT_NO_DEBUG
   f << " Couleur : " << c->getColor();
   f << " Type : " << c->getType();
   f << " Valeur : " << c->getValue();
   f << " Owner : " << c->getOwner();
   f << " Pli : " << plis[i];
#endif

      if( c->getType() == EXCUSE ) {
         if( plis[i]== infos->preneur ) {
            if( carteExcuse == infos->preneur ) {
               score_inf.attaque += n;
               score_inf.bouts++;
            } else {
               score_inf.defense += n;
               flag = 1;
            }
         } else {
            if( carteExcuse != infos->preneur ) {
               score_inf.defense += n;
            } else {
               score_inf.attaque += n;
               flag = 1;
               score_inf.bouts++;
            }
         }
      } else {
         // il doit rendre une carte basse à la place de son excuse
         if( flag==1 && n==0.5 && plis[i]==carteExcuse ) {
            flag=2;
            if( carteExcuse == infos->preneur ) {
               score_inf.defense += 0.5;
            } else {
               score_inf.attaque += 0.5;
            }
         } else {
            if( plis[i] == infos->preneur ) {
               score_inf.attaque += n;
               if( c->getType() == ATOUT ) {
                  if( c->getValue() == 21 || c->getValue() == 1 ) {
                     score_inf.bouts++;
                  }
               }
            } else {
               score_inf.defense += n;
            }
         }
      }

#ifndef QT_NO_DEBUG
      f << " Attaque : " << score_inf.attaque;
      f << " Defense : " << score_inf.defense << endl;
#endif

   }


#ifndef QT_NO_DEBUG
   f << "*** Chien ***" << endl;
#endif

   // les points du chien
   n=0;
   for( i=0; i<6; i++ ) {
      c = deckChien.at( i );
      n += c->getPoints();

#ifndef QT_NO_DEBUG
      f << " Couleur : " << c->getColor();
      f << " Type : " << c->getType();
      f << " Valeur : " << c->getValue();
      f << " Owner : " << c->getOwner() << endl;
#endif

      // on compte les possibles bouts dans le chien
      if( infos->contrat == GARDE_SANS ) { // seul cas possible avec des bouts dans le Chien
         if( c->getType() == ATOUT ) {
            if( c->getValue() == 21 || c->getValue() == 1 )
               score_inf.bouts++;
         } else if( c->getType() == EXCUSE )
            score_inf.bouts++;       
      }
   }

   if( infos->contrat == GARDE_CONTRE ) {
      score_inf.defense += n;
   } else {
      score_inf.attaque += n;
   }

#ifndef QT_NO_DEBUG
   f << " Attaque : " << score_inf.attaque;
   f << " Defense : " << score_inf.defense << endl;
   out_file.close();
#endif

   // On teste le Chelem
   if( score_inf.attaque == 86.5 && score_inf.defense == 4.5 ) {
      // On arrondit, conformément aux règles du Tarot
      score_inf.attaque = 87.0;
      score_inf.defense = 4.0;
      chelemRealise = true;
      chelemDefense = false;

   } else if( score_inf.attaque == 4.5 && score_inf.defense == 86.5  ) {
      // On arrondit, conformément aux règles du Tarot
      score_inf.attaque = 4.0;
      score_inf.defense = 87.0;
      chelemRealise = true;
      chelemDefense = true;

   } else {
      chelemRealise = false;
      chelemDefense = false;
   }

   // recherche du petit au bout
   for( i=68; i<72; i++) {
      c = mainDeck.at( i );
      if( c->getType() == ATOUT && c->getValue() == 1 ) {
         petitAuBout = true;
         break;
      }
   }

   if( petitAuBout == true ) {
      // A qui appartient le petit au bout
      // On prend la dernière carte du pli
      if( plis[71] == infos->preneur ) {
         petitAttaque = true;
      }
   }

   //---------------------------
   // On connait les points des 
   // deux camps, on en déduit
   // donc les scores
   //---------------------------

   if(score_inf.bouts == 0 ) {
      score_inf.pointsAFaire = 56;
   } else if( score_inf.bouts == 1 ) {
      score_inf.pointsAFaire = 51;
   } else if( score_inf.bouts == 2 ) {
      score_inf.pointsAFaire = 41;
   } else {
      score_inf.pointsAFaire = 36;
   }

   if( infos->contrat == PRISE ) {
      score_inf.multiplicateur = 1;
   } else if( infos->contrat == GARDE ) {
      score_inf.multiplicateur = 2;
   } else if( infos->contrat == GARDE_SANS ) {
      score_inf.multiplicateur = 4;
   } else { // GARDE_CONTRE
      score_inf.multiplicateur = 6;
   }

   score_inf.difference = (int)(score_inf.attaque - score_inf.pointsAFaire);

   if( score_inf.difference >=0 ) {
      gagne=true;
   } else {
      gagne=false;
   }

   score_inf.difference = abs(score_inf.difference);

   if( petitAuBout == true ) {
      if( petitAttaque == true ) {
         score_inf.points_petit_au_bout = -10;
      } else {
         score_inf.points_petit_au_bout = 10;
      }
   } else {
      score_inf.points_petit_au_bout = 0;
   }

   // Les points des Poignées déclarées
   if( poigneeDefense == true ) {
      score_inf.points_poignee = 10+10*typePoigneeD;
      if( gagne == true ) {
         score_inf.points_poignee *= -1;   // Les points vont au camp gagnant
      }
   } else if( poigneeAttaque == true ) {
      score_inf.points_poignee = (-1)*(10+10*typePoigneeA);
      if( gagne == false ) {
         score_inf.points_poignee *= -1;   // Les points vont au camp gagnant
      }
   } else {
      score_inf.points_poignee = 0;
   }

   // Les points des Chelem déclarés
   if( chelemDeclare == true ) {
      if( chelemRealise == true ) {
         if( chelemDefense == true ) {
            score_inf.points_chelem = 200;
         } else {
            score_inf.points_chelem = -400;
         }
      } else {
         score_inf.points_chelem = 200;
      }
   } else if( chelemRealise == true ) {
      if( chelemDefense == true ) {
         score_inf.points_chelem = 200;
      } else {
         score_inf.points_chelem = -200;
      }
   } else {
      score_inf.points_chelem = 0;
   }

   // Le total de points pour chaque défenseur est donc de :
   score_inf.points_defense = (25 + abs(score_inf.difference) + score_inf.points_petit_au_bout )*score_inf.multiplicateur + 
                              score_inf.points_poignee + score_inf.points_chelem;

   // Le preneur a rempli son contrat, il prend donc les points à la défense
   if( gagne == true ) {
      score_inf.points_defense *= (-1);
   }

   for( i=0; i<infos->nbJoueurs; i++ ) {
      if( i == infos->preneur ) {
         scores[i] += score_inf.points_defense * (infos->nbJoueurs-1) * (-1);
      } else {
         scores[i] += score_inf.points_defense;
      }
   }
}

//=============================================================================
// End of file Score.h
//=============================================================================
