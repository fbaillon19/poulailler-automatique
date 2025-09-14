# 🎛️ Fritzing Schematic - Textual Description

*While waiting for the actual Fritzing schematic, here's a detailed description to reproduce the assembly*

## 📋 Components to Place on Breadboard

```
ARDUINO NANO (placed in center of breadboard)
├── Pin A0  → LDR → 10kΩ Resistor → GND
├── Pin A4  → SDA (RTC + LCD)  
├── Pin A5  → SCL (RTC + LCD)
├── Pin D3  → 220Ω Resistor → Red LED → GND
├── Pin D5  → Button → GND (with internal pull-up)
├── Pin D6  → L298N IN2
├── Pin D7  → L298N IN1
├── Pin D8  → TOP Limit Switch → GND
├── Pin D9  → BOTTOM Limit Switch → GND
├── Pin 5V  → +5V Power Rail
└── Pin GND → GND Power Rail
```

## 🔌 Breadboard Connections

### Power Rails
```
+ Rail (red)  ← Arduino 5V
- Rail (blue) ← Arduino GND
```

### Zone 1: Light Sensor
```
Columns 10-15:
- LDR between rows a-c (columns 10-12)
- 10kΩ resistor between rows c-e (columns 12-14)  
- Arduino A0 connection → row b column 10
- +5V connection → row a column 12
- GND connection → row e column 14
```

### Zone 2: I2C Modules  
```
RTC DS3231:
- VCC → +5V Rail
- GND → GND Rail  
- SDA → Arduino A4
- SCL → Arduino A5

LCD I2C (same bus):
- VCC → +5V Rail
- GND → GND Rail
- SDA → Arduino A4  
- SCL → Arduino A5
```

### Zone 3: Button and LED
```
Button:
- Pin 1 → Arduino D5
- Pin 2 → GND Rail

LED + Resistor:
- Arduino D3 → 220Ω Resistor → LED Anode
- LED Cathode → GND Rail
```

### Zone 4: Limit Switches
```
TOP Limit Switch:
- NO (Normally Open) → Arduino D8
- Common → GND Rail

BOTTOM Limit Switch:
- NO (Normally Open) → Arduino D9  
- Common → GND Rail
```

## ⚡ L298N Module (separate from breadboard)

```
Arduino → L298N Connections:
- Arduino D7 → L298N IN1
- Arduino D6 → L298N IN2  
- Arduino 5V → L298N +5V (logic)
- Arduino GND → L298N GND

Motor Power Supply:
- 12V Supply + → L298N +12V
- 12V Supply - → L298N GND

Motor:  
- L298N OUT1 → Motor +
- L298N OUT2 → Motor -
```

## 🎨 Recommended Wire Colors

```
POWER SUPPLY:
- Red: +5V, +12V
- Black: GND, grounds

DIGITAL SIGNALS:
- Yellow: Arduino D3 (LED)
- Green: Arduino D5 (Button)
- Blue: Arduino D6, D7 (Motor)
- Orange: Arduino D8, D9 (Limit switches)

ANALOG SIGNALS:
- Purple: Arduino A0 (Light sensor)

I2C BUS:
- White: SDA (A4)
- Gray: SCL (A5)
```

## 📐 Suggested Physical Layout

```
Top view of breadboard:

     1  5  10  15  20  25  30
   ┌─────────────────────────────┐ a
 + │  LDR + Resistor             │ b  
 - │                             │ c
   │         ARDUINO             │ d
   │         NANO                │ e  
   │                             │ f
   │    Button  LED+R            │ g
   └─────────────────────────────┘ j

External modules:
- RTC and LCD: beside the breadboard
- L298N: separate enclosure  
- Limit switches: mounted on mechanics
```

## 🔧 Assembly Steps

1. **Place Arduino Nano** in center of breadboard
2. **Connect power rails** 5V/GND
3. **Mount light sensor** with its pull-down resistor
4. **Add button and LED** with their resistors
5. **Connect I2C modules** (RTC + LCD)
6. **Wire limit switches** 
7. **Connect L298N** (last step)
8. **Progressive testing** after each step

## ⚠️ Points of Caution

- **Check polarities** of power supply before connection
- **Avoid short circuits** between + and - rails
- **Motor cables**: use thicker wire (1.5mm²)
- **Cable lengths**: plan enough for movements
- **Insulation**: sheath all outdoor cables

## 📱 Simulation Tools

**Before real assembly**, you can simulate with:
- **TinkerCAD** (Autodesk) - Free, online
- **Fritzing** - Dedicated electronics assembly application
- **SimulIDE** - Complete Arduino simulator

---

*This textual schematic can be reproduced in Fritzing to generate breadboard, schematic and PCB views.*