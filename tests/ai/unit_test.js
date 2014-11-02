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
 
 this.TarotLib = this.TarotLib || {};

 
/*****************************************************************************/
/**
 * Module entry
 */
var UnitTestModule = (function () {
	var module = {};
    
    var NB_PLAYERS = 4;
    
/*****************************************************************************/
	// PRIVATE MODULE DATA
	// FIXME: load string from generated file
    module.game = new TarotLib.Game();
    module.decks = [];
    
	for (var i=0; i<NB_PLAYERS; i++)
	{
		module.decks[i] = new TarotLib.Deck(); // The 4 decks, one for each player
	}

	// Example of a deal
	module.decks[0].setCards("02-H;12-D;04-H;05-C;01-D;06-D;00-T;09-H;14-D;01-H;13-C;04-T;07-S;01-T;16-T;13-S;08-D;13-T");
	module.decks[1].setCards("01-C;19-T;04-D;11-H;11-S;13-D;02-T;06-C;05-S;09-D;14-H;01-S;04-C;06-S;04-S;07-C;21-T;12-S");
	module.decks[2].setCards("10-C;15-T;10-S;11-D;11-T;08-H;02-S;03-H;12-H;05-T;09-C;08-T;10-H;05-H;09-S;08-S;03-D;18-T");
	module.decks[3].setCards("07-H;02-C;14-S;08-C;05-D;03-S;03-C;06-T;03-T;09-T;14-T;14-C;17-T;20-T;02-D;11-C;10-D;07-T");
	var dogList = "13-H;10-T;07-D;12-T;12-C;06-H";
    
/*****************************************************************************/
module.HtmlHeader = function()
{
	systemPrint("<html><head><link href=\"style.css\" rel=\"stylesheet\" type=\"text/css\"></head><body>");
};
/*****************************************************************************/
module.HtmlFooter = function()
{
	systemPrint("</body></html>");
};

/*****************************************************************************/
module.RunCardAndDeckTests = function()
{
	systemPrint("********** TEST_1: Card and Deck class **********");
	
	var deck = new TarotLib.Deck();
	var stats = new TarotLib.Stats();
	  
    deck.setCards("01-T;09-C;13-D;05-D;09-S;03-S");
	
    if (deck.size() != 6)
    {
        systemPrint("################ Error 1 ##############");
    }
    
    deck.addCards("13-H;10-T;07-D;12-T;12-C;06-H");
    
    if (deck.size() != 12)
    {
        systemPrint("################ Error 2 ##############");
    }
    
	deck.printHtml();
	deck.clear();
    
    if (deck.size() != 0)
    {
        systemPrint("################ Error 3 ##############");
    }
    
	deck.printHtml();

	systemPrint(TarotLib.Contract.toString(TarotLib.Contract.GUARD));
	systemPrint(""); // empty string = skip line
	
	deck.clear();
	deck.makeTarotDeck();
	deck.printHtml();
};

/*****************************************************************************/
module.RunStatsTest = function()
{
	systemPrint("********** TEST_2: statistics **********");
	
	module.game.botPlace = TarotLib.Place.SOUTH;

	module.game.setBotCards("12-C;4-T;11-C;4-H;6-T;15-T;2-T;1-T;18-T;12-D;8-C;9-D;2-H;10-H;7-S;3-D;13-S;5-S");
	var contract = module.game.bot.calculateBid();
	
	if (contract == TarotLib.Contract.PASS)
	{
		systemPrint("Statistics calculation success");
	}
	else
	{
		systemPrint("Statistics calculation error");
	}

	systemPrint(""); // empty string = skip line
};
/*****************************************************************************/
module.RunDiscardTest = function()
{
	systemPrint("********** TEST_3: discard **********");

	module.game.setBotCards("01-C;19-T;04-D;11-H;11-S;13-D;02-T;06-C;05-S;09-D;14-H;01-S;04-C;06-S;04-S;07-C;21-T;12-S");

	systemPrint("Deck before: ");
	module.game.bot.deck.printHtml();

	var discard = new TarotLib.Deck();
	discard.setCards(module.game.bot.buildDiscard("13-H;10-T;07-D;12-T;12-C;06-H"));

	systemPrint("Deck after: ");
	module.game.bot.deck.printHtml();

	systemPrint("Discard: ");
	discard.printHtml();
};
/*****************************************************************************/
module.RunMissingSuitDetectionTester = function()
{
	systemPrint("\n********** MISSING SUIT DETECTION TEST **********");
	
	module.game.initialize();
	
	// Set game parameters
	module.game.botPlace = TarotLib.Place.SOUTH;
	module.game.taker = TarotLib.Place.EAST;
    module.game.contract = TarotLib.Contract.PRISE;
	
    var tricks = [
        "19-T;10-T;21-T;17-T",
        "01-S;02-S;05-S;03-S",
        "10-S;04-S;11-S;13-S",
        "20-T;01-T;18-T;11-T",
        "00-T;14-T;15-T;16-T",
        "07-T;03-T;13-T;05-T",
        "06-T;12-S;07-S;06-S",
        "13-D;06-D;08-D;14-D",
        "12-T;14-S;08-T;09-T",
        "13-C;06-C;09-C;14-C",
        "14-H;09-H;06-H;03-H",
        "01-H;10-C;13-H;04-H",
        "11-H;05-C;10-H;07-H",
        "12-C;11-D;08-C;04-C",
        "02-C;03-D;05-H;11-C",
        "03-C;05-D;04-D;01-C",
        "07-D;04-T;02-D;01-D",
        "08-H;10-D;09-D;02-T"
    ];
    
	// This step is testing the statistics builder
	for(var i=0; i<18; i++)
	{
        var trick = new TarotLib.Deck();
        trick.setCards(tricks[i]);
        for (var j = 0; j < 4; j++)
        {
            module.game.setPlayedCard(trick.get(j).getName(), j);

            if (module.game.currentPosition == 0)
            {
                systemPrint("-- Turn " + module.game.trickCounter + " --");
                module.game.playedCards[module.game.trickCounter-1].printHtml();
                module.game.printPlayers();
            }
        }
	}
};
/*****************************************************************************/
module.RunBotFunctionsTest = function()
{
	systemPrint("********** TEST_4: bot algorithms **********");

	module.game.setBotCards("01-C;19-T;04-D;11-H;11-S;13-D;02-T;06-C;05-S;09-D;14-H;01-S;04-C;06-S;04-S;07-C;21-T;12-S;00-T;01-T");
	module.game.bot.deck.printHtml();

	systemPrint("Highest club: " + module.game.bot.getHighestCard("C"));
	systemPrint("Highest spade: " + module.game.bot.getHighestCard("S"));
	systemPrint("Highest diamond: " + module.game.bot.getHighestCard("D"));
	systemPrint("Highest heart: " + module.game.bot.getHighestCard("H"));
	systemPrint("Highest trump: " + module.game.bot.getHighestCard("T"));
	systemPrint("Highest trump just over 18 (must be 19-T): " + module.game.bot.getHighestCard("T", 18));

	systemPrint("Lowest club: " + module.game.bot.getLowestCard("C", 0));
	systemPrint("Lowest spade: " + module.game.bot.getLowestCard("S", 0));
	systemPrint("Lowest diamond: " + module.game.bot.getLowestCard("D", 0));
	systemPrint("Lowest heart: " + module.game.bot.getLowestCard("H", 0));
	systemPrint("Lowest trump: " + module.game.bot.getLowestCard("T", 0));

	systemPrint("Lowest trump higher than 1: " + module.game.bot.getLowestCard("T", 2));
	systemPrint("Lowest trump higher than 2: " + module.game.bot.getLowestCard("T", 3));
};
/*****************************************************************************/
module.AiUseCasesTest = function()
{
	systemPrint(""); // skip line
	systemPrint("********** TEST_5: AI use case: play low card if the taker has cut the trick suit **********");
	
	// Use Case #1:
	// je suis le preneur, on joue coeur, je coupe et le mec derrière moi (le dernier à jouer) met un roi alors qu'il lui reste du coeur à jouer (ils le font // souvent ça, le sacrifice des grosses cartes...)

	module.game.initialize();
	
	// Set game parameters
	module.game.botPlace = TarotLib.Place.SOUTH;
	module.game.taker = TarotLib.Place.WEST;
	
	// Give only hearts to the player
	module.game.bot.deck.setCards("02-H;12-H;14-H;05-H;01-H");

	// First player (east) plays hearts
	module.game.setPlayedCard("03-H", 1);
	// Second player (north) plays hearts also
	module.game.setPlayedCard("04-H", 2);
	// The taker (west), cut the suit
	module.game.setPlayedCard("02-T", 3);
	
	var cardName = module.game.playDefenseStrategy();
	systemPrint("Played card (must be 01-H): " + cardName);
	
	systemPrint(""); // skip line
	systemPrint("********** TEST_6: AI use case: Play high card if the defence is leader **********");
	
	module.game.initialize();
	
	// Set game parameters
	module.game.botPlace = TarotLib.Place.SOUTH;
	module.game.taker = TarotLib.Place.WEST;
	
	// Give some cards to the player, and a fool
	module.game.bot.deck.setCards("02-H;12-H;13-H;05-H;01-H;00-T");

	// First player (east) plays hearts
	module.game.setPlayedCard("03-H", 1);
	// Second player (north) plays hearts also, he is the leader
	module.game.setPlayedCard("14-H", 2);
	// The taker (east), plays a low card
	module.game.setPlayedCard("04-H", 3);
	
	var cardName = module.game.playDefenseStrategy();
	systemPrint("Played card (must be 13-H): " + cardName);
	
	systemPrint(""); // skip line
	systemPrint("********** TEST_7: AI use case: Play the fool when possible to avoid playing it at the last turn **********");
	
	// Encore une excuse en fin de jeu (mais il m'est donné a priori)
	
	module.game.initialize();
	
	// Set game parameters
	module.game.botPlace = TarotLib.Place.SOUTH;
	module.game.taker = TarotLib.Place.WEST;
	
	// Give some cards to the player, and a fool
	module.game.bot.deck.setCards("02-H;12-H;13-H;05-H;01-H;00-T");

	// First player (east) plays hearts
	module.game.setPlayedCard("03-H", 1);
	// Second player (north) plays hearts also
	module.game.setPlayedCard("09-H", 2);
	// The taker (east), plays a high card, he is the leader
	module.game.setPlayedCard("14-H", 3);
	
	var cardName = module.game.playDefenseStrategy();
	systemPrint("Played card (must be 00-T): " + cardName);
	
	systemPrint(""); // skip line
	systemPrint("********** TEST_8: Try to save the one of trump of a defence partner **********");
	
	// De même qu'ils n'essaient jamais de jouer leur 21 pour essayer de sauver le 1 d'un equipier.
	
	module.game.initialize();
	
	// Set game parameters
	module.game.botPlace = TarotLib.Place.SOUTH;
	module.game.taker = TarotLib.Place.WEST;
	
	// Give some cards to the player
	module.game.bot.deck.setCards("16-T;10-T;05-T;18-T;00-T");

	// First player (east) plays trump
	module.game.setPlayedCard("02-T", 1);
	// Second player (north) plays the little of trump
	module.game.setPlayedCard("01-T", 2);
	// The taker (east), try to capture the little of trump
	module.game.setPlayedCard("14-T", 3);
	
	var cardName = module.game.playDefenseStrategy();
	systemPrint("Played card (must be 16-T): " + cardName);
	
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