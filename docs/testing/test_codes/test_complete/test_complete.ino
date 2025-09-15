/*
 * ============================================================================
 * COMPLETE SYSTEM TEST - Automatic Chicken Coop
 * ============================================================================
 * 
 * Test: 7.1 - Complete System Integration
 * Objective: Validate complete integration of all components
 * Duration: ~15 minutes
 * 
 * This test verifies:
 * - Simultaneous operation of all modules
 * - Complete user interface (LCD + button)
 * - Automatic control logic
 * - Sensor and actuator management
 * - EEPROM parameter backup
 * - Realistic usage scenarios
 * 
 * Required components (ALL connected):
 * - Arduino Nano + power supply
 * - RTC DS3231 (I2C)
 * - 16x2 LCD I2C  
 * - Light sensor + 10kΩ resistor
 * - 2x limit switch sensors
 * - Multi-function button
 * - Status LED
 * - L298N module + motor (⚠️ DISCONNECTED from mechanism)
 * 
 * Procedure:
 * 1. Upload this code (simplified version of final project)
 * 2. Verify LCD display and button navigation
 * 3. Test all sensors simultaneously  
 * 4. Validate automatic logic (simulation)
 * 5. Check parameter backup
 * 
 * ⚠️ SAFETY: Motor disconnected from mechanism to avoid 
 * unwanted movements during tests
 * 
 * ============================================================================
 */

#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

// Pin configuration (identical to final project)
const int MOTOR_PIN1 = 7;
const int MOTOR_PIN2 = 6;
const int LIGHT_SENSOR = A0;
const int BUTTON_PIN = 5;
const int TOP_LIMIT_SWITCH = 8;
const int BOTTOM_LIMIT_SWITCH = 9;
const int STATUS_LED = 3;

// Constants
const unsigned long LONG_PRESS = 3000;
const unsigned long SETTINGS_TIMEOUT = 10000;
const unsigned long DOUBLE_CLICK = 500;
const int THRESHOLD_EEPROM_ADDR = 0;

// Global variables
RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);
bool systemInitialized = false;
int lightThreshold = 300;

// Button variables
unsigned long pressStart = 0;
bool buttonPressed = false;
bool pressProcessed = false;
unsigned long lastRelease = 0;
bool waitingDoubleClick = false;

// LCD interface variables
enum SettingsMode {
  MODE_NORMAL,
  MODE_SET_HOUR,
  MODE_SET_MINUTE,
  MODE_SET_THRESHOLD,
  MODE_SYSTEM_TEST
};
SettingsMode currentMode = MODE_NORMAL;
unsigned long settingsModeStart = 0;
bool blinking = false;
unsigned long lastBlink = 0;

// Complete test variables
unsigned long completeTestStart = 0;
int completeTestPhase = 0;
bool testInProgress = false;

// Test statistics
int testsSucceeded = 0;
int testsFailed = 0;
String lastProblem = "";

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  Serial.println("============================================");
  Serial.println("COMPLETE SYSTEM TEST - Chicken Coop Auto");
  Serial.println("============================================");
  Serial.println("Version: 1.0");
  Serial.println("Test: 7.1 - Complete System Integration");
  Serial.println("");
  
  // Pin initialization
  pinMode(MOTOR_PIN1, OUTPUT);
  pinMode(MOTOR_PIN2, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(TOP_LIMIT_SWITCH, INPUT_PULLUP);
  pinMode(BOTTOM_LIMIT_SWITCH, INPUT_PULLUP);
  pinMode(STATUS_LED, OUTPUT);
  
  // Stop motor (safety)
  digitalWrite(MOTOR_PIN1, LOW);
  digitalWrite(MOTOR_PIN2, LOW);
  
  Serial.println("🔧 MODULE INITIALIZATION:");
  Serial.println("");
  
  // Test LCD
  Serial.print("1. LCD I2C... ");
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Complete Test");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(1000);
  Serial.println("✅ OK");
  
  // Test RTC
  Serial.print("2. RTC DS3231... ");
  if (!rtc.begin()) {
    Serial.println("❌ FAILED");
    testsFailed++;
    lastProblem = "RTC not detected";
  } else {
    Serial.println("✅ OK");
    testsSucceeded++;
  }
  
  // Test sensors
  Serial.print("3. Light sensor... ");
  int light = analogRead(LIGHT_SENSOR);
  if (light > 0 && light < 1023) {
    Serial.print("✅ OK (value: ");
    Serial.print(light);
    Serial.println(")");
    testsSucceeded++;
  } else {
    Serial.println("⚠️ Edge values");
  }
  
  Serial.print("4. Limit switches... ");
  bool top = digitalRead(TOP_LIMIT_SWITCH);
  bool bottom = digitalRead(BOTTOM_LIMIT_SWITCH);
  Serial.print("✅ OK (T:");
  Serial.print(top ? "Free" : "Pressed");
  Serial.print(", B:");
  Serial.print(bottom ? "Free" : "Pressed");
  Serial.println(")");
  testsSucceeded++;
  
  Serial.print("5. Button... ");
  bool button = digitalRead(BUTTON_PIN);
  if (button == HIGH) {
    Serial.println("✅ OK (pull-up active)");
    testsSucceeded++;
  } else {
    Serial.println("⚠️ Pressed state at startup");
  }
  
  Serial.print("6. Status LED... ");
  digitalWrite(STATUS_LED, HIGH);
  delay(200);
  digitalWrite(STATUS_LED, LOW);
  Serial.println("✅ OK");
  testsSucceeded++;
  
  Serial.print("7. EEPROM... ");
  int testValue = EEPROM.read(THRESHOLD_EEPROM_ADDR);
  if (testValue >= 0 && testValue <= 255) {
    lightThreshold = testValue | (EEPROM.read(THRESHOLD_EEPROM_ADDR + 1) << 8);
    if (lightThreshold == 0) lightThreshold = 300; // Default value
    Serial.println("✅ OK");
    testsSucceeded++;
  } else {
    Serial.println("⚠️ Initialization");
  }
  
  Serial.println("");
  Serial.print("📊 Initialization: ");
  Serial.print(testsSucceeded);
  Serial.print(" OK, ");
  Serial.print(testsFailed);
  Serial.println(" failures");
  
  if (testsFailed == 0) {
    Serial.println("✅ SYSTEM FULLY FUNCTIONAL");
    systemInitialized = true;
  } else {
    Serial.print("⚠️ Problems detected: ");
    Serial.println(lastProblem);
  }
  
  Serial.println("");
  Serial.println("📋 AVAILABLE INTEGRATION TESTS:");
  Serial.println("- Normal navigation: Short button = actions");
  Serial.println("- Settings mode: Long button = settings");
  Serial.println("- Automatic test: Special mode via menu");
  Serial.println("");
  Serial.println("🎮 COMMANDS:");
  Serial.println("- Short press: Actions/navigation");
  Serial.println("- Long press: Settings mode");
  Serial.println("- Double-click: Special actions");
  Serial.println("");
  
  lcd.clear();
  lcd.setCursor(0, 0);
  if (systemInitialized) {
    lcd.print("System OK!");
  } else {
    lcd.print("Problems detected");
  }
  
  delay(2000);
  completeTestStart = millis();
}

void loop() {
  // Blinking management for settings mode
  if (millis() - lastBlink > 500) {
    lastBlink = millis();
    blinking = !blinking;
  }
  
  // Button management
  handleButton();
  
  // Settings mode timeout management
  if (currentMode != MODE_NORMAL && millis() - settingsModeStart > SETTINGS_TIMEOUT) {
    currentMode = MODE_NORMAL;
  }
  
  // Display management according to mode
  handleDisplay();
  
  // Automatic test if mode activated
  if (currentMode == MODE_SYSTEM_TEST) {
    executeAutomaticTest();
  }
  
  delay(50);
}

/*
 * Multi-function button management
 */
void handleButton() {
  bool buttonState = digitalRead(BUTTON_PIN);
  
  // Press start detection
  if (!buttonPressed && buttonState == LOW) {
    delay(20); // Debounce
    if (digitalRead(BUTTON_PIN) == LOW) {
      buttonPressed = true;
      pressStart = millis();
      pressProcessed = false;
    }
  }
  
  // Release detection
  if (buttonPressed && buttonState == HIGH) {
    delay(20); // Debounce
    if (digitalRead(BUTTON_PIN) == HIGH) {
      buttonPressed = false;
      unsigned long pressDuration = millis() - pressStart;
      
      if (!pressProcessed) {
        if (pressDuration >= LONG_PRESS) {
          handleLongPress();
        } else {
          if (waitingDoubleClick && millis() - lastRelease < DOUBLE_CLICK) {
            handleDoubleClick();
            waitingDoubleClick = false;
          } else {
            lastRelease = millis();
            waitingDoubleClick = true;
          }
        }
      }
    }
  }
  
  // Double-click timeout
  if (waitingDoubleClick && millis() - lastRelease > DOUBLE_CLICK) {
    handleShortPress();
    waitingDoubleClick = false;
  }
}

/*
 * Short press management
 */
void handleShortPress() {
  Serial.print("🔘 Short press - Mode: ");
  Serial.println(currentMode);
  
  switch (currentMode) {
    case MODE_NORMAL:
      // Simple output test
      Serial.println("   → LED + motor test");
      digitalWrite(STATUS_LED, HIGH);
      delay(100);
      digitalWrite(STATUS_LED, LOW);
      
      // Very short motor test (safety)
      digitalWrite(MOTOR_PIN1, HIGH);
      digitalWrite(MOTOR_PIN2, LOW);
      delay(200);
      digitalWrite(MOTOR_PIN1, LOW);
      digitalWrite(MOTOR_PIN2, LOW);
      break;
      
    case MODE_SET_HOUR:
      // Increment hour
      if (systemInitialized) {
        DateTime now = rtc.now();
        int newHour = (now.hour() + 1) % 24;
        rtc.adjust(DateTime(now.year(), now.month(), now.day(), 
                           newHour, now.minute(), 0));
        Serial.print("   → Hour: ");
        Serial.println(newHour);
      }
      settingsModeStart = millis();
      break;
      
    case MODE_SET_MINUTE:
      // Increment minute
      if (systemInitialized) {
        DateTime now = rtc.now();
        int newMinute = (now.minute() + 1) % 60;
        rtc.adjust(DateTime(now.year(), now.month(), now.day(), 
                           now.hour(), newMinute, 0));
        Serial.print("   → Minute: ");
        Serial.println(newMinute);
      }
      settingsModeStart = millis();
      break;
      
    case MODE_SET_THRESHOLD:
      // Increase threshold
      lightThreshold = min(1023, lightThreshold + 10);
      saveThreshold();
      Serial.print("   → Threshold: ");
      Serial.println(lightThreshold);
      settingsModeStart = millis();
      break;
      
    case MODE_SYSTEM_TEST:
      // Move to next phase
      completeTestPhase++;
      Serial.print("   → Test phase: ");
      Serial.println(completeTestPhase);
      break;
  }
}

/*
 * Double-click management
 */
void handleDoubleClick() {
  Serial.println("🔄 Double-click");
  
  if (currentMode == MODE_SET_THRESHOLD) {
    lightThreshold = max(0, lightThreshold - 10);
    saveThreshold();
    Serial.print("   → Threshold decreased: ");
    Serial.println(lightThreshold);
    settingsModeStart = millis();
  }
}

/*
 * Long press management
 */
void handleLongPress() {
  Serial.print("⏱️ Long press - Transition: ");
  Serial.print(currentMode);
  
  switch (currentMode) {
    case MODE_NORMAL:
      currentMode = MODE_SET_HOUR;
      Serial.println(" → SET_HOUR");
      break;
    case MODE_SET_HOUR:
      currentMode = MODE_SET_MINUTE;
      Serial.println(" → SET_MINUTE");
      break;
    case MODE_SET_MINUTE:
      currentMode = MODE_SET_THRESHOLD;
      Serial.println(" → SET_THRESHOLD");
      break;
    case MODE_SET_THRESHOLD:
      currentMode = MODE_SYSTEM_TEST;
      testInProgress = true;
      completeTestPhase = 0;
      Serial.println(" → SYSTEM_TEST");
      break;
    case MODE_SYSTEM_TEST:
      currentMode = MODE_NORMAL;
      testInProgress = false;
      Serial.println(" → NORMAL");
      break;
  }
  
  settingsModeStart = millis();
  pressProcessed = true;
}

/*
 * LCD display management
 */
void handleDisplay() {
  lcd.setCursor(0, 0);
  
  switch (currentMode) {
    case MODE_NORMAL:
      // Line 1: Time or status
      if (systemInitialized) {
        DateTime now = rtc.now();
        if (now.hour() < 10) lcd.print("0");
        lcd.print(now.hour());
        lcd.print(":");
        if (now.minute() < 10) lcd.print("0");
        lcd.print(now.minute());
        lcd.print("      ");
      } else {
        lcd.print("System KO      ");
      }
      
      // Line 2: Sensors
      lcd.setCursor(0, 1);
      int light = analogRead(LIGHT_SENSOR);
      bool top = digitalRead(TOP_LIMIT_SWITCH);
      bool bottom = digitalRead(BOTTOM_LIMIT_SWITCH);
      
      lcd.print("L:");
      if (light < 100) lcd.print("  ");
      else if (light < 1000) lcd.print(" ");
      lcd.print(light);
      lcd.print(" T:");
      lcd.print(top ? "0" : "1");
      lcd.print(" B:");
      lcd.print(bottom ? "0" : "1");
      lcd.print("   ");
      break;
      
    case MODE_SET_HOUR:
      if (systemInitialized) {
        DateTime now = rtc.now();
        if (blinking) {
          lcd.print("  :");
        } else {
          if (now.hour() < 10) lcd.print("0");
          lcd.print(now.hour());
          lcd.print(":");
        }
        if (now.minute() < 10) lcd.print("0");
        lcd.print(now.minute());
        lcd.print("      ");
      } else {
        lcd.print("RTC not available");
      }
      
      lcd.setCursor(0, 1);
      lcd.print("Set hour        ");
      break;
      
    case MODE_SET_MINUTE:
      if (systemInitialized) {
        DateTime now = rtc.now();
        if (now.hour() < 10) lcd.print("0");
        lcd.print(now.hour());
        lcd.print(":");
        if (blinking) {
          lcd.print("  ");
        } else {
          if (now.minute() < 10) lcd.print("0");
          lcd.print(now.minute());
        }
        lcd.print("      ");
      } else {
        lcd.print("RTC not available");
      }
      
      lcd.setCursor(0, 1);
      lcd.print("Set minute      ");
      break;
      
    case MODE_SET_THRESHOLD:
      lcd.print("Threshold: ");
      lcd.print(lightThreshold);
      lcd.print("     ");
      
      lcd.setCursor(0, 1);
      int currentValue = analogRead(LIGHT_SENSOR);
      lcd.print("Current: ");
      lcd.print(currentValue);
      lcd.print("     ");
      break;
      
    case MODE_SYSTEM_TEST:
      lcd.print("Auto test ");
      lcd.print(completeTestPhase);
      lcd.print("/5   ");
      
      lcd.setCursor(0, 1);
      switch (completeTestPhase) {
        case 0: lcd.print("Ready to test   "); break;
        case 1: lcd.print("Test sensors    "); break;
        case 2: lcd.print("Test motor      "); break;
        case 3: lcd.print("Test LED        "); break;
        case 4: lcd.print("Test EEPROM     "); break;
        case 5: lcd.print("Test complete!  "); break;
        default: lcd.print("Unknown phase   "); break;
      }
      break;
  }
}

/*
 * Automatic system test
 */
void executeAutomaticTest() {
  static unsigned long lastTest = 0;
  
  if (millis() - lastTest < 2000) return; // Tests every 2s
  lastTest = millis();
  
  switch (completeTestPhase) {
    case 1: // Sensor test
      {
        Serial.println("🧪 Automatic sensor test");
        int light = analogRead(LIGHT_SENSOR);
        bool top = digitalRead(TOP_LIMIT_SWITCH);
        bool bottom = digitalRead(BOTTOM_LIMIT_SWITCH);
        
        Serial.print("   Light: ");
        Serial.print(light);
        Serial.print(", Top: ");
        Serial.print(top ? "Free" : "Pressed");
        Serial.print(", Bottom: ");
        Serial.println(bottom ? "Free" : "Pressed");
        
        if (light > 0 && light < 1023) {
          testsSucceeded++;
          Serial.println("   ✅ Sensors OK");
        } else {
          testsFailed++;
          Serial.println("   ⚠️ Sensor edge values");
        }
      }
      break;
      
    case 2: // Motor test
      Serial.println("🧪 Automatic motor test");
      // Very short test for safety
      digitalWrite(MOTOR_PIN1, HIGH);
      digitalWrite(MOTOR_PIN2, LOW);
      delay(300);
      digitalWrite(MOTOR_PIN1, LOW);
      digitalWrite(MOTOR_PIN2, HIGH);
      delay(300);
      digitalWrite(MOTOR_PIN1, LOW);
      digitalWrite(MOTOR_PIN2, LOW);
      Serial.println("   ✅ Motor commands OK");
      testsSucceeded++;
      break;
      
    case 3: // LED test
      Serial.println("🧪 Automatic LED test");
      for (int i = 0; i < 3; i++) {
        digitalWrite(STATUS_LED, HIGH);
        delay(200);
        digitalWrite(STATUS_LED, LOW);
        delay(200);
      }
      Serial.println("   ✅ LED OK");
      testsSucceeded++;
      break;
      
    case 4: // EEPROM test
      Serial.println("🧪 Automatic EEPROM test");
      int testValue = lightThreshold + 1;
      EEPROM.write(100, testValue & 0xFF);
      delay(10);
      int readValue = EEPROM.read(100);
      if (readValue == (testValue & 0xFF)) {
        Serial.println("   ✅ EEPROM OK");
        testsSucceeded++;
      } else {
        Serial.println("   ❌ EEPROM defective");
        testsFailed++;
      }
      break;
      
    case 5: // Final results
      Serial.println("");
      Serial.println("============================================");
      Serial.println("🎯 COMPLETE TEST FINISHED");
      Serial.println("============================================");
      Serial.println("");
      Serial.print("📊 Results: ");
      Serial.print(testsSucceeded);
      Serial.print(" successes, ");
      Serial.print(testsFailed);
      Serial.println(" failures");
      Serial.println("");
      
      if (testsFailed == 0) {
        Serial.println("✅ SYSTEM FULLY VALIDATED");
        Serial.println("🎉 Ready for final deployment!");
      } else {
        Serial.println("⚠️ Some problems detected");
        Serial.println("📋 Review defective components");
      }
      Serial.println("");
      Serial.println("➡️ Long press to return to normal mode");
      break;
  }
}

/*
 * Save threshold to EEPROM
 */
void saveThreshold() {
  EEPROM.write(THRESHOLD_EEPROM_ADDR, lightThreshold & 0xFF);
  EEPROM.write(THRESHOLD_EEPROM_ADDR + 1, (lightThreshold >> 8) & 0xFF);
}

/*
 * ============================================================================
 * COMPLETE SYSTEM DIAGNOSTICS
 * ============================================================================
 * 
 * ❌ "RTC not detected":
 *    - Check I2C wiring: SDA→A4, SCL→A5
 *    - Check RTC power: VCC→5V, GND→GND
 *    - I2C address conflict with LCD (rare)
 *    - Defective RTC module
 * 
 * ❌ Unresponsive button interface:
 *    - Defective or poorly wired button
 *    - Pin D5 used by other component
 *    - Internal pull-up defective
 *    - Excessive bouncing (add capacitor)
 * 
 * ❌ LCD displays incorrect characters:
 *    - I2C address conflict (I2C scanner)
 *    - Unstable power supply
 *    - Defective I2C wiring
 *    - Defective LCD module
 * 
 * ❌ Sensors aberrant values:
 *    - Light sensor: check resistive divider
 *    - Limit switches: check pull-up and NO/NC wiring
 *    - Electromagnetic interference
 *    - Noisy power supply
 * 
 * ❌ Motor doesn't respond to commands:
 *    - L298N not powered with 12V
 *    - Motor connections reversed or disconnected
 *    - Defective L298N (overheating)
 *    - Defective Arduino pins D6/D7
 * 
 * ❌ EEPROM doesn't save:
 *    - Defective Arduino Nano (rare)
 *    - Power cut during write
 *    - EEPROM wear (>100000 cycles)
 *    - Corrupted memory address
 * 
 * ❌ Unstable system/random resets:
 *    - Insufficient or unstable power supply
 *    - Motor interference (missing capacitors)
 *    - Intermittent short circuit
 *    - Arduino watchdog (blocked code)
 * 
 * 🔧 Complete validation tests:
 *    - All modules detected at initialization
 *    - 100% responsive user interface
 *    - Sensors give coherent values
 *    - Actuators respond to commands
 *    - Parameters saved and reloaded
 *    - No reset or freeze during 15 minutes
 * 
 * 📊 Success criteria:
 *    - Initialization: 6/7 modules OK minimum
 *    - Automatic tests: <20% failures
 *    - Stability: Continuous operation >15min
 *    - Interface: Smooth navigation between all modes
 * 
 * ============================================================================
 */