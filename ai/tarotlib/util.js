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
this.TarotLib = this.TarotLib||{};
 
(function() {
 
// ****************************************************************************
// DEFINITIONS
// ****************************************************************************
	var Place = {
		SOUTH:0,
		EAST:1,
		NORTH:2,
		WEST:3,
		NOWHERE:0xFF
	};

	var Suits = {
		SPADES:0,
		HEARTS:1,
		CLUBS:2,
		DIAMONDS:3,
		TRUMPS:4
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
        this.trumps = 0;  // nombres d'atouts , en comptant les bouts et l'excuse
        this.oudlers = 0;   // 0, 1, 2 ou 3
        this.majorTrumps = 0; // atouts >= 15

        this.kings = 0;
        this.queens = 0;
        this.knights = 0;
        this.jacks = 0;

        this.weddings = 0;   // nombre de mariages dans la main
        this.longSuits = 0;
        this.cuts = 0;     // aucune carte dans une couleur
        this.singletons = 0; // une seule carte dans une couleur
        this.sequences = 0;  // cartes qui se suivent (au moins 5 cartes pour être comptées)

        this.clubs = 0;
        this.spades = 0;
        this.hearts = 0;
        this.diamonds = 0;

        this.littleTrump = false;
        this.bigTrump = false;
        this.fool = false;
		
		/**
		 * @brief Update the statistics of a deck
		 */
		this.update = function(deck)
		{
			var i, k;
			var c;
			
			// looking for trumps
			for (i = 0; i < deck.size(); i++)
			{
				c = deck.get(i);
				if (c.suit == Suits.TRUMPS)
				{
					this.trumps++;
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
		
			var suit;
			var longue;
			var count = 0;
			var flag = 0;
			var distr = new Array(14); // test of a distribution

			// Normal suits
			for (i = 0; i<4; i++)
			{
				if (i == 0)
				{
					suit = Suits.SPADES;
				}
				else if (i == 1)
				{
					suit = Suits.HEARTS;
				}
				else if (i == 2)
				{
					suit = Suits.CLUBS;
				}
				else
				{
					suit = Suits.DIAMONDS;
				}

				for (k = 0; k<14; k++)
				{
					distr[k] = 0;
				}
				count = 0;

				for (k = 0; k < deck.size(); k++)
				{
					c = deck.get(k);
					if (c.suit == suit)
					{
						count++;
						distr[c.value - 1] = 1;
						if (c.value == 11)
						{
							this.jacks++;
						}
						if (c.value == 12)
						{
							this.knights++;
						}
					}
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
				if (i == 0)
				{
					this.spades = count;
				}
				else if (i == 1)
				{
					this.hearts = count;
				}
				else if (i == 2)
				{
					this.clubs = count;
				}
				else
				{
					this.diamonds = count;
				}

				if ((distr[13] == 1) && (distr[12] == 1))
				{
					this.weddings++; // mariage (king + queen)
				}
				if (distr[13] == 1)
				{
					this.kings++;     // king without queen
				}
				if (distr[12] == 1)
				{
					this.queens++;    // queen without kings
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
	 * This function transform a place integer into a string
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
	 * This function transform a suits string into an integer
	 */
	Suits.toInteger = function(color)
	{
		var suits;
		
		if (color === "A") {
			suits = Suits.TRUMPS;
		} else if (color === "S") {
			suits = Suits.SPADES;
		} else if (color === "D") {
			suits = Suits.DIAMONDS;
		} else if (color === "C") {
			suits = Suits.CLUBS;
		} else {
			suits = Suits.HEARTS;
		}
		return suits;
	}

	/**
	 * This function transform a suits integer into a string
	 */
	Suits.toString = function(color)
	{
		var suits = "";
		
		if (color === Suits.TRUMPS) {
			suits = "A";
		} else if (color === Suits.SPADES) {
			suits = "S";
		} else if (color === Suits.DIAMONDS) {
			suits = "D";
		} else if (color === Suits.CLUBS) {
			suits = "C";
		} else {
			suits = "H";
		}
		return suits;
	}

TarotLib.Place = Place;
TarotLib.Suits = Suits;
TarotLib.Contract = Contract;
TarotLib.Stats = Stats;

}());

// End of file
