
# ---------------------------------------------- VERSION 2.6 --------------------------------------------------

##  Desktop client

  * New option for no avatar (Le fait de cacher les avatars ne diminue pas la taille des encadr�s des joueurs, il faudrait que ce soit le cas sinon je ne vois pas trop l�int�r�t.)
  * Lobby: add buttons to add a bot on a table
  * New option to hide the south identity bloc on the screen (or show a minimized one)
  * Propose to upgrade to a newer version if available (periodic check or at lobby connection)
  * Re-enable in-game network hosting table
  * Add an option to display the deals window at the end of a played deal
  * Pourrait on choisir une image pour le tapis au lieu d'une couleur unie

## IA strategy

  * Un joueur joue son 21 alors que j'ai d�j� jou� et qu'il n'y a aucun point... (il avait d'autres petits atouts evidemment)
  * Par contre le preneur a encore jou� le petit pour faire tomber les atouts .
  * Don't display the avatar box when no avatar is selected
  * Un truc pas clair dans la logique, le preneur fait tomber les atouts, mais continue m�me quand les autres n'en ont plus ?!
    Tout �a pour finir avec son petit... qu'il aurait pu garder pour la fin du jeu.
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

  * New network commands: change nick, change avatar (Hizoka (ubuntu-fr) les avatars ne sont chang�s que lors du red�marrage du logiciel, on ne peut pas le faire en direct )


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
  * Pour les poign�es, le probleme c'est que ca affiche juste poign�e, il faudrait que ca precise, double, triple... voire meme, encore mieux qu'il propose les differentes possibilit� poign�es (gris� si pas assez d'atout), et si on clic il choisit les cartes, a ce moment on peut les modifier si on en a plus et on valide le tout...
  * Show a tournament podium at the end
  * Statistic graphs (points at each round)
  * Display IP address of the newly created server for convenience (for self-hosted games)
  * Add an option to allow declaring missing figures (alias "mis�res de t�te")

## General

  * Develop a TarotStudio : tournament manager (print score papers, record players, add scores manually, quick score calculation ...)
  * Move the DealCreator in the TarotStudio
  * TarotStudio --> new child UI in the TarotClub MDI window

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

## Tarot 2 players (TarotBattle!)

Certains m'ayant pos� la question, il existe plusieurs variantes du jeu de tarot � 2 joueurs.
Je vous livre ici celle qui me semble la meilleure (l'ayant souvent pratiqu�e).

Pour commencer, imaginez qu'une ligne sur la table vous s�pare de votre adversaire, et que de part et d'autre de cette ligne, il y a 8 emplacements en deux rang�es de 4.

Sch�matiquement cela donne ceci :

x x x x cette zone est le
x x x x camp de l'adversaire
--------------------------------------------------------------
x x x x cette zone est votre
x x x x camp


En pratique, au d�but de la partie, sur chaque emplacement marqu� d'un "x" est pos� un paquet de 2 cartes. Celle du dessous est pos�e face cach�e, et celle qui est dessus face visible. Chacun des joueurs voit donc 8 cartes de l'adversaire.
Les cartes qui sont plac�es face cach�e sont dites cartes "inf�rieures", celles du dessus "sup�rieures". L'ensemble des cartes sup�rieures et inf�rieures est appel� "tabl�e".

En outre, 5 paquets de cartes sont dispos�s sur le tapis.
- Un chien de 6 cartes, comme dans le jeu � 3 ou 4.
- pour chaque joueur, un paquet de 12 cartes nomm� "principal", et un paquet de 8 cartes nomm� "renfort".

Au moment des ench�res, chaque joueur prend en main son "principal" sans en montrer le contenu, et �value son jeu en fonction des cartes qu'il contient et des cartes qu'il poss�de face visible sur la table (soit 20 cartes).
Pour �viter la r�p�tition des donnes, fastidieuse, le donneur fait d'office une "prise" (ou petite). Mais la "pousse" est r�tablie, son d�compte est un compromis entre prise et garde, seuls les points de passe ou de chute �tant multipli�s par 2. Lorsque c'est son tour de parler, le donneur peut monter sur sa prise d'office si l'adversaire a pass�.
Lorsque le tour d'ench�re est fini, chaque joueur s'empare alors de son renfort et le m�le � son jeu sans que l'autre le voie, et il est dispos� du chien comme dans le jeu classique � 3 ou 4 joueurs.

Lorsque vient votre tour de jouer, vous avez le choix de jouer une carte de votre main, ou une carte de votre camp dans la tabl�e. Vous ne pouvez jouer une carte de la tabl�e que si vous ne faites pas de renonce (pas question de couper trefle par exemple si vous n'en avez pas en main mais qu'il y en a dans votre tabl�e).
Lorsque vous jouez une carte de la tabl�e et qu'elle en recouvre une autre, vous devez attendre la fin du pli pour d�couvrir la carte du dessous et la mettre face visible � la place de celle qui vient d'�tre jou�e. On ne peut jouer une carte de la tabl�e que si elle est face visible.

La poign�e est � 14 atouts, la double � 17 et la triple � 22. Comptent dans la poign�e les atouts d�tenus en main ainsi que ceux qui figurent sur la table dans votre camp. La pr�sentation de la poign�e se fait en �talant les atouts n�cessaire qu'on avait en main et �ventuellement en d�signant du doigt ceux de son camp sur la table.

Toutes les autres r�gles du jeu sont identiques au jeu classique, en dehors de la sp�cificit� de la donne et de quelques cas particuliers.

Cas particulier du petit au bout.
Le petit est �galement consid�r� au bout si un joueur ne poss�de plus de cartes en main et n'a sur sa tabl�e qu'un seul paquet de 2 cartes, le petit �tant la carte du dessus.
Il y a un autre cas de petit au bout, � l'ant�p�nulti�me pli (voir � ch�lem ci apr�s).

Cas du ch�lem
Dans la m�me configuration (un seul paquet de 2 cartes devant le joueur et plus rien en main), si c'est l'excuse qui est dessus elle est consid�r�e comme �tant men�e au bout dans le cas d'un ch�lem.
S'il s'agit d'un ch�lem non annonc� et que tous les plis ont �t� r�alis�s par le d�tenteur de l'excuse, cette derni�re est victorieuse du pli, quelle que soit la carte en face. Si un pli (ou +) a �t� r�alis� par l'adversaire, elle sera consid�r�e comme une excuse normalement men�e � l'avant dernier pli, et donc ne le remportera pas mais restera la propri�t� de son d�tenteur.
S'il s'agit d'un ch�lem annonc�, l'excuse sera toujours victorieuse, m�me si l'adversaire a d�j� r�alis� un (ou +) pli.
Bien entendu, ces cas de figure rarissimes font que c'est la carte situ�e sous l'excuse qui d�cide de la r�ussite ou de l'�chec du ch�lem.
Bien entendu, si ce cas venait � se pr�senter, pour que le petit soit consid�r� au bout il faut qu'il ait �t� jou� au coup imm�diatement pr�c�dent.

Cas de l'excuse derni�re carte jou�e et retourn�e
Sauf en cas de ch�lem, l'excuse jou�e en dernier change de camp, m�me si le joueur n'a pas pu faire autrement (par exemple il n'a plus de carte en main et un seul paquet de 2 devant lui, la carte du dessous �tant l'excuse).

La donne
La donne est extr�mement importante. D'une part les cartes doivent �tre soigneusement m�lang�es entre 2 parties. D'autre part la r�partition oblige � respecter certains principes.
- On donne d'abord � l'adversaire, puis � soi.
- La premi�re et la derni�re cartes du paquet ne peuvent aller ni au chien ni dans la tabl�e.
- Lorsqu'une carte est distribu�e dans la tabl�e, qu'il s'agisse d'une carte cach�e ou d'une carte � face visible, la suivante doit obligatoirement �tre pos�e en sym�trique dans le camp adverse.
Toutefois, les cartes inf�rieures peuvent �tre donn�es 1 fois par rang�e enti�re (soit 4 cartes) et/ou 2 fois par demi-rang�e (soit 2 cartes � chaque fois), � condition que le sym�trique soit imm�diatement r�alis� dans le camp adverse.
( vous pouvez donc donner : soit une rang�e de 4 ainsi qu'une ou deux demi-rang�es, soit 1 ou 2 demi-rang�es).
- Avant de pouvoir distribuer une carte sup�rieure dans la tabl�e, les 4 cartes inf�rieures de la rang�e doivent avoir �t� donn�es pour chaque camp.
- Le chien est constitu� carte par carte
- On donne au maximum deux cartes � la fois dans le paquet "principal" ou dans le paquet "renfort" de chaque joueur.
- La distribution du principal et du renfort se fait de fa�on sym�trique. Mettre des cartes au principal de l'adversaire implique que vous en mettiez le m�me nombre au v�tre, et identiquement pour le renfort.
- La distribution du principal et du renfort doit �tre altern�es au moins 3 fois.

Mon avis
Contrairement aux apparences, cette variante de jeu est agr�able. Elle est m�me beaucoup plus scientifique qu'elle n'y parait, et oblige les joueurs � un calcul permanent. Certes, �a ne vaut pas le jeu � 4, mais c'est une bonne mani�re de tromper le temps � deux ... je sais, y'en a d'autres... mais on parle tarot, l�, non ?
En outre, l'obligation de prise pour le donneur �vite de devoir redistribuer sans cesse et r�serve parfois des surprises.
Les r�gles de la distribution peuvent para�tre r�barbatives, en fait vous vous apercevrez apr�s 3 ou 4 donnes qu'elles sont tr�s logiques et que la sym�trie � laquelle elles obligent en simplifie grandement la m�morisation et la r�alisation.
Mais tant que vous ne ma�triserez pas bien le jeu, forcez vousde donner les cartes de la tabl�e au d�but, cela �vitera les fausses donnes.


Un conseil.
S'il peut �tre parfois utile de garder des cartes sur la table pour la fin, notamment des cartes cach�es, et ce pour induire l'adversaire en erreur ou le mettre au moins dans le doute, l'exp�rience prouve qu'en g�n�ral le preneur a int�r�t � avoir retourn� au plus vite les cartes inf�rieures, ce afin de mieux dominer l'adversaire.
Le d�but de partie a donc moins d'importance que le milieu, la fin n'�tant souvent qu'une suite logique mais calcul�e des coups qui pr�c�dent.

A noter que cette fa�on de jouer avec des cartes sur table existe aussi dans une variante � 3 joueurs, tr�s scientifique et �galement agr�able.
Je t�cherai de vous la livrer prochainement, ainsi que la r�gle du tarot � 7 (id�al pour soir�es de d�lire entre bons poteaux)


Bon amusement aux couples

