# 🐔 Poulailler Automatique Arduino

> Système automatisé d'ouverture/fermeture de porte de poulailler basé sur Arduino Nano avec gestion de l'heure et de la luminosité.

![Badge Arduino](https://img.shields.io/badge/Arduino-Nano-blue)
![Badge Licence](https://img.shields.io/badge/License-MIT-green)
![Badge Version](https://img.shields.io/badge/Version-1.3.0-orange)

[🇫🇷 Version française](#) | [🇺🇸 English version](README_EN.md)

---

## 📸 Aperçu du projet

**Version actuelle : 1.3.0** - Interface encodeur rotatif KY-040 professionnelle

## ✨ Fonctionnalités

### 🕐 Gestion automatique
- **Ouverture automatique à 7h00** du matin
- **Fermeture par détection de luminosité** (avec temporisation 10 min anti-nuages)
- **Fermeture forcée à 23h00** (sécurité)
- **Système anti-obstacle** avec timeout configurable

### 🎛️ Interface utilisateur (Version 1.3.0)
- **Encodeur rotatif KY-040** : Interface professionnelle et intuitive
- **Rotation** : Ajuster les valeurs (heure, seuil, timeouts) précisément
- **Click bref** : Ouvrir/fermer porte manuellement (ou rallumer LCD)
- **Click long (3s)** : Navigation dans les modes de réglage
- **Gestion intelligente du rétroéclairage LCD** : Extinction automatique après 30s

### 🔒 Sécurités
- **Capteurs de fin de course** haut et bas
- **Détection d'obstacles** avec timeout moteur
- **Sauvegarde EEPROM** des paramètres
- **LED d'alerte** coupure de courant

## 🆕 Nouveautés Version 1.3.0

### 🎯 Interface encodeur rotatif KY-040
- ✅ **Rotation intuitive** : Tourner pour modifier les valeurs (±1 par cran)
- ✅ **100% fiable** : Bibliothèque Encoder de Paul Stoffregen (référence Arduino)
- ✅ **Détection matérielle** : Interruptions pour réactivité parfaite
- ✅ **Plus de bugs timing** : Fini les problèmes de click/double-click
- ✅ **Code simplifié** : Architecture procédurale propre

### 🐛 Corrections importantes
- ✅ **Bug double-clic** : Supprimé, remplacé par rotation
- ✅ **Blocages LCD** : Rafraîchissement throttlé (200ms)
- ✅ **Timeouts réglage** : 30s avec extinction automatique LCD

## 🔧 Matériel requis

**Composants principaux** (voir [`docs/liste_composants.md`](docs/liste_composants.md) pour la liste complète) :
- Arduino Nano (ATmega328P)
- Module RTC DS3231 (I2C)
- Écran LCD 16x2 I2C
- Module contrôleur moteur L298N
- **Encodeur rotatif KY-040** ⭐ NOUVEAU (~2-3€)
- Capteur de luminosité (LDR)
- 2x Capteurs fin de course
- Moteur CC 12V
- LED rouge
- Alimentation 12V 2A

**Matériel optionnel recommandé :**
- 2x Condensateurs céramiques 100nF (filtrage encodeur)

**Budget estimé** : 145-205€ selon choix moteur et mécanique

## 📚 Bibliothèques Arduino requises

Installation via le gestionnaire de bibliothèques Arduino IDE :

1. **RTClib** (Adafruit) - Version >= 2.0.0
2. **LiquidCrystal_I2C** (Frank de Brabander) - Version >= 1.1.2
3. **Encoder** ⭐ (Paul Stoffregen) - Version >= 1.4.0 - NOUVEAU

## 📐 Schéma de câblage

```
Arduino Nano    →    Composant
─────────────────────────────────
A0              →    Capteur luminosité
D2              →    Encodeur CLK (interruption)
D3              →    LED coupure courant
D4              →    Encodeur DT (interruption)
D5              →    Encodeur SW (bouton)
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
git clone https://github.com/fbaillon19/poulailler-automatique.git
cd poulailler-automatique
```

### 2. Installation des bibliothèques
Ouvrir Arduino IDE → Croquis → Inclure une bibliothèque → Gérer les bibliothèques
- Rechercher et installer : `RTClib`, `LiquidCrystal_I2C`, `Encoder`

### 3. Ouverture du projet
1. Ouvrir le dossier `code/poulailler_automatique/`
2. Double-cliquer sur `poulailler_automatique_v1.3.0.ino`
3. Vérifier que les fichiers `rotary_encoder.h` et `rotary_encoder.cpp` sont bien présents

### 4. Compilation et téléversement
1. Sélectionner : **Outils → Type de carte → Arduino Nano**
2. Sélectionner : **Outils → Processeur → ATmega328P (Old Bootloader)**
3. Compiler et téléverser

## 📱 Utilisation

### Mode Normal
| Action | Résultat |
|--------|----------|
| **Tourner encodeur** | Aucun effet |
| **Click bref** | Ouvrir/fermer porte (ou rallumer LCD si éteint) |
| **Click long (3s)** | Entrer en mode réglage HEURE |

### Modes de Réglage

**Navigation :**
- **Click long** → Passer au mode suivant
- **Tourner CW (horaire)** → Incrémenter valeur (+1)
- **Tourner CCW (anti-horaire)** → Décrémenter valeur (-1)

**Séquence :**
1. MODE_REGLAGE_HEURE → Tourner pour ajuster l'heure
2. MODE_REGLAGE_MINUTE → Tourner pour ajuster les minutes
3. MODE_REGLAGE_SEUIL → Tourner pour ajuster le seuil luminosité
4. MODE_REGLAGE_TIMEOUT_OUVERTURE → Tourner pour timeout ouverture
5. MODE_REGLAGE_TIMEOUT_FERMETURE → Tourner pour timeout fermeture
6. Retour MODE_NORMAL + extinction LCD

**Timeout :** 30 secondes sans action → retour automatique MODE_NORMAL + extinction LCD

## 📂 Structure du projet

```
poulailler-automatique/
├── code/
│   └── poulailler_automatique/
│       ├── poulailler_automatique.ino      # Fichier principal
│       ├── rotary_encoder.h                # Header encodeur
│       └── rotary_encoder.cpp              # Implémentation encodeur
├── docs/
│   ├── liste_composants.md        # Liste détaillée des composants
│   ├── guide_cablage.md          # Schémas de câblage
│   ├── guide_montage.md          # Guide assemblage mécanique
│   └── README_v1.3.0.md          # Documentation version 1.3.0
├── schemas/
│   └── schema_description.md     # Description schéma Fritzing
├── README.md                     # Ce fichier
├── README_EN.md                  # Version anglaise
├── CHANGELOG.md                  # Historique des versions
├── LICENSE                       # Licence MIT
└── .gitignore                    # Fichiers à ignorer
```

## 🔍 Diagnostic et débogage

### Messages moniteur série (9600 bauds)
```
Encodeur KY-040 initialisé (bibliothèque Encoder)
  CLK=2 DT=4 SW=5
Système initialisé
Rotation: CW (+1)
  Seuil: 305
Bouton: APPUI LONG
  → MODE_REGLAGE_HEURE
```

### Messages d'état LCD
- `"Porte ouverte"` / `"Porte fermee"` : État normal
- `"Ouverture..."` / `"Fermeture..."` : Mouvement en cours
- `"Ferme dans Xmn"` : Temporisation luminosité active
- `"ERREUR OBSTACLE"` : Obstacle détecté
- `"Tourner encodeur"` : En mode réglage timeout

## 🤝 Contribution

Les contributions sont les bienvenues ! 

1. **Fork** le projet
2. Créer une **branche feature** (`git checkout -b feature/amelioration`)
3. **Commit** vos changements (`git commit -m 'Ajout fonctionnalité'`)
4. **Push** vers la branche (`git push origin feature/amelioration`)
5. Ouvrir une **Pull Request**

## 📝 Changelog

### Version 1.3.0 (Décembre 2025)
- ✅ Encodeur rotatif KY-040 pour interface professionnelle
- ✅ Bibliothèque Encoder de Paul Stoffregen (ultra-fiable)
- ✅ Rotation pour modifier valeurs (±1 par cran)
- ✅ Suppression bugs click/double-click
- ✅ Correction blocages LCD (rafraîchissement throttlé)
- ✅ Timeout réglage 30s avec extinction LCD
- ✅ Architecture procédurale propre

### Version 1.2.0 (Décembre 2025)
- ✅ Refactorisation avec OneButton
- ✅ Architecture modulaire (fichiers .h et .cpp séparés)
- ✅ Correction bug affichage modes timeout

### Version 1.1.0 (Décembre 2025)
- ✅ Gestion intelligente rétroéclairage LCD
- ✅ Extinction automatique après 30s
- ✅ Réveil automatique pendant temporisation

### Version 1.0.0 (Décembre 2025)
- ✅ Version initiale fonctionnelle

## 📄 Licence

Ce projet est sous licence **MIT** - voir le fichier [LICENSE](LICENSE) pour plus de détails.

## 🙏 Remerciements

- Communauté Arduino pour les bibliothèques
- Paul Stoffregen pour la bibliothèque Encoder
- Forums makers pour les conseils mécaniques

## 📞 Support

- **Issues GitHub** : Pour les bugs et suggestions
- **Discussions** : Questions générales
- **Wiki** : Documentation détaillée

---

⭐ **N'hésitez pas à mettre une étoile si ce projet vous a aidé !** ⭐

**Auteur** : Frédéric BAILLON  
**Version** : 1.3.0  
**Date** : Décembre 2025