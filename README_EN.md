# 🐔 Automatic Chicken Coop Door with Arduino

> Automated chicken coop door system based on Arduino Nano with time and light level management.

![Badge Arduino](https://img.shields.io/badge/Arduino-Nano-blue)
![Badge License](https://img.shields.io/badge/License-MIT-green)
![Badge Version](https://img.shields.io/badge/Version-1.0-orange)

[🇫🇷 Version française](README_FR.md) | [🇺🇸 English version](README_EN.md)

## 📸 Project Overview

*[Add photos of your completed project here]*

## ✨ Features

### 🕐 Automatic Management
- **Automatic opening at 7:00 AM** every morning
- **Light-triggered closing** (with 10-min delay to avoid cloud interference)
- **Forced closing at 11:00 PM** (safety feature)
- **Anti-jam system** with configurable timeout

### 🎛️ User Interface
- **16x2 LCD display**: Shows time + door status
- **Multi-function button**:
  - Short press: Manual open/close
  - Long press: Access settings menu
- **Complete configuration without computer**: Time, light threshold, timeouts

### 🔒 Safety Features
- **Top and bottom limit switches**
- **Obstacle detection** with motor timeout
- **EEPROM parameter backup**
- **Power outage alert LED**

## 🔧 Required Hardware

See detailed file: [`docs/en/components_list.md`](docs/en/components_list.md)

**Main components**:
- Arduino Nano
- DS3231 RTC Module
- 16x2 LCD I2C Display
- L298N Motor Driver
- Light sensor
- 2x Limit switches
- 12V DC Motor

## 📐 Wiring Diagram

```
Arduino Nano    →    Component
─────────────────────────────────
A0              →    Light sensor
D3              →    Power outage LED
D5              →    Multi-function button
D6              →    Motor Pin 2
D7              →    Motor Pin 1  
D8              →    TOP limit switch
D9              →    BOTTOM limit switch
SDA (A4)        →    LCD + RTC (I2C)
SCL (A5)        →    LCD + RTC (I2C)
5V              →    Module power supply
GND             →    Common ground
```

*Detailed Fritzing schematic: [`schemas/wiring_diagram.fzz`](schemas/)*

## 🚀 Installation

### 1. Arduino IDE Requirements
```bash
# Required libraries
- RTClib (Adafruit)
- LiquidCrystal_I2C
```

### 2. Download
```bash
git clone https://github.com/your-username/poulailler-automatique.git
cd poulailler-automatique
```

### 3. Configuration
1. Open `code/poulailler_automatique.ino`
2. Adjust LCD I2C address if needed (line 27)
3. Upload to Arduino Nano

### 4. First Setup
1. Set time: **Long press → Navigate with short presses → Long press next**
2. Adjust light threshold for your environment
3. Test motor timeouts according to your installation

## 📱 Usage

### Interface Navigation
| Action | Result |
|--------|--------|
| **Short press** | Open/close door manually |
| **Long press (3s)** | Enter settings mode |
| **Double-click** | Decrease value (settings mode) |

### Settings Sequence
1. **Hour setting** → Short press: +1h
2. **Minute setting** → Short press: +1min  
3. **Light threshold** → Short press: +5, Double-click: -5
4. **Opening timeout** → Short press: +1s, Double-click: -1s
5. **Closing timeout** → Short press: +1s, Double-click: -1s

## 🔍 Diagnostics

### LCD Status Messages
- `"Door open"` / `"Door closed"` : Normal state
- `"Opening..."` / `"Closing..."` : Movement in progress
- `"Closes in Xmin"` : Light delay timer active
- `"OBSTACLE ERROR"` : Obstacle detected, short press to retry

### Blinking LED
- **Blinking red LED** : Power outage detected (RTC lost time)

## 🛠️ Mechanical Assembly

### Construction Suggestions
- **Door**: Lightweight panel (PVC, aluminum, thin wood)
- **Guidance**: U-rails or angle brackets
- **Transmission**: Pulley + cable or rack + pinion
- **Safety**: Counterweight or spring for power failure

### Sensor Positioning
- **TOP limit switch**: Triggers when door fully open
- **BOTTOM limit switch**: Triggers when door closed at ground level
- **Light sensor**: Weather-protected, facing skyward

## 🤝 Contributing

Contributions are welcome! 

1. **Fork** the project
2. Create a **feature branch** (`git checkout -b feature/improvement`)
3. **Commit** your changes (`git commit -m 'Add feature'`)
4. **Push** to branch (`git push origin feature/improvement`)
5. Open a **Pull Request**

## 📝 Changelog

### Version 1.0.0
- ✅ Automatic time + light management
- ✅ Complete LCD interface
- ✅ Single-button settings
- ✅ Anti-jam system
- ✅ EEPROM backup

## 📄 License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Arduino community for libraries
- Maker forums for mechanical advice
- Beta testers for field feedback

## 📞 Support

- **GitHub Issues** : For bugs and suggestions
- **Wiki** : Detailed documentation
- **Discussions** : General questions

---

⭐ **Please star this repo if this project helped you!** ⭐

## 🔗 Related Projects

*Looking for variations or improvements? Check out these related projects:*
- Solar-powered version
- ESP32 WiFi connectivity
- Mobile app integration

## 🌍 Community

Join the discussion:
- **Reddit**: [r/arduino](https://reddit.com/r/arduino)
- **Arduino Forum**: [Project showcase](https://forum.arduino.cc/)
- **Discord**: Arduino makers community

---

*Made with ❤️ by the maker community*