# 🐔 Poulailler Automatique Arduino

> Système automatisé d'ouverture/fermeture de porte de poulailler basé sur Arduino Nano avec gestion de l'heure et de la luminosité.

![Badge Arduino](https://img.shields.io/badge/Arduino-Nano-blue)
![Badge Licence](https://img.shields.io/badge/License-MIT-green)
![Badge Version](https://img.shields.io/badge/Version-1.1.0-orange)

[🇫🇷 Version française](README.md) | [🇺🇸 English version](README_EN.md)

## 📸 Aperçu du projet

*[Ajoutez ici des photos de votre réalisation]*

## ✨ Fonctionnalités

### 🕐 Gestion automatique
- **Ouverture automatique à 7h00** du matin
- **Fermeture par détection de luminosité** (avec temporisation 10 min anti-nuages)
- **Hystérésis du capteur de lumière** (évite les oscillations près du seuil)
- **Fermeture forcée à 23h00** (sécurité)
- **Système anti-obstacle** avec timeout configurable

### 🎛️ Interface utilisateur
- **Écran LCD 16x2** : affichage heure + statut porte
- **Rétroéclairage intelligent** : extinction automatique après 30s (60s la nuit)
- **Bouton multifonction** :
  - Appui bref : ouvrir/fermer manuellement
  - Appui long : accès aux réglages
- **Réglage complet sans ordinateur** : heure, seuil luminosité, timeouts

### 🔒 Sécurités
- **Capteurs de fin de course** haut et bas
- **Détection d'obstacles** avec timeout moteur
- **Sauvegarde EEPROM** des paramètres
- **LED d'alerte** coupure de courant

### ⚡ Optimisations
- **Économie d'énergie** : rétroéclairage LCD avec extinction auto
- **Optimisation RAM** : utilisation de la mémoire FLASH pour les chaînes
- **Stabilité** : hystérésis pour éviter les détections erratiques

## 🔧 Matériel requis

Voir le fichier détaillé : [`docs/liste_composants.md`](docs/liste_composants.md)

**Composants principaux** :
- Arduino Nano
- Module RTC DS3231
- Écran LCD 16x2 I2C
- Module contrôleur moteur L298N
- Capteur de luminosité
- 2x fins de course
- Moteur CC 12V

## 📐 Schéma de câblage

```
Arduino Nano    →    Composant
─────────────────────────────────
A0              →    Capteur luminosité
D3              →    LED coupure courant
D5              →    Bouton multifonction
D6              →    Moteur Pin 2
D7              →    Moteur Pin 1  
D8              →    Fin course HAUT
D9              →    Fin course BAS
SDA (A4)        →    LCD + RTC (I2C)
SCL (A5)        →    LCD + RTC (I2C)
5V              →    Alimentation modules
GND             →    Masse commune
```

*Schéma Fritzing détaillé : [`schemas/schema_cablage.fzz`](schemas/)*

## 🆕 Nouveautés Version 1.1.0

### 🔆 Gestion intelligente du rétroéclairage LCD
- **Extinction automatique** après 30 secondes d'inactivité
- **Mode nuit** : extinction après 1 minute entre 22h et 6h
- **Réveil automatique** lors de toute interaction (bouton, mouvement porte)
- **Économie d'énergie** et préservation de la durée de vie du LCD

### 📊 Amélioration capteur de luminosité
- **Hystérésis configurable** (constante `LIGHT_HYSTERESIS = 20`)
- **Évite les oscillations** quand la lumière est proche du seuil
- **Plus stable** : pas de déclenchements intempestifs au crépuscule
- **Logs améliorés** avec affichage des valeurs détectées

### 🐛 Corrections de bugs
- Correction du timeout mode réglage qui ne se réinitialisait pas
- Correction du freeze d'affichage en mode réglage timeout
- Ajout des accolades obligatoires dans le switch/case
- Optimisation RAM avec macro `F()` sur toutes les chaînes

## 🚀 Installation

### 1. Prérequis Arduino IDE
```bash
# Bibliothèques nécessaires
- RTClib (Adafruit)
- LiquidCrystal_I2C
```

### 2. Téléchargement
```bash
git clone https://github.com/votre-username/poulailler-automatique.git
cd poulailler-automatique
```

### 3. Configuration
1. Ouvrir `code/poulailler_automatique.ino`
2. Ajuster l'adresse I2C du LCD si nécessaire (ligne 27)
3. Téléverser vers l'Arduino Nano

### 4. Premier démarrage
1. Régler l'heure : **Appui long → Naviguer avec appuis brefs → Appui long suivant**
2. Ajuster le seuil de luminosité selon votre environnement
3. Tester les timeouts moteur selon votre installation

## ⚙️ Configuration avancée

### Paramètres configurables dans le code

Si vous souhaitez personnaliser le comportement, vous pouvez modifier ces constantes en début de code :
```cpp
// Temporisation fermeture lumière faible
const unsigned long CLOSING_DELAY = 600000;      // 10 minutes (défaut)

// Timeouts rétroéclairage LCD
const unsigned long BACKLIGHT_TIMEOUT = 30000;   // 30s en journée
const unsigned long BACKLIGHT_NIGHT_OFF = 60000; // 60s la nuit

// Hystérésis capteur luminosité
const int LIGHT_HYSTERESIS = 20;                 // Recommandé: 10-30
```
Hystérésis du capteur de lumière :

Valeur 10 : Plus sensible, peut osciller
Valeur 20 : Recommandé - Bon compromis
Valeur 30 : Très stable, moins réactif

## 📱 Utilisation

### Navigation interface
| Action | Résultat |
|--------|----------|
| **Appui bref** | Ouvrir/fermer porte manuellement |
| **Appui long (3s)** | Entrer en mode réglage |
| **Double-clic** | Diminuer valeur (mode réglage) |

### Séquence de réglage
1. **Réglage heure** → Appui bref : +1h
2. **Réglage minute** → Appui bref : +1min  
3. **Seuil luminosité** → Appui bref : +5, Double-clic : -5
4. **Timeout ouverture** → Appui bref : +1s, Double-clic : -1s
5. **Timeout fermeture** → Appui bref : +1s, Double-clic : -1s

## 🔍 Diagnostic

### Messages d'état LCD
- `"Porte ouverte"` / `"Porte fermee"` : État normal
- `"Ouverture..."` / `"Fermeture..."` : Mouvement en cours
- `"Ferme dans Xmn"` : Temporisation luminosité active
- `"ERREUR OBSTACLE"` : Obstacle détecté, appui bref pour réessayer

### LED clignotante
- **LED rouge clignotante** : Coupure de courant détectée (RTC a perdu l'heure)

## 🛠️ Assemblage mécanique

### Suggestions de construction
- **Porte** : Panneau léger (PVC, alu, bois fin)
- **Guidage** : Rails en U ou cornières
- **Transmission** : Poulie + câble ou crémaillère + pignon
- **Sécurité** : Contrepoids ou ressort en cas de panne

### Positionnement capteurs
- **Fin course HAUT** : Déclenchement porte complètement ouverte
- **Fin course BAS** : Déclenchement porte fermée au sol
- **Capteur luminosité** : À l'abri des intempéries, orienté vers le ciel

## 🤝 Contribution

Les contributions sont les bienvenues ! 

1. **Fork** le projet
2. Créer une **branche feature** (`git checkout -b feature/amelioration`)
3. **Commit** vos changements (`git commit -m 'Ajout fonctionnalité'`)
4. **Push** vers la branche (`git push origin feature/amelioration`)
5. Ouvrir une **Pull Request**

## 📝 Changelog

### Version 1.0.0
- ✅ Gestion automatique heure + luminosité
- ✅ Interface LCD complète
- ✅ Réglages via bouton unique
- ✅ Système anti-obstacles
- ✅ Sauvegarde EEPROM

## 📄 Licence

Ce projet est sous licence **MIT** - voir le fichier [LICENSE](LICENSE) pour plus de détails.

## 🙏 Remerciements

- Communauté Arduino pour les bibliothèques
- Forums makers pour les conseils mécaniques
- Testeurs beta pour les retours terrain

## 📞 Support

- **Issues GitHub** : Pour les bugs et suggestions
- **Wiki** : Documentation détaillée
- **Discussions** : Questions générales

---

⭐ **N'hésitez pas à mettre une étoile si ce projet vous a aidé !** ⭐
