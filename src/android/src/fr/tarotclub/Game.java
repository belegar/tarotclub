
package fr.tarotclub;

import java.util.Random;

public class Game {
	// Places
	public static final int SOUTH = 0;
  	public static final int EAST = 1;
  	public static final int NORTH = 2;
  	public static final int WEST = 3;
	
	// contrats
	public static final int PASSE = 0;
	public static final int PRISE = 1;
	public static final int GARDE = 2;
	public static final int GARDE_SANS = 3;
	public static final int GARDE_CONTRE = 4;
	
	// sequence
	public static final int IDLE = 0;
	public static final int ENCHERES = 1;
	public static final int CHIEN = 2;
	public static final int GAME = 3;
	
	// misc
	public static final int NB_PLAYERS = 4;
	
	private int turn;
	private int sequence;      // indique la séquence de jeu actuelle
	private int contract;
	private int dealer;
	private int taker;
	private int gameCounter;
	private Random mRnd = new Random();
	
	Game() {
		sequence = IDLE;
	}
	
	public int getSequence() { return sequence; }
	public int getTurn() { return turn; }
	public int getContract() { return contract; }
	public int getTaker() { return taker; }
	
	/*****************************************************************************/
	void start() {
		sequence = ENCHERES;
		// dealer is the first player to tell the bid
		dealer = mRnd.nextInt(4);
		turn = dealer;
		contract = Game.PASSE;
	}
	/*****************************************************************************/
	void beginRound() {
		sequence = GAME;
		turn = dealer;
	}
	/*****************************************************************************/
	/**
	 * return false if bid sequence is finished, otherwise true
	 */
	boolean sequenceBids(int bid)
	{
		// record player's bid
		if (bid > contract) {
			contract = bid;
			taker = turn;
		}
		turn = nextPlayer(turn);
		if( turn == dealer ) {
	      return false;
		}
		return true;
	}
	/*****************************************************************************/
	int nextPlayer(int t) {
		return ((++t)%4);
	}
	
}

// End of file
