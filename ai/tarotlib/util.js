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
// PUBLIC PROPERTIES
// ****************************************************************************

// ****************************************************************************
// PRIVATE PROPERTIES
// ****************************************************************************

// ****************************************************************************
// CONSTRUCTOR
// ****************************************************************************

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

}());

// End of file
