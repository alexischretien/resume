# Resume

## Description

Programme visant a créer un "resumé" d'une histoire contenue dans un 
fichier xml. 

Le programme utilise un algorithme de partionnement sur un graphe 
des "distances" (mesure inversement proportionnelle à la similarité) 
de chaque paire de phrases du texte donnée pour partionner/regrouper
les phrases qui sont similaires. Après le partionnement, une phrase
representante est selectionnée pour representer chaque partition. 
La phrase representante choisie est la phrase ayant la moyenne de 
distances avec les autres phrases de la partition la plus basse.
Le programme affiche une phrase representante par ligne
à la sortie standard et se termine. 

## Auteurs

* Bruno Malenfant
* Alexis Chrétien

## Compilation et exécution

```
$ make
$ ./resume [<OPTIONNEL> MODE] [<OPTIONNEL> -n NB_PHRASES] [TITRE_HISTOIRE]
```

* `MODE`: Le mode utilisé pour calculer les distances apres une fusion 
de noeuds lors du partionnement. Les options sont :
    * `-s` : "Simple Linkage Clustering", mode par défaut.
    * `-c` : "Complete Linkage Clustering".
    * `-a` : "Average Linkage Clustering". 
* `NB_PHRASES` est un entier strictement positif suivant -n. Permet de
specifier le nombre de phrases à afficher. Par défaut, le programme
affiche 5 phrases. 
* `TITRE_HISTOIRE` est le titre de l'histoire pour laquelle on desir créer
une resumé. Doit être une histoire incluse dans les fichiers *xml* 
decrits dans *listeDocument.xml*.

## Exemple d'exécution

```
$ ./resume -s -n 3 "A Problem in Communication"
 Benda was not interested in the names of these things.
 A feeble nation of idle dreamers, torn by a chaos of intertribal feuds within, menaced by powerful, conquest-lusting nations from without, Arabia was enabled by Islam, the religion of her prophet Mohammed, to unite all her sons into an intense loyalty to one cause, and to turn her dream-stuff into reality by carrying her national pride and honor beyond her boundaries and spreading it over half the known world.
 He kept darting about and handing me twigs of leaves, little sticks, pieces of bark, insects, not seeming to care much whether they were complete or not; grass-blades, several dagger-shaped locust-thorns, cross-sections of curious fruits, moving so rapidly that in a few moments my notebook bulged widely, and I had to warn him that its hundred leaves were almost filled.

```
