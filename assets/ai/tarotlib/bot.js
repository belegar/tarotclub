/*=============================================================================
 * TarotClub - bot.js
 *=============================================================================
 * Specific methods and data for the bot player
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
 * Player class
 *
 * This class is used to manage a Tarot player
 */
var Bot = function() {
    this.ctor();
};

var p = Bot.prototype;

// ****************************************************************************
// PUBLIC PROPERTIES
// ****************************************************************************

// ****************************************************************************
// PRIVATE PROPERTIES
// ****************************************************************************
    this.place = 0;
    this.deck = null;
    this.stats = null;

// ****************************************************************************
// CONSTRUCTOR
// ****************************************************************************
    p.ctor = function()
	{
		this.place = 0;
        this.deck = new TarotLib.Deck();
        this.stats = new TarotLib.Stats();

        this.initialize();
	};

// ****************************************************************************
// PUBLIC STATIC METHODS
// ****************************************************************************

    p.initialize = function()
    {
	    this.deck.length = 0;
    };

    p.updateStats = function()
    {
        this.stats.update(this.deck);
    };

    p.hasHigherCard = function(suit, value)
    {
        var ret = false;

        for (var i=0; i<this.deck.size(); i++)
        {
            var card = this.deck.get(i);
            if ((card.suit === suit) &&
                (card.value > value))
            {
                ret = true;
            }
        }
        return ret;
    };

    p.hasSuit = function(suit)
    {
        var ret = false;

        for (var i=0; i<this.deck.size(); i++)
        {
            var card = this.deck.get(i);
            if (card.suit === suit)
            {
                ret = true;
            }
        }
        return ret;
    };

    /**
     * @brief Get the lowest card in the bot's deck
     * @param[in] The suit where to get the card 
     * @return The card, string format
     */
    p.getLowestCard = function(suit, minValue)
    {
        var index = 100; // init with impossible index
        var lowestValue = 25; // init with impossible value
        for (var i=0; i<this.deck.size(); i++)
        {
            var card = this.deck.get(i);
            if (card.suit === suit)
            {
                if ((card.value < lowestValue) &&
                    (card.value >= minValue))
                {
                    index = i;
                    lowestValue = card.value;
                }
            }
        }
        if (index !== 100)
        {
            return this.deck.get(index).getName();
        }   
        return undefined;
    };

    /**
     * @brief Get the highest card in the bot's deck
     * @param[in] The suit where to get the card, string format
     * @return The card, string format
     */
    p.getHighestCard = function(suit, limitValue)
    {
        var index = 100; // init with impossible index
        var highestValue = 0; // init with impossible value
		var indexJustAfterTheValue = 100;
		
        for (var i=0; i<this.deck.size(); i++)
        {
            var card = this.deck.get(i);
            if (card.suit === suit)
            {
                if (card.value > highestValue)
                {
                    index = i;
                    highestValue = card.value;
                }
				
				// Get the index just over the maxValue, if defined
				if (limitValue !== undefined)
				{
					if ((card.value > limitValue) &&
					    (indexJustAfterTheValue == 100))
					{
						indexJustAfterTheValue = i;
					}
				}
				
            }
        }
        if (index !== 100)
        {
			if (indexJustAfterTheValue !== 100)
			{
				return this.deck.get(indexJustAfterTheValue).getName();
			}
			else
			{
				return this.deck.get(index).getName();
			}
        }
        return undefined;
    };

    /**
     * @brief Play a card in the long suit of the player
     */
    p.playLongSuit = function()
    {
        var playedCard;

        if (this.stats.longSuits !== 0)
        {
            // Walk through all suits, except trumps
            for (var i=0; i<4; i++)
            {
                var suit = TarotLib.Suit.toString(i);
                if (this.stats.suits[suit] >= 5)
                {
                    playedCard = this.getLowestCard(suit, 1);
                    break;
                }
            }
        }
        return playedCard;
    };

    p.playLowestCard = function(forceSuit, minValue)
    {
        var playedCard;
        if (minValue === undefined)
        {
            if (forceSuit === "T")
            {
                minValue = 2;
            }
            else
            {
                minValue = 1;
            }
        }

		// ok, we have to play a lowest card ; try to play the excuse to save it
		if (this.stats.fool && minValue === undefined)
		{
			playedCard = "00-T";
		}
		else if (forceSuit !== undefined)
        {
            playedCard = this.getLowestCard(forceSuit, minValue);
        }
        else
        {
			// Find any low card in any suit ...
			for (var i=0; i<5; i++)
			{
				var suit = TarotLib.Suit.toString(i);
				if (this.stats.suits[suit] != 0)
				{
					playedCard = this.getLowestCard(suit, minValue);
					break;
				}
			}
        }

        // We still haven't found any cards, it means that we only have the fool and/or the little trump
        if (playedCard === undefined)
        {
            // whatever, play the little trumps or the fool ...
            playedCard = this.getLowestCard(TarotLib.Suit.TRUMPS, 0);
        }

        return playedCard;
    };

	/**
	 * @brief Play the highest card available in the deck
	 * @param[in] forceSuit: if defined, the suit to play
	 * @param[in] value: if defined, the value to play over, and stop here even if we have higher values
	 */
    p.playHighestCard = function(forceSuit, value)
    {
        var playedCard;	
        if (forceSuit !== undefined)
        {
            if (this.stats.suits[forceSuit] != 0)
            {
                playedCard = this.getHighestCard(forceSuit, value);
            }
        }

        if (playedCard === undefined)
        {
            for (var i=0; i<5; i++)
            {
                var suit = TarotLib.Suit.toString(i);
                if (this.stats.suits[suit] !== 0)
                {
                    playedCard = this.getHighestCard(suit, value);
                    break;
                }
            }
        }
        return playedCard;
    };

    p.calculateBid = function()
    {
        var total = 0;
        this.updateStats();
        

        // We start looking at bouts, each of them increase the total value of points
        if( this.stats.bigTrump === true ) {
          total += 9;
        }
        if( this.stats.fool === true ) {
          total += 7;
        }
        if( this.stats.littleTrump === true ) {
           if( this.stats.trumps === 5 ) {
             total += 5;
          } else if( (this.stats.trumps === 6) || (this.stats.trumps === 7) ) {
             total += 7;
          } else if( this.stats.trumps > 7 ) {
             total += 8;
          }
        }

        // Each atout counts two points
        // Each major atout counts one more point
        total += this.stats.suits[TarotLib.Suit.TRUMPS] * 2;
        total += this.stats.majorTrumps * 2;
        total += this.stats.kings * 6;
        total += this.stats.queens * 3;
        total += this.stats.knights * 2;
        total += this.stats.jacks;
        total += this.stats.weddings;
        total += this.stats.longSuits * 5;
        total += this.stats.cuts * 5;
        total += this.stats.singletons * 3;
        total += this.stats.sequences * 4;

        systemPrint("Bid calculated: " + total);

        // We decide on a bid depending of thresholds
        var cont;
        if( total <= 35 ) {
          cont = TarotLib.Contract.PASS;
        } else if( total >= 36  && total <= 50 ) {
          cont = TarotLib.Contract.TAKE;
        } else if( total >= 51  && total <= 65 ) {
          cont = TarotLib.Contract.GUARD;
        } else if( total >= 66  && total <= 75 ) {
          cont = TarotLib.Contract.GUARD_WITHOUT;
        } else {
          cont = TarotLib.Contract.GUARD_AGAINST;
        }
        return cont;
    };

    /**
     *  @brief Constructs a valid discard, attack strategy
     *  We're building the discard:
      *    - Save queens, knights and jacks
      *    - Try to make cuts in suits
     *
     *  @param[in] dogDeck, string format
     */
    p.buildDiscard = function(dogDeck)
    {
        var ok = false;
        var i = 0;
        var discard = new TarotLib.Deck();
        var possibleCuts = [];
        var possibleCutsCounter = 0;
      
        this.deck.addCards(dogDeck);
        var suitToAdd;
        var bestSuitToAdd = 0;

    	// Update the statistics with the dog deck
    	this.updateStats();

        // Look if we can have cuts in some suits
        for (i = 0; i < 4; i++)
        {
            var suit = TarotLib.Suit.toString(i);
            if ((this.stats.suits[suit] < 7) && (this.stats.suits[suit] > 0))
            {
                // If this suit contains a king, this is not a good candidate for a discard
                if (this.deck.hasCard(14, suit))
                {
                    possibleCuts[i] = false;
                }
                else
                {
                    possibleCuts[i] = true;
                    possibleCutsCounter++;
                    if (this.stats.suitPoints[suit] >= bestSuitToAdd)
                    {
                        bestSuitToAdd = this.stats.suitPoints[suit];
                        suitToAdd = suit;
                    }
                }
            }
        }

        // If we have possible cuts, then add the cards to the discard
        if (possibleCutsCounter > 0)
        {
            // Try to add the suit with higher points
            for (i = 0; i < this.deck.size(); i++)
            {
                var card = this.deck.get(i);
                if (card.suit === suitToAdd)
                {
        		    if (discard.size() < 6)
        		    {
                        discard.addOneCard(card.getName(), 0);
                    }
                }
            }
        }

        this.deck.removeDuplicates(discard);

        var minValue = 13;

        while (discard.size() < 6)
        {
            // Then, complete the discard to save points (queens, knights...)
            for (i = 0; i < this.deck.size(); i++)
            {
                var c = this.deck.get(i);
                if ((c.value !== 14) && (c.suit !== "T"))
                {
                    if (c.value === minValue)
                    {
                        discard.addOneCard(c.getName(), 0);
                    }
                    if (discard.size() == 6)
                    {
                        break;
                    }
                }
            }

            // Next paths: add the higher points to lower
            minValue--;
        }

        this.deck.removeDuplicates(discard);

        return discard.toString();
    };

TarotLib.Bot = Bot;
}());
// End of file
