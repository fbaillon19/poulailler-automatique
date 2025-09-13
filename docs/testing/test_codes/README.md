# 🧪 Test Codes - Chicken Coop Door

> Arduino test sketches for systematic component validation

## 📋 Overview

This directory contains individual test codes for each component of the automatic chicken coop door system. Each test is designed to validate one specific functionality in isolation.

## 📁 Test Structure

**Each test code is in its own folder** (Arduino IDE requirement):

| Test | File Location | Duration | Purpose |
|------|---------------|----------|---------|
| **Arduino Basic** | `test_arduino/test_arduino.ino` | 5 min | Programming & basic functionality |
| **RTC Module** | `test_rtc/test_rtc.ino` | 10 min | Time keeping & I2C communication |
| **LCD Display** | `test_lcd/test_lcd.ino` | 5 min | I2C display functionality |
| **Sensors** | `test_sensors/test_sensors.ino` | 10 min | Light sensor + limit switches |
| **Button** | `test_button/test_button.ino` | 5 min | Short/long press detection |
| **Status LED** | `test_led/test_led.ino` | 3 min | LED control & blinking |
| **Motor Control** | `test_motor/test_motor.ino` | 10 min | L298N motor driver |
| **Complete System** | `test_complete/test_complete.ino` | 15 min | Full integration test |

## 🚀 How to Use

### **Step 1: Open Test in Arduino IDE**
```
File → Open → Browse to:
docs/testing/test_codes/test_xxx/test_xxx.ino
```

### **Step 2: Upload to Arduino**
- Select **Arduino Nano** as board
- Select correct **COM port**
- Click **Upload** (→) button

### **Step 3: Monitor Results**
- Open **Serial Monitor** (`Ctrl+Shift+M`)
- Set baud rate to **9600**
- Follow instructions displayed in monitor

### **Step 4: Interpret Results**
- ✅ **PASS**: Test completed successfully
- ❌ **FAIL**: Check troubleshooting section
- ⚠️ **WARNING**: Review connections and try again

## 📊 Testing Sequence

### **Recommended Order:**
```
1. test_arduino    → Verify basic Arduino functionality
2. test_lcd        → Check I2C communication  
3. test_rtc        → Validate time keeping
4. test_sensors    → Test all input sensors
5. test_button     → Verify user interface
6. test_led        → Check status indicators
7. test_motor      → Validate motor control (careful!)
8. test_complete   → Full system integration
```

## ⚠️ Safety Notes

### **Before Running Motor Tests:**
- **Disconnect motor** from mechanical system
- **Limit test duration** (avoid overheating)
- **Verify motor ratings** (12V, current draw)
- **Have emergency stop** ready (power off)

### **Power Supply Safety:**
- **220V mains power** - ensure enclosure is closed
- **Verify voltages** before connecting modules
- **Use multimeter** to check 5V and 12V rails

## 🔧 Troubleshooting

### **Common Issues:**

**❌ Upload Failed**
```
Solutions:
- Check USB cable connection
- Verify correct COM port selected
- Try different USB port
- Reset Arduino before upload
```

**❌ No Serial Output**
```
Solutions:
- Check Serial Monitor baud rate (9600)
- Verify COM port in Serial Monitor
- Press Arduino reset button
- Check USB cable integrity
```

**❌ Test Results Inconsistent**
```
Solutions:
- Check power supply stability
- Verify all connections
- Look for loose wires
- Check component orientations
```

**❌ Component Not Responding**
```
Solutions:
- Verify wiring against schematic
- Check component power supply
- Test component individually
- Replace suspect component
```

## 📈 Recording Test Results

### **Test Log Template:**
```
Date: ___________
Tester: ___________

[ ] test_arduino     - Result: _______ Notes: _______
[ ] test_lcd        - Result: _______ Notes: _______  
[ ] test_rtc        - Result: _______ Notes: _______
[ ] test_sensors    - Result: _______ Notes: _______
[ ] test_button     - Result: _______ Notes: _______
[ ] test_led        - Result: _______ Notes: _______
[ ] test_motor      - Result: _______ Notes: _______
[ ] test_complete   - Result: _______ Notes: _______

Overall System Status: _______
Ready for Installation: Yes / No
```

## 📚 Reference Documentation

- **[Hardware Tests Guide](../hardware_tests.md)** - Detailed test procedures
- **[Software Tests Guide](../software_tests.md)** - Integration testing
- **[Troubleshooting Guide](../troubleshooting.md)** - Problem solutions
- **[Main Project Documentation](../../README.md)** - Complete project info

## 🆘 Getting Help

If tests fail or you need assistance:

1. **Check troubleshooting guide** first
2. **Review wiring diagrams** in main documentation  
3. **Open an issue** with test failure template
4. **Include test results** and error messages

---

**Happy Testing! 🐔**

*Systematic testing ensures a reliable automatic chicken coop door.*