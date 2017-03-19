# pacnewMngr

Ce dépôt contient un petit programme en ligne de commande pour la gestion des fichiers '.pacnew' générés par pacman, le gestionnaire de paquet d'ArchLinux.
Son fonctionnement est basé sur l'utilisation de quelques commandes unix de base, a savoir : `find`, `mv`, `vim` et `rm`.

## Fonctionnement général

Le déroulement du programme s'effectue en deux étapes successives : 
1. on recherche tous les fichiers avec l'extension '.pacnew' dans le répertoire /etc
2. pour chaque fichier trouvé on lui applique un traitement donné

Le traitement d'un fichier dépend des choix fait par l'utilisateur.
Voici le procédé actuellement mis en oeuvre :
1. on utilise la commande `diff` pour afficher les différences entre le fichier courant et le fichier .pacnew
2. on demande à l'utilisateur si il souhaite remplacer le fichier courant par le fichier  .pacnew
3. si la réponse est oui on remplace le fichier courant par le fichier .pacnew (le fichier remplacé est renommé avec l'extension '.pacold' et on passe à l'étape  suivante, sinon propose à l'utilisateur de supprimer le fichier .pacnew et on termine le traitement pour ce fichier.
4. une fois le fichier remplacé on demande à l'utilisateur si il souhaite éditer le nouveau fichier courant (ancien fichier .pacnew)
5. si la réponse est oui on ouvre le fichier avec `vim`, sinon le traitement est terminé pour ce fichier

## Configuration de base

Un certain nombre de variables peuvent être paramétrées par l'utilisateur afin de personnaliser le fonctionnement du programme.
