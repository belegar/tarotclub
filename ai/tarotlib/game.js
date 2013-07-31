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
 * This class gather a list of cards
 */
var Game = function() {
    this.initialize();
};

var p = Game.prototype;

// ****************************************************************************
// PUBLIC PROPERTIES
// ****************************************************************************
    p.players = null;

    p.NUMBER_OF_PLAYERS = 4;


    // Game variables
    p.contract = TarotLib.Contract.PASS;

    p.taker = TarotLib.Place.SOUTH;

    p.trickCounter = 0;		// number of turns, 18 with 4 players

    p.currentPosition = 0; 	// position of the current turn [0..3] with 4 players

    // Bot variables
    p.myPlace = TarotLib.Place.SOUTH;

    p.myDeck = null;

    // Variables of the current turn
    p.trickSuit = TarotLib.Suits.SPADES;

    p.firstCardValue = 0;

    p.startPosition = 0;

    p.startedWithExcuse = false;;
	
// ****************************************************************************
// PRIVATE PROPERTIES
// ****************************************************************************

// ****************************************************************************
// CONSTRUCTOR
// ****************************************************************************
	/**
	 * Initialization method.
	 */
    p.initialize = function() {
        var i;

        // Player statistics
        this.players = new Array(this.NUMBER_OF_PLAYERS);
        for(i=0; i<this.players.length; i++) {
            this.players[i] = new TarotLib.Player(i); // position of players is assigned here
            this.players[i].initialize();
        }

        this.playedCards = new Array(18); // turns for 4 players
        for(i=0; i<18; i++) {
            this.playedCards[i] = new TarotLib.Deck();
        }

        this.myDeck = new TarotLib.Deck();
	};

// ****************************************************************************
// PUBLIC STATIC METHODS
// ****************************************************************************

    p.detectMissedColor = function(place)
    {
        if (this.currentPosition === 0) {
            this.startedWithExcuse = false;
            // First played card is the color to play, except in case of excuse (0-A)
            this.trickSuit = this.playedCards[this.trickCounter].cards[0].color;
            this.firstCardValue = this.playedCards[this.trickCounter].cards[0].value;

            // special case of excuse
            if ((this.trickSuit === "A") && (this.firstCardValue === "0")) {
                this.startedWithExcuse = true;
            }
        } else if ((this.currentPosition === 1) && (this.startedWithExcuse === true)) {
            this.trickSuit = this.playedCards[this.trickCounter].cards[1].color;
        } else {
            if (this.playedCards[this.trickCounter].cards[this.currentPosition].color !== this.trickSuit) {
                this.players[place].setMissedColor(this.trickSuit);
            }
        }
    };

    /**
     * @brief This method is call at the end of each trick to analyze played cards
     */
    p.analyzeTrick = function(place)
    {
        this.detectMissedColor(place);
    };

    p.newGame = function()
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

    // Print players statistics and information
    p.printPlayers = function()
    {
        for (var j=0; j<this.players.length; j++) {
            this.players[j].print();
        }
    };

    p.setPlayedCard = function(cardName, place)
    {
        this.playedCards[this.trickCounter].addOneCard(cardName, place);

        this.analyzeTrick(place);

        this.currentPosition++;
        if (this.currentPosition >= 4) {
            this.currentPosition = 0;
            this.trickCounter++;

            // TODO: detect who won the trick
        }
    };

    p.PlayDefenseStrategy = function()
    {
        // Just play the first card available ... (let the game engine take a valid card if necessary)
        return this.myDeck.cards[0].getName();
    };

    p.playAttackStrategy = function()
    {
        return this.playDefenseStrategy();
    };

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


TarotLib.Game = Game;
}());
// End of file
