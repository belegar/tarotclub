<div class="container">	        
    <div class="row">
        <div class="col-md-3">

            {{> doc_topics}}
	
        <div class="col-md-9">
			<h1>AI script development</h1>
			<hr>
            <article>

	<h2>Introduction</h2>

	<p>
	The TarotClub client executable embeds a JavaScript engine. It is used to execute AI script files developed using this programming language. It is the same than the one for the Web except that some global objects specific to the browser are not available such as "window". This JavaScript engine follows the ECMA Script standard 5.1.
	</p>
	<img class="img-responsive" src="/doc/js_engine.png" />

	<p>
	The standard game installation provides an example of AI scripts. These files are located in the TarotClub directory and can be freely modified. Beware, any update of the game will probably erase any modification. Thus, it is recommended to actually copy the "ai" directory tree in another location and work from there 
	for any modification. It is possible to setup a new script path for the bots in the TarotClub option window.
	</p>

	<h2>Cards and deck format</h2>

	<p>
	The Tarot cards are represented internally using a string format. This format is used by the TarotEngine and by the JavaScript IA scripts.
	One card is represented as follow:
	</p>

	<pre>
	XX-Y
	</pre>

	<ul>
		<li>XX is the card facial value on two digits with a left leading zero for values inferior to ten. Fool is equal to zero, court cards  follow the ten in their standard order (Jack = 11, knight = 12, queen = 13, king = 14)</li>
		<li>Y is the suit represented by the first suit letter (Trumps = T, Spades = S, Diamonds = D, Hearts = H, Clubs = C)</li>
	</ul>

	<p>
	Examples:
	</p>

	<pre>
	7 of trumps is 07-T
	queen of spades is 13-S
	Fool of trumps is 00-T
	One of diamonds is 01-D
	</pre>

	<p>
	A deck of cards is a character string of cards separated by semi-colons. Example:
	</p>

	<pre>
	10-S;12-D;01-S;05-T;11-D;13-C;12-H;04-H;07-C;04-T;03-S;02-T;11-H;13-D;08-D;09-D;12-C;10-D
	</pre>

	<h2>Configuration file</h2>

	<p>
	An AI script can be programmed in one or more JavaScript files. It is recommended to use several files in order to split your game 
	elements into logic parts as you would do in every software programming.
	</p>

	<p>All the files must be located in the same directory. An AI directory must contains at least two mandatory files:
	</p>

	<ul>
		<li><b>package.json</b>: A JSON format configuration file that mainly lists the JavaScript source file names and path of the AI package</li>
		<li><b>main.js</b>: the main JavaScript file, entry point of the API between the TarotClub executable world and the JavaScript world</li>
	</ul>

	<p>All other files can be populated in one or more sub directories.
	Here is an example of a configuration file:
	</p>

	<pre>
	{
	    "level": "noob",
	    "files": [
	        "tarotlib/system.js",
	        "tarotlib/util.js",
	        "tarotlib/card.js",
	        "tarotlib/deck.js",
	        "tarotlib/player.js",
	        "tarotlib/bot.js",
	        "tarotlib/game.js",
	        "main.js"
	    ]
	}
	</pre>

	<p>
	The two mandatory nodes are the "level" and "files" ones. The "files" node is an array than contains all the JavaScript files that 
	must be loaded during the initialization of the game. The list order is important because of the lack of external include facility in JavaScript. Indeed, the first file indicated in the array is loaded first.
	</p>

	<p>
	The path of the file must be relative to the root of the configuration file.
	</p>

	<p>
	When uploading an AI archive on the website, all the files must be located at the root of the archive, without any top directory.
	</p>
	
	<p>
	An example of an AI archive file can be <a href="/downloads/ai.zip">downloaded here</a>.
	</p>

	<h2>How to debug your AI scripts</h2>

	<h3>Using a web browser environment</h3>

	<p>
	Thanks to the JavaScript language, it is possible to call your script IA from a web browser such as Firefox or Chrome. These web browsers include some 
	wonderful developer tools to debug your script step by step and inspect every variable in memory.
	</p>

	<p>The only thing you will have to to is to create a fake tarot engine environment and call the Game API functions manually. An example of such a test 
	environment is available within the TarotClub source code in the "tests" root directory, accessible through the BitBucket project source code browser
	 <a href="https://bitbucket.org/tarotclub/tarotclub/src" target="_blank">here</a>. </p>

	<h3>Using a command line bot client</h3>

	<p>A command line stand alone executable named "botclient" is available in every TarotClub installation directory starting with the 2.5.3 version.</p>
	<p>This executable includes the TarotClub network protocol and is able to connect easily to any TarotClub server.</p>

	<p>Command line arguments are:</p>

	<pre>
	Usage: botclient.exe [options] -c xx.xx.xx.xx config.json
	Options are:
	        -h      Print this help
	        -p      Specify the server TCP port number
	        -t      Specify the table number to join

	Mandatory arguments are:
	        -c      Connect to the specified server IP address

	The filename is a json configuration file for bots and must be passed as a last argument.
	</pre>

	<p>Example of call (the bot will join table 1): botclient.exe -c 5.196.29.170 bots.json</p>

	<p>The bot configuration file contains a JSON array. Each entry is a description of a bot identity. You can specify a location where to find you IA 
	script package file. If you specify an avatar image file, take care to use an Internet address so that the image can be fetched by the remote server.</p>

	<p>Example of a bot configuration JSON file, with two bots defined:</p>

	<pre>
	[
		{
	        "name": "Robocop",
	        "avatar": "http://tof.canardpc.com/view/e36c718a-b72c-46e9-96be-a6e0c549d422.jpg",
	        "gender": "male",
			"bot_file_path": "../assets/ai"
	    },
		{
	        "name": "R2D2",
	        "avatar": "",
	        "gender": "male",
			"bot_file_path": "example.zip"
	    }
	]

	</pre>

	<h3>Using the desktop client</h3>

	<p>One other way to test your AI scripts is to change the bots parameters in the options window to point to your own script package</p>

	<p>Simply specify in the field the path to your script package file.</p>

	<img class="img-responsive" src="/doc/bot_script_option.png" />

	<h2>Game API</h2>

	<p>
	The communication between the TarotClub engine is very simple. The game engine will call pre-defined functions implemented in the JavaScript world. Some functions can have arguments, most of the time passed in string format, and some of the API functions must return something (such as the card played).
	</p>

	<p>Here is the complete API reference. The functions are sorted by called order.</p>

	<div class="panel panel-primary">
		<div class="panel-heading">
		<h3 class="panel-title">EnterGame(place, mode)</h3>
		</div>
		<div class="panel-body">
			<p>This function is the first one called when the player enter the game around the table. It can be used to perform one time initializations.</p>
		</div>
		
		<ul class="list-group">
			<li class="list-group-item"><b>Place:</b> the assigned place by the server around the table, string format. Values are:<br />
				<ul>
					<li>South</li>
					<li>North</li>
					<li>West</li>
					<li>East</li>
					<li>Fifth (not supported, reserved for future use)</li>
				</ul>
			</li>
			<li class="list-group-item"><b>Mode:</b> the current game mode. Possible values are: <br />
				<ul>
					<li>one_deal</li>
					<li>tournament</li>
				</ul>
			</li>
		</ul>
	</div>


	<div class="panel panel-primary">
		<div class="panel-heading">
		<h3 class="panel-title">ReceiveCards(cards)</h3>
		</div>
		<div class="panel-body">
			<p>This function is called when the cards are dealt to players.</p>
		</div>
		
		<ul class="list-group">
			<li class="list-group-item"><b> cards:</b> Cards of the bot (18 cards for a 4 players game), string format</li>
		</ul>
	</div>


	<div class="panel panel-primary">
		<div class="panel-heading">
		<h3 class="panel-title">AnnounceBid()</h3>
		</div>
		<div class="panel-body">
			<p>This function is called when the bot must declare a bid.</p>
		</div>
		
		<ul class="list-group">
			<li class="list-group-item"><b>Return value: </b> Bid announced, string format. Possible values are:
				<ul>
					<li>Pass</li>
					<li>Take</li>
					<li>Guard</li>
					<li>Guard without</li>
					<li>Guard against</li>
				</ul>
			
			</li>
		</ul>
	</div>

	<div class="panel panel-primary">
		<div class="panel-heading">
		<h3 class="panel-title">AnnounceSlam()</h3>
		</div>
		<div class="panel-body">
			<p>This function is called after the bid step. The bot must tell if he wants to declare a slam.</p>
		</div>
		
		<ul class="list-group">
			<li class="list-group-item"><b>Return value: </b> Boolean true if a slam is announced, otherwise false</li>
		</ul>
	</div>

	<div class="panel panel-primary">
		<div class="panel-heading">
		<h3 class="panel-title">BuildDiscard(dog)</h3>
		</div>
		<div class="panel-body">
			<p>This function is called for the bot to build the discard.</p>
		</div>
		
		<ul class="list-group">
			<li class="list-group-item"><b>dog: </b> Dog deck, string format (6 cards in a 4 players game)</li>
			<li class="list-group-item"><b>Return value: </b> the discard, string format (6 cards in a 4 players game)</li>
		</ul>
	</div>

	<div class="panel panel-primary">
		<div class="panel-heading">
		<h3 class="panel-title">StartDeal(taker, contract)</h3>
		</div>
		<div class="panel-body">
			<p>This function is called to indicate that a new game is about to start, after the bid sequence. Informational purpose, 
			the script should memorize the arguments for a later use.</p>
		</div>
		
		<ul class="list-group">
			<li class="list-group-item"><b>taker: </b> Indicates the place of the taker (string format, see above)</li>
			<li class="list-group-item"><b>contract: </b> Indicates the contract announced by the taker (string format, see above)</li>
		</ul>
	</div>

	<div class="panel panel-primary">
		<div class="panel-heading">
		<h3 class="panel-title">AskForHandle()</h3>
		</div>
		<div class="panel-body">
			<p>This function is called by the server to ask if the bot has a handle to announce. This function is called one time, during the
			first turn, before playing any card.</p>
		</div>
		
		<ul class="list-group">
			<li class="list-group-item"><b>Return value: </b> The declared handle (deck of cards, string format), or an empty string if there 
			is no handle to declare</li>
		</ul>
	</div>


	<div class="panel panel-primary">
		<div class="panel-heading">
		<h3 class="panel-title">ShowHandle(handle, team)</h3>
		</div>
		<div class="panel-body">
			<p>This function is called when an other player as declared a handle.</p>
		</div>
		
		<ul class="list-group">
			<li class="list-group-item"><b>handle: </b> handle declared, card list in string format</li>
			<li class="list-group-item"><b>team: </b> "0" if declared by the Attack or "1" if declared by the Defense</li>
		</ul>
	</div>

	<div class="panel panel-primary">
		<div class="panel-heading">
		<h3 class="panel-title">PlayCard()</h3>
		</div>
		<div class="panel-body">
			<p>This function is called when the bot must play a card. Notice that in case of a non-valid card returned, the game engine will take
				a random valid card in the bot's deck.</p>
		</div>
		
		<ul class="list-group">
			<li class="list-group-item"><b>Return value: </b> The card played, string format</li>
		</ul>
	</div>

	<div class="panel panel-primary">
		<div class="panel-heading">
		<h3 class="panel-title">PlayedCard(cardName, place)</h3>
		</div>
		<div class="panel-body">
			<p>This function is called when any card is played on the board game (even bot's ones).</p>
		</div>
		
		<ul class="list-group">
			<li class="list-group-item"><b>cardName: </b> The name of the card, string format</li>
			<li class="list-group-item"><b>place: </b> The place of the player's card just played (string format, see above)</li>
		</ul>
	</div>


	              </article>
	                
	                   
            </div>
        </div>
    </div>
</div> <!-- Container -->