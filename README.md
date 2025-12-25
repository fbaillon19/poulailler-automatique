# 🐔 Poulailler Automatique Arduino

> Système automatisé d'ouverture/fermeture de porte de poulailler basé sur Arduino Nano avec gestion de l'heure et de la luminosité.

![Badge Arduino](https://img.shields.io/badge/Arduino-Nano-blue)
![Badge Licence](https://img.shields.io/badge/License-MIT-green)
![Badge Version](https://img.shields.io/badge/Version-1.2.0-orange)

[🇫🇷 Version française](#) | [🇺🇸 English version](README_EN.md)

---

## 📸 Aperçu du projet

**Version actuelle : 1.2.0** - Refactorisée avec OneButton pour une gestion bouton robuste

## ✨ Fonctionnalités

### 🕐 Gestion automatique
- **Ouverture automatique à 7h00** du matin
- **Fermeture par détection de luminosité** (avec temporisation 10 min anti-nuages)
- **Fermeture forcée à 23h00** (sécurité)
- **Système anti-obstacle** avec timeout configurable

### 🎛️ Interface utilisateur
- **Écran LCD 16x2** : affichage heure + statut porte
- **Bouton multifonction** (géré par OneButton) :
  - Appui bref : ouvrir/fermer manuellement
  - Double-clic : éteindre/rallumer LCD
  - Appui long : accès aux réglages
- **Réglage complet sans ordinateur** : heure, seuil luminosité, timeouts
- **Gestion intelligente du rétroéclairage LCD** : extinction automatique après 30s d'inactivité

### 🔒 Sécurités
- **Capteurs de fin de course** haut et bas
- **Détection d'obstacles** avec timeout moteur
- **Sauvegarde EEPROM** des paramètres
- **LED d'alerte** coupure de courant

## 🆕 Nouveautés Version 1.2.0

### 🎯 Architecture refactorisée
- ✅ **Code modulaire** : Séparation en fichiers `.ino`, `.h`, `.cpp`
- ✅ **Bibliothèque OneButton** : Gestion bouton robuste et fiable
- ✅ **Messages debug Serial** : Débogage facilité
- ✅ **Code plus maintenable** : Structure claire et documentée

### 🐛 Corrections importantes
- ✅ **Affichage modes timeout** : Bug résolu avec définition enum explicite
- ✅ **Gestion scope variables** : Accolades dans switch/case
- ✅ **LCD toujours à jour** : Rallumage automatique lors des transitions

## 🔧 Matériel requis

**Composants principaux** (voir [`docs/liste_composants.md`](docs/liste_composants.md) pour la liste complète) :
- Arduino Nano (ATmega328P)
- Module RTC DS3231 (I2C)
- Écran LCD 16x2 I2C
- Module contrôleur moteur L298N
- Capteur de luminosité (LDR)
- 2x Capteurs fin de course
- Moteur CC 12V
- Bouton poussoir
- LED rouge
- Alimentation 12V 2A

**Budget estimé** : 140-200€ selon choix moteur et mécanique

## 📚 Bibliothèques Arduino requises

Installation via le gestionnaire de bibliothèques Arduino IDE :

1. **RTClib** (Adafruit) - Version >= 2.0.0
2. **LiquidCrystal_I2C** (Frank de Brabander) - Version >= 1.1.2
3. **OneButton** ⭐ (Matthias Hertel) - Version >= 2.0.0

## 📐 Schéma de câblage

```
Arduino Nano    →    Composant
─────────────────────────────────
A0              →    Capteur luminosité
D3              →    LED coupure courant
D5              →    Bouton multifonction
D6              →    L298N IN2
D7              →    L298N IN1
D8              →    Fin course HAUT
D9              →    Fin course BAS
SDA (A4)        →    LCD + RTC (I2C)
SCL (A5)        →    LCD + RTC (I2C)
5V              →    Alimentation modules
GND             →    Masse commune
```

**Schémas détaillés** : Voir [`docs/guide_cablage.md`](docs/guide_cablage.md)

## 🚀 Installation rapide

### 1. Téléchargement
```bash
git clone https://github.com/votre-username/poulailler-automatique.git
cd poulailler-automatique
```

### 2. Installation des bibliothèques
Ouvrir Arduino IDE → Croquis → Inclure une bibliothèque → Gérer les bibliothèques
- Rechercher et installer : `RTClib`, `LiquidCrystal_I2C`, `OneButton`

### 3. Ouverture du projet
1. Ouvrir le dossier `code/poulailler_automatique/`
2. Double-cliquer sur `poulailler_automatique_v1.2.0.ino`
3. Vérifier que les fichiers `button.h` et `button.cpp` sont bien présents

### 4. Compilation et téléversement
1. Sélectionner : **Outils → Type de carte → Arduino Nano**
2. Sélectionner : **Outils → Processeur → ATmega328P (Old Bootloader)**
3. Compiler et téléverser

## 📱 Utilisation

### Navigation interface
| Action | Résultat |
|--------|----------|
| **Appui bref** | Ouvrir/fermer porte manuellement |
| **Double-clic** | Éteindre/rallumer LCD |
| **Appui long (3s)** | Entrer en mode réglage |

### Séquence de réglage
1. **Réglage heure** → Appui bref : +1h
2. **Réglage minute** → Appui bref : +1min
3. **Seuil luminosité** → Appui bref : +5, Double-clic : -5
4. **Timeout ouverture** → Appui bref : +1s, Double-clic : -1s
5. **Timeout fermeture** → Appui bref : +1s, Double-clic : -1s

## 📂 Structure du projet

```
poulailler-automatique/
├── code/
│   └── poulailler_automatique/
│       ├── poulailler_automatique_v1.2.0.ino  # Fichier principal
│       ├── button.h                            # Header gestion bouton
│       └── button.cpp                          # Implémentation bouton
├── docs/
│   ├── liste_composants.md        # Liste détaillée des composants
│   ├── guide_cablage.md          # Schémas de câblage
│   ├── guide_montage.md          # Guide assemblage mécanique
│   └── README_v1.2.0.md          # Documentation version 1.2.0
├── schemas/
│   └── schema_description.md     # Description schéma Fritzing
├── README.md                     # Ce fichier
├── README_EN.md                  # Version anglaise
├── LICENSE                       # Licence MIT
└── .gitignore                    # Fichiers à ignorer
```

## 🔍 Diagnostic et débogage

### Messages moniteur série (9600 bauds)
```
Bouton initialisé avec OneButton / Button initialized with OneButton
Système initialisé / System initialized
Click détecté / Click detected
Appui long détecté / Long press detected
Passage MODE_REGLAGE_HEURE / Switch to hour setting
```

### Messages d'état LCD
- `"Porte ouverte"` / `"Porte fermee"` : État normal
- `"Ouverture..."` / `"Fermeture..."` : Mouvement en cours
- `"Ferme dans Xmn"` : Temporisation luminosité active
- `"ERREUR OBSTACLE"` : Obstacle détecté

### LED clignotante
- **LED rouge clignotante** : Coupure de courant détectée (RTC a perdu l'heure)

## 🤝 Contribution

Les contributions sont les bienvenues ! 

1. **Fork** le projet
2. Créer une **branche feature** (`git checkout -b feature/amelioration`)
3. **Commit** vos changements (`git commit -m 'Ajout fonctionnalité'`)
4. **Push** vers la branche (`git push origin feature/amelioration`)
5. Ouvrir une **Pull Request**

## 📝 Changelog

### Version 1.2.0 (Décembre 2025)
- ✅ Refactorisation avec OneButton pour gestion bouton robuste
- ✅ Architecture modulaire (fichiers .h et .cpp séparés)
- ✅ Correction bug affichage modes timeout (enum explicite)
- ✅ Rallumage automatique LCD lors des transitions
- ✅ Messages debug Serial détaillés
- ✅ Gestion scope variables dans switch/case

### Version 1.1.0 (Décembre 2025)
- ✅ Gestion automatique heure + luminosité
- ✅ Interface LCD complète
- ✅ Réglages via bouton unique
- ✅ Système anti-obstacles
- ✅ Sauvegarde EEPROM
- ✅ Gestion intelligente rétroéclairage LCD

## 📄 Licence

Ce projet est sous licence **MIT** - voir le fichier [LICENSE](LICENSE) pour plus de détails.

## 🙏 Remerciements

- Communauté Arduino pour les bibliothèques
- Matthias Hertel pour la bibliothèque OneButton
- Forums makers pour les conseils mécaniques

## 📞 Support

- **Issues GitHub** : Pour les bugs et suggestions
- **Discussions** : Questions générales
- **Wiki** : Documentation détaillée (à venir)

---

⭐ **N'hésitez pas à mettre une étoile si ce projet vous a aidé !** ⭐

**Auteur** : Frédéric BAILLON  
**Version** : 1.2.0  
**Date** : Décembre 2025
