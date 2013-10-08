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
this.TarotLib = this.TarotLib||{};

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
	/**
	 * Array of cards
	 */
	p.cards = null;
	
// ****************************************************************************
// PRIVATE PROPERTIES
// ****************************************************************************

// ****************************************************************************
// CONSTRUCTOR
// ****************************************************************************
    p.ctor = function()
	{
		this.cards = new Array();
	};

// ****************************************************************************
// PUBLIC STATIC METHODS
// ****************************************************************************
	
	// debug code only
    p.print = function()
	{
		for(var i=0; i<this.cards.length; i++) {
            this.cards[i].print();
		}
	};
	
    p.clear = function()
	{
		for (var i=0; i<this.cards.length; i++) {
			this.cards[i] = undefined;
		}
		this.cards.length = 0;
	};
	
    p.addOneCard = function(cardName, place)
	{
        this.cards[this.cards.length] = new TarotLib.Card(cardName, place);
	};
	
    /**
     * Take out one card from the deck
     * Return the card name (string format) of a card
     */
    p.takeCard = function(index)
	{
        var card = this.cards[index].getName();
		this.cards[index] = undefined;
		return card;
	};
	
    p.setCards = function(list)
	{
		var result = list.split(/;/g);		
		
        this.clear();
		
		for (var i=0; i<result.length; i++) {
            this.cards[i] = new TarotLib.Card(result[i]);
		}
	};

TarotLib.Deck = Deck;
}());
// End of file
