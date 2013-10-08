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
 * Deck class
 *
 * This class gather a list of cards
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
    p.position = TarotLib.Place.SOUTH;

    /**
     * Table of suits, for each one, true if suit is available for that player
     */
    p.hasSuits = null;

// ****************************************************************************
// CONSTRUCTOR
// ****************************************************************************
    p.ctor = function(place)
	{
		this.position = place;
        this.hasSuits = new Array(5);

        for (var i=0; i<this.hasSuits.length; i++) {
            this.hasSuits[i] = true;
        }
	};

// ****************************************************************************
// PUBLIC STATIC METHODS
// ****************************************************************************

	p.initialize = function()
	{
		for (var i=0; i<this.hasSuits.length; i++) {
            this.hasSuits[i] = true;
        }
	}

    p.print = function()
    {
        for (var i=0; i<this.hasSuits.length; i++) {
            if (this.hasSuits[i] === false) {
                systemPrint("Player " + TarotLib.Place.toString(this.position) + " has a missed suit: " + TarotLib.Suits.toString(i));
            }
        }
    };

    p.setMissedColor = function(color)
    {
        this.hasSuits[TarotLib.Suits.toInteger(color)] = false;
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

TarotLib.Player = Player;
}());
// End of file
