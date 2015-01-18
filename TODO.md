 
# ---------------------------------------------- VERSION 2.6 --------------------------------------------------
 
##  Desktop client
  
  * Allow to choose the AI script configuration file (user customization) from the GUI option
  * New option for no avatar
  * Save and load game
  * Lobby: add buttons to add a bot on a table
  * New option to hide the south identity bloc on the screen (or show a minimized one)
  * Propose to upgrade to a newer version if available (periodic check or at lobby connection)
  
## IA strategy

  * Un joueur joue son 21 alors que j'ai déjà joué et qu'il n'y a aucun point... (il avait d'autres petits atouts evidemment)
  * Par contre le preneur a encore joué le petit pour faire tomber les atouts .
  * Don't display the avatar box when no avatar is selected
  * Un truc pas clair dans la logique, le preneur fait tomber les atouts, mais continue même quand les autres n'en ont plus ?!
    Tout ça pour finir avec son petit... qu'il aurait pu garder pour la fin du jeu.

## UI improvements / Gfx engine
  
  * Create a old-style gradient (pale-yellow) in all the cards
  * Add animation when player cards are played

## Code architecture / engine / network

  * Add a maximum play delay for network players, if timeout a random card is played
  * New network commands: change nick, change avatar
  * Dedicated server information and commands (display stats, tables and players)
  * Log all the messages (per table and of the hall, one file per day)
  
# ---------------------------------------------- BACKLOG --------------------------------------------------  
  
## Code architecture / engine / network

  * Use a shared_ptr to pass a reference of the Protocol work thread to users (Lobby, client, )
  * Change the network protocol into JSON-RPC
  * Encrypted protocol / authenticated server
  * WebSocket server (allow HTML5 JS clients)
  * Log raw bad packets with IPs 
  * Create an Observer status to only be a spectator for a current game
  
##  Desktop client
  
  * Display icon close to the avatar when a player has declared a handle and/or a slam
  * Re-enable local server creation with the TarotClub desktop, includes the mongoose HTTP source to manage REST API
  * Use the Canvas to propose Handle (with the menu)
  * Lobby improvements: see status, number of connected players, scores, player names
  * Add title in popup window (to show the dog or handle)
  * Add an option to display the deals window at the end of a played deal
  * Display the dog / discard at the end of the deal
  * Display all the played cards at the end of the deal
  * Display the player's declared handle by clicking on the icon
  * J'aimerais bien qu'un clic droit sur une carte l'affiche par dessus les autres afin de bien la voir
  * Create a previous trick dock window (to comply to the offcial Tarot rules, even if it is redundant with the deal info dock)
  * Pour les poignées, le probleme c'est que ca affiche juste poignée, il faudrait que ca precise, double, triple... voire meme, encore mieux qu'il propose les differentes possibilité poignées (grisé si pas assez d'atout), et si on clic il choisit les cartes, a ce moment on peut les modifier si on en a plus et on valide le tout...	
  * Show a tournament podium at the end
  * Show the discard at the end of the deal (and the points)
  * Statistic graphs (points at each round)
  * Display IP address of the newly created server for convenience (for self-hosted games)

## Game modes

  * 3 and 5 players game
  * Different game modes (legacy Tarot Rules, Uno style, Magic style)
  * Game modes described in Wikipedia (6, 7 players, 1 player solitaire style)
  * Championship mode like in football  / with points
  * Game by mail mode :)
  
## UI improvements / Gfx engine
  
  * Try wrap Qt UI elements to allow easy porting
  * Dog / Handle: grey not allowed cards
  * Use an HTML5 canvas and JavaScript to allow an easy porting on all platforms
  * Dynamically create SVG images (usage of one card "background" for all the cards, and a set of illustrations for all the cards)

## Porting to other OS

  * Mac OS X version
  * iPhone version
  * Windows Phone version
  * FreeBSD TCDS compatibility
  
