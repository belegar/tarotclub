


# Planned for 2.3.7

## Debian package

  * Remove ImageMagick script creation
  * update control / rules / changelog
  * add postinst script to refresh fonts cache
  * review links made in /usr/lib, necessary?

## Arch package
  
  * Add initial version

## Unit tests
  
  * Add unit tests on player CanPlayCard()
  * Add unit tests on trick winner detection
  * Add unit tests on score calculation

## Other bugs
  * Game is freezing if: Game menu > Exit
  * On a pris le petit du preneur, et le preneur joue son excuse au dernier pli. Malgré cela, au calcul des points, le logiciel lui conserve les 3 bouts
  * Peux-tu aussi regarder pourquoi il faut soit un clic soit un double clic pour jouer une carte ?
  * Edit : 12 atouts mais on ne me demande pas si je veux déclarer une poignée, il compte bien le nombre d'atout apres avoir eu le chien ? car avant le chien je n'en avais que 9...

## IA strategy

  * je suis le preneur, on joue coeur, je coupe et le mec derrière moi (le dernier à jouer) met un roi alors qu'il lui reste du coeur à jouer (ils le font souvent ça, le sacrifice des grosses cartes...)
  * Si je me lance à faire des series d'atouts (surtout en partant de mes plus hauts), c'est pour aller à la chasse au petit mais eux ils jouent d'entrée leurs gros atouts (du coup les adversaires mettent les petits qu'ils ont) puis enchaine avec un petit atout...
Donc soit tu pars à la chasse soit tu fais tomber les atouts mais pas en mettant tes gros...
  * De même qu'ils n'essaient jamais de jouer leur 21 pour essayer de sauver le 1 d'un equipier. 
  * Un joueur joue son 21 alors que j'ai déjà joué et qu'il n'y a aucun point... (il avait d'autres petits atouts evidemment)
  * Encore une excuse en fin de jeu (mais il m'est donné a priori)

# Planned for 2.4.0 (Target date: October 2014)

  * Use a command line TCDS configuration file
  * Change the Lobby protocol into JSON-RPC
  * Beginner AI level (attack and defense)
  * JavaScript AI framework (TarotLib)
  * Display a more comprehensive card name in the history of tricks dock window (eg: 13-D ==> display D with a diamond icon)
  * Allow to choose the AI script configuration file (user customization)
  * Advanced rooms in the Lobby: see status, number of connected players, scores, player names
  * Update Duktape version to 0.11, correct dedicated server build in release with VC++
  * New network commands: change nick, change avatar
  * Ability of seeing current players in a room (server connection wizard)
  * Serait possible d'ajouter une partie reseau ouvert ? genre qui se connecterait sur un serveur qui servirait à ca et qui nous ajouterait directement dans une nouvelle partie ?
  * Add title in popup window (to show the dog or handle)

# Planned for 2.5.0 (Target date: December 2014)

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
  * Embedded HTTP server in the dedicated server (game management)

# Planned for 2.6.0

The goal is to focus on the GUI optimizations/corrections and new features.


  * Pour les poignées, le probleme c'est que ca affiche juste poignée, il faudrait que ca precise, double, triple... voire meme, encore mieux qu'il propose les differentes possibilité poignées (grisé si pas assez d'atout), et si on clic il choisit les cartes, a ce moment on peut les modifier si on en a plus et on valide le tout...	
  * Show a tournament podium at the end
  * Allow to display HTTP avatars
  * Show the discard at the end of the deal (and the points)
  * Automatically save the QMainWindow state before closing (size, position …)
  * Allow domain names instead and IP fields
  * Statistic graphs (points at each round)
  * Save and load game
  * Display IP address of the newly created server for convenience (for self-hosted games)
	

# Planned for 2.7.0 version

The goal is to focus on new gameplay modes

  * Open the server side game mode to allow scripting new game modes (or at least, json configuration file for participants)
  * Championship system with points
  * Different game modes (legacy Tarot Rules, Uno style, Magic style …)
  * Championship mode like in football
  * Create an Observer status to only be a spectator for a current game
  * 3 and 5 players game
	

# Planned for another time

  * Encrypted protocol / authenticated server
  * Create a website with registered users ; link the TarotClub server with these users
  * Mac OS X version
  * iPhone version
  * Windows Phone version
	
