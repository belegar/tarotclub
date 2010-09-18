/*=============================================================================
 * TarotClub - Score.java
 *=============================================================================
 * Calculation of the game points
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

package fr.tarotclub;


public class Score
{
	int[] plis;      	// indique quelle carte a été remportée par quel joueur
	boolean  gagne;     // vrai si le coup est gagné par l'attaque
	int carteExcuse;   	// A qui appartient l'excuse

	// Informations sur le petit au bout
	boolean  petitAuBout;   // vrai si le petit a été mené au bout
	boolean  petitAttaque;  // vrai si le petit au bout a été remporté par l'attaque
	// Informations sur le Chelem
	boolean  chelemDeclare; // vrai si un Chelem a été déclaré
	boolean  chelemRealise; // vrai si le Chelem a été réalisé
	boolean  chelemDefense; // vrai si le Chelem a été réalisé par la défense
	// Informations sur les poignées
	boolean  poigneeDefense;   // vrai si la défense a déclaré une poignée
	boolean  poigneeAttaque;   // vrai si l'attaque a déclaré une poignée
	
	int  typePoigneeD;  // (Défense) simple, double ou triple
	int  typePoigneeA;  // (Attaque) simple, double ou triple
		
	Score() {
		plis = new int[78];
	}
	/*****************************************************************************/
	void reset() {
	   for(int i = 0; i<78; i++) {
	      plis[i] = -1;
	   }

	   gagne = false;

	   chelemDeclare = false;
	   chelemRealise = false;
	   chelemDefense = false;

	   petitAuBout = false;
	   petitAttaque = false;

	   poigneeDefense = false;
	   poigneeAttaque = false;

	   typePoigneeD = -1;
	   typePoigneeA = -1;
	}
	/*****************************************************************************/
	void setExcuse(int place) {
		carteExcuse = place;
	}
	/*****************************************************************************/
	void setPli(int i, int place) {
		if( i>77 || i<0 ) {
	      return;
		}
	   plis[i] = place;
	}
	/*****************************************************************************/
	/**
	 * Calcule les points de la défense et de l'attaque
	 */
	void calcul(Deck stack, Deck chien, Card[] cards, Infos infos)
	{
	   int i;
	   float n;
	   Card c;
	   int flag = 0;

	   infos.bouts = 0;
	   infos.attaque = 0.0f;
	   infos.defense = 0.0f;
	   petitAuBout = false;
	   petitAttaque = false;

	   for( i=0; i<72; i++ ) {
	      c = cards[stack.getCard(i)];
	      n = c.getPoints();

	      if( c.getSuit() == Card.EXCUSE ) {
	         if( plis[i] == infos.taker ) {
	            if( carteExcuse == infos.taker ) {
	            	infos.attaque += n;
	            	infos.bouts++;
	            } else {
	            	infos.defense += n;
	               flag = 1;
	            }
	         } else {
	            if( carteExcuse != infos.taker ) {
	            	infos.defense += n;
	            } else {
	               infos.attaque += n;
	               flag = 1;
	               infos.bouts++;
	            }
	         }
	      } else {
	         // il doit rendre une carte basse à la place de son excuse
	         if((flag == 1) && (n == 0.5f) && (plis[i] == carteExcuse)) {
	            flag = 2;
	            if( carteExcuse == infos.taker ) {
	            	infos.defense += 0.5f;
	            } else {
	            	infos.attaque += 0.5f;
	            }
	         } else {
	            if (plis[i] == infos.taker) {
	            	infos.attaque += n;
	               if( c.getSuit() == Card.ATOUT ) {
	                  if((c.getValue() == 21) || (c.getValue() == 1)) {
	                	  infos.bouts++;
	                  }
	               }
	            } else {
	            	infos.defense += n;
	            }
	         }
	      }
	   }

	   // les points du chien
	   n = 0.0f;
	   for( i=0; i<6; i++ ) {
	      c = cards[chien.getCard(i)];
	      n += c.getPoints();

	      // on compte les possibles bouts dans le chien
	      if( infos.contract == Game.GARDE_SANS ) { // seul cas possible avec des bouts dans le Chien
	         if( c.getSuit() == Card.ATOUT ) {
	            if ((c.getValue() == 21) || (c.getValue() == 1))
	            	infos.bouts++;
	         } else if( c.getSuit() == Card.EXCUSE )
	        	 infos.bouts++;
	      }
	   }

	   if( infos.contract == Game.GARDE_CONTRE ) {
		   infos.defense += n;
	   } else {
		   infos.attaque += n;
	   }

	   // On teste le Chelem
	   if ((infos.attaque == 86.5f) && (infos.defense == 4.5f)) {
	      // On arrondit, conformément aux règles du Tarot
		   infos.attaque = 87.0f;
		   infos.defense = 4.0f;
	      chelemRealise = true;
	      chelemDefense = false;

	   } else if ((infos.attaque == 4.5f) && (infos.defense == 86.5f)) {
	      // On arrondit, conformément aux règles du Tarot
		   infos.attaque = 4.0f;
		   infos.defense = 87.0f;
	      chelemRealise = true;
	      chelemDefense = true;
	   } else {
	      chelemRealise = false;
	      chelemDefense = false;
	   }

	   // recherche du petit au bout
	   for( i=68; i<72; i++) {
	      c = cards[stack.getCard(i)];
	      if ((c.getSuit() == Card.ATOUT) && (c.getValue() == 1)) {
	         petitAuBout = true;
	         break;
	      }
	   }

	   if( petitAuBout == true ) {
	      // A qui appartient le petit au bout
	      // On prend la dernière carte du pli
	      if( plis[71] == infos.taker ) {
	         petitAttaque = true;
	      }
	   }

	   //---------------------------
	   // On connait les points des
	   // deux camps, on en déduit
	   // donc les scores
	   //---------------------------

	   if(infos.bouts == 0 ) {
		   infos.pointsAFaire = 56;
	   } else if( infos.bouts == 1 ) {
		   infos.pointsAFaire = 51;
	   } else if( infos.bouts == 2 ) {
		   infos.pointsAFaire = 41;
	   } else {
		   infos.pointsAFaire = 36;
	   }

	   if( infos.contract == Game.PRISE ) {
		   infos.multiplicateur = 1;
	   } else if( infos.contract == Game.GARDE ) {
		   infos.multiplicateur = 2;
	   } else if( infos.contract == Game.GARDE_SANS ) {
		   infos.multiplicateur = 4;
	   } else { // GARDE_CONTRE
		   infos.multiplicateur = 6;
	   }

	   infos.difference = (int)(infos.attaque - infos.pointsAFaire);

	   if( infos.difference >=0 ) {
	      gagne = true;
	   } else {
	      gagne = false;
	   }

	   infos.difference = Math.abs(infos.difference);

	   if( petitAuBout == true ) {
	      if( petitAttaque == true ) {
	         infos.points_petit_au_bout = -10;
	      } else {
	         infos.points_petit_au_bout = 10;
	      }
	   } else {
	      infos.points_petit_au_bout = 0;
	   }

	   // Les points des Poignées déclarées
	   if( poigneeDefense == true ) {
	      infos.points_poignee = 10+10*typePoigneeD;
	      if( gagne == true ) {
	         infos.points_poignee *= -1;   // Les points vont au camp gagnant
	      }
	   } else if( poigneeAttaque == true ) {
	      infos.points_poignee = (-1)*(10+10*typePoigneeA);
	      if( gagne == false ) {
	         infos.points_poignee *= -1;   // Les points vont au camp gagnant
	      }
	   } else {
	      infos.points_poignee = 0;
	   }

	   // Les points des Chelem déclarés
	   if( chelemDeclare == true ) {
	      if( chelemRealise == true ) {
	         if( chelemDefense == true ) {
	            infos.points_chelem = 200;
	         } else {
	            infos.points_chelem = -400;
	         }
	      } else {
	         infos.points_chelem = 200;
	      }
	   } else if( chelemRealise == true ) {
	      if( chelemDefense == true ) {
	         infos.points_chelem = 200;
	      } else {
	         infos.points_chelem = -200;
	      }
	   } else {
	      infos.points_chelem = 0;
	   }

	   // Le total de points pour chaque défenseur est donc de :
	   infos.points_defense = (25 + Math.abs(infos.difference) + infos.points_petit_au_bout )*infos.multiplicateur +
	                              infos.points_poignee + infos.points_chelem;

	   // Le preneur a rempli son contrat, il prend donc les points à la défense
	   if( gagne == true ) {
	      infos.points_defense *= (-1);
	   }
	}	
	// End of class
}

//=============================================================================
//End of file Score.java
//=============================================================================
