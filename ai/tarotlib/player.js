/*=============================================================================
 * TarotClub - player.js
 *=============================================================================
 * Tarot player Javascript class, gather statistics of one player
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
 * @brief Player class
 *
 * This class is used to manage a Tarot player
 */
var Player = function(place) {
    this.hasSuits = new Array(5);
    this.place = place;

    this.initialize();
};

var p = Player.prototype;

// ****************************************************************************
// PUBLIC METHODS
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

    p.setMissedSuit = function(suit)
    {
        this.hasSuits[suit] = false;
    };

    // return true if there is at least one suit missing, including trumps
    p.hasMissedSuit = function()
    {
        var missed = false;

        for (var i=0; i<this.hasSuits.length; i++)
        {
            if (this.hasSuits[i] === false)
            {
                missed = true;
            }
        }
        return missed;
    };

TarotLib.Player = Player;
}());
// End of file
