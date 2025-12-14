# ProjetSimon
Simon Game - ESP32

Un jeu de type Simon implémenté sur ESP32 avec écran OLED, LEDs, buzzer et boutons. Le joueur doit reproduire une séquence lumineuse et sonore qui s’allonge à chaque tour. Le jeu propose plusieurs niveaux de difficulté et conserve le record (high score) en mémoire.

Fonctionnalités

Affichage sur écran OLED 128x64 via la librairie Adafruit SSD1306.

4 LEDs et 4 boutons pour le gameplay.

Buzzer pour les sons et feedback.

Séquences aléatoires qui deviennent progressivement plus longues.

Plusieurs niveaux de difficulté :

Facile

Normale

Difficile

Extrême

Sauvegarde automatique du high score dans la mémoire flash de l’ESP32 via Preferences.

Animations pour :

Lancement du jeu

Bienvenue

Fin de partie

Sélection de difficulté

Mode veille via un interrupteur (deep sleep).

Matériel requis

ESP32

Écran OLED 128x64 compatible I²C

4 x LEDs (rouge, vert, bleu, jaune)

4 x résistances (220-330 Ω) pour LEDs

4 x boutons-poussoirs

1 x buzzer

1 x interrupteur pour le mode veille

Câblage et breadboard

Schéma de connexion
Composant	ESP32 Pin
LED rouge	25
LED verte	26
LED bleue	27
LED jaune	14
Bouton rouge	17
Bouton vert	5
Bouton bleu	18
Bouton jaune	19
Buzzer	16
Switch veille	4
LED de démarrage	33
OLED SDA	21 (I2C)
OLED SCL	22 (I2C)

(Adapter selon votre câblage.)

Installation

Installer les librairies nécessaires dans l’IDE Arduino :

Adafruit GFX Library

Adafruit SSD1306

Copier le code source dans l’IDE Arduino.

Sélectionner la carte ESP32 et le port correct.

Compiler et téléverser.

Utilisation

Allumer l’ESP32. L’écran OLED affichera un message de bienvenue.

Choisir la difficulté avec les boutons :

Boutons pour naviguer et confirmer le choix.

Le jeu commence :

Observer la séquence lumineuse et sonore.

Reproduire la séquence avec les boutons.

Si la séquence est correctement reproduite :

Le score augmente et une nouvelle séquence est ajoutée.

Si la séquence est incorrecte :

Affichage d’une croix sur l’OLED et le son Game Over.

Retour au menu de sélection de difficulté.

Le high score est sauvegardé automatiquement.

Difficultés
Niveau	Vitesse LED/Son (ms)	Séquence ajoutée par tour
Facile	600	1
Normale	400	1
Difficile	250	2
Extrême	150	3
Fonctionnalités avancées

Animations OLED : démarrage, bienvenue, fin de partie.

Mode veille automatique : maintenir le switch bascule l’ESP32 en deep sleep.

Feedback visuel et sonore pour chaque action.

Auteur

Clément HUBERT
