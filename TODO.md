# Planned for 2.4 (Target date: November 2014)
  
## IA strategy

  * Correct lost deck synchronization between engine and JS world (bad card number played) (CORRECTED: bug was in CanPlayCard())
  * je suis le preneur, on joue coeur, je coupe et le mec derrière moi (le dernier à jouer) met un roi alors qu'il lui reste du coeur à jouer (ils le font souvent ça, le sacrifice des grosses cartes...)
  * Si je me lance à faire des series d'atouts (surtout en partant de mes plus hauts), c'est pour aller à la chasse au petit mais eux ils jouent d'entrée leurs gros atouts (du coup les adversaires mettent les petits qu'ils ont) puis enchaine avec un petit atout...
Donc soit tu pars à la chasse soit tu fais tomber les atouts mais pas en mettant tes gros...
  * De même qu'ils n'essaient jamais de jouer leur 21 pour essayer de sauver le 1 d'un equipier. 
  * Un joueur joue son 21 alors que j'ai déjà joué et qu'il n'y a aucun point... (il avait d'autres petits atouts evidemment)
  * Encore une excuse en fin de jeu (mais il m'est donné a priori)
  
# BACKLOG

## New features
  
  * Use the Canvas to propose Handle (with the menu)
  * Change the Lobby protocol into JSON-RPC
  * Allow to choose the AI script configuration file (user customization)
  * Lobby improvements: see status, number of connected players, scores, player names
  * New network commands: change nick, change avatar
  * Add title in popup window (to show the dog or handle)
  * Add an option to display the deals window at the end of a played deal
  * Display the dog / discard at the end of the deal
  * Display all the played cards at the end of the deal
  * Display icon close to the avatar when a player has declared a handle and/or a slam
  * Display the player's declared handle by clicking on the icon
  * J'aimerais bien qu'un clic droit sur une carte l'affiche par dessus les autres afin de bien la voir
  * On devrait pouvoir importer une image perso 
  * WebSocket server (allow HTML5 JS clients)
  * Create a previous trick dock window (to comply to the offcial Tarot rules, even if it is redundant with the deal info dock)
  * Add server parameters (number of deals of the tournament, TCP port for the dedicated server …)
  * Server protocol for remote management
  * Client/Server stuff (commands, disconnections, replace a disconnected client by a bot …)
  * Log raw bad packets with IPs 
  * Pour les poignées, le probleme c'est que ca affiche juste poignée, il faudrait que ca precise, double, triple... voire meme, encore mieux qu'il propose les differentes possibilité poignées (grisé si pas assez d'atout), et si on clic il choisit les cartes, a ce moment on peut les modifier si on en a plus et on valide le tout...	
  * Show a tournament podium at the end
  * Allow to display HTTP avatars
  * Show the discard at the end of the deal (and the points)
  * Allow domain names instead and IP fields
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
  
## UI improvements
  
  * Try wrap Qt UI elements to allow easy porting
  * Add animation when player cards are played
  * Dog / Handle: grey not allowed cards

## Gfx engine

  * Use an HTML5 canvas and JavaScript to allow an easy porting on all platforms
  * Dynamically create SVG images (usage of one card "background" for all the cards, and a set of illustrations for all the cards)

