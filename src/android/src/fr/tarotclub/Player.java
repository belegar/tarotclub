
/* License here! */

package fr.tarotclub;


public class Player {
  private Stats stats = new Stats(); 
  // public because too much pain if private
  public Deck mDeck = new Deck();

  /*****************************************************************************/
  public Player() {

  }
  /*****************************************************************************/
  /**
   * Update statistics by analyzing the deck
   */
  void updateStats(Card[] cards){
     int i, k, val, count, flag, longue, coul;
     Card c;

     stats.raz();
     count = 0; // compteur
     flag = 0;  // indicateur de petit

     // recherche des atouts
     for( i=0; i<mDeck.size(); i++) {
    	c = cards[mDeck.getCard(i)];
        if( c.getSuit() == Card.ATOUT ) {
           stats.atouts++;
           val = c.getValue();
           if( val >= 15 ) {
              stats.atoutsMajeurs++; // atout majeur
           }
           if( val == 21 ) {
              stats.vingtEtUn = true;
              stats.bouts++;
           }
           if( val == 1 ) {
              stats.petit = true;
              stats.bouts++;
           }
        }

        if( c.getSuit() == Card.EXCUSE ) {
           stats.atouts++;
           stats.bouts++;
           stats.excuse = true;
        }
     }

     int[] distr = new int[14]; // teste une distribution

     // on s'occupe maintenant des couleurs
     for( i=0; i<4; i++ ) {
        if( i==0 ) {
           coul = Card.SPADES;
        } else if( i==1 ) {
           coul = Card.HEARTS;
        } else if( i==2 ) {
           coul = Card.CLUBS;
        } else {
           coul = Card.DIAMONDS;
        }

        for( k=0; k<14; k++ ) {
           distr[k] = 0;
        }
        count = 0;

        for( k=0; k<mDeck.size(); k++ ) {
        	c = cards[mDeck.getCard(i)];
           if( c.getSuit() == coul ) {
              count++;
              val = c.getValue();
              distr[val-1] = 1;
              if( val == 11 ) {
                 stats.valets++; // valet
              }
              if( val == 12 ) {
                 stats.cavaliers++; // cavalier
              }
           }
        }

        if( count == 1 ) {
           stats.singletons++;     // singleton
        }
        if( count == 0 ) {
           stats.coupes++;         // coupe
        }

        // Nombre de cartes dans chaque couleur
        if( i==0 ) {
           stats.pics = count;
        } else if( i==1 ) {
           stats.coeurs = count;
        } else if( i==2 ) {
           stats.trefles = count;
        } else {
           stats.carreaux = count;
        }

        if( distr[13] == 1 && distr[12] == 1 ) {
           stats.mariages++; // mariage (dame+roi)
        }
        if( distr[13] == 1 ) {
           stats.rois++;     // roi sans dame
        }
        if( distr[12] == 1 ) {
           stats.dames++;    // dame sans roi
        }

        // test des séquences :
        count = 0;  // longueur de la séquence
        flag = 0;   // couleur trouvée : on est dans la séquence
        longue = 0;

        for( k=0; k<14; k++ ) {
           if( distr[k] == 1 ) {
              longue++;
              // début d'une séquence
              if( flag == 0 ) {
                 flag = 1;
                 count++;
              } else {
                 count++;
              }
           } else if( flag == 1 ) {
              if( count >= 5 ) {
                 stats.sequences++;
              }
              count = 0;
              flag = 0;
           }
        }
        if( longue >=5 ) {
           stats.longues++;
        }
     }
  }
 
     /*****************************************************************************/
     /**
      * Bid calculation, return the bid of the player
      */
     int calculEnchere()
     {
        int total = 0;
        int cont;

        // On distribue des points en fonction des stats
        if( stats.vingtEtUn == true ) {
           total += 9;
        }
        if( stats.excuse == true ) {
           total += 7;
        }
        if( stats.petit == true ) {
           if( stats.atouts == 5 ) {
              total += 5;
           } else if( stats.atouts == 6 || stats.atouts == 7 ) {
              total += 7;
           } else if( stats.atouts > 7 ) {
              total += 8;
           }
        }

        // Chaque atout vaut deux points
        // Chaque atout majeur vaut 1 point supplémentaire
        total += stats.atouts * 2;
        total += stats.atoutsMajeurs * 2;
        total += stats.rois * 6;
        total += stats.dames * 3;
        total += stats.cavaliers * 2;
        total += stats.valets;
        total += stats.mariages;
        total += stats.longues * 5;
        total += stats.coupes * 5;
        total += stats.singletons * 3;
        total += stats.sequences * 4;

        // on décide de l'enchère :
        if( total <= 35 ) {
           cont = Game.PASSE;
        } else if( total >= 36  && total <= 50 ) {
           cont = Game.PRISE;
        } else if( total >= 51  && total <= 65 ) {
           cont = Game.GARDE;
        } else if( total >= 66  && total <= 75 ) {
           cont = Game.GARDE_SANS;
        } else {
           cont = Game.GARDE_CONTRE;
        }
        return cont;
     }

}
  
class Stats {
		
   public int   atouts;  // nombres d'atouts , en comptant les bouts et l'excuse
   public int   bouts;   // 0, 1, 2 ou 3
   public int   atoutsMajeurs; // atouts >= 15

   public int   rois;
   public int   dames;
   public int   cavaliers;
   public int   valets;

   public int   mariages;   // nombre de mariages dans la main
   public int   longues;
   public int   coupes;     // aucune carte dans une couleur
   public int   singletons; // une seule carte dans une couleur
   public int   sequences;  // cartes qui se suivent (au moins 5 cartes pour être comptées)
   public int   trefles;
   public int   pics;
   public int   coeurs;
   public int   carreaux;
   
   public boolean  petit;
   public boolean  vingtEtUn;
   public boolean  excuse;

   Stats() {
	   raz();
   }
   
   void raz() {
    atouts = 0;
	bouts = 0;
	atoutsMajeurs = 0;
	
	rois = 0;
	dames = 0;
	cavaliers = 0;
	valets = 0;
	
	mariages = 0;
	longues = 0;
	coupes = 0;
	singletons = 0;
	sequences = 0;
	
	trefles = 0;
	pics = 0;
	coeurs = 0;
	carreaux = 0;
	
	petit = false;
	vingtEtUn = false;
	excuse = false;	   
   }
}
