# 🛠️ Troubleshooting Guide - Automatic Chicken Coop

## 🎯 How to Use This Guide

This guide follows a **symptom → diagnosis → solution** approach to quickly resolve the most common problems. Solutions are classified by **priority** (🔴 Urgent → 🟡 Important → 🟢 Minor).

> ⚠️ **Safety**: Always disconnect power before any intervention on connections.

---

## 🚨 Critical Problems (Non-functional System)

### ❌ LCD Screen Completely Dark

**Symptoms:**
- Black screen, no backlight
- No display at startup

**Step-by-step Diagnosis:**
```
1. Check general power supply
   □ Multimeter on 5V output: _____ V (expected 4.8-5.2V)
   □ Arduino LED on: □ Yes □ No

2. Check LCD connections
   □ LCD VCC → Arduino 5V: □ OK
   □ LCD GND → Arduino GND: □ OK
   □ SDA/SCL properly connected: □ OK

3. I2C scanner test
   □ Upload I2C scanner (see software tests)
   □ Device detected at 0x27 or 0x3F: □ Yes □ No
```

**Solutions by Priority:**
- 🔴 **Power supply failure** → Replace power supply module
- 🔴 **Defective connection** → Re-solder I2C connections
- 🟡 **Incorrect I2C address** → Modify in code (line ~27)
- 🟡 **Defective LCD** → Replace display

### ❌ Arduino Not Responding (LED Off)

**Symptoms:**
- Arduino power LED off
- No serial communication
- LCD screen dark

**Diagnosis:**
```
1. Check motor power supply
   □ 12V on L298N terminals: _____ V
   □ 5V logic L298N: _____ V

2. Test control signals
   □ D7 voltage on command: _____ V (expected 5V or 0V)
   □ D6 voltage on command: _____ V
   □ Signal inversion: □ OK

3. Direct motor test
   □ Connect motor directly to 12V: □ Turns □ Blocked
```

**Solutions:**
- 🔴 **L298N failure** → Replace motor module
- 🔴 **Motor seized/blocked** → Check mechanics, lubricate
- 🔴 **12V power absent** → Check transformer, fuses
- 🟡 **Motor connections reversed** → Swap OUT1/OUT2

---

## ⚠️ Functional Problems (Partially Operational System)

### 🕐 Incorrect or Drifting Time

**Symptoms:**
- Wrong displayed time
- Shifted automatic triggers
- Red LED blinking

**Diagnosis:**
```
1. RTC battery status
   □ CR2032 battery voltage: _____ V (expected >2.5V)
   □ RTC.lostPower() returns: □ true □ false

2. Accuracy test
   □ Note RTC time: _____
   □ Wait 1h, measure drift: _____ seconds
```

**Solutions:**
- 🟡 **Low RTC battery** → Replace CR2032 battery
- 🟡 **Time never set** → Use settings mode or uncomment rtc.adjust()
- 🟢 **Normal drift** → Recalibrate periodically

### 🌙 Light-triggered Closing Not Working

**Symptoms:**
- Door stays open at night
- No delay timer start
- Sensor doesn't react to shading

**Diagnosis:**
```
1. Light sensor test
   □ Value in daylight: _____ (expected >500)
   □ Value in darkness: _____ (expected <100)
   □ Configured threshold: _____

2. Condition checks
   □ Time > 7:00 AM: □ OK
   □ Door actually open: □ OK
   □ Sensor clean and accessible: □ OK
```

**Solutions:**
- 🟡 **Poorly calibrated threshold** → Adjust via settings mode (sensor value +50)
- 🟡 **Dirty/wet sensor** → Clean with dry cloth
- 🟡 **Poorly positioned sensor** → Orient towards sky, shelter from rain
- 🟢 **Parasitic artificial lighting** → Move or mask light sources

### 🔘 Button Unresponsive or Hypersensitive

**Symptoms:**
- Presses ignored
- Multiple triggers
- Cannot enter settings mode

**Diagnosis:**
```
1. Button electrical test
   □ Button resistance at rest: _____ kΩ (expected >10k)
   □ Button resistance pressed: _____ kΩ (expected <1k)
   □ Bouncing visible in Serial: □ Yes □ No

2. Software test
   □ digitalRead(5) at rest: □ HIGH □ LOW
   □ digitalRead(5) pressed: □ HIGH □ LOW
```

**Solutions:**
- 🟡 **Faulty button** → Replace push button
- 🟡 **Excessive bouncing** → Add 100nF capacitor in parallel
- 🟡 **Missing pull-up** → Check INPUT_PULLUP in code
- 🟢 **Timing sensitivity** → Adjust debounce constants in code

### 🛑 Limit Switches Don't Stop Motor

**Symptoms:**
- Motor continues after final position
- Systematic obstacle error
- Only stops on timeout

**Diagnosis:**
```
1. Limit switch electrical test
   □ Top LS at rest: □ HIGH □ LOW
   □ Top LS activated: □ HIGH □ LOW
   □ Bottom LS at rest: □ HIGH □ LOW
   □ Bottom LS activated: □ HIGH □ LOW

2. Mechanical test
   □ Manual top LS activation: □ Audible click
   □ Manual bottom LS activation: □ Audible click
   □ Correctly adjusted position: □ OK
```

**Solutions:**
- 🟡 **Faulty limit switch** → Replace micro-switch
- 🟡 **Poor positioning** → Readjust support for precise activation
- 🟡 **Defective connection** → Check continuity, re-solder
- 🟢 **Inverted logic** → Check polarity and pull-ups

---

## 🔧 Configuration and Settings Problems

### ⚙️ Cannot Modify Parameters

**Symptoms:**
- Settings mode accessible but values don't change
- Parameters return to default values at restart
- EEPROM not saving

**Diagnosis:**
```
1. Mode navigation test
   □ Long press enters mode: □ OK
   □ Navigation between modes: □ OK
   □ Values display: □ OK

2. Save test
   □ Modify a parameter
   □ Restart Arduino
   □ Check if value preserved: □ Yes □ No
```

**Solutions:**
- 🟡 **EEPROM failure** → Very rare, test with another Arduino
- 🟡 **Code compilation** → Check that saveXXX() functions are called
- 🟢 **Timeout timing** → Increase TIMEOUT_REGLAGE if too short

### 🎛️ Inappropriate Motor Timeouts

**Symptoms:**
- Obstacle error while door moves normally
- Movement too slow or too fast
- Premature stop

**Diagnosis:**
```
1. Measure actual travel time
   □ Complete opening: _____ seconds
   □ Complete closing: _____ seconds
   □ Configured opening timeout: _____ s
   □ Configured closing timeout: _____ s

2. Analyze mechanics
   □ Movement resistance: □ Normal □ Excessive
   □ Lubrication: □ OK □ Insufficient
```

**Solutions:**
- 🟡 **Timeout too short** → Increase via settings mode (+20% margin)
- 🟡 **Seized mechanics** → Lubricate rails, check alignment
- 🟢 **Optimization** → Adjust timeouts precisely for installation

---

## 📱 Display and Interface Problems

### 📺 Distorted LCD Display or Strange Characters

**Symptoms:**
- Unrecognizable characters
- Partially displayed lines
- Erratic blinking

**Diagnosis:**
```
1. LCD power supply check
   □ Stable 5V voltage: _____ V
   □ LCD current: _____ mA (expected <50mA)

2. I2C communication test
   □ Scanner detects LCD: □ Yes at 0x____
   □ Simple code test (Hello World): □ OK □ FAIL
```

**Solutions:**
- 🟡 **I2C interference** → Shorten SDA/SCL wires, add ferrites
- 🟡 **Unstable power supply** → Add 100µF capacitor on LCD 5V
- 🟡 **Poorly adjusted contrast** → Adjust potentiometer on back of I2C module
- 🟢 **Character encoding** → Check encoding, avoid special characters

### 🔄 Slow or Lagging Interface

**Symptoms:**
- Significant delay between button press and reaction
- LCD refreshes slowly
- System seems to "lag"

**Diagnosis:**
```
1. Measure cycle time
   □ Add millis() at start/end of loop()
   □ Average cycle time: _____ ms (expected <100ms)

2. Identify bottlenecks
   □ I2C communication: _____ ms
   □ Sensor reading: _____ ms
   □ Business logic: _____ ms
```

**Solutions:**
- 🟡 **Excessive delays** → Reduce delay(50) to delay(10) if stable
- 🟡 **I2C too slow** → Check short wires, no parasitic resistances
- 🟢 **Code optimization** → Avoid heavy operations in loop()

---

## 🌦️ Environmental Problems

### ❄️ Cold Weather Malfunctions

**Symptoms:**
- LCD unreadable or dark in extreme cold
- Motor struggles to start
- Frosted light sensor

**Preventive Solutions:**
- 🟡 **Enclosure insulation** → Add thermal insulation, anti-condensation
- 🟡 **Low temperature oil** → Use special cold weather lubricant
- 🟡 **LED heating** → High power LED near sensor for defrosting
- 🟢 **Adaptive thresholds** → Adjust light thresholds by season

### 🌧️ Humidity and Infiltration Problems

**Symptoms:**
- Corrosion on connectors
- Intermittent malfunctions in humid weather
- Erratic sensors

**Solutions:**
- 🟡 **Enhanced sealing** → Silicone seals, cable glands, IP65
- 🟡 **Desiccant packets** → Silica gel in electronic enclosure
- 🟡 **Sensor protection** → Transparent covers, ventilation
- 🟢 **Preventive maintenance** → Semi-annual sealing check

---

## 🔍 Advanced Diagnostic Tools

### 📊 Complete Debug Code

```cpp
// Integrate temporarily for diagnosis
void completeDebug() {
  Serial.println("=== COMPLETE SYSTEM DEBUG ===");
  
  // Power supply status
  Serial.print("Light sensor: "); Serial.println(analogRead(CAPTEUR_LUMIERE));
  
  // Digital sensor status
  Serial.print("Button (D5): "); Serial.println(digitalRead(BOUTON_PIN));
  Serial.print("Top LS (D8): "); Serial.println(digitalRead(FIN_COURSE_HAUT));
  Serial.print("Bottom LS (D9): "); Serial.println(digitalRead(FIN_COURSE_BAS));
  
  // RTC status
  DateTime now = rtc.now();
  Serial.print("RTC time: "); 
  Serial.print(now.hour()); Serial.print(":"); Serial.println(now.minute());
  Serial.print("RTC lostPower: "); Serial.println(rtc.lostPower());
  
  // System status
  Serial.print("Door open: "); Serial.println(porteOuverte);
  Serial.print("Motor state: "); Serial.println(etatActuel);
  Serial.print("Interface mode: "); Serial.println(modeActuel);
  
  // EEPROM parameters
  Serial.print("Light threshold: "); Serial.println(seuilLumiere);
  Serial.print("Opening timeout: "); Serial.println(timeoutOuverture);
  Serial.print("Closing timeout: "); Serial.println(timeoutFermeture);
  
  Serial.println("========================");
}
```

### 🔧 Complete Reset Procedure

```cpp
// Reset EEPROM to default values
void resetEEPROM() {
  EEPROM.write(SEUIL_EEPROM_ADDR, SEUIL_DEFAULT & 0xFF);
  EEPROM.write(SEUIL_EEPROM_ADDR + 1, (SEUIL_DEFAULT >> 8) & 0xFF);
  EEPROM.write(TIMEOUT_OUVERTURE_ADDR, TIMEOUT_OUVERTURE_DEFAULT);
  EEPROM.write(TIMEOUT_FERMETURE_ADDR, TIMEOUT_FERMETURE_DEFAULT);
  Serial.println("EEPROM reset to default values");
}
```

---

## 📞 Support Escalation

### 🆘 When to Contact the Community

**Problems requiring external help:**
- Symptoms not covered by this guide
- Solutions attempted without success
- Hardware modifications considered
- Code improvements

**Information to provide:**
- Precise symptoms observed
- Tests already performed
- Complete debug code results
- Installation photos if mechanical problem

### 📚 Additional Resources

- **GitHub Issues**: [link to repo]
- **Arduino Forums**: Automation project section
- **Maker Community**: Local FabLab groups

---

*Troubleshooting Guide - Version 1.0 - Automatic Chicken Coop Project*