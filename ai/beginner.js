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
	this.deck = new Deck();
	
/********* Methods *********/
	this.Initialize = function()
	{
		for (var i=0; i<this.hasSuits.length; i++) {
			this.hasSuits[i] = true;
		}
		this.deck.Clear();
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
	
	/**
	 * @brief Retreive a card in the players deck. Replaced by an empty slot
	 */
	this.GetCard = function(index)
	{
		var card = this.deck[index];
		this.deck[index] = undefined;
		return card;
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
	
	// Game variables
	this.contract;
	this.taker;
	this.myPlace;
	
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
	
	this.PlayDefenseStrategy = function()
	{
		var myDeck = new Deck();
		
		// Retreive my cards
		myDeck.SetCards(TDeck.GetBotCards());
		
		// just play the first card available ... (let the game engine take a valid card if necessary)
		return myDeck.cards[0].GetName();
	}
	
	this.PlayAttackStrategy = function()
	{
	
	
	}
	
	this.GetRandomCard = function(place)
	{
		for (var i=0; this.players[place].deck.length; i++) {
			var card = this.players[place].GetCard(i);
			if (card != undefined) {
				card.Print();
				//this.IsValid();
			}
		}
	}

	this.IsValid = function(place, card)
	{
		return true;
	}
	
/*		
		if (card.color)
	
		bool Player::canPlayCard( Deck *mainDeck, Card *cVerif , int gameCounter, int nbPlayers )
{
    int debut;
    int rang;
    debut = (gameCounter-1)/nbPlayers;
    debut = debut*nbPlayers;

    rang = gameCounter - debut;

    // Le joueur entame le tour, il peut commencer comme il veut
    if( rang == 1 ) {
        return true;
    }

    // on traite un cas simple
    if( cVerif->getType() == EXCUSE ) {
        return true;
    }

    // premi�re carte (couleur demand�e)
    CardType type;
    CardColor   coul;
    //////////////

    bool possedeCouleur=false; // vrai si le joueur posse�de la couleur demandee
    bool possedeAtout=false;   // vrai si le joueur poss�de un atout
    bool precedentAtout=false; // vrai si un atout max pr�c�dent
    int  precAtoutMax=0;
    int  atoutMax=0; // atout maxi poss�d�
    bool ret=true;
    Card *c;
    int i,n;

    // on cherche la couleur demand�e
    c = mainDeck->at( debut ); // premi�re carte jou�e � ce tour

    type = c->getType();
    coul = c->getColor();
    if( type == EXCUSE ) { // a�e, le joueur a commenc� avec une excuse
        // le joueur peut jouer n'importe quelle carte apr�s l'excuse, c'est lui qui d�cide alors de la couleur
        if( rang == 2 ) {
            return true;
        }
        c = mainDeck->at( debut + 1 ); // la couleur demand�e est donc la seconde carte
        type = c->getType();
        coul = c->getColor();
    }

    if( type == CARTE && cVerif->getColor() == coul ) {
        return true;
    }

    // Quelques indications sur les plis pr�c�dents
    // On regarde s'il y a un atout
    for( i=0; i<rang-1; i++ ) {
        c = mainDeck->at( debut + i );
        if( c->getType() == ATOUT ) {
            precedentAtout = true;
            if( c->getValue() > precAtoutMax ) {
                precAtoutMax = c->getValue();
            }
        }
    }

    // Quelques indications sur les cartes du joueur
    n = myDeck.count();

    for( i=0; i<n; i++ ) {
        c = myDeck.at( i );

        if( c->getType() == ATOUT ) {
            possedeAtout=true;
            if( c->getValue() > atoutMax ) {
                atoutMax = c->getValue();
            }
        }
        else if( c->getType() == CARTE ) {
            if( c->getColor() == coul ) {
                possedeCouleur = true;
            }
        }
    }

    // cas 1 : le type demand� est ATOUT
    if( type == ATOUT ) {
        if( cVerif->getType() == ATOUT ) {
            if( cVerif->getValue() > precAtoutMax ) {
                return true;
            } else {
                if( atoutMax > precAtoutMax ) {
                    return false;  // doit surcouper !
                } else {
                    return true;   // sinon tant pis, on doit quand m�me jouer la couleur demand�e
                }
            }
        } else {
            if( possedeAtout == true ) {
                return false;
            } else {
                return true;
            }
        }
    } else {// cas 2 : le type demand� est CARTE
        // le joueur poss�de la couleur demand�e, il doit donc la jouer cela
        if( possedeCouleur == true ) {
            return false;
        } else { // pas la couleur demand�e
            if( cVerif->getType() == ATOUT ) {
                // doit-il surcouper ?
                if( precedentAtout==true ) {
                    if( cVerif->getValue() > precAtoutMax ) { // carte de surcoupe
                        return true;
                    } else {
                        // a-t-il alors un atout plus fort ?
                        if( atoutMax > precAtoutMax ) {
                            return false; // oui, il doit donc surcouper
                        } else {
                            return true;
                        }
                    }
                }
            } else {
                // a-t-il un atout pour couper ?
                if( possedeAtout == true ) {
                    return false; // oui, il DOIT couper
                } else {
                    return true; // non, il peut se d�fausser
                }
            }
        }
    }
    return ret;
}
	*/
	
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
 * @brief This function is called when we receive our cards for the new deal
 *
 * @param[in] cards The list of cards, string format
 */
function ReceiveCard(cards)
{
	// FIXME: store cards into memory
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

/*****************************************************************************/
// JAVASCRIPT UNIT TEST OF BASE CLASSES

function SystemPrint(message)
{
	//document.write(message + "<br />");
	//print(message + "<br />");
}



// End of file
