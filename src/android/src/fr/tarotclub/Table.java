/* License here! */

package fr.tarotclub;

import java.util.List;

class Table {
  // Places
  public static final int SOUTH = 0;
  public static final int EAST = 1;
  public static final int NORTH = 2;
  public static final int WEST = 3;

  private Deck deck;
  private Deck chien;
  private List<Player> mPlayers;
  
  public Table() {
	  mPlayers.add(new Player("Sud", SOUTH));
	  mPlayers.add(new Player("Est", EAST));
	  mPlayers.add(new Player("Ouest", NORTH));
	  mPlayers.add(new Player("Nord", WEST));
	  Init();
  }
  
  // init cards for the round
  public Init() {
	// create normal cards
	for (int i=0; i<4; i++) {
		// 14 cards per suit (1-10 + joker, knight, queen, king)
		for (int j=0; j<14; j++) {
			float points;
			if (j == 11) {
				points = 1.5;
			} else if (j == 12) {
				points = 2.5;
			} else if (j == 13) {
				points = 3.5;
			} else if (j == 14) {
				points = 4.5;
			} else {
				points = 0.5;
			}
			Card card = new Card(j+1, i, points);
			deck.addCard(card);
		}
	}
	// atouts cards
	for (int i=0; i<21; i++) {
		float points;
		if (i == 0 || i == 21) {
			points = 4.5;
		} else {
			points = 0.5;
		}
		Card card = new Card(i+1, Card.ATOUT, points);
		deck.addCard(card);
	}
	// Excuse card
	Card card = new Card(1, Card.EXCUSE, 4.5);
	deck.addCard(card);
  }
}


