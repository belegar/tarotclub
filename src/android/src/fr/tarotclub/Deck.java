
/* License here! */

package fr.tarotclub;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

public class Deck {

  private ArrayList<Card> mCards;

  Comparator<Card> comparator = new Comparator<Card>() {
	  @Override
	  public int compare(Card obj1, Card obj2) {
		  if (obj1.getId() < obj2.getId())
			  return 1;
		  else if (obj1.getId() > obj2.getId())
			  return -1;
		  else
			  return 0;
	  }
  };
  
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
  
  public Card getCardById(int id) {
	for (int i=0; i<mCards.size(); i++) {
		Card c = mCards.get(i);
		if (c.getId() == id)
			return c;
    }
    return null;
  }
  
  public boolean removeCard(Card card) {
    return mCards.remove(card);
  }

  public void shuffle() {
	  Collections.shuffle(mCards);
  }
  
  public void tidy() {
	  Collections.sort(mCards, comparator);
  }
  
  public int size() {
	  return mCards.size();
  }
  
  public void clear() {
	  mCards.clear();
  }
}
