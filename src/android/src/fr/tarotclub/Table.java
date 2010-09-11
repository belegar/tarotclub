/* License here! */

package fr.tarotclub;


class Table {
  // Places
  public static final int SOUTH = 0;
  public static final int EAST = 1;
  public static final int NORTH = 2;
  public static final int WEST = 3;

  private Deck deck;  // main deck, never empty
  private Deck stack; // stack of cards played turing this turn
  private Deck chien; // chien cards
  private Player[] mPlayers; // every player have his own deck
  
  public Table() {
	  mPlayers = new Player[4];
	  deck = new Deck();
	  stack = new Deck();
	  chien = new Deck();
	  
	  mPlayers[0] = new Player("Sud", SOUTH);
	  mPlayers[1] = new Player("Est", EAST);
	  mPlayers[2] = new Player("Ouest", NORTH);
	  mPlayers[3] = new Player("Nord", WEST);
	  Init();
  }
  
  Card getCard(int idx) {
	  return deck.getCard(idx);
  }
  
  // init cards for the round
  public void Init() {
	int id;
	// create normal cards
	for (int i=0; i<4; i++) {
		if (i == Card.SPADES) {
			id = 0;
		} else if (i == Card.HEARTS) {
			id = 14;
		} else if (i == Card.CLUBS) {
			id = 28;
		} else {
			id = 42;
		}
		
		// 14 cards per suit (1-10 + joker, knight, queen, king)
		for (int j=0; j<14; j++) {
			float points;
			if (j == 11) {
				points = 1.5f;
			} else if (j == 12) {
				points = 2.5f;
			} else if (j == 13) {
				points = 3.5f;
			} else if (j == 14) {
				points = 4.5f;
			} else {
				points = 0.5f;
			}
			Card card = new Card(j+1, i, points, id+j);
			deck.addCard(card);
		}
	}
	// atouts cards
	for (int i=56; i<77; i++) {
		float points;
		if (i == 56 || i == 76) {
			points = 4.5f;
		} else {
			points = 0.5f;
		}
		Card card = new Card(i+1, Card.ATOUT, points, i);
		deck.addCard(card);
	}
	// Excuse card
	Card card = new Card(1, Card.EXCUSE, 4.5f, 77);
	deck.addCard(card);
  }
}


