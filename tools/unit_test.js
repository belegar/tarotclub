/*=============================================================================
 * TarotClub - unit_test.js
 *=============================================================================
 * Unit test of artificial intelligence script files
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
 
 this.TarotLib = this.TarotLib||{};
 
/*****************************************************************************/
/**
 * Module entry
 */
var UnitTestModule = (function () {
	var module = {};
/*****************************************************************************/
	// PRIVATE MODULE DATA
	// FIXME: load string from generated file
	var cardList = "13-T;15-T;19-T;21-T;10-T;20-T;3-T;5-T;7-D;9-D;11-D;14-D;11-C;8-C;13-C;4-C;9-S;6-S;12-S;11-S;5-H;13-H;8-H;12-H;16-T;6-T;12-T;4-T;8-T;6-D;9-T;1-T;1-C;7-C;14-C;9-C;14-T;3-H;7-T;2-C;11-T;12-D;2-T;10-H;7-H;4-H;6-H;1-H;3-C;12-C;5-C;6-C;5-S;14-S;13-S;0-T;3-S;10-S;18-T;7-S;5-D;3-D;8-S;10-D;9-H;14-H;4-S;2-H;17-T;1-D;2-S;1-S";
	var ownerList = "1;2;3;0;0;1;2;3;1;2;3;0;0;1;2;3;2;3;0;1;0;1;2;3;1;2;3;0;1;2;3;0;3;0;1;2;1;2;3;0;1;2;3;0;1;2;3;0;1;2;3;0;2;3;0;1;3;0;1;2;1;2;3;0;0;1;2;3;1;2;3;0";
	var dogList = "6-C;1-C;1-S;12-D;11-C;13-H";
	
	module.cards = cardList.split(/;/g);
	module.owners = ownerList.split(/;/g);
	module.dog = dogList.split(/;/g);
	
	// opponents
	module.players = new Array(4); 
	for (var i=0; i<4; i++)
	{
		module.players[i] = new TarotLib.Game();
	}
	
/*****************************************************************************/
module.RunUnitTest = function()
{
	systemPrint("********** START UNIT TEST **********");
	
	var deck = new TarotLib.Deck();
	var stats = new TarotLib.Stats();
	
	deck.setCards("7-C;21-T;13-C;4-C;9-S;11-T;12-D;2-T;10-H");
	
	stats.update(deck);
	
	deck.print();
	deck.clear();
	deck.print();

	systemPrint(TarotLib.Contract.toString(TarotLib.Contract.GUARD));
	
	systemPrint(""); // empty string = skip line
}
/*****************************************************************************/
module.RunStatsTest = function()
{
	systemPrint("********** STATS TEST **********");
	
	module.players[0].botPlace = TarotLib.Place.SOUTH;

	module.players[0].setBotCards("12-C;4-T;11-C;4-H;6-T;15-T;2-T;1-T;18-T;12-D;8-C;9-D;2-H;10-H;7-S;3-D;13-S;5-S");
	var contract = module.players[0].bot.calculateBid();
	
	if (contract == TarotLib.Contract.PASS) {
		systemPrint("Statistics calculation success");
	} else {
		systemPrint("Statistics calculation error");
	}

	systemPrint(""); // empty string = skip line
}
/*****************************************************************************/
module.RunMissingSuitDetectionTester = function()
{
	systemPrint("\n********** MISSING SUIT DETECTION TEST **********");
	
	module.players[0].initialize();
	
	// Set game parameters
	module.players[0].botPlace = TarotLib.Place.SOUTH;
	module.players[0].taker = TarotLib.Place.EAST;
    module.players[0].contract = TarotLib.Contract.PRISE;
	
	// This step is testing the statistics builder
	for(var i=0; i<72; i++)
	{
		module.players[0].setPlayedCard(module.cards[i], parseInt(module.owners[i]));

		if (module.players[0].currentPosition == 0)
		{
			systemPrint("-- Turn " + module.players[0].trickCounter + " --");
			module.players[0].playedCards[module.players[0].trickCounter-1].print();
			module.players[0].printPlayers();
		}
	}
}
/*****************************************************************************/
module.RunFakeGame = function()
{
	systemPrint("");
	systemPrint("\n********** FAKE GAME SIMULATOR **********");

	// Opponents
	module.players[0].myPlace = TarotLib.Place.SOUTH;
	module.players[1].myPlace = TarotLib.Place.EAST;
	module.players[2].myPlace = TarotLib.Place.NORTH;
	module.players[3].myPlace = TarotLib.Place.WEST;
	
	// We send cards to the bot and the players
	var botCards = new Array(4);
	for (var i=0; i<4; i++)
	{
		botCards[i] = "";
	}
	
	for(i=0; i<72; i++)
	{	
		var owner = parseInt(module.owners[i]);
		if (botCards[owner].length != 0)
		{
			botCards[owner] += ";"
		}
		botCards[owner] += module.cards[i];
	}

	// Give cards to all the players
	for (i=0; i<4; i++)
	{
		module.players[i].initialize();
		module.players[i].botPlace = i;
		module.players[i].taker = TarotLib.Place.EAST;
    	module.players[i].contract = TarotLib.Contract.PRISE;
		module.players[i].setBotCards(botCards[i]);
	}
	
	// Let's play 18 tricks
	for (i = 0; i<18; i++)
	{
		systemPrint("---- Trick " + (i+1) + " ----")
		for (var j=0; j<4; j++)
		{
			systemPrint("Player cards: " + TarotLib.Place.toString(j));
			module.players[j].printBot();
			
			var card = module.players[j].playDefenseStrategy();
			
			systemPrint("Played card: " + card);
			systemPrint("");
			for (var k=0; k<4; k++)
			{
				module.players[k].setPlayedCard(card, j);
			}
		}
	}
};

/*****************************************************************************/
/**
 * Module end
 */ 
	return module;
}());
/*****************************************************************************/

//RunUnitTest();
//RunFakeGame();