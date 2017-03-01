# TarotClub network protocol

## Transport layer and header

TCP/IP on port 4269

## Tarot Game Notation

FIXME: describe the JSON deal format

## Protocol

### Login Server

As setup, all servers in the cluster share a large random number, known as the cluster secret(*).
  * Client connects to login server.
  * Using Diffie-Hellman key exchange, client and server negotiates keys for login.
The server issues a challenge to the client, consisting of a 128 bit random number.
Client calculates a hash of this number concatenated with the password the user enters, and supplies the hash to the server.
Server verifies that the hash of the challenge and stored password matches what the client supplied, and issues an authentication ticket consisting of user id, ticket expiration time, and hash of a cluster secret combined with these two items, and supplies to the client. This ticket may also contain the IP address of the client (part of the has as well, in that case).
Login server also generates a random key for use by this client during this sessions, and supplies to the client. It also records the key for the user, and the expiry time of the ticket.
Client connects to arbitrary server that is part of the game server cluster.
Client starts connection by sending authentication ticket.
The new server verifies that the ticket has not expired, and that the hash is correct. Using the user id in the ticket, the new server retrieves the encryption key for the user from the login server.
The new server and the client also negotiate sequence numbers for future communications at this point.
Once authenticated, the new server and client exchange data encrypted with the session key, where the encrypted data includes a hash of the data proper, and a sequence number. Each of these packets only need to have the client ID and ticket identifier (a small integer) as header, not the full authentication cookie.
Periodically, the server that the client is currently connected to checks whether the session authentication ticket is about to expire; if this is the case, re-contact login server to get a new ticket and forward it to the client.



You generally do not want to send the password over the link at all, not even with encryption. The usual method is a challenge-response protocol.

The client connects to the server, sending in the user-name (but not password)
The server responds by sending out unique random number
The client encrypts that random number using the hash of their password as the key
The client sends the encrypted random number to the server
The server encrypts the random number with the correct hash of the user's password
The server compares the two encrypted random numbers
This has a couple of advantages. First, it means the password never goes over the link in any form. Second, it's immune to a replay attack -- if an attacker records the conversation, they can't replay the client's replies later to log in, because the random number will have changed.

Securing the connection (i.e., encrypting the content) is a little simpler. Typically, one of the two (doesn't really matter much which) picks a random number, encrypts it with the other's public key, and sends it to the other. The other decrypts it, and they encrypt the rest of the session using that as a key for symmetric encryption.

Libraries: Beecrypt and OpenSSL are a couple of obvious ones. Unless you have a fairly specific reason to do otherwise, TLS is what you probably want to use (it does quite a bit more than what I've outlined above, including two-way authentication, so not only does the server know who the client is, but the client also knows who the server is, so it's reasonably verified that it's not connected to somebody else who might just collect his credit card number and run with it).

Edit:

To authenticate each packet without the overhead of encrypting everything, you could do something like this:

The server sends its public key with the challenge
The client generates a random number, encrypts it with the server's public key, and sends it back with its response
The number is the first number used for counter-mode encryption
The client includes one counter-mode result with each packet it sends
Counter mode means you just generate consecutive numbers, and encrypt each in turn, using the right key. In this case, the key would be the hash of the client's password. What this means is that each packet will contain a unique random number that both the client and the server can generate, but nobody else can. By using the counter-mode encryption, each packet will have a unique random number. By starting from a random number, each session will have a unique sequence of random numbers.

To minimize overhead, you could send just a part of the result with each packet -- e.g., if you use AES in counter mode, it'll generate 16 bytes of result for each number you encrypt. Include only (say) two bytes of that with each packet, so you only have to encrypt a number once every 8 packets. In theory, this cuts security -- an attacker could just try all 65536 possible values for a packet, but if you assume the connection has been compromised after (say) two bad attempts, the chances of an attacker getting the right value become pretty small (and, of course, you can pretty much pick the chances you're willing to live with by controlling the number of bad attempts you allow and the size of authentication you include in each packet).

### Lobby connection

Client perform a TCP connection to the game server.

Client receive a request for user login with several parameters. uuid: assigned uuid of the new client

{
	"cmd": "RequestLogin",
	"uuid": 766
} 

3. Server receive connection parameters

{
	"cmd": "ReplyLogin",
	// Player identity
	"nickname": "Belegar",
	"avatar": "http://wwww.fdshjkfjds.com/moi.jpg", 
	"gender": "Male"
}

4. Client reseive the list of available tables

{
     "cmd": "AccessGranted"
	 "tables": [
		{ "name": "Table 1", "uuid", 890},
		{ "name": "Table 2", "uuid", 891}
	]
}

### List of players, status and event

Server (lobby) --> Client

{
	"cmd": "PlayerList", 
	"counter": 3590896,
	"players": [
		{ "uuid": 37, "nickname": "Belegar", "avatar": "http://wwww.fdshjkfjds.com/moi.jpg", "gender": "Male", "table": 0, "place": "Nowhere" },
		{ "uuid": 42, "nickname": "Terminator", "table": 1000, "place": "South" },
		{ ... }
	]
}

The counter is used to synchronize the lobby users events. Client must initialize their 
counter to this value on message reception. Further server events increment this counter, if a 
client detect any missing value it must ask for a full lobby list again.


### Asynchronous events

Server (lobby) --> Client

{
	"cmd": "Event",
	"type": "Nick",
	"counter": 3590896,
	"player": {
		"uuid": 37,
		"nickname": "Belegar", 
		"avatar": "http://wwww.fdshjkfjds.com/moi.jpg",
		"gender": "Male",
		"table": 0, 
		"place": "South"
	}
}

Event types are:

  * New: new player in the lobby
  * Nick: Change of the nickname
  * Avatar: Change the avatar
  * Gender: change of the gender
  * Join: Join a table
  * Leave: Leave a table
  * Quit: quit the lobby

### Change nickame

Client can ask to the server to change the identity

{
	"cmd": "RequestChangeNickname",
	"nickname": "Zouma"
}

### Chat message

All the stats messages are always sent to the lobby.
Source indicate the UUID of the source of the message (player, Table or Lobby).
Target indicate the destination UUID of the message.

{
	"cmd": "ChatMessage", 
	"source": 27,
	"target": 789
}

### Join/Quit a table

Client send a request to the lobby with the specified table number to join

{
	"cmd": "RequestJoinTable",
	"table_id": 27
}

If there is at least one free slot around the table, the server replies:

{
	"cmd": "ReplyJoinTable",
	"players": [
        {
            "place": "South",
            "uuid": 10
        },
        {
            "place": "East",
            "uuid": 11
        },
        {
            "place": "North",
            "uuid": 12
        },
        {
            "place": "West",
            "uuid": 13
        }
    ],
	"table_id": 27,
	"place": "North",
	"size": 4	// Max number of players
}

The server sends some indication regarding the players around the table. The client is in charge to retrieve its own place 
thanks to the array of players.

Finally, the client sends an acknowledge:

{
	"cmd": "Ack",
	"step": "JoinTable"
}

in case of error, the following data is sent by the server:

{
	"cmd": "Error",
	"code": 7,
	"reason": "Table is full"
}

Client can quit a table

{
	"cmd": "RequestQuitTable",
	"table_id": 98987
}

### Deal type

{
	"type": "Random", // or "Numbered", "Custom"
	"file": "./mygame.json",  // File name (local to the server)
	"seed": 78895687790009  // deal number
}

### New game

A dedicated command must be sent to start a new game. This command can be issued from the administrator of the table or the server itself, depending 
of the game parameters.

Client (admin) --> Server (table)

{
	"cmd": "RequestNewGame",
	"mode": "QuickDeal", // or "SimpleTournament", "Custom"
	"deals": [
		... // list of deal type
	]
}

Server (table) --> Client (all)

{
	"cmd": "NewGame",
	"mode": "QuickDeal",
	"turns": 4
}

Client (all) --> Server (table)

{
	"cmd": "Ack",
	"step": "Ready"
}

### New deal

Server (table) --> Client

{
	"cmd": "NewDeal",
	"cards": "T-02;S-11; ...."
}


Client (all) --> Server (table)

{
	"cmd": "Ack",
	"step": "NewDeal"
}

### Tarot game start sequence (bid and discard)

Server (table) --> Client

{
	"cmd": "RequestBid",
	"place": "South",		// Current player
	"contract": "Guard"		// highest previous bid announced
}

Client  --> Server (table)

{
	"cmd": "ReplyBid",
	"contract": "Pass",
	"slam": false			// Set to true to announce a slam
}

Then the server broadcast to the table the announced bid for this player

Server (table) --> Client (all)

{
	"cmd": "ShowBid",
	"place": "South",
	"contract": "Pass",
	"slam": false
}

Client (all) --> Server (table)

{
	"cmd": "Ack",
	"step": "Bid"
}

If all players have passed :
Server (table) --> Client (all)

{
	"cmd": "AllPassed",
}

Client (all) --> Server (table)

{
	"cmd": "Ack",
	"step": "AllPassed"
}

Otherwise, show the dog if allowed:

Server (table) --> Client (all)

{
	"cmd": "ShowDog",
	"dog": "T02S11...."
}

Client (all) --> Server (table)

{
	"cmd": "Ack",
	"step": "Dog"
}

Then the server sends to the taker the discard order, if required:

Server (table) --> Client (taker)

{
	"cmd": "BuildDiscard",
}

Client (taker) --> Server (table)

{
	"cmd": "Discard",
	"discard": "T02S11...."
}

Then the server starts the deal

Server (table) --> Client (all)

{
	"cmd": "StartDeal",
    "taker": "North",
    "contract": "Guard",
    "slam": false
}

Client (all) --> Server (table)

{
	"cmd": "Ack",
	"step": "Start"
}

### Tarot game sequence

For the first turn where players can decalre a handle

Server (table) --> Client (player)

{
	"cmd": "AskForHandle",
}

The Client will then always answer with the handle, if any, or any invalid deck (an empty deck is fine)

Client (player) --> Server (table)

{
	"cmd": "Handle",
	"handle": "T02S11...." // or "handle": "" if no handle to set
}

In a case of an handle, it is shown to all players of the table:

Server (table) --> Client (all)

{
	"cmd": "ShowHandle",
	"place": "South",
	"handle": "T02S11...."
}

Client (all) --> Server (table)

{
	"cmd": "Ack",
	"step": "Handle"
}

After that, for each game turn, the sequence is the same. The server ask to play a card:

Server (table) --> Client (all)

{
	"cmd": "PlayCard",
	"place": "South",
}

Client (player) --> Server (table)

{
	"cmd": "Card",
	"card": "T02"
}

Server (table) --> Client (all)

{
	"cmd": "ShowCard",
	"place": "South",
	"card": "T02"
}

Client (all) --> Server (table)

{
	"cmd": "Ack",
	"step": "Card"
}

At the end of the trick, the server sends:

Server (table) --> Client (all)

{
	"cmd": "EndOfTrick",
	"place": "North" 		// winner of the trick 
}

Client (all) --> Server (table)

{
	"cmd": "Ack",
	"step": "Trick"
}

At the end of the deal:

Server (table) --> Client (all)

{
	"cmd": "EndOfDeal",
	"deal": {
	    // ... deal contents
	}
	"points", points.pointsAttack);
	"oudlers", points.oudlers);
	"little_bonus", points.littleEndianOwner.Value());
	"handle_bonus", points.handlePoints);
	"slam_bonus", points.slamDone);
	
}

Client (all) --> Server (table)

{
	"cmd": "Ack",
	"step": "Deal"
}

At the end of the game:

Server (table) --> Client (all)

{
	"cmd": "EndOfGame",
	"winner": "North"
}


