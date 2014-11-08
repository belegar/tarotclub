# Planned for 2.5 (Target date: January 2015)
    
##  Desktop client

  * Faire en sorte de choisir le nombre de donnes pour les tournois dans la prochaine version
  * Allow to choose an local avatar file /   * On devrait pouvoir importer une image perso 
  * Lobby: add buttons to add a bot on a table
  * Lobby: add buttons to manage the server list (add/remove)
  * Allow to add new server addresses in the lobby, save them in the configuration file
  * Add new protocol command to get the list of tables instead of using a REST api
  * Allow to choose the AI script configuration file (user customization)
  * Display icon close to the avatar when a player has declared a handle and/or a slam
  * Peut on avoir de la couleur dans l'onglet information pour les coeurs et carreaux mais surtout les numeros 11 - 12 - 13 - 14... ce n'est vraiment pas intuitif, il faut reflechir...
  * Create a old-style gradient (pale-yellow) in all the cards
  * Indicate the table id and name in the deal file result

  
## TCDS dedicated server 

  * Add server parameters (number of deals of the tournament, TCP port for the dedicated server …)
  * TCDS JSON parameter to choose the server web port (default 8080)
  * Start to integrate the SQLite database for statistics (see design document)
  * Use a different log filename than the desktop version
  
## Code architecture / engine

  * Use a shared_ptr to pass a reference of the Protocol work thread to users (Lobby, client, )
  * Dynamically create SVG images (usage of one card "background" for all the cards, and a set of illustrations for all the cards)
  
## IA strategy

  * Un joueur joue son 21 alors que j'ai déjà joué et qu'il n'y a aucun point... (il avait d'autres petits atouts evidemment)
  * Par contre le preneur a encore joué le petit pour faire tomber les atouts .
  
    
# BACKLOG
  
## New features
    
  * Re-enable local server creation with the TarotClub desktop, includes the mongoose HTTP source to manage REST API
  * Use the Canvas to propose Handle (with the menu)
  * Change the Lobby protocol into JSON-RPC
  * Lobby improvements: see status, number of connected players, scores, player names
  * New network commands: change nick, change avatar
  * Add title in popup window (to show the dog or handle)
  * Add an option to display the deals window at the end of a played deal
  * Display the dog / discard at the end of the deal
  * Display all the played cards at the end of the deal
  * Display the player's declared handle by clicking on the icon
  * J'aimerais bien qu'un clic droit sur une carte l'affiche par dessus les autres afin de bien la voir
  * WebSocket server (allow HTML5 JS clients)
  * Create a previous trick dock window (to comply to the offcial Tarot rules, even if it is redundant with the deal info dock)
  * Server protocol for remote management
  * Client/Server stuff (commands, disconnections, replace a disconnected client by a bot …)
  * Log raw bad packets with IPs 
  * Pour les poignées, le probleme c'est que ca affiche juste poignée, il faudrait que ca precise, double, triple... voire meme, encore mieux qu'il propose les differentes possibilité poignées (grisé si pas assez d'atout), et si on clic il choisit les cartes, a ce moment on peut les modifier si on en a plus et on valide le tout...	
  * Show a tournament podium at the end
  * Allow to display HTTP avatars
  * Show the discard at the end of the deal (and the points)
  * Statistic graphs (points at each round)
  * Save and load game
  * Display IP address of the newly created server for convenience (for self-hosted games)
  * Open the server side game mode to allow scripting new game modes (or at least, json configuration file for participants)
  * Championship system with points
  * Different game modes (legacy Tarot Rules, Uno style, Magic style …)
  * Championship mode like in football
  * Create an Observer status to only be a spectator for a current game
  * 3 and 5 players game
  * Encrypted protocol / authenticated server
  * Create a website with registered users ; link the TarotClub server with these users
  * Mac OS X version
  * iPhone version
  * Windows Phone version
  
## UI improvements / Gfx engine
  
  * Try wrap Qt UI elements to allow easy porting
  * Add animation when player cards are played
  * Dog / Handle: grey not allowed cards
  * Use an HTML5 canvas and JavaScript to allow an easy porting on all platforms

