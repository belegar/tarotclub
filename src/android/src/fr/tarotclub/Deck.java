
/* License here! */

package fr.tarotclub;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

public class Deck {

  public ArrayList<Integer> mCards;

  Comparator<Integer> comparator = new Comparator<Integer>() {
	  @Override
	  public int compare(Integer obj1, Integer obj2) {
		  if (obj1.intValue() < obj2.intValue())
			  return 1;
		  else if (obj1.intValue() > obj2.intValue())
			  return -1;
		  else
			  return 0;
	  }
  };
  
  public Deck() {
	  mCards = new ArrayList<Integer>();
  }

  public int getCardById(int id) {
	int c = -1;
	for (int i=0; i<mCards.size(); i++) {
		c = mCards.get(i).intValue();
		if (c == id)
			return c;
    }
	return c;
  }
  
  public int getCard(int idx) {
    return mCards.get(idx).intValue();
  }
  
  public void addCard(int id) {
    mCards.add(Integer.valueOf(id));
  }
  
  public boolean removeCard(int id) {
    return mCards.remove(Integer.valueOf(id));
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
