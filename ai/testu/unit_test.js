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

	/**
	 * These cards are in the order of a complete game
	 */
	var cardList = "13-T;15-T;19-T;21-T;10-T;20-T;3-T;5-T;7-D;9-D;11-D;14-D;11-C;8-C;13-C;4-C;9-S;6-S;12-S;11-S;5-H;13-H;8-H;12-H;16-T;6-T;12-T;4-T;8-T;6-D;9-T;1-T;1-C;7-C;14-C;9-C;14-T;3-H;7-T;2-C;11-T;12-D;2-T;10-H;7-H;4-H;6-H;1-H;3-C;12-C;5-C;6-C;5-S;14-S;13-S;0-T;3-S;10-S;18-T;7-S;5-D;3-D;8-S;10-D;9-H;14-H;4-S;2-H;17-T;1-D;2-S;1-S";
	var ownerList = "1;2;3;0;0;1;2;3;1;2;3;0;0;1;2;3;2;3;0;1;0;1;2;3;1;2;3;0;1;2;3;0;3;0;1;2;1;2;3;0;1;2;3;0;1;2;3;0;1;2;3;0;2;3;0;1;3;0;1;2;1;2;3;0;0;1;2;3;1;2;3;0";
	module.cards = cardList.split(/;/g);
	module.owners = ownerList.split(/;/g);

	module.decks = new Array(4);	// The 4 decks, one for each player
	module.players = new Array(4);  // opponents, 4 Game instances = 4 bots playing!
	for (var i=0; i<4; i++)
	{
		module.players[i] = new TarotLib.Game();
		module.decks[i] = new TarotLib.Deck();
	}

	// Example of a deal
	module.decks[0].setCards("13-H;8-C;3-S;11-C;4-D;20-T;5-D;9-H;1-H;4-S;11-S;5-S;9-T;8-S;12-T;4-H;14-C;2-H");
	module.decks[1].setCards("2-C;14-S;10-D;5-C;5-T;2-D;0-T;12-D;6-T;7-C;2-T;13-S;10-T;3-D;12-C;12-S;14-T;3-C");
	module.decks[2].setCards("10-C;7-H;12-H;21-T;13-C;1-S;15-T;3-H;10-H;19-T;8-D;6-C;11-T;7-S;4-T;14-D;13-T;8-T");
	module.decks[3].setCards("8-H;16-T;14-H;10-S;1-C;4-C;11-D;6-D;1-D;18-T;7-D;13-D;5-H;6-S;9-S;9-C;17-T;7-T");
	var dogList = "9-D;6-H;11-H;1-T;3-T;2-S";

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
		module.players[0].setPlayedCard(module.cards[i], module.owners[i]);

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
	
	// Give cards to all the players
	for (i=0; i<4; i++)
	{
		module.players[i].initialize();
		module.players[i].botPlace = i;
		module.players[i].taker = TarotLib.Place.EAST;
    	module.players[i].contract = TarotLib.Contract.PRISE;
		module.players[i].setBotCards(module.decks[i].toString());
	}

	// The take has to build its discard
	systemPrint("Dog deck: " + dogList);
	var discard = module.players[TarotLib.Place.EAST].bot.buildDiscard(dogList);
	systemPrint("Bot build a discard: " + discard);
	 
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