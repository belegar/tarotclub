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
 * @brief Deck class
 *
 * This class gather a list of cards
 */
var Deck = function() {
    this.cards = new Array();
};

var p = Deck.prototype;

// ****************************************************************************
// PUBLIC METHODS
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
            buffer += this.cards[i].getName();
		}
		return buffer;
	};
		
    p.makeTarotDeck = function()
	{
		this.clear();
		// The 4 suits
		for (var i = 0; i < 4; i++)
		{
			// From ace to the king, 14 cards
			for (var j = 1; j <= 14; j++)
			{
				var name = ("00" + j).slice(-2) + "-" + TarotLib.Suit.toString(i);
				this.addOneCard(name);
			}
		}

		// The 22 trumps, including the fool
		for (var i = 0; i <= 21; i++)
		{
			var name = ("00" + i).slice(-2) + "-T";	
			this.addOneCard(name);
		}
   };

	p.toHtmlString = function()
	{
		var buffer = "<table><tr>";
		for(var i=0; i < this.cards.length; i++)
		{
            buffer += "<td>" + this.cards[i].getHtmlName() + "</td>";
		}
		buffer += "</tr></table>";
		return buffer;
	};

	// debug code only
    p.print = function()
	{
		systemPrint(this.toString());
	};

	p.printHtml = function()
	{
		systemPrint(this.toHtmlString());
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

	p.hasCard = function(value, suit)
	{
		var ret = false;
		for (var i = 0; i < this.cards.length; i++)
		{
			if ((this.cards[i].value == value) && (this.cards[i].suit == suit))
			{
				ret = true;
			}
		}
		return ret;
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
		for (var i = 0; i < this.cards.length; i++)
		{
			var card = this.cards[i].getName();
			if (cardName == card)
			{
				this.cards.splice(i, 1); // properly delete the element in the array	
			}
		}
	};
	
    /**
     * @brief build a deck with a string of cards passed in parameters
     */
    p.setCards = function(list)
	{
        this.clear();
        this.addCards(list);
	};
    
    /**
     * @brief Add some cards to the deck with a string of cards passed in parameters
     */
    p.addCards = function(list)
    {
        var result = list.split(/;/g);
        var size = this.cards.length;
		for (var i = 0; i < result.length; i++)
		{
            this.cards[size + i] = new TarotLib.Card(result[i]);
		}
    };

TarotLib.Deck = Deck;
}());
// End of file
