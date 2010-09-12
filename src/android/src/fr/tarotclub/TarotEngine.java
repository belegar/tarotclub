/* License here! */

package fr.tarotclub;

import android.content.Context;
import android.graphics.Canvas;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.view.MotionEvent;


class TarotEngine extends TarotView {
  // Places
  public static final int SOUTH = 0;
  public static final int EAST = 1;
  public static final int NORTH = 2;
  public static final int WEST = 3;
  
  // variables
  public static final int NB_HAND_CARDS = 18;

  private Card[] cards;
  private Deck deck;  // main deck
  private Deck chien; // chien cards
  private Player[] mPlayers; // every player have his own deck
  
  private RefreshHandler mRedrawHandler = new RefreshHandler();
  
  /*****************************************************************************/
  /**
   * Create a simple handler that we can use to cause animation to happen.  We
   * set ourselves as a target and we can use the sleep()
   * function to cause an update/invalidate to occur at a later date.
   */
  class RefreshHandler extends Handler {

      @Override
      public void handleMessage(Message msg) {
    	  TarotEngine.this.update();
    	  TarotEngine.this.invalidate();
      }

      public void sleep(long delayMillis) {
      	this.removeMessages(0);
          sendMessageDelayed(obtainMessage(0), delayMillis);
      }
  };
  
  /*****************************************************************************/
  @Override
  public void onDraw(Canvas canvas) {
	  drawBackground(canvas);
	  drawCardFootPrint(60, 60, canvas); //west
	  drawCardFootPrint(192, 60, canvas); //east
	  drawCardFootPrint(126, 5, canvas); //north
	  drawCardFootPrint(126, 122, canvas); //south
	  
	  // show player's cards
	  for (int i=0; i<mPlayers[TarotEngine.SOUTH].mDeck.size(); i++) {
		  int id = mPlayers[TarotEngine.SOUTH].mDeck.getCard(i);
		  if ((id >= 0) && (id<=77)) {
			  cards[id].setPosition(30+20*i, 239);
			  DrawCard(canvas, cards[id]);
		  }
	  }
  }
  /*****************************************************************************/
  /**
   * Handles the basic update loop, checking to see if we are in the running
   * state, determining if a move should be made, updating the snake's location.
   */
  public void update() { 
	  mPlayers[TarotEngine.SOUTH].mDeck.tidy();
	  //mRedrawHandler.sleep(200);
	  TarotEngine.this.invalidate();
  }
  /*****************************************************************************/
  public TarotEngine(Context context, AttributeSet attrs) {
	  super(context, attrs);
	  
	  cards = new Card[78];
	  
	  mPlayers = new Player[4];
	  deck = new Deck();
	  chien = new Deck();
	  
	  mPlayers[0] = new Player("Sud", SOUTH);
	  mPlayers[1] = new Player("Est", EAST);
	  mPlayers[2] = new Player("Ouest", NORTH);
	  mPlayers[3] = new Player("Nord", WEST);
	  Init();
  }
  /*****************************************************************************/
  @Override
  public boolean onTouchEvent(MotionEvent event) {
    boolean ret = false;

    switch (event.getAction()) {
      case MotionEvent.ACTION_DOWN:
    	selectCard(event.getX(), event.getY());
        break;
      case MotionEvent.ACTION_UP:
      case MotionEvent.ACTION_CANCEL:
        break;
      case MotionEvent.ACTION_MOVE:
        break;
    }
    return ret;
  }
  /*****************************************************************************/
  // detect which card is under the finger
  private void selectCard(float x, float y) {
	  for (int i=0; i<mPlayers[TarotEngine.SOUTH].mDeck.size(); i++) {
		  int id = mPlayers[TarotEngine.SOUTH].mDeck.getCard(i);
		  if ((id >= 0) && (id<=77)) {
			  Card c = cards[id];
			  if (y >= c.getY()) {
				  if ((x >= c.getX()) && (x <= (c.getX()+Card.WIDTH))) {
					  cards[id].movePosition(0, -20);
					  TarotEngine.this.invalidate();
				  }
			  }
		  }
	  }
  }
  /*****************************************************************************/
  public void deal() {
	  deck.clear();
	  chien.clear();

	  // init main deck
	  for (int i=0; i<78; i++) {
		  deck.addCard(i);
	  }
  
	  // shuffle cards and send to players
	  deck.shuffle();
	  for (int i=0; i<4; i++) {
		  int n = i*NB_HAND_CARDS;
		  mPlayers[i].mDeck.clear();
		  for (int j=0; j<NB_HAND_CARDS; j++) {
			  int c = deck.getCard(n+j);
			  mPlayers[i].mDeck.addCard(c);
		  }
	  }
	  // rest of the cards go to the chien
	  for (int i=0; i<6; i++) {
		  int c = deck.getCard(72+i);
		  chien.addCard(c);
	  }
	  
	  deck.clear();
  }
  /*****************************************************************************/
  public void newGame() {
	deal();
	update();
  } 
  /*****************************************************************************/
  // init cards for the game
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
			cards[id+j] = new Card(j+1, i, points, id+j);
			
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
		cards[i] = new Card(i+1, Card.ATOUT, points, i);
	}
	// Excuse card
	cards[77] = new Card(1, Card.EXCUSE, 4.5f, 77);
  }
}


