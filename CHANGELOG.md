# 📝 Changelog - Poulailler Automatique

Toutes les modifications notables de ce projet sont documentées dans ce fichier.

Le format est basé sur [Keep a Changelog](https://keepachangelog.com/fr/1.0.0/),
et ce projet adhère au [Versionnage Sémantique](https://semver.org/lang/fr/).

---

## [1.3.0] - 2025-12-26

### 🎉 Ajouté
- **Encodeur rotatif KY-040** : Interface professionnelle et intuitive
- **Bibliothèque Encoder** : Utilisation de la bibliothèque de Paul Stoffregen (ultra-fiable)
- **Rotation pour modifier valeurs** : Tourner CW/CCW pour incrémenter/décrémenter (±1 par cran)
- **Architecture procédurale** : Code simplifié sans POO
- **Condensateurs 100nF** : Recommandation pour filtrage encodeur (CLK-GND, DT-GND)
- **Messages Serial détaillés** : Debug complet avec indication direction rotation

### 🔧 Modifié
- **Click bref** : Ouvrir/fermer porte (ou rallumer LCD)
- **Click long** : Navigation modes réglage (3 secondes)
- **Suppression double-click** : Remplacé par rotation encodeur
- **Pins utilisées** : CLK=D2, DT=D4, SW=D5
- **Timeout réglage** : 30s (au lieu de 60s) avec extinction LCD
- **Rafraîchissement LCD** : Throttlé à 200ms pour éviter blocages

### 🐛 Corrigé
- **Bug double-click** : Supprimé complètement (remplacé par rotation)
- **Blocages LCD** : Plus de problèmes de timing avec OneButton
- **Direction rotation** : Sens horaire (CW) incrémente correctement
- **Déclarations forward** : Ajout prototypes fonctions pour compilation

### 🗑️ Supprimé
- **Bibliothèque OneButton** : Plus nécessaire
- **Gestion manuelle interruptions** : Déléguée à bibliothèque Encoder
- **Code complexe timing bouton** : Simplifié grâce à Encoder

### 📚 Documentation
- Ajout README_v1.3.0.md avec guide encodeur KY-040
- Mise à jour README.md principal
- Section troubleshooting encodeur rotatif

---

## [1.2.0] - 2025-12-25

### 🎉 Ajouté
- **Architecture modulaire** : Séparation en fichiers `button.h` et `button.cpp`
- **Bibliothèque OneButton** : Gestion professionnelle des événements bouton
- **Messages debug Serial** : Traces détaillées pour chaque action
- **Documentation v1.2.0** : Guide d'installation et utilisation OneButton
- **Rallumage automatique LCD** : Lors de chaque transition de mode
- **Définitions enum explicites** : Valeurs 0-5 pour éviter ambiguïtés

### 🔧 Modifié
- **Gestion du bouton** : Passage de code manuel à OneButton (callbacks)
- **Structure du code** : 1 fichier monolithique → 3 fichiers modulaires
- **Timing bouton** : 
  - Appui long : 3000ms
  - Clic : 250ms max
  - Double-clic : 500ms entre clics
- **Fonction `gererAffichageLCD()`** : Scope local avec accolades dans `case MODE_REGLAGE_SEUIL`

### 🐛 Corrigé
- **Bug affichage modes timeout** : L'enum n'était pas visible dans tous les fichiers
  - Solution : Définition explicite dans `button.h` avec valeurs `= 0, = 1...`
- **LCD ne s'affichait pas** : Problème de scope dans switch/case
  - Solution : Ajout d'accolades `{}` autour des case avec variables locales
- **LCD s'éteignait en mode réglage** : Timeout inactivité se déclenchait
  - Solution : Appel `allumerLCD()` à chaque transition de mode
- **Méthode OneButton** : `setIdleTicks()` n'existe pas
  - Solution : Utilisation de `setIdleMs()` à la place

### 📚 Documentation
- Ajout README_v1.2.0.md avec guide OneButton
- Mise à jour README.md principal
- Ajout section troubleshooting compilation

### 🔍 Débogage amélioré
- Messages Serial pour chaque événement bouton
- Affichage mode actuel lors des transitions
- Logs détaillés des actions utilisateur

---

## [1.1.0] - 2025-12-24

### 🎉 Ajouté
- **Gestion rétroéclairage LCD** : Extinction automatique après 30s d'inactivité
- **Réveil automatique LCD** : Toutes les minutes pendant temporisation fermeture
- **Double-clic** : Éteindre manuellement le LCD
- **Variable `derniereActivite`** : Tracking inactivité utilisateur
- **Constante `TIMEOUT_LCD`** : 30 secondes configurable

### 🔧 Modifié
- **Gestion bouton** : Ajout détection double-clic (500ms)
- **Fonction `allumerLCD()`** : Met à jour `derniereActivite`
- **Loop principale** : Logique extinction automatique LCD

### 🐛 Corrigé
- **LCD restait allumé** : Ajout timeout inactivité
- **Pas de réveil pendant temporisation** : Timer 1 minute ajouté
- **Consommation énergie** : LCD s'éteint la nuit

---

## [1.0.0] - 2025-12-20

### 🎉 Version initiale
- **Ouverture automatique** à 7h00
- **Fermeture luminosité** avec temporisation 10 minutes anti-nuages
- **Fermeture forcée** à 23h00
- **Interface LCD 16x2** : Affichage heure + statut
- **Bouton multifonction** :
  - Appui bref : Ouvrir/fermer manuellement
  - Appui long : Accès réglages
- **Système anti-obstacle** : Timeout moteur configurable
- **Sauvegarde EEPROM** : Seuil luminosité, timeouts
- **LED alerte** : Clignotement si coupure courant RTC
- **Capteurs fin de course** : Haut et bas
- **Modes de réglage** :
  - Heure
  - Minutes
  - Seuil luminosité
  - Timeout ouverture
  - Timeout fermeture
- **RTC DS3231** : Horloge temps réel précise
- **Gestion I2C** : LCD + RTC sur même bus
- **Documentation complète** : 
  - Guide câblage
  - Liste composants
  - Guide montage mécanique
  - Schémas Fritzing

---

## [Non publié] - En développement

### 🚀 Prévu pour futures versions
- [ ] Module WiFi ESP32 pour contrôle distant
- [ ] Application mobile iOS/Android
- [ ] Capteur de température DHT22
- [ ] Historique EEPROM des ouvertures/fermetures
- [ ] Mode manuel permanent (bypass automatique)
- [ ] Calibration automatique seuil luminosité
- [ ] Notification push en cas d'erreur
- [ ] Alimentation solaire + batterie
- [ ] Multi-langues (FR/EN)
- [ ] Écran OLED au lieu de LCD

---

## Types de changements

- `Ajouté` : Nouvelles fonctionnalités
- `Modifié` : Changements de fonctionnalités existantes
- `Obsolète` : Fonctionnalités bientôt retirées
- `Supprimé` : Fonctionnalités retirées
- `Corrigé` : Corrections de bugs
- `Sécurité` : Corrections de vulnérabilités

---

**[1.3.0]** : https://github.com/fbaillon19/poulailler-automatique/releases/tag/v1.3.0  
**[1.2.0]** : https://github.com/fbaillon19/poulailler-automatique/releases/tag/v1.2.0  
**[1.1.0]** : https://github.com/fbaillon19/poulailler-automatique/releases/tag/v1.1.0  
**[1.0.0]** : https://github.com/fbaillon19/poulailler-automatique/releases/tag/v1.0.0
