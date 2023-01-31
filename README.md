# ouvrir_boite_servo_capteur_ultrasons
Contrôle de l'ouverture d'une boîte avec servomoteur et détection ultrasonique sur Arduino.

Retrouvez ici la vidéo de démonstration (+ quelques détails du code abordés) :

<h2>Matériel</h2>

* 1 Arduino Nano ATmega 328 ;
* 3 LED et leurs résistances adaptées ;
* 1 buzzer et une résistance adaptée ;
* 1 capteur ultrasonique HC-SR04 ;
* 1 servomoteur SG90.

Le schéma de branchement est disponible dans les fichiers du projet.

<h2>Caractéristiques du projet</h2>

(1) Lorsqu'un objet est détecté par le servomoteur, la LED verte s'allume et le servomoteur s'active.

(2) Si le capteur ne détecte plus rien pendant un certain temps (court), la LED jaune s'allume : cela signifie qu'une fermeture est prévue dans 5 secondes, un compte à rebours est donc lancé. Si un objet est détecté par le capteur avant la fin du compte à rebours, retour à l'étape (1).

(3) Si aucun n'objet a n'été détecter à l'issue des 5 secondes (qui est la limite du compte à rebours), la LED rouge s'allume et le servomoteur s'active à nouveau. Si un objet est détecté par le capteur, retour à l'étape (1).

- Pour modifier la distance à laquelle doit être placé au maximum un objet pour activer le servomoteur : ligne 16 (variable distanceMax).
- Pour modifier la durée du compte à rebours : ligne 116 (boucle if-else).

Notez que le capteur ultrasonique détecte mieux les objets plats et placer à +/-15° de l'axe 0.
