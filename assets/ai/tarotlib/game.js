/*=============================================================================
 * TarotClub - game.js
 *=============================================================================
 * Tarot game Javascript class, contains the strategy functions
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
this.TarotLib = this.TarotLib || {};

(function() {

/**
 * @brief Game class
 *
 * Manages all game states and events, record all played cards
 * The Game class also contains statistics on other players
 */
var Game = function() {
    var i;

    // all the players, including the bot
    this.players = new Array(p.NUMBER_OF_PLAYERS);
    // Player statistics
    for(i=0; i<p.NUMBER_OF_PLAYERS; i++) {
        this.players[i] = new TarotLib.Player(i);	// assigned place is passed in argument
    }

    this.playedCards = new Array(18); // turns for 4 players;
    for(i=0; i<18; i++) {
        this.playedCards[i] = new TarotLib.Deck();
    }

    this.bot = new TarotLib.Bot();

    // Game state variables
    this.contract = "";
    this.taker = 0;
    this.trickCounter = 0;
    this.currentPosition = 0;
    this.botPlace = 0;

    // Variables of the current turn
    this.trickSuit = 0;
    this.firstCardValue = 0;
    this.firstPlayer = 0;
    this.startedWithExcuse = false;
};

var p = Game.prototype;

// ****************************************************************************
// PUBLIC PROPERTIES
// ****************************************************************************
    p.NUMBER_OF_PLAYERS = 4;

// ****************************************************************************
// PUBLIC METHODS
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

    p.setBotCards = function(cards)
    {
    	this.bot.deck.setCards(cards);
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
    	this.bot.deck.print();
    };

    p.beforeAttack = function()
    {
    	var botRelativePlace = ((this.botPlace + 4) - this.firstPlayer) % 4;
        var takerRelativePlace = ((this.taker + 4) - this.firstPlayer) % 4;
    	if (botRelativePlace < takerRelativePlace)
    	{
            return true;
    	}
    	else
    	{
            return false;
    	}
    };

    p.isPlayed = function(suit, value)
    {
        var ret = false;

        for (var i=0; i<this.playedCards[this.trickCounter].size(); i++)
        {
            var card = this.playedCards[this.trickCounter].get(i);
            if ((card.suit === suit) &&
                (card.value === value))
            {
                ret = true;
            }
        }
        return ret;
    };

    /**
     * @brief Detect the highest trump of the current trick
     */
    p.highestTrump = function()
    {
        return this.playedCards[this.trickCounter].highestTrump();
    };

    /**
     * @brief Detect the highest suit of the current trick
     */
    p.highestSuit = function()
    {
        return this.playedCards[this.trickCounter].highestSuit(this.trickSuit);
    };

    /**
     * @brief Detect the leader of the current trick
     * @return The leader is found, Card class format, otherwise return undefined
     */
    p.detectLeader = function()
    {
        // Each trick is won by the highest trump in it, or the highest card
        // of the suit led if no trumps were played.
        var leader = this.highestTrump();
        if (leader === undefined)
        {
            leader = this.highestSuit();
        }
        if (leader === undefined)
        {
            systemPrint("leader cannot be undefined!");
        }
        return leader;
    };
	
	/**
	 * @brief Return true if the player has cut (whatever is the value, he may have overcut or played low trump)
	 * @param The taker played suit
	 */
	p.takerHasCut = function(takerPlayedSuit)
	{
		var hasCut = false;
		if ((this.trickSuit !== "T") && (takerPlayedSuit === "T"))
		{
			hasCut = true;
		}
		return hasCut;
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
                this.players[place].setMissedSuit(this.trickSuit);
            }
        }
    };

    p.setPlayedCard = function(cardName, place)
    {
        this.playedCards[this.trickCounter].addOneCard(cardName, place);
        this.detectMissedColor(place);

        if (this.currentPosition === 0)
        {
            this.firstPlayer = place;   
        }

        if (place === this.botPlace)
        {
            // Remove card from the bot's deck
            this.bot.deck.removeCard(cardName);
        }

        this.currentPosition++;
        if (this.currentPosition >= 4) {
            this.currentPosition = 0;
            this.trickCounter++;
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
		this.bot.updateStats();
		
		// We are the first player
		if ((this.currentPosition === 0) ||
            ((this.startedWithExcuse === true) && (this.currentPosition === 1)))
		{
			/**
			 * Si le preneur a une coupe, jouer dans la coupe, sinon:
             * Si main faible, jouer dans la courte, commencer par la carte la plus petite
		  	 * Si main forte, jouer dans la longue, par la carte la plus faible
		  	 * Ne pas jouer Atout, sauf si le bot ne peut pas faire autrement. Alors, jouer tout sauf le petit.
			 */

            // Make the taker cut
            if (this.players[this.taker].hasMissedSuit())
            {
                for (var i=0; i<5; i++)
                {
                    if (this.players[this.taker].hasSuits[i] === true)
                    {
                        playedCard = this.bot.playLowestCard(TarotLib.Suit.toString(i));
						systemPrint("defense: taker has a missed suit, make it cut!");
                        break;
                    }
                }
            }
            else
            {
    			playedCard = this.bot.playLongSuit();
    			if (playedCard === undefined)
    			{
    				playedCard = this.bot.playLowestCard();
    			}

    			if (playedCard === undefined)
    			{
    				systemPrint("Problem here!");
    			}
            }
		}
		else
		{
            // We have to play the suit color previously played
            if (this.beforeAttack())
            {
                // look if we have the suit
                if (this.bot.hasSuit(this.trickSuit))
                {
                    // play a low card in the required suit
                    playedCard = this.bot.playLowestCard(this.trickSuit);
                }
                else
                {
                    if (this.bot.hasSuit(TarotLib.Suit.TRUMPS))
                    {
                        // We need to cut, with a low trump since we are playing before the taker
                        // If no cuts, the fool is played
                        playedCard = this.bot.playHighestCard("T", 0);       
                    }
                    else
                    {
                        // Whatever, play any low card
                        playedCard = this.bot.playLowestCard();
    					systemPrint("defense: play a low card before the attack");
                    }
                }
            }
            else
            {
				// We are playing after the attack
                var leaderCard = this.detectLeader();
                if (leaderCard.owner === this.taker)
				// ==> The Taker IS the leader!!
                {
                    // look if we have the suit requested
                    if (this.bot.hasSuit(this.trickSuit))
                    {
                        if (this.trickSuit === "T")
                        {
                            // We have to play higher than any trumps played, if we can!!
                            var card = this.highestTrump();
                            if (this.bot.hasHigherCard("T", card.value))
                            {
                                playedCard = this.bot.playHighestCard("T", card.value);
								systemPrint("defense: play higher trump, just over than the taker :)");								
                            }
                            else
                            {
                                // Try to play whatever trump, except the little
                                playedCard = this.bot.playLowestCard("T", 2);    
                            }
                        }
                        else
                        {
							if (this.takerHasCut(leaderCard.suit))
							// ==> Taker is the leader and has cut
							{
								playedCard = this.bot.playLowestCard(this.trickSuit);
								systemPrint("defense: taker has cut, play low card");
							}
							else
							{
								// Can we play an higher card than the taker?
								var card = this.highestSuit();
								if (this.bot.hasHigherCard(this.trickSuit, card.value))
								{
									// We can play higher than the taker! Give points to the defense :)
									playedCard = this.bot.playHighestCard(this.trickSuit);
								}
								else
								{
									playedCard = this.bot.playLowestCard(this.trickSuit);
								}
							}
                        }
                    }
                    else
                    {
                        if (this.bot.hasSuit(TarotLib.Suit.TRUMPS))
                        {
                            // We have to play higher than any trumps played, if we can!!
                            var card = this.highestTrump();

                            if (card === undefined)
                            {
                                // We are the first to cut, try to save the little
                                playedCard = this.bot.playLowestCard("T", 1);
                            }
                            else if (this.bot.hasHigherCard("T", card.value))
                            {
                                playedCard = this.bot.playHighestCard("T", card.value);
                                systemPrint("defense: play higher trump, just over than the taker :)");                             
                            }
                            else
                            {
                                // Try to play whatever trump, except the little
                                playedCard = this.bot.playLowestCard("T", 1);
                            }
                        }
                        else
                        {
                            // Whatever, play any low card
                            playedCard = this.bot.playLowestCard();
                            systemPrint("defense: taker is the leader, play a low card after him");
                        }
                    }
                }
                else
				// ==> Taker is NOT the leader
                {
                    // look if we have the suit requested
                    if (this.bot.hasSuit(this.trickSuit))
                    {
                        // We have the suit requested, give points to the defense!!

                        // Try to play the little trunk if possible
                        if (this.trickSuit === "T")
                        {
                            // We have to play higher than any trumps played, if we can!!
                            var card = this.highestTrump();
                            if (this.bot.hasHigherCard("T", card.value))
                            {
                                playedCard = this.bot.playLowestCard("T", card.value);    
                            }
                            else
                            {
                                // Try to play the little trump to save it, otherwise the fool will be taken
                                playedCard = this.bot.playLowestCard("T", 1);       
                            }
                        }
                        else
                        {
                            playedCard = this.bot.playHighestCard(this.trickSuit);    
                        }
                    }
                    else
                    {
                        // We don't have the suit requested!
                        if (this.bot.hasSuit(TarotLib.Suit.TRUMPS))
                        {
                            // We need to cut, with a low trump since we are playing after the taker
                            // If no cuts, the fool is played
                            playedCard = this.bot.playLowestCard("T", 1);       
                        }
                        else
                        {
                            // We have no any trumps, let's give some points to the defense!
                            playedCard = this.bot.playHighestCard();
                        }
                    }

                    if (playedCard === undefined)
                    {
                        systemPrint("Problem here!");
                    }
                }
            }
		}
	
        if (playedCard === undefined)
		{
			systemPrint("Undefined card played");
		}
        return playedCard;
    };

    /*
        Basic attack strategy
    */
    p.playAttackStrategy = function()
    {
        var playedCard = undefined;
		this.bot.updateStats();
    
        // We are the first player
		if ((this.currentPosition === 0) ||
            ((this.startedWithExcuse === true) && (this.currentPosition === 1)))
		{
            var found = false;

            // Look if opponents have missed suits to make them cut
            for (var i=0; i<4; i++)
            {
                if (this.taker !== i)
                {
                    // forget trumps, search for missing suits only
                    for (var j=0; j<4; j++)
                    {
                        if (this.players[i].hasSuits[j] === false)
                        {
                            playedCard = this.bot.playLowestCard(TarotLib.Suit.toString(i));
                            found = true;
                            systemPrint("attack: playing first, lowest card");
                            break;
                        }
                    }
                }

                if (found)
                {
                    break;
                }
            }

            if (!found)
            {
                // No any cut in opponents... play trumps starting from the top!
                if (this.bot.hasSuit("T"))
                {
                    var opponentsHaveTrumps = false;
                    var onlyOneOfTrump = false;
                    // Look if opponents have trumps
                    for (var j=0; j<4; j++)
                    {
                        if (this.players[j].hasSuits[4] == true)
                        {
                            opponentsHaveTrumps = true;
                        }
                    }

                    if ((this.bot.stats.suits['T'] === 1) &&
                        (this.bot.stats.littleTrump))
                    {
                        onlyOneOfTrump = true;
                    }

                    if (onlyOneOfTrump && opponentsHaveTrumps)
                    {
                        // we have only the one of trump, save it because it can be captured and play low card
                        playedCard = this.bot.playLowestCard();
                        systemPrint("attack: playing first, lowest any card (saving one of trump)");
                    }
                    else
                    {
                        playedCard = this.bot.playHighestCard('T');
                        systemPrint("attack: playing first, highest trump");
                    }
                }
                else
                {
                    // no any trumps :( play low card
                    playedCard = this.bot.playLowestCard();
                    systemPrint("attack: playing first, lowest any card");
                }
            }
        }
        else
        {
            // do we have the suit?
            if (this.bot.hasSuit(this.trickSuit))
            {
               // yes: do we have higher card? yes ==> play higher to become master, otherwise play low card
                playedCard = this.bot.playHighestCard(this.trickSuit);
                systemPrint("attack: playing a higher card");
            }
            else
            {
                // no, we have to cut, if we have some trumps
                if (this.bot.hasSuit("T"))
                {
                    // Over-cut a previous trump? 
                    var card = this.highestTrump();
                    if (card === undefined)
                    {
                        // no any trump played before, cut with a low trump
                        playedCard = this.bot.playLowestCard('T', 1);
                        systemPrint("attack: cut with a low trump");    
                    }
                    else
                    {
                        if (this.isPlayed("T", 1))
                        {
                            // if the one of trump has been played, over-cut with a very high card
                            playedCard = this.bot.playHighestCard("T");
                            systemPrint("attack: try to catch the one of trump!");
                        }
                        else
                        {
                            // over-cut, with a trump just higher than the highest one
                            playedCard = this.bot.playLowestCard("T", card.value);
                            systemPrint("attack: over-cut, just as necessary");
                        }
                    }
                }
                else
                {
                    // Play any low card that we have in hand
                    playedCard = this.bot.playLowestCard();
                    systemPrint("attack: play low card :(");
                }
                
            }
        }
        return playedCard;
    };

	
TarotLib.Game = Game;
}());
// End of file
