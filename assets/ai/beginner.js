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

var CurrentGame = new TarotLib.Game();

// ****************************************************************************
// TAROTCLUB API CALLED BY THE C++ world
// The functions are called in the order of definition in this file
// All the parameters are strings
// ****************************************************************************

/**
 * @brief This function is called when the player has been accepted around the table
 *
 * @param[in] place The place assigned by the server around the table
 */
function EnterGame(place, mode)
{
    CurrentGame.botPlace = TarotLib.Place.fromString(place);
}

/**
 * @brief This function is called when the cards are dealt to players
 *
 * @param[in] cards Cards of the bots (18 for 4 players), string format
 */
function ReceiveCards(cards)
{
    CurrentGame.initialize();
    CurrentGame.setBotCards(cards);
}

/**
 * @brief This function is called when the bot must declare a bid
 *
 * @return The bid announced, string format
 */
function AnnounceBid()
{
    var cont = TarotLib.Contract.toString(CurrentGame.bot.calculateBid());
    systemPrint("The bot " + TarotLib.Place.toString(CurrentGame.botPlace) + " is announcing bid: " + cont);

	return cont;
}

/**
 * @brief This function is called to ask if the bot wants to declare a slam
 *
 * @return true if the bot declares a slam, otherwise false, Boolean format
 */
function AnnounceSlam()
{
    return false;
}

/**
 * @brief This function is called for the bot to build the discard
 *
 * @param[in] dog Dog deck, string format
 * @return the discard, string format
 */
function BuildDiscard(dog)
{
    var discard = CurrentGame.bot.buildDiscard(dog);
    var place = TarotLib.Place.toString(CurrentGame.botPlace);

    systemPrint(place + " discard is: " + discard);
    return discard;
}

/**
 * @brief This function is called to indicate a new game
 *
 * Informational purpose, the script should memorize the arguments for later use
 *
 * @param[in] taker Indicates the place of the taker (string format)
 * @param[in] contract Indicates the contract announced by the taker (string format)
 */
function StartDeal(taker, contract)
{
    CurrentGame.taker = TarotLib.Place.fromString(taker);
    CurrentGame.contract = TarotLib.Contract.fromString(contract);
}

/**
 * @brief This function is called by the server to ask for a declared handle
 *
 * @return The declared handle, or an empty string if there is no handle to declare
 */
function AskForHandle()
{
    // FIXME: manage handle declaration
    return "";
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
function PlayCard()
{
    var cardName;

    if (CurrentGame.taker === CurrentGame.botPlace)
    {
        systemPrint("Play ATTACK!");
        cardName = CurrentGame.playAttackStrategy();
    }
    else
    {
        systemPrint("Play DEFENSE!");
        cardName = CurrentGame.playDefenseStrategy();
    }

    var place = TarotLib.Place.toString(CurrentGame.botPlace);
    systemPrint(place + " is playing " + cardName);
    systemPrint(place + " script deck is: " + CurrentGame.bot.deck.toString());

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
    CurrentGame.setPlayedCard(cardName, TarotLib.Place.fromString(place));
}


// End of file
