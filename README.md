# Projet c-wire

Ce programme permet de faire la synthèse de données d'un système de distribution d'électricité.
C'est à dire, il permet de savoir si les stations (centale, sous-stations HVA, sous-stations HVB, postes LV) sous en surproduction
ou en sous- production.
Le programme utilise un arbre AVL pour organiser les données des stations en fonction de leur ID, ce qui permet un accès rapide et équilibré aux données.

# Fonctionnalités principales du programme:

1 - Lecture des données depuis un fichier CSV.

2 - Tri et insertion des stations dans un arbre AVL.

3 - Traitement des stations pour obtenir les 10 stations avec la plus grande et la plus faible consommation.

4 - Génération de fichiers de sortie formatés selon les spécifications.
Si la catégorie de station est "LV" et que le type de consommateur est "all", un fichier supplémentaire contenant les stations avec les valeurs extrêmes de "capacité - consommation" est généré.



