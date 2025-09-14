# 🔌 Detailed Wiring Guide - Automatic Chicken Coop

## 📋 Connection Table

| Arduino Pin | Component | Pin/Terminal | Description |
|-------------|-----------|--------------|-------------|
| **A0** | Light sensor | Signal | Analog input 0-1023 |
| **A4 (SDA)** | RTC DS3231 | SDA | I2C data bus |
| **A4 (SDA)** | LCD I2C | SDA | I2C data bus |
| **A5 (SCL)** | RTC DS3231 | SCL | I2C clock bus |
| **A5 (SCL)** | LCD I2C | SCL | I2C clock bus |
| **D3** | Red LED | Anode (+) | Power outage indicator |
| **D5** | Button | Pin 1 | Multi-function button |
| **D6** | L298N | IN2 | Motor control direction 2 |
| **D7** | L298N | IN1 | Motor control direction 1 |
| **D8** | TOP limit switch | NO (Normally Open) | Door open detection |
| **D9** | BOTTOM limit switch | NO (Normally Open) | Door closed detection |
| **5V** | All modules | VCC/+ | +5V power supply |
| **GND** | All modules | GND/- | Common ground |

## 🔧 Connection Details

### 📡 Light Sensor (LDR)
```
Arduino A0  ────┬──── LDR ──── +5V
                │
               10kΩ (Pull-down)
                │
               GND
```

### ⏰ RTC DS3231 Module
```
Arduino     RTC DS3231
──────────────────────
5V      →   VCC
GND     →   GND  
A4(SDA) →   SDA
A5(SCL) →   SCL
```

### 🖥️ 16x2 LCD I2C
```
Arduino     LCD I2C
─────────────────────
5V      →   VCC
GND     →   GND
A4(SDA) →   SDA
A5(SCL) →   SCL
```

### 🔘 Multi-function Button
```
Arduino D5  ────┬──── Button ──── GND
                │
          10kΩ (Pull-up) OR use INPUT_PULLUP
                │
               +5V
```

### 💡 Indicator LED
```
Arduino D3  ──── 220Ω Resistor ──── LED Anode(+) ──── LED Cathode(-) ──── GND
```

### 🏁 Limit Switch Sensors
```
                 TOP Limit Switch
Arduino D8  ────┬──── NO (Normally Open) ──── GND
                │
          10kΩ (Pull-up) OR use INPUT_PULLUP
                │
               +5V

                 BOTTOM Limit Switch  
Arduino D9  ────┬──── NO (Normally Open) ──── GND
                │
          10kΩ (Pull-up) OR use INPUT_PULLUP
                │
               +5V
```

### ⚡ L298N Motor Driver Module
```
Arduino     L298N       DC Motor      Power Supply
─────────────────────────────────────────────────────
D7      →   IN1
D6      →   IN2
            OUT1    →   Motor +
            OUT2    →   Motor -
5V      →   +5V (logic)
GND     →   GND
            +12V    ←                ← 12V Supply +
            GND     ←                ← 12V Supply -
```

## ⚡ Power Supply Schematic

```
12V 2A Power Supply
│
├─── L298N (+12V, motor)
│
└─── 5V Regulator (LM7805 + capacitors)
     │
     ├─── Arduino Nano (5V)
     ├─── RTC DS3231 (5V)
     ├─── LCD I2C (5V)  
     ├─── LED + resistor (5V)
     └─── Sensor pull-ups (5V)
```

## 🔍 Important Points

### ⚠️ Electrical Safety
- **2A fuse** on 12V power supply
- **Flyback diodes** on motor (protection against voltage spikes)
- **470µF capacitor** in parallel on motor power supply (filtering)

### 🛡️ Arduino Protection
- Using `INPUT_PULLUP` avoids external resistors
- Verify stable 5V power supply (multimeter)
- Avoid short circuits with multimeter in continuity mode

### 📐 I2C Addresses
- **RTC DS3231**: fixed address 0x68
- **LCD I2C**: usually 0x27 (check with I2C scanner if problem)

### 🔧 Connection Testing
```cpp
// Simple test code to verify inputs
void setup() {
  Serial.begin(9600);
  pinMode(8, INPUT_PULLUP); // Top limit switch
  pinMode(9, INPUT_PULLUP); // Bottom limit switch
  pinMode(5, INPUT_PULLUP); // Button
}

void loop() {
  Serial.print("Button: "); Serial.print(digitalRead(5));
  Serial.print(" | Top: "); Serial.print(digitalRead(8));
  Serial.print(" | Bottom: "); Serial.print(digitalRead(9));
  Serial.print(" | Light: "); Serial.println(analogRead(A0));
  delay(500);
}
```

## 📦 Enclosure Integration

### 🏠 Recommended Layout
- **Arduino + breadboard**: central mounting
- **LCD**: front face of enclosure (visible)
- **Button**: front face (accessible)
- **LED**: front face (visible)
- **Connectors**: waterproof cable glands for external cables

### 🌧️ Outdoor Protection
- **Limit switch sensors**: IP65 minimum enclosures
- **Light sensor**: sheltered from direct rain
- **Motor**: IP54 protection or waterproof housing

---

*This guide assumes a 12V installation. Adapt values according to your power supply.*