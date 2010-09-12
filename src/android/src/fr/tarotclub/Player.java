
/* License here! */

package fr.tarotclub;

public class Player {

  private String mName = new String();
  private int mPlace;
  
  // public because useless to make it private
  public Deck mDeck = new Deck();

  public Player(String name, int place) {
	  mName = name;
	  mPlace = place;
  }
}
