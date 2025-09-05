# 📋 Liste finale des composants - Poulailler Automatique

## 🖥️ **Microcontrôleur et modules**

| Composant | Spécifications | Quantité | Prix estimé | Liens/Recherche |
|-----------|----------------|----------|-------------|-----------------|
| **Arduino Nano** | ATmega328P, USB Mini-B | 1 | 8-12€ | "Arduino Nano v3.0" |
| **Module RTC DS3231** | I2C, pile CR2032 incluse | 1 | 3-5€ | "DS3231 RTC module" |
| **Écran LCD 16x2 I2C** | HD44780 + PCF8574, rétroéclairage | 1 | 4-6€ | "LCD 1602 I2C module" |
| **Module L298N** | Dual H-Bridge, 2A par canal | 1 | 4-6€ | "L298N motor driver" |

## ⚡ **Alimentation**

| Composant | Spécifications | Quantité | Prix | Lien fourni |
|-----------|----------------|----------|------|-------------|
| **Module AC-DC Jolooyo** | 220V → 12V/1A + 5V/1A isolé | 1 | ~15€ | [Amazon Jolooyo](https://www.amazon.fr/gp/product/B07Z8M3SQJ/) |
| **Fusible** | 1A, type rapide | 2 | 1€ | Magasin électronique |
| **Porte-fusible** | Châssis, sécurisé | 1 | 2€ | Avec fusibles |

## 📡 **Capteurs**

| Composant | Spécifications | Quantité | Prix | Notes |
|-----------|----------------|----------|------|-------|
| **Photorésistance LDR** | GL5528 ou similaire | 1 | 1€ | "LDR photoresistor GL5528" |
| **Capteurs fin de course** | Micro-switch avec levier | 2 | 3€ | "Limit switch microswitch" |
| **Bouton poussoir** | NO, 6x6mm ou plus gros | 1 | 1€ | "Push button switch" |

## 🔧 **Composants passifs**

| Composant | Valeur | Quantité | Prix | Notes |
|-----------|--------|----------|------|-------|
| **Résistances** | 220Ω (LED) | 1 | - | Lot 100 pcs ~3€ |
| | 10kΩ (diviseur LDR) | 1 | - | |
| | 10kΩ (pull-up bouton)* | 1 | - | *Optionnel si INPUT_PULLUP |
| **Condensateurs** | 100nF céramique | 3 | - | Lot assortiment ~5€ |
| | 10µF électrolytique | 1 | - | |
| | 470µF électrolytique | 1 | - | |
| **LED** | Rouge 5mm | 1 | 0.5€ | Indicateur coupure courant |

## 🔌 **Connecteurs et câblage**

| Composant | Spécifications | Quantité | Prix | Usage |
|-----------|----------------|----------|------|--------|
| **Borniers à vis** | 2 pins, pas 5.08mm | 4 | 6€ | Moteur, alim 12V, capteurs |
| **Connecteurs femelles** | 15 pins (Arduino Nano) | 2 | 2€ | Support amovible Arduino |
| **Connecteur mâle** | 4 pins (LCD I2C) | 1 | 1€ | Connexion LCD |
| **Fil rigide** | 0.6mm, plusieurs couleurs | 5m | 3€ | Câblage perfboard |
| **Fil souple** | 1mm², rouge/noir | 2m | 2€ | Alimentation |

## 🏠 **Support et boîtier**

| Composant | Spécifications | Quantité | Prix | Notes |
|-----------|----------------|----------|------|-------|
| **Perfboard** | 80x60mm, pastilles + pistes | 1 | 3-5€ | "Stripboard 80x60" |
| **Boîtier étanche** | IP54 min, ~120x80x60mm | 1 | 8-15€ | "Waterproof enclosure" |
| **Presse-étoupe** | PG7 ou M12, étanche | 4-6 | 8€ | Passages câbles |
| **Entretoises M3** | 10mm + vis | 4 | 2€ | Fixation perfboard |

## ⚙️ **Mécanique moteur**

| Composant | Spécifications | Quantité | Prix | Notes |
|-----------|----------------|----------|------|-------|
| **Moteur CC 12V** | Couple suffisant pour votre porte | 1 | 15-25€ | Selon puissance nécessaire |
| **Réducteur** | Rapport selon vitesse souhaitée | 1 | 10-20€ | Optionnel si moteur lent |
| **Système transmission** | Poulie+câble OU crémaillère | 1 | 10-30€ | Selon votre conception |

## 🧰 **Consommables et outillage**

| Article | Spécifications | Prix | Notes |
|---------|----------------|------|-------|
| **Soudure** | 0.6-0.8mm, 60/40 ou sans plomb | 3€ | Si pas déjà en stock |
| **Flux de soudure** | Améliore qualité soudures | 2€ | Très recommandé |
| **Gaine thermorétractable** | Assortiment couleurs | 3€ | Protection connexions |
| **Colliers de serrage** | Divers diamètres | 2€ | Fixation câbles |

## 💰 **Récapitulatif des coûts**

### **Électronique (obligatoire)**
```
Microcontrôleur + modules : 20-30€
Alimentation Jolooyo      : 15€
Capteurs                  : 5€
Composants passifs        : 8€
Connecteurs + câblage     : 15€
Support + boîtier         : 20€
TOTAL ÉLECTRONIQUE        : 85-95€
```

### **Mécanique (variable selon conception)**
```
Moteur + réducteur        : 25-45€
Système transmission      : 10-30€
Fixations mécaniques      : 10-20€
TOTAL MÉCANIQUE          : 45-95€
```

### **Consommables**
```
Soudure + flux + divers   : 10€
```

## 🎯 **COÛT TOTAL PROJET : 140-200€**

*Variation selon choix moteur et système mécanique*

## 🛒 **Où acheter**

### **Électronique** :
- **Amazon** : Modules Arduino, alimentation Jolooyo
- **AliExpress** : Composants en lot (plus économique, délai plus long)
- **Gotronic/Lextronic** : Distributeurs français (rapide, support technique)

### **Mécanique** :
- **Leroy Merlin/Castorama** : Profilés, visserie, moteur basique
- **RS Components/Farnell** : Moteurs et réducteurs de qualité
- **eBay** : Moteurs 12V d'occasion (économique)

### **Boîtier et électrique** :
- **Rexel/Sonepar** : Matériel électrique professionnel
- **Amazon** : Boîtiers étanches et presse-étoupes

## ✅ **Checklist avant commande**

```
☐ Vérifier dimensions boîtier vs composants
☐ Confirmer puissance moteur nécessaire
☐ Vérifier stock outillage (fer à souder, multimètre)
☐ Prévoir 10% de marge sur quantités (pièces de rechange)
☐ Valider compatibilité connecteurs (pas de vis, diamètres)
☐ Vérifier tension réseau (220V France confirmé)
```

## 🔄 **Évolutions futures possibles**

**Avec cette base, vous pourrez facilement ajouter** :
- Capteur de température DHT22 (3€)
- Module WiFi ESP01 (5€) pour monitoring distant
- Buzzer d'alarme (2€) pour notifications sonores
- Alimentation de secours sur batterie 12V
- Capteur de courant moteur pour détection de blocage avancée

---

**Cette liste vous donne tout pour réaliser un poulailler automatique professionnel et évolutif !** 🐔✨