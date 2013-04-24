/*=============================================================================
 * TarotClub - beginner.js
 *=============================================================================
 * Beginner's level artificial intelligence
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * This file must be used under the terms of the CeCILL.
 * This source file is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at
 * http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
 *
 *=============================================================================
 */

/*****************************************************************************/
// GLOBAL DEFINITIONS
 
var BidValue = {
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
	this.HasSuits = new Array(5);
	
/********* Methods *********/
	this.Initialize = function()
	{
		for (var i=0; i<this.HasSuits.length; i++) {
			this.HasSuits[i] = true;
		}
	}
	
	this.Print = function()
	{
		var place = PlaceToString(this.position);
		for (var i=0; i<this.HasSuits.length; i++) {
			if (this.HasSuits[i] == false) {
				SystemPrint("Player " + place + " has a missed suit: " + SuitsToString(i));
			}
		}
	}
	
	this.SetMissedColor = function(color)
	{
		this.HasSuits[SuitsToInteger(color)] = false;
	}
	
	// return true if there is at least one suit missing, including trumps
	this.HasMissedSuit = function()
	{
		var missed = false;
		
		for (var i=0; i<this.HasSuits.length; i++) {
			if (this.HasSuits[i] == false) {
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
// Variables
	this.players = new Array(4);
	for(var i=0; i<this.players.length; i++) {
		this.players[i] = new Player(i); // position of players is assigned here
		this.players[i].Initialize();
	}
	
	this.playedCards = new Array(18); // turns for 4 players
	for(var i=0; i<18; i++) {
		this.playedCards[i] = new Deck();
	}
	
	this.turnCounter = 0;		// number of turns, 18 with 4 players
	this.currentPosition = 0; 	// position of the current turn [0..3] with 4 players
	
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
			this.trickColor = this.playedCards[this.turnCounter].cards[0].color;
			this.firstCardValue = this.playedCards[this.turnCounter].cards[0].value;
			
			// special case of excuse
			if ((this.trickColor == "A") && (this.firstCardValue == "0")) {
				this.startedWithExcuse = true;			
			}
		} else if ((this.currentPosition == 1) && (this.startedWithExcuse == true)) {
			this.trickColor = this.playedCards[this.turnCounter].cards[1].color;
		} else {
			if (this.playedCards[this.turnCounter].cards[this.currentPosition].color != this.trickColor) {
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
		this.turnCounter = 0;
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
		this.playedCards[this.turnCounter].AddOneCard(cardName, place);
	
		this.AnalyzeTrick(place);
	
		this.currentPosition++;
		if (this.currentPosition >= 4) {
			this.currentPosition = 0;
			this.turnCounter++;
		}
	}
}

var CurrentGame = new GameState();


/*****************************************************************************/
// JAVASCRIPT UNIT TEST OF BASE CLASSES

function SystemPrint(message)
{
	document.write(message + "<br />");
	//print(message + "<br />");
}

function RunUnitTest()
{
	SystemPrint("********** START UNIT TEST **********");
	myCard = new Card("7-C");
	deck = new Deck();
	deck.SetCards("7-C;21-A");
	deck.Print();
	deck.Clear();
	deck.Print();
	
	SystemPrint(""); // empty string = skip line
}

function CreateFakeGame()
{
	// FIXME: load string from generated file
	var cardList = "13-A;15-A;19-A;21-A;10-A;20-A;3-A;5-A;7-D;9-D;11-D;14-D;11-C;8-C;13-C;4-C;9-S;6-S;12-S;11-S;5-H;13-H;8-H;12-H;16-A;6-A;12-A;4-A;8-A;6-D;9-A;1-A;1-C;7-C;14-C;9-C;14-A;3-H;7-A;2-C;11-A;12-D;2-A;10-H;7-H;4-H;6-H;1-H;3-C;12-C;5-C;6-C;5-S;14-S;13-S;0-A;3-S;10-S;18-A;7-S;5-D;3-D;8-S;10-D;9-H;14-H;4-S;2-H;17-A;1-D;2-S;1-S";
	var ownerList = "1;2;3;0;0;1;2;3;1;2;3;0;0;1;2;3;2;3;0;1;0;1;2;3;1;2;3;0;1;2;3;0;3;0;1;2;1;2;3;0;1;2;3;0;1;2;3;0;1;2;3;0;2;3;0;1;3;0;1;2;1;2;3;0;0;1;2;3;1;2;3;0";
	var dogList = "6-C;1-C;1-S;12-D;11-C;13-H";
	
	this.cards = cardList.split(/;/g);
	this.owners = ownerList.split(/;/g);
	this.dog = dogList.split(/;/g);
	
	SystemPrint("\n********** NEW GAME SIMULATOR **********");
	CurrentGame.Initialize();
	
	// Send the cards to the AI engine
	for(var i=0; i<72; i++) {
		CurrentGame.SetPlayedCard(this.cards[i], parseInt(this.owners[i]));
		if (CurrentGame.currentPosition == 0) {
			SystemPrint("-- Turn " + CurrentGame.turnCounter + " --");
			CurrentGame.playedCards[CurrentGame.turnCounter-1].Print();
			CurrentGame.PrintPlayers();
		}
	}
	
	this.ReadCSV = function(locfile)
	{
		// load a whole csv file, and then split it line by line
		var req = new XMLHttpRequest();
		req.open("POST", locfile, false);
		req.send("");
		return req.responseText.split(/\n/g);
	}
}

function RunFakeGame()
{
	this.fakeGame = new CreateFakeGame();
}

RunUnitTest();
RunFakeGame();



/*****************************************************************************/
// TAROTCLUB API CALLABLE FROM C++ WORLD

/**
 * @brief This function is called when any card is played (even bot's ones)
 *
 * @param[in] cardName The name of the card, string format
 * @param[in] place The place of the player's card just played
 */
function PlayedCard(cardName, place)
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
//	debugger;

    myDeck = new Deck();
	myDeck.SetCards(TDeck.GetBotCards());
	
	mainDeck = new Deck();
	mainDeck.SetCards(TDeck.GetMainCards());
		
	return name = myDeck.cards[0].GetName();
}

/**
 * @brief This function is called when the bot must declare a bid
 * 
 * @return The bid announced, use the definition forrmat
 * @see Variable 'BidValue'
 */
function CalculateBid()
{
   total = 0;
   
   print("The bot is announcing a bid.");
   
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
      cont = BidValue.PASSE;
   } else if( total >= 36  && total <= 50 ) {
      cont = BidValue.PRISE;
   } else if( total >= 51  && total <= 65 ) {
      cont = BidValue.GARDE;
   } else if( total >= 66  && total <= 75 ) {
      cont = BidValue.GARDE_SANS;
   } else {
      cont = BidValue.GARDE_CONTRE;
   }
   return cont;
}

// End of file
