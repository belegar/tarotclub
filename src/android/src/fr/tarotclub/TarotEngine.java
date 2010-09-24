/*=============================================================================
 * TarotClub - TarotEngine.java
 *=============================================================================
 * Main Tarot process class, handle user events and sequence states
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

import java.util.HashMap;
import java.util.Map;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Point;
import android.graphics.RectF;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;


public class TarotEngine extends TarotView {
	// machine states	
	private static final int STATE_PLAY = 0;
	private static final int STATE_WAIT_SOUTH = 1;
	private static final int STATE_END_TURN = 2;
	
	// chien management
	private static final int CHIEN_ADD_CARD = 0;
	private static final int CHIEN_REMOVE_CARD = 1;
	
	// miscellaneous constants
	private static final int NB_HAND_CARDS = 18;
	private static final long DELAY_MS = 500;
	private static final int SOUTH_CARD_POS_Y = 239;
	private static final int SOUTH_CARD_SEL_Y = 220;

	private int state = STATE_PLAY;
	private int selId = -1;
	private float selX, selY;
	private Deck stack;  // main deck
	private HashMap<Integer, Integer> middle = new HashMap<Integer, Integer>(); // Place, id: middle cards played
	private Deck chien; // chien cards
	private Player[] mPlayers; // every player have his own deck
	private Game mGame = new Game();
	private GameHandler mGameHandler = new GameHandler(); 
	private static final Point[] coord = { 	new Point(126, 122),
		  									new Point(192, 60),
		  									new Point(126, 5),
		  									new Point(60, 60) };
	private static final RectF central = new RectF(60, 50, 280, 70+Card.HEIGHT); // chien area to drop the cards
	private static final RectF southCard = new RectF(126, 122, 126+Card.WIDTH, 122+Card.HEIGHT);
  
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
    		  	case STATE_WAIT_SOUTH:
    		  		return;
     		  
    		  	case STATE_PLAY:
    		  		mGame.next();
    		  		if (mGame.getTurn() == Game.SOUTH) {
    		  			state = STATE_WAIT_SOUTH;
    		  		} else {
    		  			manageCardGame(mPlayers[mGame.getTurn()].play(stack, cards, mGame.getGameCounter()));
    		  			if (mGame.isEndOfTurn() == true) {
    		  				state = STATE_END_TURN;
    		  			} else {
    		  				mGame.nextPlayer();
    		  			}
    		  		}
    		  		break;
    		  		
    		  	case STATE_END_TURN:
    		  		if (mGame.endOfTurn(stack, chien, cards) == true) {
    		  			// End of game, clean the board and show score
    		  			showResult(mGame.getInfos());
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
  public void manageChienDeck(int id, int action) {
	  if (action == CHIEN_ADD_CARD) {
		  if (chien.size()<6) {
			  chien.addCard(id);
			  mPlayers[Game.SOUTH].mDeck.removeCard(id);
		  }
  	  } else {
  		  chien.removeCard(id);
  		  mPlayers[Game.SOUTH].mDeck.addCard(id);
  	  }
  }
  /*****************************************************************************/
  public void southPlayCard(float x, float y) {
	  // south has selected a card
	  // the card is played if it is dropped on the south footprint
	  if (southCard.contains(x, y)) {		  
		  manageCardGame(selId);
		  if (mGame.isEndOfTurn() == true) {
			  state = STATE_END_TURN;
		  } else {
			  mGame.nextPlayer();
			  state = STATE_PLAY;
		  }
		  mGameHandler.sleep(DELAY_MS);
	  }
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
		  } else {
			  if (mGame.getContract() < Game.GARDE_SANS){
				  if (mGame.getTaker() == Game.SOUTH) {
					  // add chien cards to the player's deck
					  for (int i=0; i<chien.size(); i++) {
						  mPlayers[Game.SOUTH].mDeck.addCard(chien.getCard(i));
					  }
					  state = STATE_WAIT_SOUTH;
					  chien.clear();
					  mGame.setSequence(Game.CHIEN);
					  hideBidTexts();
					  update();
					  return;
				  } else {
					  mPlayers[mGame.getTaker()].choixChien(chien, cards);
				  }
			  }
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
	  int cardSpace;
	  drawBackground(canvas);
	  
	  if (mGame.getSequence() == Game.RESULT) {
		  return;
	  } else if (mGame.getSequence() == Game.CHIEN) {
		  drawChienArea(central, canvas);
	  } else {
		  for (int i=0; i<Game.NB_PLAYERS; i++) {
			  drawCardFootPrint(coord[i], canvas);
		  }
	  }
	  
	  // show player's cards
	  cardSpace = mPlayers[Game.SOUTH].mDeck.getSpace();
	  for (int i=0; i<mPlayers[Game.SOUTH].mDeck.size(); i++) {
		  int id = mPlayers[Game.SOUTH].mDeck.getCard(i);
		  if ((id >= 0) && (id<=77) && (id!=selId)) {
			  cards[id].setPosition(2+cardSpace*i, SOUTH_CARD_POS_Y);
			  DrawCard(canvas, cards[id]);
		  }
	  }
	  
	  if (mGame.getSequence() == Game.GAME) {
		// show already played cards
		  for (Map.Entry<Integer, Integer> e : middle.entrySet()) {
		  	  // cards of other players
			  cards[e.getValue()].setPosition(coord[e.getKey()]);
			  DrawCard(canvas, cards[e.getValue()]);
		  }
	  } else if (mGame.getSequence() == Game.CHIEN) {
		  // show cards of chien in the middle
		  cardSpace = chien.getSpace();
		  for (int i=0; i<chien.size(); i++) {
			  // chien under construction
			  int id = chien.getCard(i);
			  if ((id >= 0) && (id<=77) && (id!=selId)) {
				  cards[id].setPosition(70+cardSpace*i, 60);
				  DrawCard(canvas, cards[id]);
			  }
		  }
	  }

	  // center the selected card under the finger (in the low half to see the card, human fingers are big -_-)
	  if (selId>=0) {
		  cards[selId].setPosition(selX, selY);
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
	  
	  mPlayers[0].mDeck.setWidth(mScreenWidth);
	  chien.setWidth(200);
  }
  /*****************************************************************************/
  /**
   * return true if you want re-entry event 
   * return false if you only want a one-shot event
   */
  @Override
  public boolean onTouchEvent(MotionEvent event) {
    boolean ret = false;
    float x = event.getX();
    float y = event.getY();
    
    switch (event.getAction()) {
      
      case MotionEvent.ACTION_UP:
    	  if (selId >= 0) {
    		  if (mGame.getSequence() == Game.GAME) {
    			  southPlayCard(x, y);
    		  } else if (mGame.getSequence() == Game.CHIEN) {
    			  if (central.contains(x, y) == true) {
    				  if (chien.hasCard(selId) == false ) {
    					  manageChienDeck(selId, CHIEN_ADD_CARD);
    				  }
    			  } else if(chien.hasCard(selId)){
    				  manageChienDeck(selId, CHIEN_REMOVE_CARD);
    			  }
    		  }
    	  }
    	  selId = -1;
    	  clearText();
    	  TarotEngine.this.invalidate();
    	break;
      case MotionEvent.ACTION_CANCEL:
        break;
        
      case MotionEvent.ACTION_DOWN:
      case MotionEvent.ACTION_MOVE:
    	
		if (y>SOUTH_CARD_POS_Y) {
			selectCard(x, y);
		} else if (selId>=0) {
    		selX = x-Card.WIDTH/2;
    		selY = y-Card.HEIGHT/1.5f;
    		TarotEngine.this.invalidate();
		} else if (central.contains(x, y)) {
    		selectChienCard(x, y);
		}
		ret = true;
        break;
    }
    return ret;
  }
  /*****************************************************************************/
  // detect which card is under the finger
  private void selectChienCard(float x, float y) {  
	  if (mGame.getSequence() != Game.CHIEN)
		  return;
	  
	  int id = chien.getSelectedCard(x-central.left);
	  if (id >= 0) {
		  selId = id;
		  selX = x-Card.WIDTH/2;
		  selY = y-Card.HEIGHT/1.5f;
		  TarotEngine.this.invalidate();
	  }
  }
  /*****************************************************************************/
  // detect which card is under the finger
  private void selectCard(float x, float y) {
	  boolean ok = false;  
	  if (state != STATE_WAIT_SOUTH) {
		  return;
	  }
	  
	  int id = mPlayers[Game.SOUTH].mDeck.getSelectedCard(x);
	  if (id >= 0) {
		  Card c = cards[id];

		  // test if the card can be played
		  if (mGame.getSequence() == Game.GAME) {  
			  ok = mPlayers[Game.SOUTH].canPlayCard(stack, cards, c, mGame.getGameCounter());
		  } else if (mGame.getSequence() == Game.CHIEN) {
			  if ((c.getSuit() != Card.ATOUT) &&
				  (c.getSuit() != Card.EXCUSE) &&
				  (c.getValue() < 14))
			  {
				  ok = true;
			  }
		  } else {
			  return;
		  }
		  
		  if (ok == true) {
			  selId = id;
			  selX = c.getX();
			  selY = SOUTH_CARD_SEL_Y;
			  TarotEngine.this.invalidate();
			  return;
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
			  if (c == 77) {
				  mGame.setExcuse(i);
			  }
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
	clearText();
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

//=============================================================================
//End of file TarotEngine.java
//=============================================================================
