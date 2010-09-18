/* License here! */

package fr.tarotclub;

import android.graphics.Point;

class Card {
  // Suits
  public static final int SPADES = 0;
  public static final int HEARTS = 1;
  public static final int CLUBS = 2;
  public static final int DIAMONDS = 3;
  public static final int ATOUT = 4;
  public static final int EXCUSE = 5;

  public static int WIDTH = 61;
  public static int HEIGHT = 112;

  private int mValue;
  private int mSuit;
  private float mPoints;
  private int mId;
  
  private float mX;
  private float mY;

  public Card(int value, int suit, float points, int id) {
    mValue = value;
    mSuit = suit;
    mPoints = points;
    mId = id;
    mX = 1;
    mY = 1;
  }

  public float getX() { return mX; }
  public float getY() { return mY; }
  public int getValue() { return mValue; }
  public int getSuit() { return mSuit; }
  public int getId() { return mId;}
  public float getPoints() { return mPoints; }

  public void setPosition(Point point) {
	  mX = point.x;
	  mY = point.y;
  }
  
  public void setPosition(float x, float y) {
    mX = x;
    mY = y;
  }

  public void movePosition(float dx, float dy) {
    mX -= dx;
    mY -= dy;
  }
}


