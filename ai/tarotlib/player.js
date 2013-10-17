/*=============================================================================
 * TarotClub - player.js
 *=============================================================================
 * Tarot deck Javascript class
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
 * Player class
 *
 * This class is used to manage a Tarot player
 */
var Player = function(place) {
    this.ctor(place);
};

var p = Player.prototype;

// ****************************************************************************
// PUBLIC PROPERTIES
// ****************************************************************************

// ****************************************************************************
// PRIVATE PROPERTIES
// ****************************************************************************
    this.place = 0;
    this.deck = null;
    this.stats = null;

    /**
     * Table of suits, for each one, true if suit is available for that player
     */
    this.hasSuits = null;

// ****************************************************************************
// CONSTRUCTOR
// ****************************************************************************
    p.ctor = function(place)
	{
		this.hasSuits = new Array(5);
		this.place = place;
        this.deck = new TarotLib.Deck();
        this.stats = new TarotLib.Stats();

        this.initialize();
	};

// ****************************************************************************
// PUBLIC STATIC METHODS
// ****************************************************************************

	p.initialize = function()
	{
		this.hasSuits[TarotLib.Suit.SPADES] = true;
		this.hasSuits[TarotLib.Suit.HEARTS] = true;
		this.hasSuits[TarotLib.Suit.CLUBS] = true;
		this.hasSuits[TarotLib.Suit.DIAMONDS] = true;
		this.hasSuits[TarotLib.Suit.TRUMPS] = true;
	}

    p.print = function()
    {
        for (var suit in this.hasSuits) {
            if (this.hasSuits[suit] === false) {
                systemPrint("Player " + TarotLib.Place.toString(this.place) + " has a missed suit: " + suit);
            }
        }
    };

    p.setMissedColor = function(color)
    {
        this.hasSuits[color] = false;
    };

    // return true if there is at least one suit missing, including trumps
    p.hasMissedSuit = function()
    {
        var missed = false;

        for (var i=0; i<this.hasSuits.length; i++) {
            if (this.hasSuits[i] === false) {
                missed = true;
            }
        }
        return missed;
    };

    p.updateStats = function()
    {
        this.stats.update(this.deck);
    };

    /**
     * @brief Take the lowest card in the bot's deck
     * @param[in] The suit where to get the card 
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

    p.playLongSuit = function()
    {
        var playedCard = undefined;

        if (this.stats.longSuits != 0)
        {
            if (this.stats.clubs >= 5)
            {
                playedCard = this.takeLowestCard(TarotLib.Suit.CLUBS, 1);
            }
            else if (this.stats.diamonds >= 5)
            {
                playedCard = this.takeLowestCard(TarotLib.Suit.DIAMONDS, 1);
            }
            else if (this.stats.spades >= 5)
            {
                playedCard = this.takeLowestCard(TarotLib.Suit.SPADES, 1);
            }
            else
            {
                playedCard = this.takeLowestCard(TarotLib.Suit.HEARTS, 1);
            }
        }
        return playedCard;
    };

    p.playLowestCard = function(forceSuit)
    {
        var playedCard = undefined;

        if (this.stats.clubs > 0)
        {
            if ((forceSuit == TarotLib.Suit.CLUBS) ||
                (forceSuit == undefined))
            {
                playedCard = this.takeLowestCard(TarotLib.Suit.CLUBS, 1);
            }
        }
        else if (this.stats.diamonds > 0)
        {
            if ((forceSuit == TarotLib.Suit.DIAMONDS) ||
                (forceSuit == undefined))
            {
                playedCard = this.takeLowestCard(TarotLib.Suit.DIAMONDS, 1);
            }
        }
        else if (this.stats.spades > 0)
        {
            if ((forceSuit == TarotLib.Suit.SPADES) ||
                (forceSuit == undefined))
            {
                playedCard = this.takeLowestCard(TarotLib.Suit.SPADES, 1);
            }
        }
        else if (this.stats.hearts > 0)
        {
            if ((forceSuit == TarotLib.Suit.HEARTS) ||
                (forceSuit == undefined))
            {
                playedCard = this.takeLowestCard(TarotLib.Suit.HEARTS, 1);
            }
        }
        else
        {
            if ((forceSuit == TarotLib.Suit.TRUMPS) ||
                (forceSuit == undefined))
            {
                // avoid to play the one of trumps!
                playedCard = this.takeLowestCard(TarotLib.Suit.TRUMPS, 2);
            }
        }

        if (playedCard == undefined)
        {
            // whatever, play the little trumps or the fool ...
            playedCard = this.takeLowestCard(TarotLib.Suit.TRUMPS, 0);
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
        total += this.stats.trumps * 2;
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

TarotLib.Player = Player;
}());
// End of file
