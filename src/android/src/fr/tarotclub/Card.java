/* License here! */

package fr.tarotclub;

class Card {
  // Suits
  public static final int CLUBS = 0;
  public static final int DIAMONDS = 1;
  public static final int SPADES = 2;
  public static final int HEARTS = 3;
  public static final int ATOUT = 4;
  public static final int EXCUSE = 5;

  public static int WIDTH = 61;
  public static int HEIGHT = 112;

  private int mValue;
  private int mSuit;
  private float mPoints;
  
  private float mX;
  private float mY;

  public Card(int value, int suit, float points) {
    mValue = value;
    mSuit = suit;
    mPoints = points;
    mX = 1;
    mY = 1;
  }

  public float GetX() { return mX; }
  public float GetY() { return mY; }
  public int GetValue() { return mValue; }
  public int GetSuit() { return mSuit; }
  public float Getpoints() { return mPoints; }

  public void SetPosition(float x, float y) {
    mX = x;
    mY = y;
  }

  public void MovePosition(float dx, float dy) {
    mX -= dx;
    mY -= dy;
  }
}


