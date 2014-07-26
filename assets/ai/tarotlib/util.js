/*=============================================================================
 * TarotClub - util.js
 *=============================================================================
 * Various utility classes and global definitions, must be included at first
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
 
// ****************************************************************************
// DEFINITIONS
// ****************************************************************************
	this.Place = {
		SOUTH:0,
		EAST:1,
		NORTH:2,
		WEST:3,
        FIFTH:4,
        NOWHERE:5
	};

	var Suit = {
		SPADES: "S",
		HEARTS: "H",
		CLUBS: "C",
		DIAMONDS: "D",
		TRUMPS: "T"
	};

	var Contract = {
		PASS:0,
		TAKE:1,
		GUARD:2,
		GUARD_WITHOUT:3,
		GUARD_AGAINST:4
	};
	
// ****************************************************************************
// PUBLIC METHODS
// ****************************************************************************
	function Stats()
	{
        this.suits = new Array(5);
        this.suitPoints = new Array(5);
 
        this.reset = function()
        {
            this.oudlers = 0;       // 0, 1, 2 ou 3
            this.majorTrumps = 0;   // atouts >= 15

	        this.kings = 0;
	        this.queens = 0;
	        this.knights = 0;
	        this.jacks = 0;

            this.weddings = 0;      // nombre de mariages dans la main
	        this.longSuits = 0;
            this.cuts = 0;          // aucune carte dans une couleur
            this.singletons = 0;    // une seule carte dans une couleur
            this.sequences = 0;     // cartes qui se suivent (au moins 5 cartes pour être comptées)
            this.bestSuit = "";		// best suit detected

	        for (var i=0; i<5; i++)
	        {
	        	this.suits[Suit.toString(i)] = 0;
	        	this.suitPoints[Suit.toString(i)] = 0;
	        }

	        this.littleTrump = false;
	        this.bigTrump = false;
	        this.fool = false;	
        };
		
		/**
		 * @brief Update the statistics of a deck
		 * @param[in] deck Deck class of cards
		 */
		this.update = function(deck)
		{
			var i, k;
			var c;

			this.reset();
			
			// looking for trumps
			for (i = 0; i < deck.size(); i++)
			{
				c = deck.get(i);
				if (c.suit == Suit.TRUMPS)
				{
					this.suits[TarotLib.Suit.TRUMPS]++;
					if (c.value >= 15)
					{
						this.majorTrumps++;
					}
					if (c.value == 21)
					{
						this.bigTrump = true;
						this.oudlers++;
					}
					if (c.value == 1)
					{
						this.littleTrump = true;
						this.oudlers++;
					}
					if (c.value == 0)
					{
						this.fool = true;
						this.oudlers++;
					}
				}
			}
		
			var longue;
			var count = 0;
			var flag = 0;
			var distr = new Array(14); // test of a distribution (14 cards in a suit)
			var highestPoints = 0;

			// Normal suits
			for (i = 0; i<4; i++)
			{
				for (k = 0; k<14; k++)
				{
					distr[k] = 0;
				}
				count = 0;

				var points = 0;
				for (k = 0; k < deck.size(); k++)
				{
					c = deck.get(k);
					if (c.suit == Suit.toString(i))
					{
						count++;
						distr[c.value - 1] = 1;
						if (c.value == 11)
						{
							this.jacks++;
							points += 1.5;
						}
						else if (c.value == 12)
						{
							this.knights++;
							points += 2.5;
						}
						else if (c.value == 13)
						{
							this.queens++;
							points += 3.5;
						}
						else if (c.value == 14)
						{
							this.kings++;
							points += 4.5;
						}
						else
						{
							points += 0.5;
						}
					}
				}

				// Save the points
				this.suitPoints[Suit.toString(i)] = points;
				if (points >= highestPoints)
				{
					highestPoints = points;
					this.bestSuit = Suit.toString(i);
				}

				if (count == 1)
				{
					this.singletons++;
				}
				if (count == 0)
				{
					this.cuts++;
				}

				// Number of cards in each normal suit
				this.suits[Suit.toString(i)] = count;

				if ((distr[13] == 1) && (distr[12] == 1))
				{
					this.weddings++; // mariage (king + queen)
				}

				// sequences
				count = 0;  // length of the sequence
				flag = 0;   // suit found for this sequence
				longue = 0;

				for (k = 0; k < 14; k++)
				{
					if (distr[k] == 1)
					{
						longue++;
						// start of a sequence
						if (flag == 0)
						{
							flag = 1;
							count++;
						}
						else
						{
							count++;
						}
					}
					else if (flag == 1)
					{
						if (count >= 5)
						{
							this.sequences++;
						}
						count = 0;
						flag = 0;
					}
				}
				if (longue >= 5)
				{
					this.longSuits++;
				}
			}
		}; // end update()
	}; // end Stats

// ****************************************************************************
// PUBLIC STATIC METHODS
// ****************************************************************************
    /**
     * integer Place --> string Place
     */
	Place.toString = function(place)
	{
		var position = "";
		
		if (place === Place.SOUTH) {
			position = "South";
		} else if (place === Place.NORTH) {
			position = "North";
		} else if (place === Place.WEST) {
			position = "West";
		} else if (place === Place.EAST) {
			position = "East";
		} else {
			position = "Nobody";
		}
		return position;
	};

    /**
     * string Place --> integer Place
     */
    Place.fromString = function(place)
    {
        var position = 0;

        if (place === "South") {
            position = Place.SOUTH;
        } else if (place === "North") {
            position = Place.NORTH;
        } else if (place === "West") {
            position = Place.WEST;
        } else if (place === "East") {
            position = Place.EAST;
        } else if (place === "Fifth") {
            position = Place.FIFTH;
        } else {
            position = "Nowhere";
        }
        return position;
    };

    /**
     * integer Contract --> string Contract
     */
    Contract.toString = function(contract)
    {
        var text;

        if (contract === Contract.PASS) {
            text = "Pass";
        } else if (contract === Contract.TAKE) {
            text = "Take";
        } else if (contract === Contract.GUARD) {
            text = "Guard";
        } else if (contract === Contract.GUARD_WITHOUT) {
            text = "Guard without";
        } else if (contract === Contract.GUARD_AGAINST) {
            text = "Guard against";
        }
        return text;
    }

    /**
     * string Contract --> integer Contract
     */
    Contract.fromString = function(contract)
    {
        var value;

        if (contract === "Pass") {
            value = Contract.PASS;
        } else if (contract === "Take") {
            value = Contract.TAKE;
        } else if (contract === "Guard") {
            value = Contract.GUARD;
        } else if (contract === "Guard without") {
            value = Contract.GUARD_WITHOUT;
        } else if (contract === "Guard against") {
            value = Contract.GUARD_AGAINST;
        }
        return value;
    }

	Suit.toString = function(intValue)
	{
		var suit;

		if (intValue == 0) {
			suit = Suit.SPADES;
		} else if (intValue == 1) {
			suit = Suit.HEARTS;
		} else if (intValue == 2) {
			suit = Suit.CLUBS;
		} else if (intValue == 3) {
			suit = Suit.DIAMONDS;
		} else {
			suit = Suit.TRUMPS;
		}
		return suit;
	};
	

TarotLib.Place = Place;
TarotLib.Suit = Suit;
TarotLib.Contract = Contract;
TarotLib.Stats = Stats;

}());

// End of file
