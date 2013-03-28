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

/*****************************************************************************/
// UTILITY FUNCTIONS AND CLASSES

function Card(cardName)
{
	var elem = cardName.split("-");
	this.value = elem[0];
	this.color = elem[1];

// Methods	
	this.GetName = function()
	{
		return (this.value + "-" + this.color);
	}
	
	// debug code only
	this.Print = function()
	{
		SystemPrint("Card value: " + this.value + " Color: " + this.color);
	}
}

function Deck()
{
	this.cards = new Array();
	
// Methods		
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
	
	this.AddOneCard = function(cardName)
	{
		cards[cards.length] = new Card(cardName);
	}
	
	this.AddCards = function(list)
	{		
		var result = list.split(/;/g);		
		for (var i=0; i<result.length; i++) {
			this.cards[i] = new Card(result[i]);
		}
	}
}

/**
 * @brief This object will store statistics on each turn
 */ 
function Turn()
{
	

}

/*****************************************************************************/
// GAME OBJECTS FOR STATISTICS STORAGE

function Player()
{
	this.hasDiamonds = false;
	this.hasSpades = false;
	this.hasHearts = false;
	this.hasClubs = false;
	this.hasTrumps = false;
	
// Methods		
	this.Initialize = function()
	{
		this.NoDiamond = false;
	}
	
	this.Print = function()
	{
		SystemPrint("Has diamonds: " + this.hasDiamonds);
	}
}

function GameState()
{
// Variables
	this.players = new Array(4);
	for(var i=0; i<this.players.length; i++) {
		this.players[i] = new Player();
	}
	
	this.deck = new Deck(); // deck of played cards
	this.place = new Array(); // who has played
	
	this.gameCounter = 0;		// [1..72] with 4 players
	this.turnCounter = 0;		// number of turns, 18 with 4 players
	this.currentPosition = 0; 	// position of the current turn
	
// Methods	
	this.Initialize = function()
	{
		for(var i=0; i<this.players.length; i++) {
			this.players[i].Initialize();
		}
		gameCounter = 0;
		deck.Clear();
	}
	
	this.Print = function()
	{
		for(var i=0; i<this.players.length; i++) {
			this.players[i].Print();
		}
	}
	
	this.CardPlayed = function(cardName, place)
	{
		
	}
}

CurrentGame = new GameState();


/*****************************************************************************/
// JAVASCRIPT UNIT TEST OF BASE CLASSES

function SystemPrint(message)
{
	document.write(message + "<br />");
}

function RunUnitTest()
{
	myCard = new Card("7-C");
	deck = new Deck();
	deck.AddCards("7-C;21-A");
	deck.Print();
	deck.Clear();
	deck.Print();
	
	CurrentGame.Initialize();
	CurrentGame.Print();
}

RunUnitTest();


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
	myDeck.AddCards(TDeck.GetBotCards());
	
	mainDeck = new Deck();
	mainDeck.AddCards(TDeck.GetMainCards());
		
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
