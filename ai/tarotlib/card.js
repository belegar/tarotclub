/*=============================================================================
 * TarotClub - card.js
 *=============================================================================
 * Tarot card Javascript class
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
 * Card class
 *
 * This class allow you to specify an owner along with the card value
 * If you don't provide the second argument, the owner will be set to HYPERSPACE
 */
var Card = function(cardName, place) {
    this.initialize(cardName, place);
};

var p = Card.prototype;

// ****************************************************************************
// PUBLIC PROPERTIES
// ****************************************************************************
	/**
	 * Face value of the card
	 * @property target
	 * @type Object
	 * @default null
	 * @readonly
	 */
	p.value = null;
	
	p.color = null;
	
	p.owner = TarotLib.Place.NOWHERE;
	
// ****************************************************************************
// PRIVATE PROPERTIES
// ****************************************************************************

// ****************************************************************************
// CONSTRUCTOR
// ****************************************************************************
	/**
	 * Initialization method.
	 */
    p.initialize = function(cardName, place) {
		var elem = cardName.split("-");
		this.value = elem[0];
		this.color = elem[1];
		this.owner = place;
	};

// ****************************************************************************
// PUBLIC STATIC METHODS
// ****************************************************************************

    p.getName = function()
	{
		return (this.value + "-" + this.color);
	};
	
	// debug code only
    p.print = function()
	{
		var place = TarotLib.Place.toString(this.owner);
        systemPrint("Card value: " + this.value + " Color: " + this.color + " Owner: " + place);
	};

TarotLib.Card = Card;
}());
// End of file
