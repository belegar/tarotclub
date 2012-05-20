/*=============================================================================
 * TarotClub - Game.java
 *=============================================================================
 * Store all game variables
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
	public static final int RESULT = 4;
	
	// poignées
	public static final int SIMPLE = 0;
	public static final int DOUBLE = 0;
	public static final int TRIPLE = 0;
	
	// misc
	public static final int NB_PLAYERS = 4;
	
	private int turn;
	private int sequence;      // indique la séquence de jeu actuelle
	private Infos infos;
	private int dealer;
	private int gameCounter;
	private Score score;
	private Random mRnd = new Random();
	
	Game() {
		infos = new Infos();
		score = new Score();
		sequence = IDLE;
	}
	
	public void setSequence(int seq) { sequence = seq; }
	public void setExcuse(int place) { score.setExcuse(place); }
	/*****************************************************************************/
	public int getSequence() { return sequence; }
	public int getTurn() { return turn; }
	public int getContract() { return infos.contract; }
	public int getTaker() { return infos.taker; }
	public int getGameCounter() { return gameCounter; }
	public Infos getInfos() { return infos; }
	
	/*****************************************************************************/
	void start() {
		sequence = ENCHERES;
		// dealer is the first player to tell the bid
		dealer = mRnd.nextInt(4);
		turn = dealer;
		infos.contract = Game.PASSE;
	}
	/*****************************************************************************/
	void beginRound() {
		sequence = GAME;
		gameCounter = 0;
		score.reset();
		turn = dealer;
	}
	/*****************************************************************************/
	/**
	 * return false if bid sequence is finished, otherwise true
	 */
	boolean sequenceBids(int bid)
	{
		// record player's bid
		if (bid > infos.contract) {
			infos.contract = bid;
			infos.taker = turn;
		}
		nextPlayer();
		if (turn == dealer) {
	      return false;
		}
		return true;
	}
	/*****************************************************************************/
	void nextPlayer() {
		turn = ((++turn)%4);
	}
	/*****************************************************************************/
	boolean isEndOfTurn() {
		if(((gameCounter%Game.NB_PLAYERS) == 0) && (gameCounter != 0)) {
			return true;
		} else {
			return false;
		}
	}
	/*****************************************************************************/
	/**
	 * return true if the turn is finished, otherwise false
	 */
	void next() {
		gameCounter++;
	}
	/*****************************************************************************/
	/**
	 * Fin d'un tour, on calcule le gagnant du pli et on prépare la suite
	 * On retourne également le nombre de points réalisés par le preneur
	 * retourne true si la partie est terminée, sinon false
	 */
	boolean endOfTurn(Deck stack, Deck chien, Card[] cards)
	{
	   int i;
	   // On détecte qui a remporté le pli, c'est lui qui entame le prochain tour
	   turn = calculateTurnWinner(stack, cards);

	   for (i = (gameCounter-NB_PLAYERS); i<gameCounter; i++ ) {
	      score.setPli(i, turn);
	   }
	   if (gameCounter < 72) {
	      return false;
	   } else { // fin du jeu
	      score.calcul(stack, chien, cards, infos);
	      sequence = RESULT;
	      return true;
	   }
	}
	/*****************************************************************************/
	/**
	 * Calculation of the winner of this turn. Return: winner place
	 */
	int calculateTurnWinner(Deck stack, Card[] cards)
	{
	   Card c;
	   int type;
	   int pl = NORTH;

	   int i, debut = 0;
	   int leader = 0; // 0 est le premier joueur qui a joué, 1 le second, etc.
	   // par défaut, celui qui entame est le leader (car couleur demandée)

	   // étape 1 : on cherche la couleur demandée

	   c = cards[stack.getCard(gameCounter - NB_PLAYERS)]; // première carte jouée à ce tour

	   type = c.getSuit();

	   // aïe, le joueur a commencé avec une excuse
	   if( type == Card.EXCUSE ) {
	      c = cards[stack.getCard(gameCounter - NB_PLAYERS + 1)]; // la couleur demandée est donc la seconde carte
	      debut = 1;
	      type = c.getSuit();
	      leader = 2;
	   }

	   debut = debut + gameCounter - NB_PLAYERS + 1 ; // le début est décalé si le premier pli est l'excuse

	   int  drapeau; // indique une coupe, donc le leader est forcément un atout
	   int  valLeader;
	   valLeader = c.getValue();

	   if( type == Card.ATOUT ) {
	      drapeau = 1;   // on cherche une éventuelle surcoupe
	   } else {
	      drapeau = 0;   // on cherche une coupe
	   }

	   for( i = debut; i< gameCounter; i++) {
	      c = cards[stack.getCard(i)];

	      // coupe !
	      if( c.getSuit() == Card.ATOUT ) {
	         if( c.getValue() > valLeader && drapeau == 1 ) { // surcoupe ??
	            valLeader = c.getValue();
	            leader = i - (gameCounter - NB_PLAYERS);
	         } else if( drapeau == 0 ) {
	            valLeader = c.getValue();
	            leader = i - (gameCounter - NB_PLAYERS);
	            drapeau = 1;
	         }

	      // pas de coupe détectée
	      } else if( c.getSuit() < Card.ATOUT && drapeau == 0 ) {
	         if( c.getSuit() == type) { // ahah, même couleur !
	            if( c.getValue() > valLeader ) {
	               valLeader = c.getValue();
	               leader = i - (gameCounter - NB_PLAYERS);
	            }
	         }
	      }
	   }

	   // place du leader
	   if( turn == SOUTH) {
	      int tab[] = {EAST, NORTH, WEST, SOUTH};
	      pl = tab[leader];
	   } else if( turn == EAST) {
		   int tab[] = {NORTH, WEST, SOUTH, EAST};
	      pl = tab[leader];
	   } else if( turn == NORTH) {
		   int tab[] = {WEST, SOUTH, EAST, NORTH};
	      pl = tab[leader];
	   } else {
		   int tab[] = {SOUTH, EAST, NORTH, WEST};
	      pl = tab[leader];
	   }

	   return pl;
	}
	
	// end of class
}

//=============================================================================
//End of file Game.java
//=============================================================================
