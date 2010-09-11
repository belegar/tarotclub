
/* License here! */

package fr.tarotclub;

import java.util.Collections;
import java.util.List;

public class Deck {

  private List<Card> mCards;

  public Deck() {
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

  public void Shuffle() {
	  Collections.shuffle(mCards);
  }
}
