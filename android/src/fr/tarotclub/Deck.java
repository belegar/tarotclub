/*=============================================================================
 * TarotClub - Deck.java
 *=============================================================================
 * Model a deck of cards
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

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

public class Deck {

	private int mWidth = 0; // width to draw the deck
	public ArrayList<Integer> mCards;

  /*****************************************************************************/
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
  /*****************************************************************************/
  public Deck() {
	  mCards = new ArrayList<Integer>();
  }
  /*****************************************************************************/
  public void setWidth(int width) {
	  mWidth = width;
  }
  /*****************************************************************************/
  /**
   * return the id of the card under the x coordinate
   */
  int getSelectedCard(float x) {
	  int pos = (int)(x/getSpace());
	  if ((pos>=0) && (pos<mCards.size())) {
		  return mCards.get(pos);
	  } else {
		  return -1;
	  }
  }
  /*****************************************************************************/
  int getSpace() {
	  int size = mCards.size();
	  if (size<2)
		  return Card.WIDTH;
	  float space = (mWidth-Card.WIDTH)/(size-1); 
	  return (int)(space+0.5f);
  }
  /*****************************************************************************/
  public boolean hasCard(int id) {
	 return mCards.contains(Integer.valueOf(id)); 
  }
  /*****************************************************************************/
  public int getCard(int idx) {
    return mCards.get(idx).intValue();
  }
  /*****************************************************************************/
  public void addCard(int id) {
    mCards.add(Integer.valueOf(id));
  }
  /*****************************************************************************/
  public boolean removeCard(int id) {
    return mCards.remove(Integer.valueOf(id));
  }
  /*****************************************************************************/
  public void shuffle() {
	  Collections.shuffle(mCards);
  }
  /*****************************************************************************/
  public void tidy() {
	  Collections.sort(mCards, comparator);
  }
  /*****************************************************************************/
  public int size() {
	  return mCards.size();
  }
  /*****************************************************************************/
  public void clear() {
	  mCards.clear();
  }
}

//=============================================================================
//End of file Deck.java
//=============================================================================
