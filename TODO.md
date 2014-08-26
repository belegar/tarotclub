# Planned for 2.3.7

## Bugs
  * Game is freezing if: Game menu > Exit (==> Linux only, not a freeze but very slow)
  * Peux-tu aussi regarder pourquoi il faut soit un clic soit un double clic pour jouer une carte ? (==> to reproduce it: Si je laisse le programme faire, � la fin d'un tour, il ramasse les cartes et je peux jouer avec un seul clic. Par contre si je clique sur la table pour ramasser plus vite les cartes, alors j'ai pratiquement toujours besoin d'un double clic pour jouer ma carte suivante)
  * Edit : 12 atouts mais on ne me demande pas si je veux d�clarer une poign�e, il compte bien le nombre d'atout apres avoir eu le chien ? car avant le chien je n'en avais que 9...

# Planned for 2.4.0 (Target date: November 2014)

## Debian source package

  * Remove ImageMagick script creation
  * update control / rules / changelog
  * add postinst script to refresh fonts cache
  * review links made in /usr/lib, necessary?

## Arch package
  
  * Add initial version

## Unit tests
  
  * Add unit tests on trick winner detection
  * Add unit tests on score calculation

## Engine improvements

  * Delete TarotDeck static class, not compatible with muti tables TCDS egine
  * Deck class: change the list of Card pointers into a list of Card objects (3 bytes instead of 4, and resolve the above problem)
  
## TarotWidget Engine
  * Use an HTML5 canvas and JavaScript to allow an easy porting on all platforms
  
## UI improvements
  * Try wrap Qt UI elements to allow easy porting
  * Move cards played by the players at the middle of the table, with fancy arrangement (random position, as in a real game)
  * Add animation when player cards are played
  * Dog / Handle: grey not allowed cards
  
## New features
  
  * Use the Canvas to propose Handle (with the menu)
  * Use a command line TCDS configuration file
  * Change the Lobby protocol into JSON-RPC
  * Use a dynamic-table creation thanks to the JSON configuration file
  * Add a name on each table
  * Display a more comprehensive card name in the history of tricks dock window (eg: 13-D ==> display D with a diamond icon)
  * Allow to choose the AI script configuration file (user customization)
  * Advanced rooms in the Lobby: see status, number of connected players, scores, player names
  * Update Duktape version to 0.11, correct dedicated server build in release with VC++
  * New network commands: change nick, change avatar
  * Ability of seeing current players in a room (server connection wizard)
  * Serait possible d'ajouter une partie reseau ouvert ? genre qui se connecterait sur un serveur qui servirait � ca et qui nous ajouterait directement dans une nouvelle partie ?
  * Add title in popup window (to show the dog or handle)
  * Add new window: list the previous played deals (in .tarotclub directory), and display the players cards (Use QML ?! Plug-in engine ?!)

## IA strategy

  * Correct lost deck synchronization between engine and JS world (bad card number played) (CORRECTED: bug was in CanPlayCard())
  * je suis le preneur, on joue coeur, je coupe et le mec derri�re moi (le dernier � jouer) met un roi alors qu'il lui reste du coeur � jouer (ils le font souvent �a, le sacrifice des grosses cartes...)
  * Si je me lance � faire des series d'atouts (surtout en partant de mes plus hauts), c'est pour aller � la chasse au petit mais eux ils jouent d'entr�e leurs gros atouts (du coup les adversaires mettent les petits qu'ils ont) puis enchaine avec un petit atout...
Donc soit tu pars � la chasse soit tu fais tomber les atouts mais pas en mettant tes gros...
  * De m�me qu'ils n'essaient jamais de jouer leur 21 pour essayer de sauver le 1 d'un equipier. 
  * Un joueur joue son 21 alors que j'ai d�j� jou� et qu'il n'y a aucun point... (il avait d'autres petits atouts evidemment)
  * Encore une excuse en fin de jeu (mais il m'est donn� a priori)
  
# Planned for 2.5.0 (Target date: January 2014)

  * Display the dog / discard at the end of the deal
  * Display all the played cards at the end of the deal
  * Display icon close to the avatar when a player has declared a handle and/or a slam
  * Display the player's declared handle by clicking on the icon
  * J'aimerais bien qu'un clic droit sur une carte l'affiche par dessus les autres afin de bien la voir
  * On devrait pouvoir importer une image perso 
  * WebSocket server (allow HTML5 JS clients)
  * Create a previous trick dock window (to comply to the offcial Tarot rules, even if it is redundant with the deal info dock)
  * Add server parameters (number of deals of the tournament, TCP port for the dedicated server �)
  * Server protocol for remote management
  * Client/Server stuff (commands, disconnections, replace a disconnected client by a bot �)
  * Log raw bad packets with IPs
  * Embedded HTTP server in the dedicated server (game management)

# Planned for 2.6.0

The goal is to focus on the GUI optimizations/corrections and new features.


  * Pour les poign�es, le probleme c'est que ca affiche juste poign�e, il faudrait que ca precise, double, triple... voire meme, encore mieux qu'il propose les differentes possibilit� poign�es (gris� si pas assez d'atout), et si on clic il choisit les cartes, a ce moment on peut les modifier si on en a plus et on valide le tout...	
  * Show a tournament podium at the end
  * Allow to display HTTP avatars
  * Show the discard at the end of the deal (and the points)
  * Automatically save the QMainWindow state before closing (size, position �)
  * Allow domain names instead and IP fields
  * Statistic graphs (points at each round)
  * Save and load game
  * Display IP address of the newly created server for convenience (for self-hosted games)
	

# Planned for 2.7.0 version

The goal is to focus on new gameplay modes

  * Open the server side game mode to allow scripting new game modes (or at least, json configuration file for participants)
  * Championship system with points
  * Different game modes (legacy Tarot Rules, Uno style, Magic style �)
  * Championship mode like in football
  * Create an Observer status to only be a spectator for a current game
  * 3 and 5 players game
	

# Planned for another time

  * Encrypted protocol / authenticated server
  * Create a website with registered users ; link the TarotClub server with these users
  * Mac OS X version
  * iPhone version
  * Windows Phone version
	
