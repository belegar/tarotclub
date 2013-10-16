/*=============================================================================
 * TarotClub - game.js
 *=============================================================================
 * Tarot game Javascript class
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * TarotClub is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotClub is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */

// namespace
this.TarotLib = this.TarotLib||{};

(function() {

/**
 * Game class
 *
 * MManage all game states and events, record all played cards
 * The Game class also contains statistics on other players
 */
var Game = function() {
    this.ctor();
};

var p = Game.prototype;

// ****************************************************************************
// PUBLIC STATIC PROPERTIES
// ****************************************************************************
    p.NUMBER_OF_PLAYERS = 4;

// ****************************************************************************
// PUBLIC PROPERTIES
// ****************************************************************************
	
    // Game state variables
    this.contract = "";
    this.taker = 0;
    this.trickCounter = 0;		// number of turns, 18 with 4 players
    this.currentPosition = 0; 	// position of the current turn [0..3] with 4 players
	this.playedCards = null;
	this.players = null;		// all the players, including the bot

	this.botPlace = 0;

    // Variables of the current turn
    this.trickSuit = 0;
    this.firstCardValue = 0;
    this.startedWithExcuse = false;;

// ****************************************************************************
// CONSTRUCTOR
// ****************************************************************************
    p.ctor = function()
	{
        var i;

		this.players = new Array(p.NUMBER_OF_PLAYERS);
        // Player statistics
        for(i=0; i<this.players.length; i++) {
            this.players[i] = new TarotLib.Player(i);	// assigned place is passed in argument
        }
		
		this.playedCards = new Array(18); // turns for 4 players;
        for(i=0; i<18; i++) {
            this.playedCards[i] = new TarotLib.Deck();
        }
    };

// ****************************************************************************
// PUBLIC STATIC METHODS
// ****************************************************************************
	/**
	 * @brief Reinitialize internal state variables before starting a new deal
	 */
	p.initialize = function()
    {
        var i;
        for(i=0; i<this.players.length; i++) {
            this.players[i].initialize();
        }

        this.trickCounter = 0;
        this.currentPosition = 0;

        for(i=0; i<this.playedCards.length; i++) {
            this.playedCards[i].clear();
        }
    };

    p.calculateBid = function()
    {
    	return this.players[this.botPlace].calculateBid();
    };

    p.setBotCards = function(cards)
    {
    	this.players[this.botPlace].deck.setCards(cards);
    };

	// Print players statistics and information
    p.printPlayers = function()
    {
        for (var j=0; j<this.players.length; j++)
        {
            this.players[j].print();
        }
    };

    p.printBot = function()
    {
    	this.players[this.botPlace].deck.print();
    };
	
    p.detectMissedColor = function(place)
    {
        if (this.currentPosition === 0) {
            this.startedWithExcuse = false;
            // First played card is the color to play, except in case of excuse (0-T)
            this.trickSuit = this.playedCards[this.trickCounter].cards[0].suit;
            this.firstCardValue = this.playedCards[this.trickCounter].cards[0].value;

            // special case of excuse
            if ((this.trickSuit === "T") && (this.firstCardValue === "0")) {
                this.startedWithExcuse = true;
            }
        } else if ((this.currentPosition === 1) && (this.startedWithExcuse === true)) {
            this.trickSuit = this.playedCards[this.trickCounter].cards[1].suit;
        } else {
            if (this.playedCards[this.trickCounter].cards[this.currentPosition].suit !== this.trickSuit) {
                this.players[place].setMissedColor(this.trickSuit);
            }
        }
    };

    p.setPlayedCard = function(cardName, place)
    {
        this.playedCards[this.trickCounter].addOneCard(cardName, place);

        this.detectMissedColor(place);

        this.currentPosition++;
        if (this.currentPosition >= 4) {
            this.currentPosition = 0;
            this.trickCounter++;

            // TODO: detect who won the trick
        }
    };

	/**
		Defense strategy, for a basic AI, is only to keep high value cards for its team and trying to make the attacker cut
		Limitations:
		  - No signalisation management
		  - Limited memory on previous tricks
		  - No dog analysis
		*/
    p.playDefenseStrategy = function()
    {
		var playedCard = undefined;
		this.players[this.botPlace].updateStats();
		
		if (this.currentPosition == 0)
		{
			// if we have a long suit, we start with it
			playedCard = this.players[this.botPlace].playLongSuit();
			if (playedCard == undefined)
			{
				playedCard = this.players[this.botPlace].playLowestCard();
			}

			if (playedCard == undefined)
			{
				systemPrint("Problem here!");
			}
		}
		/*
		
		Algorithm:
			1. If first player
		          PlayFirst()
			2. If DefensePosition = before
				If (defense may be leader)
				   play high card
				else
				   play low card				  
				
			3. If DefensePosition = after
				if AttackLeader() == true
				    CanPlayHigher()  (return true if the bot can play a higher card than the attacker)
						PlayHigherCard
			    else
				     GivePointsToDefense !!!
		   
		PlayFirst() (Entame)
		  Si main faible, jouer dans la courte, commencer par la carte la plus petite
		  Si main forte, jouer dans la longue, par la carte la plus faible
		  Ne pas jouer Atout, sauf si le bot ne peut pas faire autrement. Alors, jouer tout sauf le petit.
		  
		  PlayAttackerCut()
		
		Utility functions:
		  - GetPosition() ==> before or after the taker (fond, milieu, devant, fond = preneur joue en dernier)
		  - Ajouter aux statistiques les points par couleur
		
		*/
	
        if (playedCard == undefined)
		{
			systemPrint("Undefined card played");
		}
        return playedCard;
    };

    p.playAttackStrategy = function()
    {
        return this.playDefenseStrategy();
    };
	
	/*
    p.getRandomCard = function(place)
    {
        var index = 0;
        for (var i=0; this.myDeck.cards.length; i++) {
            if (this.myDeck.cards[i] !== undefined) {
                if (this.isValid(this.myDeck.cards[i].getName()) === true) {
                    index = i;
                }
            }
        }
        return this.myDeck.cards.takeCard(index);
    };

    p.isValid = function(card)
    {
        // FIXME
        return true;
    };
	*/
	
TarotLib.Game = Game;
}());
// End of file
