/*=============================================================================
 * TarotClub - deck.js
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
this.TarotLib = this.TarotLib || {};

(function() {

/**
 * Deck class
 *
 * This class gather a list of cards
 */
var Deck = function() {
	this.ctor();
};

var p = Deck.prototype;

// ****************************************************************************
// PUBLIC PROPERTIES
// ****************************************************************************

	
// ****************************************************************************
// PRIVATE PROPERTIES
// ****************************************************************************
	this.cards;
	
// ****************************************************************************
// CONSTRUCTOR
// ****************************************************************************
    p.ctor = function()
	{
		// nothing for now
		this.cards = new Array();
	};

// ****************************************************************************
// PUBLIC STATIC METHODS
// ****************************************************************************
	
	/**
	 * @brief Get a string of cards, separated by semi-colon
	 * @return The deck in string format
	 */
	p.toString = function()
	{
		var buffer = "";
		for(var i=0; i < this.cards.length; i++)
		{
			if (i != 0)
			{
				buffer += ";";
			}
            buffer += this.cards[i].value + "-" + this.cards[i].suit;
		}
		return buffer;
	};

	// debug code only
    p.print = function()
	{
		systemPrint(this.toString());
	};
	
	p.size = function()
	{
		return this.cards.length;
	};
	
	/**
	 * @brief Return a copy of one card of the deck
	 */
	p.get = function(index)
	{
		return this.cards[index];
	};
	
    p.clear = function()
	{
		this.cards.length = 0;
	};
	
    p.addOneCard = function(cardName, place)
	{
        this.cards[this.cards.length] = new TarotLib.Card(cardName, place);
	};
	
	/**
     * @brief Take out one card from the deck
     * @param[in] Card name, string format
     */
    p.removeCard = function(cardName)
	{
		for (var i=0; i<this.cards.length; i++)
		{
			var card = this.cards[i].getName();
			if (cardName == card)
			{
				this.cards.splice(i, 1); // properly delete the element in the array	
			}
		}
	};
	
    p.setCards = function(list)
	{
		var result = list.split(/;/g);		
		
        this.clear();
		for (var i=0; i<result.length; i++)
		{
            this.cards[i] = new TarotLib.Card(result[i]);
		}
	};

TarotLib.Deck = Deck;
}());
// End of file
