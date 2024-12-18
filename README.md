# Projet c-wire

Ce programme permet de faire la synthèse de données d'un système de distribution d'électricité.
C'est à dire, il permet de savoir si les stations (centale, sous-stations HVA, sous-stations HVB, postes LV) sous en surproduction
ou en sous- production.
Le programme utilise un arbre AVL pour organiser les données des stations en fonction de leur ID, ce qui permet un accès rapide et équilibré aux données.

# Fonctionnalités principales du programme:


1 - Lecture des données depuis un fichier CSV. 

2 - Tri et insertion des stations dans un arbre AVL.

3 - Génération de rapports détaillés avec tri des stations par ID.

4 - Gestion des différents types de stations (HV-B, HV-A, LV) et des différents types de consommateurs (entreprises, individuels, tous).


# Organisation de notre code source : 




-c-wire.sh                 ---> Script SHELL nécessaire à l'execution du programme dans le terminal.

/codeC                     ---> Répertoire contenant les code c du programme.

|

|

|

|- main.c                   ---> démarrage du programme, Code principal de ce dernier.

|

|-avl.c                    ---> Implémentations et fonctions pour l'arbre AVL.

|

|-avl.h                    ---> Déclarations des sturctures et fonctions AVL.

/ref                   ---> Répertoire contenant les fichiers d'entrée CSV.

|

|

|

|-hva_comp.csv                   ---> Données pour les station HV-A.

|

|-hvb_comp.csv                   ---> Données pour les stations HV-B.

|

|-lv_all.csv                     ---> Données pour les postes LV (tous consommateurs).

|

|-lv_all_minmax.csv              ---> Résultats extrêmes pour LV (généré).

|

|-lv_comp.csv                    ---> Données pour les consommateurs entreprises LV.

|

|-lv_indiv.csv                  ---> Données pour les consommateurs individuels LV.


/input                  ---> Répertoire contenant le fichier dat.

|

|

|

|-c-wire_v00.dat                   ---> Fichier dat nécessaire au lancement du programme en shell.



# Compilation :

1-Ouvrir un terminal et placez-vous dans le répertoire /codeC pour appeler le programme:

cd projet-info-s3/projet_preing2/codeC

2-Compilez le programme avec la commande make:

make

# Execution :





