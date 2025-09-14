# 🔧 Hardware Testing Guide - Automatic Chicken Coop

## 🎯 Testing Objectives

This test series validates each hardware component **individually** before final assembly. Each test is independent and can be performed with minimal setup.

> ⚠️ **Important**: Perform these tests in order to avoid damaging components.

## ⚡ Test 1 - Power Supply Validation

### 🎯 Objective
Verify that the power supply provides correct and stable voltages.

### 🛠️ Required Materials
- AC-DC power supply module
- Multimeter
- 470Ω resistive load (for load testing)

### 📋 Procedure

#### 1.1 No-load Test
```
1. Connect ONLY the power supply to mains
2. Measure 12V output voltage: ________V (expected: 11.5-12.5V)
3. Measure 5V output voltage: ________V (expected: 4.8-5.2V)
4. Check for ripple absence (oscilloscope if available)
```

#### 1.2 12V Load Test
```
1. Connect 470Ω resistor to 12V output
2. Measure voltage under load: ________V
3. Calculate current: I = U/R = ____/470 = ___mA
4. Verify stability for 5 minutes
```

#### 1.3 5V Load Test
```
1. Connect 470Ω resistor to 5V output
2. Measure voltage under load: ________V
3. Regulator temperature: ______°C (must stay < 60°C)
```

### ✅ Validation Criteria
- [ ] Voltages within ±5% tolerance
- [ ] No excessive heating
- [ ] 5-minute stability

---

## 🖥️ Test 2 - Arduino Nano and I2C

### 🎯 Objective
Validate the microcontroller and I2C bus before connecting modules.

### 🛠️ Required Materials
- Arduino Nano
- USB cable
- 4.7kΩ pull-up resistors (if needed)

### 📋 Procedure

#### 2.1 Basic Arduino Test
```cpp
// Test code to upload
void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Arduino OK - LED ON");
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("Arduino OK - LED OFF");
  delay(1000);
}
```

#### 2.2 I/O Pin Test
```cpp
// Test all used pins
const int pins[] = {3, 5, 6, 7, 8, 9, A0};
const int nbPins = 7;

void setup() {
  Serial.begin(9600);
  for(int i = 0; i < nbPins; i++) {
    pinMode(pins[i], OUTPUT);
  }
}

void loop() {
  for(int i = 0; i < nbPins; i++) {
    digitalWrite(pins[i], HIGH);
    Serial.print("Pin "); Serial.print(pins[i]); Serial.println(" HIGH");
    delay(200);
    digitalWrite(pins[i], LOW);
    delay(200);
  }
}
```

### ✅ Validation Criteria
- [ ] Built-in LED blinks
- [ ] Serial messages received
- [ ] All pins toggle (verify with multimeter)

---

## ⏰ Test 3 - RTC DS3231 Module

### 🎯 Objective
Verify real-time clock module functionality.

### 🛠️ Required Materials
- Arduino Nano (validated in test 2)
- RTC DS3231 module
- RTClib library installed

### 📋 Procedure

#### 3.1 Connections
```
Arduino  →  RTC DS3231
─────────────────────
5V       →  VCC
GND      →  GND
A4(SDA)  →  SDA
A5(SCL)  →  SCL
```

#### 3.2 Test Code
```cpp
#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;

void setup() {
  Serial.begin(9600);
  
  if (!rtc.begin()) {
    Serial.println("❌ RTC not found!");
    while (1);
  }
  
  Serial.println("✅ RTC detected");
  
  // Set current time (uncomment if needed)
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  if (rtc.lostPower()) {
    Serial.println("⚠️ RTC lost power - setting required");
  }
}

void loop() {
  DateTime now = rtc.now();
  
  Serial.print("Date/Time: ");
  Serial.print(now.day()); Serial.print("/");
  Serial.print(now.month()); Serial.print("/");
  Serial.print(now.year()); Serial.print(" ");
  Serial.print(now.hour()); Serial.print(":");
  Serial.print(now.minute()); Serial.print(":");
  Serial.println(now.second());
  
  Serial.print("Temperature: ");
  Serial.print(rtc.getTemperature());
  Serial.println("°C");
  
  delay(2000);
}
```

#### 3.3 Specific Tests
```
1. Verify date/time display: _______________
2. Module temperature: ______°C (about 20-25°C indoors)
3. Power loss test:
   - Disconnect Arduino for 30 seconds
   - Reconnect and verify time continues
4. Accuracy: note displayed time: ________
   Wait 1 hour and check drift: _____ seconds
```

### ✅ Validation Criteria
- [ ] I2C communication established
- [ ] Consistent date/time
- [ ] Plausible temperature
- [ ] Time kept after power loss
- [ ] Acceptable accuracy (< 1min/day)

---

## 🖥️ Test 4 - I2C LCD Display

### 🎯 Objective
Validate LCD display and determine its I2C address.

### 🛠️ Required Materials
- Arduino Nano + RTC (validated previously)
- 16x2 I2C LCD
- LiquidCrystal_I2C library

### 📋 Procedure

#### 4.1 Connections
```
Arduino  →  LCD I2C
──────────────────
5V       →  VCC
GND      →  GND
A4(SDA)  →  SDA
A5(SCL)  →  SCL
```

#### 4.2 I2C Scanner (determine address)
```cpp
#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("I2C Scanner...");
}

void loop() {
  byte error, address;
  int nDevices = 0;

  for(address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      nDevices++;
    }
  }
  
  if (nDevices == 0) {
    Serial.println("❌ No I2C devices found");
  } else {
    Serial.print("✅ ");
    Serial.print(nDevices);
    Serial.println(" device(s) found");
  }
  
  delay(5000);
}
```

#### 4.3 Display Test
```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Use address found by scanner (usually 0x27 or 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  
  // Character test
  lcd.setCursor(0, 0);
  lcd.print("Chicken Coop");
  lcd.setCursor(0, 1);
  lcd.print("LCD Test OK!");
  
  Serial.println("✅ LCD initialized");
}

void loop() {
  // Backlight blink test
  lcd.backlight();
  delay(1000);
  lcd.noBacklight();
  delay(1000);
}
```

#### 4.4 Additional Tests
```
1. I2C address detected: 0x____
2. Text display: ☐ OK ☐ Missing characters ☐ Unreadable
3. Backlight: ☐ Works ☐ Faulty
4. Contrast: ☐ Good ☐ Needs adjustment (potentiometer on back)
5. Special characters test: àéèç°
```

### ✅ Validation Criteria
- [ ] I2C address detected
- [ ] Text readable on both lines
- [ ] Backlight functional
- [ ] Correct contrast

---

## ⚙️ Test 5 - L298N Motor Driver

### 🎯 Objective
Validate motor control without connecting the final motor.

### 🛠️ Required Materials
- Arduino Nano (validated)
- L298N module
- 12V power supply
- Multimeter
- 12V test motor (or LED + resistor for simulation)

### 📋 Procedure

#### 5.1 Connections
```
Arduino  →  L298N
─────────────────
D7       →  IN1
D6       →  IN2
5V       →  +5V (logic)
GND      →  GND

Power Supply  →  L298N
─────────────────────
12V+     →  +12V
12V-     →  GND
```

#### 5.2 Test Code
```cpp
const int MOTOR_PIN1 = 7;
const int MOTOR_PIN2 = 6;

void setup() {
  Serial.begin(9600);
  pinMode(MOTOR_PIN1, OUTPUT);
  pinMode(MOTOR_PIN2, OUTPUT);
  
  // Initial stop
  digitalWrite(MOTOR_PIN1, LOW);
  digitalWrite(MOTOR_PIN2, LOW);
  
  Serial.println("L298N Motor Driver Test");
  delay(2000);
}

void loop() {
  // Direction 1 test
  Serial.println("Direction 1 - 5 seconds");
  digitalWrite(MOTOR_PIN1, HIGH);
  digitalWrite(MOTOR_PIN2, LOW);
  delay(5000);
  
  // Stop
  Serial.println("Stop - 2 seconds");
  digitalWrite(MOTOR_PIN1, LOW);
  digitalWrite(MOTOR_PIN2, LOW);
  delay(2000);
  
  // Direction 2 test
  Serial.println("Direction 2 - 5 seconds");
  digitalWrite(MOTOR_PIN1, LOW);
  digitalWrite(MOTOR_PIN2, HIGH);
  delay(5000);
  
  // Stop
  Serial.println("Stop - 2 seconds");
  digitalWrite(MOTOR_PIN1, LOW);
  digitalWrite(MOTOR_PIN2, LOW);
  delay(2000);
}
```

#### 5.3 Validation Measurements
```
1. OUT1 voltage direction 1: ______V (expected ~12V)
2. OUT2 voltage direction 1: ______V (expected ~0V)
3. OUT1 voltage direction 2: ______V (expected ~0V)
4. OUT2 voltage direction 2: ______V (expected ~12V)
5. No-load current: ______mA
6. L298N temperature after 5min: ______°C
```

#### 5.4 Real Motor Test
```cpp
// ⚠️ Caution: Low power motor only for this test
// Add to previous code in loop():

Serial.println("Real motor test - 2 seconds each direction");

// Direction 1 - short
digitalWrite(MOTOR_PIN1, HIGH);
digitalWrite(MOTOR_PIN2, LOW);
delay(2000);

// Stop
digitalWrite(MOTOR_PIN1, LOW);
digitalWrite(MOTOR_PIN2, LOW);
delay(1000);

// Direction 2 - short
digitalWrite(MOTOR_PIN1, LOW);
digitalWrite(MOTOR_PIN2, HIGH);
delay(2000);

// Final stop
digitalWrite(MOTOR_PIN1, LOW);
digitalWrite(MOTOR_PIN2, LOW);
delay(5000);
```

### ✅ Validation Criteria
- [ ] Correct output voltages
- [ ] Functional polarity inversion
- [ ] No excessive heating
- [ ] Motor rotation in both directions
- [ ] Clean motor stop

---

## 📊 Hardware Testing Summary

### 🗂️ Summary Sheet

| Test | Component | Status | Notes |
|------|-----------|--------|-------|
| 1 | Power Supply | ☐ OK ☐ FAIL | |
| 2 | Arduino Nano | ☐ OK ☐ FAIL | |
| 3 | RTC DS3231 | ☐ OK ☐ FAIL | I2C Address: 0x68 |
| 4 | LCD I2C | ☐ OK ☐ FAIL | I2C Address: 0x___ |
| 5 | L298N | ☐ OK ☐ FAIL | |

### 🔧 Corrective Actions if Failed

**Test 1 failed**: Check fuse, mains connections, 5V regulator
**Test 2 failed**: Try another Arduino, check USB cable, drivers
**Test 3 failed**: Check CR2032 battery, solder joints, RTClib library
**Test 4 failed**: Run I2C scanner, adjust contrast, check power supply
**Test 5 failed**: Check 12V supply, connections, L298N module

### ➡️ Next Steps

Once all hardware tests are validated, proceed to **software tests** for complete system integration.

---

*Hardware Testing Guide - Version 1.0 - Automatic Chicken Coop Project*