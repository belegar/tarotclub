/*=============================================================================
 * TarotClub - unit_test.js
 *=============================================================================
 * Unit test of artificial intelligence script files
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
/**
 * Module entry
 */
var UnitTestModule = (function () {
	var module = {};
/*****************************************************************************/
	// PRIVATE MODULE DATA
	// FIXME: load string from generated file
	var cardList = "13-A;15-A;19-A;21-A;10-A;20-A;3-A;5-A;7-D;9-D;11-D;14-D;11-C;8-C;13-C;4-C;9-S;6-S;12-S;11-S;5-H;13-H;8-H;12-H;16-A;6-A;12-A;4-A;8-A;6-D;9-A;1-A;1-C;7-C;14-C;9-C;14-A;3-H;7-A;2-C;11-A;12-D;2-A;10-H;7-H;4-H;6-H;1-H;3-C;12-C;5-C;6-C;5-S;14-S;13-S;0-A;3-S;10-S;18-A;7-S;5-D;3-D;8-S;10-D;9-H;14-H;4-S;2-H;17-A;1-D;2-S;1-S";
	var ownerList = "1;2;3;0;0;1;2;3;1;2;3;0;0;1;2;3;2;3;0;1;0;1;2;3;1;2;3;0;1;2;3;0;3;0;1;2;1;2;3;0;1;2;3;0;1;2;3;0;1;2;3;0;2;3;0;1;3;0;1;2;1;2;3;0;0;1;2;3;1;2;3;0";
	var dogList = "6-C;1-C;1-S;12-D;11-C;13-H";
	
	module.cards = cardList.split(/;/g);
	module.owners = ownerList.split(/;/g);
	module.dog = dogList.split(/;/g);
	
	module.players = function()
	{
		this.deck = new Array(3);
		
		for (var i=0; i<3; i++) {
			this.deck[i] = new Deck();
		}
	}
	
/*****************************************************************************/
module.RunUnitTest = function()
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
/*****************************************************************************/
module.RunStatsTester = function()
{
	SystemPrint("\n********** STATISTICS TEST **********");
	CurrentGame.Initialize();
	
	// Set game parameters
	EnterGame(Place.SOUTH);
	StartGame(Place.EAST, Contract.PRISE);
	
	// This step is testing the statistics builder
	for(var i=0; i<72; i++) {
		PlayedCard(module.cards[i], module.owners[i]);
		if (CurrentGame.currentPosition == 0) {
			SystemPrint("-- Turn " + CurrentGame.trickCounter + " --");
			CurrentGame.playedCards[CurrentGame.trickCounter-1].Print();
			CurrentGame.PrintPlayers();
		}
	}
}
/*****************************************************************************/
module.RunFakeGame = function()
{
	SystemPrint("");
	SystemPrint("\n********** FAKE GAME SIMULATOR **********");

	CurrentGame.Initialize();
	
	// Set game parameters
	EnterGame(Place.SOUTH);
	StartGame(Place.EAST, Contract.PRISE);
	
	// We send cards to the bot and the players
	var botCards = "";
	for(var i=0; i<72; i++) {
		var owner = parseInt(module.owners[i]);
		if (owner == Place.SOUTH) {
			if (botCards.length != 0) {
				botCards += ";"
			}
			botCards += module.cards[i];
		}
	}
	ReceiveCards(botCards);
	
	// We play all the turns
	for (var i=0; i<18; i++) {
		
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

/*****************************************************************************/
/**
 * Module end
 */ 
	return module;
}());
/*****************************************************************************/

//RunUnitTest();
//RunFakeGame();