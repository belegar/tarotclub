
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

Certains m'ayant posé la question, il existe plusieurs variantes du jeu de tarot à 2 joueurs.
Je vous livre ici celle qui me semble la meilleure (l'ayant souvent pratiquée).

Pour commencer, imaginez qu'une ligne sur la table vous sépare de votre adversaire, et que de part et d'autre de cette ligne, il y a 8 emplacements en deux rangées de 4.

Schématiquement cela donne ceci :

x x x x cette zone est le
x x x x camp de l'adversaire
--------------------------------------------------------------
x x x x cette zone est votre
x x x x camp


En pratique, au début de la partie, sur chaque emplacement marqué d'un "x" est posé un paquet de 2 cartes. Celle du dessous est posée face cachée, et celle qui est dessus face visible. Chacun des joueurs voit donc 8 cartes de l'adversaire.
Les cartes qui sont placées face cachée sont dites cartes "inférieures", celles du dessus "supérieures". L'ensemble des cartes supérieures et inférieures est appelé "tablée".

En outre, 5 paquets de cartes sont disposés sur le tapis.
- Un chien de 6 cartes, comme dans le jeu à 3 ou 4.
- pour chaque joueur, un paquet de 12 cartes nommé "principal", et un paquet de 8 cartes nommé "renfort".

Au moment des enchères, chaque joueur prend en main son "principal" sans en montrer le contenu, et évalue son jeu en fonction des cartes qu'il contient et des cartes qu'il possède face visible sur la table (soit 20 cartes).
Pour éviter la répétition des donnes, fastidieuse, le donneur fait d'office une "prise" (ou petite). Mais la "pousse" est rétablie, son décompte est un compromis entre prise et garde, seuls les points de passe ou de chute étant multipliés par 2. Lorsque c'est son tour de parler, le donneur peut monter sur sa prise d'office si l'adversaire a passé.
Lorsque le tour d'enchère est fini, chaque joueur s'empare alors de son renfort et le mêle à son jeu sans que l'autre le voie, et il est disposé du chien comme dans le jeu classique à 3 ou 4 joueurs.

Lorsque vient votre tour de jouer, vous avez le choix de jouer une carte de votre main, ou une carte de votre camp dans la tablée. Vous ne pouvez jouer une carte de la tablée que si vous ne faites pas de renonce (pas question de couper trefle par exemple si vous n'en avez pas en main mais qu'il y en a dans votre tablée).
Lorsque vous jouez une carte de la tablée et qu'elle en recouvre une autre, vous devez attendre la fin du pli pour découvrir la carte du dessous et la mettre face visible à la place de celle qui vient d'être jouée. On ne peut jouer une carte de la tablée que si elle est face visible.

La poignée est à 14 atouts, la double à 17 et la triple à 22. Comptent dans la poignée les atouts détenus en main ainsi que ceux qui figurent sur la table dans votre camp. La présentation de la poignée se fait en étalant les atouts nécessaire qu'on avait en main et éventuellement en désignant du doigt ceux de son camp sur la table.

Toutes les autres règles du jeu sont identiques au jeu classique, en dehors de la spécificité de la donne et de quelques cas particuliers.

Cas particulier du petit au bout.
Le petit est également considéré au bout si un joueur ne possède plus de cartes en main et n'a sur sa tablée qu'un seul paquet de 2 cartes, le petit étant la carte du dessus.
Il y a un autre cas de petit au bout, à l'antépénultième pli (voir à chélem ci après).

Cas du chélem
Dans la même configuration (un seul paquet de 2 cartes devant le joueur et plus rien en main), si c'est l'excuse qui est dessus elle est considérée comme étant menée au bout dans le cas d'un chélem.
S'il s'agit d'un chélem non annoncé et que tous les plis ont été réalisés par le détenteur de l'excuse, cette dernière est victorieuse du pli, quelle que soit la carte en face. Si un pli (ou +) a été réalisé par l'adversaire, elle sera considérée comme une excuse normalement menée à l'avant dernier pli, et donc ne le remportera pas mais restera la propriété de son détenteur.
S'il s'agit d'un chélem annoncé, l'excuse sera toujours victorieuse, même si l'adversaire a déjà réalisé un (ou +) pli.
Bien entendu, ces cas de figure rarissimes font que c'est la carte située sous l'excuse qui décide de la réussite ou de l'échec du chélem.
Bien entendu, si ce cas venait à se présenter, pour que le petit soit considéré au bout il faut qu'il ait été joué au coup immédiatement précédent.

Cas de l'excuse dernière carte jouée et retournée
Sauf en cas de chélem, l'excuse jouée en dernier change de camp, même si le joueur n'a pas pu faire autrement (par exemple il n'a plus de carte en main et un seul paquet de 2 devant lui, la carte du dessous étant l'excuse).

La donne
La donne est extrèmement importante. D'une part les cartes doivent être soigneusement mélangées entre 2 parties. D'autre part la répartition oblige à respecter certains principes.
- On donne d'abord à l'adversaire, puis à soi.
- La première et la dernière cartes du paquet ne peuvent aller ni au chien ni dans la tablée.
- Lorsqu'une carte est distribuée dans la tablée, qu'il s'agisse d'une carte cachée ou d'une carte à face visible, la suivante doit obligatoirement être posée en symétrique dans le camp adverse.
Toutefois, les cartes inférieures peuvent être données 1 fois par rangée entière (soit 4 cartes) et/ou 2 fois par demi-rangée (soit 2 cartes à chaque fois), à condition que le symétrique soit immédiatement réalisé dans le camp adverse.
( vous pouvez donc donner : soit une rangée de 4 ainsi qu'une ou deux demi-rangées, soit 1 ou 2 demi-rangées).
- Avant de pouvoir distribuer une carte supérieure dans la tablée, les 4 cartes inférieures de la rangée doivent avoir été données pour chaque camp.
- Le chien est constitué carte par carte
- On donne au maximum deux cartes à la fois dans le paquet "principal" ou dans le paquet "renfort" de chaque joueur.
- La distribution du principal et du renfort se fait de façon symétrique. Mettre des cartes au principal de l'adversaire implique que vous en mettiez le même nombre au vôtre, et identiquement pour le renfort.
- La distribution du principal et du renfort doit être alternées au moins 3 fois.

Mon avis
Contrairement aux apparences, cette variante de jeu est agréable. Elle est même beaucoup plus scientifique qu'elle n'y parait, et oblige les joueurs à un calcul permanent. Certes, ça ne vaut pas le jeu à 4, mais c'est une bonne manière de tromper le temps à deux ... je sais, y'en a d'autres... mais on parle tarot, là, non ?
En outre, l'obligation de prise pour le donneur évite de devoir redistribuer sans cesse et réserve parfois des surprises.
Les règles de la distribution peuvent paraître rébarbatives, en fait vous vous apercevrez après 3 ou 4 donnes qu'elles sont très logiques et que la symétrie à laquelle elles obligent en simplifie grandement la mémorisation et la réalisation.
Mais tant que vous ne maîtriserez pas bien le jeu, forcez vousde donner les cartes de la tablée au début, cela évitera les fausses donnes.


Un conseil.
S'il peut être parfois utile de garder des cartes sur la table pour la fin, notamment des cartes cachées, et ce pour induire l'adversaire en erreur ou le mettre au moins dans le doute, l'expérience prouve qu'en général le preneur a intérêt à avoir retourné au plus vite les cartes inférieures, ce afin de mieux dominer l'adversaire.
Le début de partie a donc moins d'importance que le milieu, la fin n'étant souvent qu'une suite logique mais calculée des coups qui précèdent.

A noter que cette façon de jouer avec des cartes sur table existe aussi dans une variante à 3 joueurs, très scientifique et également agréable.
Je tâcherai de vous la livrer prochainement, ainsi que la règle du tarot à 7 (idéal pour soirées de délire entre bons poteaux)


Bon amusement aux couples

