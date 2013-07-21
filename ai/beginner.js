/*=============================================================================
 * TarotClub - beginner.js
 *=============================================================================
 * Beginner's level artificial intelligence
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

/*****************************************************************************/
// GLOBAL DEFINITIONS
 
var Contract = {
    PASSE:0,
    PRISE:1,
    GARDE:2,
    GARDE_SANS:3,
    GARDE_CONTRE:4
}

var Place = {
	SOUTH:0,
	EAST:1,
	NORTH:2,
	WEST:3
}

var Suits = {
	SPADES:0,
	HEARTS:1,
	CLUBS:2,
	DIAMONDS:3,
	TRUMPS:4
}

/*****************************************************************************/
// UTILITY FUNCTIONS AND CLASSES

/**
 * This function transform a place integer into a string
 */
function PlaceToString(place)
{
    var position = "";
	
	if (place == Place.SOUTH) {
		position = "South";
	} else if (place == Place.NORTH) {
		position = "North";
	} else if (place == Place.WEST) {
		position = "West";
	} else if (place == Place.EAST) {
		position = "East";
	} else {
		position = "Nobody";
	}
	return position;
}

/**
 * This function transform a suits string into an integer
 */
function SuitsToInteger(color)
{
    var suits;
	
	if (color == "A") {
		suits = Suits.TRUMPS;
	} else if (color == "S") {
		suits = Suits.SPADES;
	} else if (color == "D") {
		suits = Suits.DIAMONDS;
	} else if (color == "C") {
		suits = Suits.CLUBS;
	} else {
		suits = Suits.HEARTS;
	}
	return suits;
}

/**
 * This function transform a suits integer into a string
 */
function SuitsToString(color)
{
    var suits = "";
	
	if (color == Suits.TRUMPS) {
		suits = "A";
	} else if (color == Suits.SPADES) {
		suits = "S";
	} else if (color == Suits.DIAMONDS) {
		suits = "D";
	} else if (color == Suits.CLUBS) {
		suits = "C";
	} else {
		suits = "H";
	}
	return suits;
}

/**
 * This class allow you to specify an owner along with the card value
 * If you don't provide the second argument, the owner will be set to HYPERSPACE
 */
function Card(cardName, place)
{
	var elem = cardName.split("-");
	this.value = elem[0];
	this.color = elem[1];
	this.owner = place;

/********* Methods *********/
	this.GetName = function()
	{
		return (this.value + "-" + this.color);
	}
	
	// debug code only
	this.Print = function()
	{
		var place = PlaceToString(this.owner);
		SystemPrint("Card value: " + this.value + " Color: " + this.color + " Owner: " + place);
	}
}

function Deck()
{
	this.cards = new Array();
	
/********* Methods *********/
	// debug code only
	this.Print = function()
	{
		for(var i=0; i<this.cards.length; i++) {
			this.cards[i].Print();
		}
	}
	
	this.Clear = function()
	{
		for (var i=0; i<this.cards.length; i++) {
			this.cards[i] = undefined;
		}
		this.cards.length = 0;
	}
	
	this.AddOneCard = function(cardName, place)
	{
		this.cards[this.cards.length] = new Card(cardName, place);
	}
	
	// Take out one card from the deck
	// Return the card name (string format) of a card
	this.TakeCard = function(index)
	{
		var card = this.cards[index].GetName();
		this.cards[index] = undefined;
		return card;
	}
	
	this.SetCards = function(list)
	{
		var result = list.split(/;/g);		
		
		this.Clear();
		
		for (var i=0; i<result.length; i++) {
			this.cards[i] = new Card(result[i]);
		}
	}
}

/*****************************************************************************/
// GAME OBJECTS FOR STATISTICS STORAGE

function Player(place)
{
	this.position = place;
	this.hasSuits = new Array(5);
	
/********* Methods *********/
	this.Initialize = function()
	{
		for (var i=0; i<this.hasSuits.length; i++) {
			this.hasSuits[i] = true;
		}
	}
	
	this.Print = function()
	{
		var place = PlaceToString(this.position);
		for (var i=0; i<this.hasSuits.length; i++) {
			if (this.hasSuits[i] == false) {
				SystemPrint("Player " + place + " has a missed suit: " + SuitsToString(i));
			}
		}
	}
	
	this.SetMissedColor = function(color)
	{
		this.hasSuits[SuitsToInteger(color)] = false;
	}
	
	// return true if there is at least one suit missing, including trumps
	this.HasMissedSuit = function()
	{
		var missed = false;
		
		for (var i=0; i<this.hasSuits.length; i++) {
			if (this.hasSuits[i] == false) {
				missed = true;
			}
		}
		return missed;
	}
}

/*****************************************************************************/
// MAIN BOT ENGINE

function GameState()
{
	// Player statistics
	this.players = new Array(4);
	for(var i=0; i<this.players.length; i++) {
		this.players[i] = new Player(i); // position of players is assigned here
		this.players[i].Initialize();
	}
	
	this.playedCards = new Array(18); // turns for 4 players
	for(var i=0; i<18; i++) {
		this.playedCards[i] = new Deck();
	}
	
	// Game variables
	this.contract;
	this.taker;
	this.trickCounter = 0;		// number of turns, 18 with 4 players
	this.currentPosition = 0; 	// position of the current turn [0..3] with 4 players

	// Bot variables
	this.myPlace;
	this.myDeck = new Deck();
	
	// Variables of the current turn
	this.trickColor;
	this.firstCardValue;
	this.startPosition;
	this.startedWithExcuse = false;
	
/********* Methods *********/
	this.DetectMissedColor = function(place)
	{
		if (this.currentPosition == 0) {
			this.startedWithExcuse = false;
			// First played card is the color to play, except in case of excuse (0-A)
			this.trickColor = this.playedCards[this.trickCounter].cards[0].color;
			this.firstCardValue = this.playedCards[this.trickCounter].cards[0].value;
			
			// special case of excuse
			if ((this.trickColor == "A") && (this.firstCardValue == "0")) {
				this.startedWithExcuse = true;			
			}
		} else if ((this.currentPosition == 1) && (this.startedWithExcuse == true)) {
			this.trickColor = this.playedCards[this.trickCounter].cards[1].color;
		} else {
			if (this.playedCards[this.trickCounter].cards[this.currentPosition].color != this.trickColor) {
				this.players[place].SetMissedColor(this.trickColor);
			}
		}
	}

	/**
	 * @brief This method is call at the end of each trick to analyze played cards
	 */
	this.AnalyzeTrick = function(place)
	{
		this.DetectMissedColor(place);
	}

	this.Initialize = function()
	{
		for(var i=0; i<this.players.length; i++) {
			this.players[i].Initialize();
		}
		this.trickCounter = 0;
		this.currentPosition = 0;
		
		for(var i=0; i<this.playedCards.length; i++) {
			this.playedCards[i].Clear();
		}
	}
	
	// Print players statistics and information
	this.PrintPlayers = function()
	{
		for (var j=0; j<this.players.length; j++) {
			this.players[j].Print();
		}
	}
	
	this.SetPlayedCard = function(cardName, place)
	{
		this.playedCards[this.trickCounter].AddOneCard(cardName, place);
	
		this.AnalyzeTrick(place);
	
		this.currentPosition++;
		if (this.currentPosition >= 4) {
			this.currentPosition = 0;
			this.trickCounter++;
			
			// TODO: detect who won the trick
		}
	}
	
	this.PlayDefenseStrategy = function()
	{
		// Just play the first card available ... (let the game engine take a valid card if necessary)
		return this.myDeck.cards[0].GetName();
	}
	
	this.PlayAttackStrategy = function()
	{
		return this.PlayDefenseStrategy();
	}
	
	this.GetRandomCard = function(place)
	{
		var index = 0;
		for (var i=0; this.myDeck.cards.length; i++) {
			if (this.myDeck.cards[i] != undefined) {
				if (this.IsValid(this.myDeck.cards[i].GetName()) == true) {
					index = i;
				}
			}
		}
		return this.myDeck.cards.TakeCard(index);
	}

	this.IsValid = function(card)
	{
		// FIXME
		return true;
	}
}

var CurrentGame = new GameState();

/*****************************************************************************/
// TAROTCLUB API CALLABLE FROM C++ WORLD

/**
 * @brief This function is called when the player has been accepted around the table
 *
 * @param[in] place The place assigned by the server around the table
 */
function EnterGame(place)
{
	CurrentGame.myPlace = place;
}

/**
 * @brief This function is called when the cards are delt to players
 *
 * @param[in] cards Cards of the bots (18 for 4 players), string format 
 */
function ReceiveCards(cards)
{
	CurrentGame.myDeck.SetCards(cards);
}

/**
 * @brief This function is called when the bot must declare a bid
 * 
 * @return The bid announced, use the definition forrmat
 * @see Variable 'Contract'
 */
function AnnounceBid()
{
   total = 0;
   
   SystemPrint("The bot is announcing a bid.");
   
   // We start looking at bouts, each of them increase the total value of points
   if( TStats.hasVingtEtUn() == true ) {
      total += 9;
   }
   if( TStats.hasExcuse == true ) {
      total += 7;
   }
   if( TStats.hasPetit == true ) {
      if( TStats.getNumberOfAtouts == 5 ) {
         total += 5;
      } else if( TStats.getNumberOfAtouts == 6 || TStats.getNumberOfAtouts == 7 ) {
         total += 7;
      } else if( TStats.getNumberOfAtouts > 7 ) {
         total += 8;
      }
   }

   // Each atout counts two points
   // Each major atout counts one more point
   total += TStats.getNumberOfAtouts() * 2;
   total += TStats.getNumberOfAtoutsMajeurs() * 2;
   total += TStats.getNumberOfRois() * 6;
   total += TStats.getNumberOfDames() * 3;
   total += TStats.getNumberOfCavaliers() * 2;
   total += TStats.getNumberOfValets();
   total += TStats.getNumberOfMariages();
   total += TStats.getNumberOfLongues() * 5;
   total += TStats.getNumberOfCoupes() * 5;
   total += TStats.getNumberOfSingletons() * 3;
   total += TStats.getNumberOfSequences() * 4;

   // We decide on a bid depending of thresholds
   if( total <= 35 ) {
      cont = Contract.PASSE;
   } else if( total >= 36  && total <= 50 ) {
      cont = Contract.PRISE;
   } else if( total >= 51  && total <= 65 ) {
      cont = Contract.GARDE;
   } else if( total >= 66  && total <= 75 ) {
      cont = Contract.GARDE_SANS;
   } else {
      cont = Contract.GARDE_CONTRE;
   }
   return cont;
}

/**
 * @brief This function is called for the bot to build the discard
 * 
 * @param[in] dog Dog deck, string format
 */
function BuildDiscard(dog)
{

}

/**
 * @brief This function is called when a player as declared a handle
 * 
 * @param[in] handle declared, card list in string format
 * @param[in] team, Attack or Defense
 */
function ShowHandle(handle, team)
{
}

/**
 * @brief This function is called to indicate a new game
 * 
 * Informational purpose, the script should memorize the arguments for later use
 *
 * @param[in] taker Indicates the place of the taker
 * @param[in] contract Indicates the contract announed by the taker
 */
function StartGame(taker, contract)
{
	CurrentGame.taker = taker;
	CurrentGame.contract = contract;
}

/**
 * @brief This function is called when the bot must play a card
 * 
 * The counter in parameter is useful to calculate the current position
 * in the current turn and to know what are the cards played by the 
 * previous players. 
 *
 * Use this counter along with the main deck of cards to analyse the game.
 *
 * Notice that in case of a non-valid card returned, the game engine will take
 * a random valid card in the bot's deck.
 *
 * @param[in] gameCounter Current game counter, incremented at each player's turn  [1..72]
 * @return The card played, string format
 */
function PlayCard(gameCounter)
{
	// FIXME: delete gameCounter, not necessary for AI
//	debugger;

	var cardName;
	
	if (CurrentGame.taker == CurrentGame.myPlace) {
		cardName = CurrentGame.PlayAttackStrategy();
	} else {
		cardName = CurrentGame.PlayDefenseStrategy();
	}

	return cardName;
}

/**
 * @brief This function is called when any card is played (even bot's ones)
 *
 * @param[in] cardName The name of the card, string format
 * @param[in] place The place of the player's card just played
 */
function PlayedCard(cardName, place)
{
	CurrentGame.SetPlayedCard(cardName, parseInt(place));
}

/*****************************************************************************/
// JAVASCRIPT UNIT TEST OF BASE CLASSES

function SystemPrint(message)
{
//	document.write(message + "<br />");
	ScriptDebug(message + "<br />");
}




// End of file
