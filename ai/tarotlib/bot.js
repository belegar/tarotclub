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
	}

    p.updateStats = function()
    {
        this.stats.update(this.deck);
    };

    p.hasSuit = function(suit)
    {
        var ret = false;

        for (var i=0; i<this.deck.size(); i++)
        {
            var card = this.deck.get(i);
            if (card.suit == suit)
            {
                ret = true;
            }
        }
        return ret;
    };

    /**
     * @brief Take the lowest card in the bot's deck, delete it from the deck!!
     * @param[in] The suit where to get the card 
     * @return The card, string format
     */
    p.takeLowestCard = function(suit, minValue)
    {
        var index = 100; // init with impossible index
        var startValue = 25; // init with impossible value
        for (var i=0; i<this.deck.size(); i++)
        {
            var card = this.deck.get(i);
            if (card.suit == suit)
            {
                if ((card.value < startValue) &&
                    (card.value >= minValue))
                {
                    index = i;
                }
            }
        }
        if (index != 100)
        {
            return this.deck.takeCard(index);
        }   
        return undefined;
    };

    /**
     * @brief Take the highest card in the bot's deck, delete it from the deck!!
     * @param[in] The suit where to get the card, string format
     * @return The card, string format
     */
    p.takeHighestCard = function(suit)
    {
        var index = 100; // init with impossible index
        var startValue = 0; // init with impossible value
        for (var i=0; i<this.deck.size(); i++)
        {
            var card = this.deck.get(i);
            if (card.suit == suit)
            {
                if (card.value > startValue)
                {
                    index = i;
                }
            }
        }
        if (index != 100)
        {
            return this.deck.takeCard(index);
        }
        return undefined;
    };

    p.playLongSuit = function()
    {
        var playedCard = undefined;

        if (this.stats.longSuits != 0)
        {
            // Walk through all suits, except trumps
            for (var i=0; i<4; i++)
            {
                var suit = TarotLib.Suit.toString(i);
                if (this.stats.suits[suit] >= 5)
                {
                    playedCard = this.takeLowestCard(suit, 1);
                    break;
                }
            }
        }
        return playedCard;
    };

    p.playLowestCard = function(forceSuit, minValue)
    {
        var playedCard = undefined;
        if (minValue == undefined)
        {
            if (forceSuit == "T")
            {
                minValue = 2;
            }
            else
            {
                minValue = 1;
            }
        }

        if (forceSuit != undefined)
        {
            if (this.stats.suits[forceSuit] != 0)
            {
                playedCard = this.takeLowestCard(forceSuit, minValue);
            }
        }

        if (playedCard == undefined)
        {
            for (var i=0; i<5; i++)
            {
                var suit = TarotLib.Suit.toString(i);
                if (this.stats.suits[suit] != 0)
                {
                    playedCard = this.takeLowestCard(suit, minValue);
                    break;
                }
            }
        }

        // We still haven't found any cards, it means that we only have the fool and/or the little trump
        if (playedCard == undefined)
        {
            // whatever, play the little trumps or the fool ...
            playedCard = this.takeLowestCard(TarotLib.Suit.TRUMPS, 0);
        }

        return playedCard;
    };

    p.playHighestCard = function(forceSuit)
    {
        var playedCard = undefined;

        if (forceSuit != undefined)
        {
            if (this.stats.suits[forceSuit] != 0)
            {
                playedCard = this.takeHighestCard(forceSuit);
            }
        }

        if (playedCard == undefined)
        {
            for (var i=0; i<5; i++)
            {
                var suit = TarotLib.Suit.toString(i);
                if (this.stats.suits[suit] != 0)
                {
                    playedCard = this.takeHighestCard(suit);
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

        // We decide on a bid depending of thresholds
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

TarotLib.Bot = Bot;
}());
// End of file
