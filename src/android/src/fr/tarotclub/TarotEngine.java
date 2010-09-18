/* License here! */

package fr.tarotclub;

import java.util.HashMap;
import java.util.Map;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Point;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;


public class TarotEngine extends TarotView {
	// machine states	
	private static final int STATE_PLAY = 0;
	private static final int STATE_END_TURN = 1;
	
	// variables
	private static final int NB_HAND_CARDS = 18;
	private static final long DELAY_MS = 500;

	private int state = STATE_PLAY;
	private int selId = -1;
	private float selX, selY;
	private Deck stack;  // main deck
	private HashMap<Integer, Integer> middle = new HashMap<Integer, Integer>(); // Place, id
	private Deck chien; // chien cards
	private Player[] mPlayers; // every player have his own deck
	private Game mGame = new Game();
	private GameHandler mGameHandler = new GameHandler(); 
	private Point[] coord = { new Point(126, 122),
		  					new Point(192, 60),
		  					new Point(126, 5),
		  					new Point(60, 60) };
  
  /*****************************************************************************/
  /**
   * Main game engine, a quick and dirty state machine
   */
  class GameHandler extends Handler {
      @Override
      public void handleMessage(Message msg) {
    	  if (mGame.getSequence() == Game.ENCHERES) {
    		  if (mGame.getTurn() == Game.SOUTH) {
    			  showBidButtons(mGame.getContract());
    		  } else {
    			 manageBids(mPlayers[mGame.getTurn()].calculEnchere());
    		  }
    	  } else if (mGame.getSequence() == Game.GAME) {
    		  if (mGame.getGameCounter() == 0) {
    			  //cleanups
    			  hideBidTexts();
    		  }
    		  
    		  switch (state) {
    		  	case STATE_PLAY:
    		  		mGame.next();
    		  	//  if (mGame.getTurn() == Game.SOUTH) {
    		  		manageCardGame(mPlayers[mGame.getTurn()].play(stack, cards, mGame.getGameCounter()));
    		  		if (mGame.isEndOfTurn() == true) {
    		  			state = STATE_END_TURN;
    		  		} else {
    		  			mGame.nextPlayer();
    		  		}
    		  		break;
    		  	case STATE_END_TURN:
    		  		if (mGame.endOfTurn(stack, chien, cards) == true) {
    		  			// End of game, show score
    		  			//TODO !!
    		  		} else {
    		  			
    		  			state = STATE_PLAY;
    		  		}
    		  		middle.clear();
    		  		break;
    		  
    		  }
    		  TarotEngine.this.invalidate();
		      mGameHandler.sleep(DELAY_MS);
    	  }
      }

      public void sleep(long delayMillis) {
    	  this.removeMessages(0);
          sendMessageDelayed(obtainMessage(0), delayMillis);
      }
  }
  /*****************************************************************************/
  /**
   * id is the identification of the card played
   */
  public void manageCardGame(int id) {
	  stack.addCard(id);
	  mPlayers[mGame.getTurn()].mDeck.removeCard(id);
	  middle.put(new Integer(mGame.getTurn()), new Integer(id));
  }
  /*****************************************************************************/
  public void manageBids(int bid) {
	  if (bid<= mGame.getContract())
		  bid = Game.PASSE;
	  
	  showBid(mGame.getTurn(), bid);
	  hideBidButtons();
	  if (mGame.sequenceBids(bid) == false) {
		  // restart if all players passed, otherwise start game
		  if (mGame.getContract() == Game.PASSE) {
			  newGame();
		  } else if (mGame.getContract() > Game.GARDE){
			  // give the chien to the attack or the defense			  
		  } else {
			// TODO: if south, graphical chien process
			  mPlayers[mGame.getTaker()].choixChien(chien, cards);
			  mGame.beginRound();
			  mGameHandler.sleep(DELAY_MS);
		  }
	  } else {
		  mGameHandler.sleep(DELAY_MS);
	  }
  }
  /*****************************************************************************/
  @Override
  public void onDraw(Canvas canvas) {
	  drawBackground(canvas);
	  
	  for (int i=0; i<Game.NB_PLAYERS; i++) {
		  drawCardFootPrint(coord[i], canvas);
	  }
	  
//	  drawChienArea(270, 5, canvas);
	  
	  // show player's cards
	  for (int i=0; i<mPlayers[Game.SOUTH].mDeck.size(); i++) {
		  int id = mPlayers[Game.SOUTH].mDeck.getCard(i);
		  if ((id >= 0) && (id<=77) && (id!=selId)) {
			  cards[id].setPosition(20+22*i, 239);
			  DrawCard(canvas, cards[id]);
		  }
	  }
	  
	  // show already played cards
	  for (Map.Entry<Integer, Integer> e : middle.entrySet()) {
		  cards[e.getValue()].setPosition(coord[e.getKey()]);
		  DrawCard(canvas, cards[e.getValue()]);
	  }

	  // center the selected card under the finger (in the low half to see the card, human fingers are big -_-)
	  if (selId>=0) {
		  cards[selId].setPosition(selX-Card.WIDTH/2, selY-Card.HEIGHT/1.5f);
		  DrawCard(canvas, cards[selId]);
	  }
  }
  /*****************************************************************************/
  public void update() { 
	  mPlayers[Game.SOUTH].mDeck.tidy();
	  TarotEngine.this.invalidate();
  }
  /*****************************************************************************/
  public TarotEngine(Context context, AttributeSet attrs) {
	  super(context, attrs);

	  mPlayers = new Player[4];
	  stack = new Deck();
	  chien = new Deck();
	  
	  mPlayers[0] = new Player();
	  mPlayers[1] = new Player();
	  mPlayers[2] = new Player();
	  mPlayers[3] = new Player();
  }
  /*****************************************************************************/
  @Override
  public boolean onTouchEvent(MotionEvent event) {
    boolean ret = false;

    switch (event.getAction()) {
      case MotionEvent.ACTION_DOWN:
    	selectCard(event.getX(), event.getY());
    	ret = true;
        break;
      case MotionEvent.ACTION_UP:
    	selId = -1;
    	TarotEngine.this.invalidate();
    	break;
      case MotionEvent.ACTION_CANCEL:
        break;
      case MotionEvent.ACTION_MOVE:
    	if (selId>=0) {
    		selX = event.getX();
    		selY = event.getY();
    		TarotEngine.this.invalidate();
    		ret = true;
    	}
        break;
    }
    return ret;
  }
  /*****************************************************************************/
  // detect which card is under the finger
  private void selectCard(float x, float y) {
	  for (int i=0; i<mPlayers[Game.SOUTH].mDeck.size(); i++) {
		  int id = mPlayers[Game.SOUTH].mDeck.getCard(i);
		  if ((id >= 0) && (id<=77)) {
			  Card c = cards[id];
			  if (y >= c.getY()) {
				  if ((x >= c.getX()) && (x <= (c.getX()+Card.WIDTH))) {
					  selId = id;
					  selX = x;
					  selY = y;
					  TarotEngine.this.invalidate();
				  }
			  }
		  }
	  }
  }
  /*****************************************************************************/
  public void deal() {
	  stack.clear();
	  chien.clear();

	  // init main deck
	  for (int i=0; i<78; i++) {
		  stack.addCard(i);
	  }
  
	  // shuffle cards and send to players
	  stack.shuffle();
	  for (int i=0; i<4; i++) {
		  int n = i*NB_HAND_CARDS;
		  mPlayers[i].mDeck.clear();
		  for (int j=0; j<NB_HAND_CARDS; j++) {
			  int c = stack.getCard(n+j);
			  mPlayers[i].mDeck.addCard(c);
		  }
		  mPlayers[i].updateStats(cards);
	  }
	  // rest of the cards go to the chien
	  for (int i=0; i<6; i++) {
		  int c = stack.getCard(72+i);
		  chien.addCard(c);
	  }
	  
	  stack.clear();
  }
  /*****************************************************************************/
  public void newGame() {
	deal();
	update();
	hideBidTexts();
	mGame.start();
	state = STATE_PLAY;
	mGameHandler.sleep(DELAY_MS);
  } 
  /*****************************************************************************/
  public void setupHandlers() {
	buttonPasse.setOnClickListener(new OnClickListener() {
		public void onClick(View v) {
			manageBids(Game.PASSE);
		}
	 });
	buttonPrise.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				manageBids(Game.PRISE);
			}
		});
	buttonGarde.setOnClickListener(new OnClickListener() {
		    public void onClick(View v) {
		    	manageBids(Game.GARDE);
		    }
		});	  
	buttonGardeSans.setOnClickListener(new OnClickListener() {
		    public void onClick(View v) {
		    	manageBids(Game.GARDE_SANS);
		    }
		});
	buttonGardeContre.setOnClickListener(new OnClickListener() {
		    public void onClick(View v) {
		    	manageBids(Game.GARDE_CONTRE);
		    }
	});
  }
 
  // End of class
}

 
// End of file
