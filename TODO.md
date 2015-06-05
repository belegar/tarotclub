 
# ---------------------------------------------- VERSION 2.6 --------------------------------------------------
 
##  Desktop client
  
  * New option for no avatar (Le fait de cacher les avatars ne diminue pas la taille des encadrés des joueurs, il faudrait que ce soit le cas sinon je ne vois pas trop l’intérêt.)
  * Lobby: add buttons to add a bot on a table
  * New option to hide the south identity bloc on the screen (or show a minimized one)
  * Propose to upgrade to a newer version if available (periodic check or at lobby connection)
  * Re-enable in-game network hosting table
  * Add an option to display the deals window at the end of a played deal
  * Pourrait on choisir une image pour le tapis au lieu d'une couleur unie
  
## IA strategy

  * Un joueur joue son 21 alors que j'ai déjà joué et qu'il n'y a aucun point... (il avait d'autres petits atouts evidemment)
  * Par contre le preneur a encore joué le petit pour faire tomber les atouts .
  * Don't display the avatar box when no avatar is selected
  * Un truc pas clair dans la logique, le preneur fait tomber les atouts, mais continue même quand les autres n'en ont plus ?!
    Tout ça pour finir avec son petit... qu'il aurait pu garder pour la fin du jeu.
  * Allow a multi-language AI script execution by using standard input/ouput and launching external process
  * Add Duktape debug and JSEditor for in-game modification of scripts

## UI improvements / Gfx engine
  
  * Span the table border to the view, allow 16/9 form factor (better visualization of the player's deck)
  * Improve the previously played deals visualizer (Add more informations (discard, dog, turns number) Display cards like player around the table (south, north ...)
  * Display the dog / discard at the end of the deal (correction: in the deals visualizer)
  * Show the discard at the end of the deal (and the points)
  * Add animation when player cards are played
  
  * Bug: refresh current ui setting of the options window if clicked on "default" button

## Code architecture / engine / network

  * New network commands: change nick, change avatar (Hizoka (ubuntu-fr) les avatars ne sont changés que lors du redémarrage du logiciel, on ne peut pas le faire en direct )
  
  
# ---------------------------------------------- BACKLOG --------------------------------------------------  
  
## Code architecture / engine / network

  * Change the network protocol into JSON-RPC
  * Encrypted protocol / authenticated server
  * WebSocket server (allow HTML5 JS clients)
  * Log raw bad packets with IPs 
  * Create an Observer status to only be a spectator for a current game
  * Add a timer when playing online
  
##  Desktop client
  
  * Save and load game
  * Display icon close to the avatar when a player has declared a handle and/or a slam
  * Re-enable local server creation with the TarotClub desktop, includes the mongoose HTTP source to manage REST API
  * Use the Canvas to propose Handle (with the menu)
  * Lobby improvements: see status, number of connected players, scores, player names
  * Add a nice title in all popup window (to show the dog or handle)
  * Display the player's declared handle by clicking on the icon
  * J'aimerais bien qu'un clic droit sur une carte l'affiche par dessus les autres afin de bien la voir
  * Create a previous trick dock window (to comply to the offcial Tarot rules, even if it is redundant with the deal info dock)
  * Pour les poignées, le probleme c'est que ca affiche juste poignée, il faudrait que ca precise, double, triple... voire meme, encore mieux qu'il propose les differentes possibilité poignées (grisé si pas assez d'atout), et si on clic il choisit les cartes, a ce moment on peut les modifier si on en a plus et on valide le tout...	
  * Show a tournament podium at the end
  * Statistic graphs (points at each round)
  * Display IP address of the newly created server for convenience (for self-hosted games)
  * Add an option to allow declaring missing figures (alias "misères de tête")
  
## UI improvements / Gfx engine
  
  * Try wrap Qt UI elements to allow easy porting
  * Dog / Handle: grey not allowed cards
  * Use an HTML5 canvas and JavaScript to allow an easy porting on all platforms
  * Dynamically create SVG images (usage of one card "background" for all the cards, and a set of illustrations for all the cards)
  * Create a old-style gradient (pale-yellow) in all the cards
  
## Porting to other OS

  * Mac OS X version
  * iPhone version
  * Windows Phone version
  
