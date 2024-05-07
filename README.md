# Blobwar

Blobwar est un jeu à deux joueurs en tour par tour. Le but du jeu est d'éliminer tous les pions adverses ou d'être majoritaire en fin de partie (lorsque le plateau est rempli). À son tour un joueur choisit un de ses pions, et le déplace sur une case vide à une distance 1 ou 2. S'il se déplace à distance 1 le pion est dupliqué. À distance 2 c'est un saut et le pion se déplace directement, sans duplication. Lorsqu'un pion arrive sur sa case de destination, tous ses voisins deviennent de sa couleur.

## Compilation et exécution

```
make
./blobwar
```

## Stratégies

L'IA utilise différentes stratégies :
- un algorithme glouton
- un algorithme min-max
- un algorithme alpha-beta

Pour changer de stratégie, il faut modifier dans la classe de stategy dans le fichier `launchStrategy.cc`

```c++
// GreedyStrategy strategy(blobs, holes, cplayer, func);
// MinMaxStrategy strategy(blobs, holes, cplayer, func);
AlphaBetaStrategy strategy(blobs, holes, cplayer, func);
strategy.computeBestMove();
```