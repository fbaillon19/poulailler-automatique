# 🔌 Guide de câblage détaillé - Poulailler Automatique

## 📋 Table des connexions

| Arduino Pin | Composant | Broche/Pin | Description |
|-------------|-----------|------------|-------------|
| **A0** | Capteur luminosité | Signal | Entrée analogique 0-1023 |
| **A4 (SDA)** | RTC DS3231 | SDA | Bus I2C données |
| **A4 (SDA)** | LCD I2C | SDA | Bus I2C données |
| **A5 (SCL)** | RTC DS3231 | SCL | Bus I2C horloge |
| **A5 (SCL)** | LCD I2C | SCL | Bus I2C horloge |
| **D3** | LED rouge | Anode (+) | Indicateur coupure courant |
| **D5** | Bouton | Pin 1 | Bouton multifonction |
| **D6** | L298N | IN2 | Contrôle moteur sens 2 |
| **D7** | L298N | IN1 | Contrôle moteur sens 1 |
| **D8** | Fin course HAUT | NO (Normalement Ouvert) | Détection porte ouverte |
| **D9** | Fin course BAS | NO (Normalement Ouvert) | Détection porte fermée |
| **5V** | Tous modules | VCC/+ | Alimentation +5V |
| **GND** | Tous modules | GND/- | Masse commune |

## 🔧 Détail des connexions

### 📡 Capteur de luminosité (LDR)
```
Arduino A0  ────┬──── LDR ──── +5V
                │
               10kΩ (Pull-down)
                │
               GND
```

### ⏰ Module RTC DS3231
```
Arduino     RTC DS3231
──────────────────────
5V      →   VCC
GND     →   GND  
A4(SDA) →   SDA
A5(SCL) →   SCL
```

### 🖥️ LCD 16x2 I2C
```
Arduino     LCD I2C
─────────────────────
5V      →   VCC
GND     →   GND
A4(SDA) →   SDA
A5(SCL) →   SCL
```

### 🔘 Bouton multifonction
```
Arduino D5  ────┬──── Bouton ──── GND
                │
          10kΩ (Pull-up) OU utiliser INPUT_PULLUP
                │
               +5V
```

### 💡 LED indicateur
```
Arduino D3  ──── Résistance 220Ω ──── LED Anode(+) ──── LED Cathode(-) ──── GND
```

### 🏁 Capteurs de fin de course
```
                 Fin de course HAUT
Arduino D8  ────┬──── NO (Normalement Ouvert) ──── GND
                │
          10kΩ (Pull-up) OU utiliser INPUT_PULLUP
                │
               +5V

                 Fin de course BAS  
Arduino D9  ────┬──── NO (Normalement Ouvert) ──── GND
                │
          10kΩ (Pull-up) OU utiliser INPUT_PULLUP
                │
               +5V
```

### ⚡ Module contrôleur moteur L298N
```
Arduino     L298N       Moteur CC      Alimentation
─────────────────────────────────────────────────────
D7      →   IN1
D6      →   IN2
            OUT1    →   Moteur +
            OUT2    →   Moteur -
5V      →   +5V (logique)
GND     →   GND
            +12V    ←                ← Alim 12V +
            GND     ←                ← Alim 12V -
```

## ⚡ Schéma d'alimentation

```
Alimentation 12V 2A
│
├─── L298N (+12V, moteur)
│
└─── Régulateur 5V (LM7805 + condensateurs)
     │
     ├─── Arduino Nano (5V)
     ├─── RTC DS3231 (5V)
     ├─── LCD I2C (5V)  
     ├─── LED + résistance (5V)
     └─── Pull-ups capteurs (5V)
```

## 🔍 Points d'attention

### ⚠️ Sécurité électrique
- **Fusible 2A** sur l'alimentation 12V
- **Diodes de roue libre** sur le moteur (protection contre les surtensions)
- **Condensateur 470µF** en parallèle sur l'alimentation moteur (filtrage)

### 🛡️ Protection Arduino
- Utiliser `INPUT_PULLUP` évite les résistances externes
- Vérifier que l'alimentation 5V est stable (multimètre)
- Éviter les courts-circuits avec un multimètre en mode continuité

### 📐 Adresses I2C
- **RTC DS3231** : adresse fixe 0x68
- **LCD I2C** : généralement 0x27 (vérifier avec un scanner I2C si problème)

### 🔧 Test des connexions
```cpp
// Code de test simple pour vérifier les entrées
void setup() {
  Serial.begin(9600);
  pinMode(8, INPUT_PULLUP); // Fin course haut
  pinMode(9, INPUT_PULLUP); // Fin course bas
  pinMode(5, INPUT_PULLUP); // Bouton
}

void loop() {
  Serial.print("Bouton: "); Serial.print(digitalRead(5));
  Serial.print(" | Haut: "); Serial.print(digitalRead(8));
  Serial.print(" | Bas: "); Serial.print(digitalRead(9));
  Serial.print(" | Lumière: "); Serial.println(analogRead(A0));
  delay(500);
}
```

## 📦 Intégration dans boîtier

### 🏠 Disposition recommandée
- **Arduino + breadboard** : fixation centrale
- **LCD** : face avant du boîtier (visible)
- **Bouton** : face avant (accessible)
- **LED** : face avant (visible)
- **Connecteurs** : presse-étoupes étanches pour câbles extérieurs

### 🌧️ Protection extérieure
- **Capteurs fin de course** : boîtiers IP65 minimum
- **Capteur luminosité** : à l'abri de la pluie directe
- **Moteur** : protection IP54 ou carter étanche

---

*Ce guide suppose une installation 12V. Adaptez les valeurs selon votre alimentation.*
