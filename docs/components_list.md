# 📋 Final Components List - Automatic Chicken Coop

## 🖥️ **Microcontroller and Modules**

| Component | Specifications | Quantity | Estimated Price | Links/Search |
|-----------|----------------|----------|-----------------|--------------|
| **Arduino Nano** | ATmega328P, USB Mini-B | 1 | $8-12 | "Arduino Nano v3.0" |
| **RTC DS3231 Module** | I2C, CR2032 battery included | 1 | $3-5 | "DS3231 RTC module" |
| **16x2 LCD I2C Display** | HD44780 + PCF8574, backlight | 1 | $4-6 | "LCD 1602 I2C module" |
| **L298N Module** | Dual H-Bridge, 2A per channel | 1 | $4-6 | "L298N motor driver" |

## ⚡ **Power Supply**

| Component | Specifications | Quantity | Price | Provided Link |
|-----------|----------------|----------|-------|---------------|
| **AC-DC Jolooyo Module** | 220V → 12V/1A + 5V/1A isolated | 1 | ~$15 | [Amazon Jolooyo](https://www.amazon.fr/gp/product/B07Z8M3SQJ/) |
| **Fuse** | 1A, fast type | 2 | $1 | Electronics store |
| **Fuse Holder** | Chassis mount, secure | 1 | $2 | With fuses |

## 📡 **Sensors**

| Component | Specifications | Quantity | Price | Notes |
|-----------|----------------|----------|-------|-------|
| **LDR Photoresistor** | GL5528 or similar | 1 | $1 | "LDR photoresistor GL5528" |
| **Limit Switch Sensors** | Micro-switch with lever | 2 | $3 | "Limit switch microswitch" |
| **Push Button** | NO, 6x6mm or larger | 1 | $1 | "Push button switch" |

## 🔧 **Passive Components**

| Component | Value | Quantity | Price | Notes |
|-----------|-------|----------|-------|-------|
| **Resistors** | 220Ω (LED) | 1 | - | 100 pcs kit ~$3 |
| | 10kΩ (LDR divider) | 1 | - | |
| | 10kΩ (button pull-up)* | 1 | - | *Optional if INPUT_PULLUP |
| **Capacitors** | 100nF ceramic | 3 | - | Assortment kit ~$5 |
| | 10µF electrolytic | 1 | - | |
| | 470µF electrolytic | 1 | - | |
| **LED** | Red 5mm | 1 | $0.5 | Power outage indicator |

## 🔌 **Connectors and Wiring**

| Component | Specifications | Quantity | Price | Usage |
|-----------|----------------|----------|-------|--------|
| **Screw Terminals** | 2 pins, 5.08mm pitch | 4 | $6 | Motor, 12V supply, sensors |
| **Female Connectors** | 15 pins (Arduino Nano) | 2 | $2 | Removable Arduino socket |
| **Male Connector** | 4 pins (LCD I2C) | 1 | $1 | LCD connection |
| **Solid Wire** | 0.6mm, multiple colors | 5m | $3 | Perfboard wiring |
| **Stranded Wire** | 1mm², red/black | 2m | $2 | Power supply |

## 🏠 **Support and Enclosure**

| Component | Specifications | Quantity | Price | Notes |
|-----------|----------------|----------|-------|-------|
| **Perfboard** | 80x60mm, pads + traces | 1 | $3-5 | "Stripboard 80x60" |
| **Waterproof Enclosure** | IP54 min, ~120x80x60mm | 1 | $8-15 | "Waterproof enclosure" |
| **Cable Gland** | PG7 or M12, waterproof | 4-6 | $8 | Cable passages |
| **M3 Standoffs** | 10mm + screws | 4 | $2 | Perfboard mounting |

## ⚙️ **Motor Mechanics**

| Component | Specifications | Quantity | Price | Notes |
|-----------|----------------|----------|-------|-------|
| **12V DC Motor** | Sufficient torque for your door | 1 | $15-25 | According to required power |
| **Gearbox** | Ratio according to desired speed | 1 | $10-20 | Optional if motor is slow |
| **Transmission System** | Pulley+cable OR rack & pinion | 1 | $10-30 | According to your design |

## 🧰 **Consumables and Tools**

| Item | Specifications | Price | Notes |
|------|----------------|-------|-------|
| **Solder** | 0.6-0.8mm, 60/40 or lead-free | $3 | If not already in stock |
| **Flux** | Improves solder quality | $2 | Highly recommended |
| **Heat Shrink Tubing** | Assorted colors | $3 | Connection protection |
| **Cable Ties** | Various diameters | $2 | Cable management |

## 💰 **Cost Summary**

### **Electronics (mandatory)**
```
Microcontroller + modules : $20-30
Jolooyo Power Supply      : $15
Sensors                   : $5
Passive components        : $8
Connectors + wiring       : $15
Support + enclosure       : $20
TOTAL ELECTRONICS         : $85-95
```

### **Mechanics (variable according to design)**
```
Motor + gearbox           : $25-45
Transmission system       : $10-30
Mechanical fixings        : $10-20
TOTAL MECHANICS          : $45-95
```

### **Consumables**
```
Solder + flux + misc      : $10
```

## 🎯 **TOTAL PROJECT COST: $140-200**

*Variation according to motor choice and mechanical system*

## 🛒 **Where to Buy**

### **Electronics**:
- **Amazon**: Arduino modules, Jolooyo power supply
- **AliExpress**: Components in bulk (more economical, longer delivery)
- **Digi-Key/Mouser**: Quality distributors (fast, technical support)

### **Mechanics**:
- **Home Depot/Lowes**: Profiles, hardware, basic motor
- **McMaster-Carr**: Quality motors and gearboxes
- **eBay**: Used 12V motors (economical)

### **Enclosure and Electrical**:
- **Electrical supply stores**: Professional electrical material
- **Amazon**: Waterproof enclosures and cable glands

## ✅ **Checklist Before Ordering**

```
☐ Verify enclosure dimensions vs components
☐ Confirm required motor power
☐ Check tool inventory (soldering iron, multimeter)
☐ Plan 10% margin on quantities (spare parts)
☐ Validate connector compatibility (screw pitch, diameters)
☐ Verify mains voltage (110V/220V according to country)
```

## 🔄 **Possible Future Upgrades**

**With this base, you can easily add**:
- DHT22 temperature sensor ($3)
- ESP01 WiFi module ($5) for remote monitoring
- Alarm buzzer ($2) for sound notifications
- 12V battery backup power
- Motor current sensor for advanced jam detection

---

**This list gives you everything to build a professional and expandable automatic chicken coop!** 🐔✨