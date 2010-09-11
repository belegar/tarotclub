
/* License here! */

package fr.tarotclub;

import java.util.ArrayList;
import java.util.Collections;

public class Deck {

  private ArrayList<Card> mCards;

  public Deck() {
	  mCards = new ArrayList<Card>();
  }

  public void addCard(Card card) {
    mCards.add(card);
  }

  public Card getCard(int index) {
    if ( (index < mCards.size()) && (mCards.size()!=0) ) {
      return mCards.get(index);
    }
    return null;
  }
  
  public boolean removeCard(Card card) {
    return mCards.remove(card);
  }

  public void shuffle() {
	  Collections.shuffle(mCards);
  }
  
  public int size() {
	  return mCards.size();
  }
}
