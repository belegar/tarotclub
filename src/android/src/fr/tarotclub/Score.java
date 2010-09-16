
/* License here! */

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
	
	// Game information
	float attaque;
	float defense;
	int   bouts;
	int   pointsAFaire;
	int   difference;
	int   points_petit_au_bout;
	int   multiplicateur;
	int   points_poignee;
	int   points_chelem;
	int   points_defense;
	
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
	void calcul(Deck stack, Deck chien, Card[] cards)
	{
		/*
	   int i;
	   float n;
	   Card c;
	   int flag = 0;

	   bouts = 0;
	   attaque = 0.0f;
	   defense = 0.0f;
	   petitAuBout = false;
	   petitAttaque = false;

	   for( i=0; i<72; i++ ) {
	      c = mainDeck.at( i );
	      n = c->getPoints();

	      if( c->getType() == EXCUSE ) {
	         if( plis[i] == infos.preneur ) {
	            if( carteExcuse == infos.preneur ) {
	               score_inf.attaque += n;
	               score_inf.bouts++;
	            } else {
	               score_inf.defense += n;
	               flag = 1;
	            }
	         } else {
	            if( carteExcuse != infos.preneur ) {
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
	            if( carteExcuse == infos.preneur ) {
	               score_inf.defense += 0.5;
	            } else {
	               score_inf.attaque += 0.5;
	            }
	         } else {
	            if( plis[i] == infos.preneur ) {
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
	   }


	   // les points du chien
	   n=0;
	   for( i=0; i<6; i++ ) {
	      c = deckChien.at( i );
	      n += c->getPoints();

	      // on compte les possibles bouts dans le chien
	      if( infos.contrat == GARDE_SANS ) { // seul cas possible avec des bouts dans le Chien
	         if( c->getType() == ATOUT ) {
	            if( c->getValue() == 21 || c->getValue() == 1 )
	               score_inf.bouts++;
	         } else if( c->getType() == EXCUSE )
	            score_inf.bouts++;
	      }
	   }

	   if( infos.contrat == GARDE_CONTRE ) {
	      score_inf.defense += n;
	   } else {
	      score_inf.attaque += n;
	   }

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
	      if( plis[71] == infos.preneur ) {
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

	   if( infos.contrat == PRISE ) {
	      score_inf.multiplicateur = 1;
	   } else if( infos.contrat == GARDE ) {
	      score_inf.multiplicateur = 2;
	   } else if( infos.contrat == GARDE_SANS ) {
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

	   setPoints(infos);
	   */
	}
	
	// End of class
}

// End of file
