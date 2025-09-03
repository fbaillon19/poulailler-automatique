# 🎛️ Schéma Fritzing - Description textuelle

*En attendant le vrai schéma Fritzing, voici une description détaillée pour reproduire le montage*

## 📋 Composants à placer sur la breadboard

```
ARDUINO NANO (placé au centre de la breadboard)
├── Pin A0  → LDR → Résistance 10kΩ → GND
├── Pin A4  → SDA (RTC + LCD)  
├── Pin A5  → SCL (RTC + LCD)
├── Pin D3  → Résistance 220Ω → LED Rouge → GND
├── Pin D5  → Bouton → GND (avec pull-up interne)
├── Pin D6  → L298N IN2
├── Pin D7  → L298N IN1
├── Pin D8  → Fin de course HAUT → GND
├── Pin D9  → Fin de course BAS → GND
├── Pin 5V  → Rail d'alimentation +5V
└── Pin GND → Rail d'alimentation GND
```

## 🔌 Connections sur breadboard

### Rails d'alimentation
```
Rail + (rouge)  ← Arduino 5V
Rail - (bleu)   ← Arduino GND
```

### Zone 1 : Capteur de luminosité
```
Colonne 10-15 :
- LDR entre lignes a-c (colonnes 10-12)
- Résistance 10kΩ entre lignes c-e (colonnes 12-14)  
- Connexion Arduino A0 → ligne b colonne 10
- Connexion +5V → ligne a colonne 12
- Connexion GND → ligne e colonne 14
```

### Zone 2 : Modules I2C  
```
RTC DS3231 :
- VCC → Rail +5V
- GND → Rail GND  
- SDA → Arduino A4
- SCL → Arduino A5

LCD I2C (même bus) :
- VCC → Rail +5V
- GND → Rail GND
- SDA → Arduino A4  
- SCL → Arduino A5
```

### Zone 3 : Bouton et LED
```
Bouton :
- Pin 1 → Arduino D5
- Pin 2 → Rail GND

LED + Résistance :
- Arduino D3 → Résistance 220Ω → LED Anode
- LED Cathode → Rail GND
```

### Zone 4 : Fins de course
```
Fin de course HAUT :
- NO (Normalement Ouvert) → Arduino D8
- Commun → Rail GND

Fin de course BAS :
- NO (Normalement Ouvert) → Arduino D9  
- Commun → Rail GND
```

## ⚡ Module L298N (séparé de la breadboard)

```
Connexions Arduino → L298N :
- Arduino D7 → L298N IN1
- Arduino D6 → L298N IN2  
- Arduino 5V → L298N +5V (logique)
- Arduino GND → L298N GND

Alimentation moteur :
- Alim 12V + → L298N +12V
- Alim 12V - → L298N GND

Moteur :  
- L298N OUT1 → Moteur +
- L298N OUT2 → Moteur -
```

## 🎨 Couleurs de câbles recommandées

```
ALIMENTATION :
- Rouge : +5V, +12V
- Noir : GND, masses

SIGNAUX NUMÉRIQUES :
- Jaune : Arduino D3 (LED)
- Vert : Arduino D5 (Bouton)
- Bleu : Arduino D6, D7 (Moteur)
- Orange : Arduino D8, D9 (Fins de course)

SIGNAUX ANALOGIQUES :
- Violet : Arduino A0 (Capteur lumière)

BUS I2C :
- Blanc : SDA (A4)
- Gris : SCL (A5)
```

## 📐 Disposition physique suggérée

```
Vue de dessus de la breadboard :

     1  5  10  15  20  25  30
   ┌─────────────────────────────┐ a
 + │  LDR + Résist.              │ b  
 - │                             │ c
   │         ARDUINO             │ d
   │         NANO                │ e  
   │                             │ f
   │    Bouton  LED+R            │ g
   └─────────────────────────────┘ j

Modules externes :
- RTC et LCD : à côté de la breadboard
- L298N : boîtier séparé  
- Fins de course : fixés sur la mécanique
```

## 🔧 Étapes de montage

1. **Placer l'Arduino Nano** au centre de la breadboard
2. **Connecter les rails** d'alimentation 5V/GND
3. **Monter le capteur de luminosité** avec sa résistance pull-down
4. **Ajouter bouton et LED** avec leurs résistances
5. **Connecter les modules I2C** (RTC + LCD)
6. **Câbler les fins de course** 
7. **Raccorder le L298N** (dernière étape)
8. **Tests progressifs** après chaque étape

## ⚠️ Points de vigilance

- **Vérifier les polarités** de l'alimentation avant branchement
- **Éviter les courts-circuits** entre rails + et -
- **Câbles moteur** : utiliser du câble plus gros (1.5mm²)
- **Longueurs de câbles** : prévoir suffisamment pour les mouvements
- **Isolation** : gainer tous les câbles extérieurs

## 📱 Outils de simulation

**Avant le montage réel**, vous pouvez simuler avec :
- **TinkerCAD** (Autodesk) - Gratuit, en ligne
- **Fritzing** - Application dédiée aux montages électroniques
- **SimulIDE** - Simulateur Arduino complet

---

*Ce schéma textuel peut être reproduit dans Fritzing pour générer les vues breadboard, schématique et PCB.*
