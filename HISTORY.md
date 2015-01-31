# Branch 2.5

## Corrections in version 2.5.3

  * Kanzlei font is now included in the executable. Updated all packages setup to remove it from the installation.

## Corrections in version 2.5.2

  * Corrected the selection colour around the name box
  * Try of a new box colour for the bid (less flashy than the red one)
  * NEW lobby hall dock-able window GUI
  * Corrected slow server performance and hazardous disconnections by adding a Tcp Server event thread
  * Corrected the lobby player list not cleared on disconnection
  * Removed version name and icon, updated AboutBox with links
  * Removed SVG avatars to keep one design style
  * Correction: Lobby GUI is not updated when playing a local game
  * Corrected blocking game under linux (EGAIN was closing the client socket())
  * Duktape updated to version 1.1.0
  * AI correction: sometimes, the discard was not valid (more than 6 cards)
  * Auto discard correction

## Corrections in version 2.5.1 (03/01/2015)

  * Lobby window: removed default focus on buttons while chatting
  * "Petit sec" deals are avoided
  * [Arch] Added missing icon in the menu launcher
  * [Ubuntu] Added missing documentation in the package
  * Corrected generated deal file name for the desktop version
  * Corrected deals visualizer
  * Corrected defense AI script bug
  * Deleted the quote parameter (obsolete)

## Corrections in version 2.5.0 (28/12/2014)

  * Information dock window: display king/queen etc. instead of 14, 13 .. and make hearts and diamonds in red
  * Add a GUI option to choose the number of tournament turns
  * Added more default avatar images
  * Lobby: add buttons to manage the server list (add/remove)
  * New option window for bots, allowing future custom bots configuration
  * Allow to choose an local avatar file or import/use a remote image file via an URL
  * The Deal Editor can now load a custom deal file
  * Add new protocol command to get the list of tables instead of using a REST api, protocol version updated to V2
  * Removed lobby window initialization before showing it
  * Deal table widget is now correctly stretched
  * Issue #26: impossible to play a local game after a network game
  * Issue #33: Excuse must be played when no more trumps
  * Issue #28, #34: Game blocked if handle declaration is refused
  * Use a different log filename than the desktop version (prefixed by "tcds" instead of "desktop")
  * New option: the number of tournament turns is now configurable
  * New option: parameter to choose the server web port (default 8080)
  * Duktape upgraded to 1.02
  * Integration of the SQLite database for statistics
  * Embedded webpage to manage the server (first version)

# Version 2.4 - Codename <Pirate!> (03/11/2014)

## Internal modifications

    ~ Mkdir utility did not manage the disk name separator properly (Windows only)
	~ Excuse (fool) card was not correctly assigned at the end of a deal
	+ Deck class: change the list of Card pointers into a list of Card objects (2 bytes instead of 4), deleted TarotDeck files
	+ Update Duktape version to 0.12
	
## Desktop improvements
    
    + Display a more comprehensive card name in the history of tricks dock window (eg: 13-D ==> display D with a diamond icon)
	+ Improved board organization and graphics, cards are showed in the middle of the screen
	+ New avatars and avatar chooser
	+ New window to display the previously played deals
	
## Artificial intelligence changes

    + Corrected defense playing after the taker if he has cut a suit
	+ Plays the excuse as early as possible to avoid playing it at the last trick
	+ Defense try to save the one of trump of a partner
	
## Dedicated server changes
    
	- Removed the rooms, a Tarot server now simply has some tables
    + Use a command line TCDS configuration file, new dynamic creation of tables (number, names)
	+ Usage of a work thread to share packets analysis
	+ Dynamic bot creation (per table)
	+ New Lobby
	+ New experimental embedded HTTP server on port 8080, REST API available
	~ Corrected build and some crashes under Visual C++ 2013

# Branch 2.3.x - Codename <Snowboard>

## version 2.3.7 (30/08/2014)

    ~ Added "codename" string in the AboutBox
    ~ Correction of tournament score initialization (server and client sides)
    ~ CppCheck, Coverity Scan, Simian (duplicate code), Vera++ and MISRA C++ corrections
    ~ CanPlayCard() algorithm correction in one test case
    ~ Added unit tests (Player class, CanPlayCard(), scoring)
    ~ Ticket #21: Ctrl+D is used for two menu entries
    ~ Ticket #22: Lobby French translation "Connextion"
    ~ Removed English translation file (native language)
    ~ Corrected and optimized deal log generation (less unused information generated)
    ~ Corrected fool owner if played at last trick
    ~ Corrected handle detection after building the discard
    ~ Corrected the need of card double-click if the previous trick flushing was not automatic (Qt event problem)
    ~ Corrected game freeze under Linux (Menu game > quit after first launch)
    + Added Tarot rules in the Ubuntu package and Windows setup
    + New hand-made makefile do build the TCDS executable
    + New code coverage script
    + New deal reading facility
    + Window and docks geometry is now memorized on exit and applied on start

## version 2.3.6 (01/08/2014)

    ~ Still missing some translations, added factorization of texts to simplify the translation process
    ~ Corrected the Mkdir() method under windows

## version 2.3.5 (31/07/2014)

    ~ About box was too large (switched to a text edit control)   
    ~ Various defense AI corrections and function bugs
    ~ JS script context is now reloaded at each new game with a proper heap cleaning
    ~ Corrected upper limit of the numbered deal (32-bit signed value)
    ~ Tournament box is now cleared when the game starts
    ~ Correction of all the diamond suit cards (rendering problem) - Contribution by Hizoka
    + Change in the network protocol: clear separation between a game and a deal (preparation for network game enhencements)
    + New AI attack, basic version, with discard build   
    
## version 2.3.4 (19/07/2014)

    ~ Correction: little endian bonus in case of slam and fool in last trick was not detected
    ~ Correction: translated deal result text in french
    ~ Correction of the highest suit dection (trick winner)
    ~ Correction of the last trick, it was not taken into account
    + New drop shadow effect on cards
    + New radial gradient effect for the background color

## version 2.3.3 (17/07/2014)

    ~ Correction: show handle sequence was not managed well
    ~ Correction: the dog cards was hidden in the player's hand
    ~ Correction: the slam tick was not cleared between two deals
    
## version 2.3.2 (14/07/2014)

    ~ Correction: the user does not send the discard to the server
    ~ Correction: the deal JSON output was not JSON compliant
    ~ Removed winner message box (no sense for one deal)
    + Auto play works also for bid step
    + JSON reader now uses its own parser instead of the Duktape's one
    + New JSON configuration file for AI scripts

## version 2.3.1 (05/07/2014)

    ~ Corrected french translation, bug in translation file loading and in Canvas item texts
    ~ Make the debian package dependant of QtSvg

## version 2.3.0 (05/07/2014)

    + Show south cards with a constant padding, middle alignment
    + Use JSON instead of XML for client/server configuration and various generated files (deals, statistics ...)
    + Advanced logging system in files and in the DebugDock window
    + Network protocol robustness's: better header, more synchronization with clients, separate class file
    + Restart game without previous disconnection
    + Exit game gracefully, cooperative termination of threads with join()
    + Beginner AI level (defense only)
    + New option: suit order selection
    + Show the game result in the Canvas instead of a Window (Android compatibility)
    + Big internal rework of code architecture (
    + Full standard C++11 dedicated server, without Qt. Compatible with MSVC++ 2013 Express.
    + Separate and independant, platform agnostic C++ library classes
    + JSON and ScriptEngine standard interface
    ~ Modified the JavaScript engine: use of Duktape 0.10
    - Removed integrated help system. Replace by the launch of a web browser showing provided HTML help pages

# Branch 2.2.x - Codename <Juliette>

## version 2.2.0 (04/10/2013)

    + Dedicated server with multi game rooms, independent executable
    + Connection to a server, server hosting inside the graphical client
    + Resize cards and canvas contents with the window size
    + First version of the dedicated server
    + Network engine: class serializations for all packets (same as Identity.h)
    + New board organization, in game menu, larger Avatars
    + Show declared handles on the screen
	~ Updated code style and translation into English

# Branch 2.1.x - Codename <GLaDOS>
    
## version 2.1.0 (10/08/2013)

    + Script Engine for AI using QtScript, first version of Javascript API
    + Rework options and config files (separation between server and client)
    + Qt 5.0 version
    + Various user interface improvements
    + Integrated help system including Tarot game rules in English and French
    + New avatars from Futurama
    + New source code design, new code style, translated most of the code (class names, file names, methods, comments) into English
    + Enhanced network protocol
    + Add a shadow under each card area
    + Allow to validate turn by clicking - or - timeout
    + Add a lexical and Tarot rules in the help menu (using QtHelp system)
    + Translation files (French and English)
    + Debian/Ubuntu installation packages
    + On each platform, store the configuration files in HOME_DIR/.tarotclub/

# Branch 2.0.x - Codename <Kritzkrieg>
    
## version 2.0.0 (13/08/2010)

    + Configure time between players and at the end of a turn
    + Windows installer with Innosetup
    + Tournament mode (10 rounds) and one shot round
    + Give the seed used to generate the current deal
    + Deal editor
    + Ubuntu package
    + Show player's deck statistics
    + Show cards played each turn in a dock window
    + Chelem + poignées implementation and scoring
    + Show the total of points in the score table
    + Server-side tournament mode and result UI

# Branch 1.3.x - Codename <Dragon Ball>
    
## version 1.3.0 (04/10/2006)

    + Nouveau système de dock windows pour afficher des informations
    + Dock window pour le chat durant les parties réseau
    + Compatible complètement avec Qt 4.3.0 (plus de Q3Support)
    + Architecture client/serveur complète avec serveur dédié indépendant
    + Support de bots prévu dès la conception

# Branch 1.2.x - Codename <Tschaï>
    
## version 1.2.1

    + Traduction anglaise
    + Sélection de la langue dans les options
    + Exécutable plus petit grâce à UPX (http://upx.sourceforge.net/)

## version 1.2.0

    + Le jeu en réseau
    + Splash screen au démarrage
    + Choix du tapis de jeu
    + Refonte du système de sauvegarde
    + Nouvelle version de fichier XML de préférences
    + Passage à Qt 4.0.1
    + Abandon de Visual C++ au profit de MinGW (Windows)
    ~ Modification du choix des avatars
    - Abandon (temporaire) des scripts Lua

# Branch 1.1.x - Codename <Winnie>
    
## version 1.1.3

    + Nouvelle donnée de l'API : cartesJoueurs[]
    + Nouveau fichier d'IA : contribution de Hervé BENOIT
    ~ Correction de l'appartenance des cartes
    ~ Changement de l'icône de l'exécutable (Windows seulement)
    ~ Passage à Lua 5.0.2
    ~ Séparation du code du moteur de jeu et du code de l'interface
    ~ Passage à 65000 donnes différentes
    ~ Changements dans le menu
    ~ Bug de sauvegarde des avatars corrigé

## version 1.1.2

    + Le Chien s'affiche rangé par couleur
    + Portage du code de Qt 2.3 à Qt 3.2.1
    + Changements des Id des cartes
    + Les scripts Lua doivent maintenant ętre dans des fonctions
    + Donnes numérotées et possibilité de fournir une donne précise
    ~ Console Lua améliorée
    ~ L'ordinateur produit un chien valide (sans Roi ni atouts)

## version 1.1.1

    + Nouvelles fonctions de l'API :
    + println() : affiche un message dans la console
    + maitre() : retourne la position du maitre
    + place() : (retourne : nord, sud, etc.)
    + des variables globales pour accéder cartes déjà jouées etc.

## version 1.1.0

    + Une console Lua permet de visualiser les messages d'erreurs
    + Nouvelle option : visualisation du pli précédent
    + Indication du joueur qui joue : plus de boule mais la boite du joueur est cerclées de rouge
    + Le nom des joueurs s'affiche également dans la table des scores
    + La taille de la fenętre s'adapte à la résolution de l'écran
    + Le nom des joueurs est limité à 10 caractčres
    + Nouvelle option pour modifier les temps de pause dans le séquençage du jeu
    ~ Le Chelem est maintenant convenablement détecté
    ~ Le chien n'est pas affiché en cas de Garde sans ou de Garde Contre
    ~ Chemins relatifs changés en absolus : on peut maintenant démarrer TarotClub depuis n'importe quel emplacement
    ~ Quelques bugs corrigés et des simplifications de code
    ~ Classe Deck complètement refondue : + simple et ne fait plus planter le jeu

# Branch 1.0.x - Codename <Teddy>

## version 1.0.0 (03/12/2003)

	+ Version initiale

(end of HISTORY)
