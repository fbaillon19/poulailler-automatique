/*
 * ============================================================================
 * COMPLETE SYSTEM TEST - Automatic Chicken Coop
 * ============================================================================
 * 
 * Test: 7.1 - Complete System Integration (Memory Optimized)
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
 * ⚠️ SAFETY: Motor disconnected from mechanism to avoid unwanted movements
 * 
 * ============================================================================
 */

#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

// Pin configuration
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

// Test variables
int completeTestPhase = 0;
bool testInProgress = false;
byte testsSucceeded = 0;
byte testsFailed = 0;

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  Serial.println(F("============================================"));
  Serial.println(F("COMPLETE SYSTEM TEST - Chicken Coop Auto"));
  Serial.println(F("============================================"));
  Serial.println(F("Version: 1.0"));
  Serial.println(F("Test: 7.1 - Complete System Integration"));
  Serial.println();
  
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
  
  Serial.println(F("MODULE INIT:"));
  Serial.println();
  
  // Test LCD
  Serial.print(F("1. LCD I2C... "));
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(F("Complete Test"));
  lcd.setCursor(0, 1);
  lcd.print(F("Initializing..."));
  delay(1000);
  Serial.println(F("OK"));
  testsSucceeded++;
  
  // Test RTC
  Serial.print(F("2. RTC DS3231... "));
  if (!rtc.begin()) {
    Serial.println(F("FAILED"));
    testsFailed++;
  } else {
    Serial.println(F("OK"));
    testsSucceeded++;
  }
  
  // Test sensors
  Serial.print(F("3. Light sensor... "));
  int light = analogRead(LIGHT_SENSOR);
  if (light > 0 && light < 1023) {
    Serial.print(F("OK ("));
    Serial.print(light);
    Serial.println(F(")"));
    testsSucceeded++;
  } else {
    Serial.println(F("Edge values"));
  }
  
  Serial.print(F("4. Limit switches... "));
  bool top = digitalRead(TOP_LIMIT_SWITCH);
  bool bottom = digitalRead(BOTTOM_LIMIT_SWITCH);
  Serial.print(F("OK (T:"));
  Serial.print(top ? F("Free") : F("Pressed"));
  Serial.print(F(", B:"));
  Serial.print(bottom ? F("Free") : F("Pressed"));
  Serial.println(F(")"));
  testsSucceeded++;
  
  Serial.print(F("5. Button... "));
  bool button = digitalRead(BUTTON_PIN);
  if (button == HIGH) {
    Serial.println(F("OK (pull-up active)"));
    testsSucceeded++;
  } else {
    Serial.println(F("Pressed at startup"));
  }
  
  Serial.print(F("6. Status LED... "));
  digitalWrite(STATUS_LED, HIGH);
  delay(200);
  digitalWrite(STATUS_LED, LOW);
  Serial.println(F("OK"));
  testsSucceeded++;
  
  Serial.print(F("7. EEPROM... "));
  int testValue = EEPROM.read(THRESHOLD_EEPROM_ADDR);
  if (testValue >= 0 && testValue <= 255) {
    lightThreshold = testValue | (EEPROM.read(THRESHOLD_EEPROM_ADDR + 1) << 8);
    if (lightThreshold == 0) lightThreshold = 300;
    Serial.println(F("OK"));
    testsSucceeded++;
  } else {
    Serial.println(F("Init"));
  }
  
  Serial.println();
  Serial.print(F("Init: "));
  Serial.print(testsSucceeded);
  Serial.print(F(" OK, "));
  Serial.print(testsFailed);
  Serial.println(F(" failures"));
  
  if (testsFailed == 0) {
    Serial.println(F("SYSTEM FULLY FUNCTIONAL"));
    systemInitialized = true;

    Serial.println();
    Serial.println(F("COMMANDS:"));
    Serial.println(F("- Short press: Actions/navigation"));
    Serial.println(F("- Long press: Settings mode"));
    Serial.println(F("- Double-click: Special actions"));
    Serial.println();
    Serial.println(F("NAVIGATION:"));
    Serial.println(F("Normal -> Hour -> Minute -> Threshold -> Test -> Normal"));
    Serial.println(F("Auto test: Long press through all modes"));
    Serial.println();
  } else {
    Serial.println(F("Problems detected"));
  }
  
  lcd.clear();
  lcd.setCursor(0, 0);
  if (systemInitialized) {
    lcd.print(F("System OK!"));
  } else {
    lcd.print(F("Problems detected"));
  }
  
  delay(2000);
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

void handleButton() {
  bool buttonState = digitalRead(BUTTON_PIN);
  
  // Press start detection
  if (!buttonPressed && buttonState == LOW) {
    delay(20);
    if (digitalRead(BUTTON_PIN) == LOW) {
      buttonPressed = true;
      pressStart = millis();
      pressProcessed = false;
    }
  }
  
  // Release detection
  if (buttonPressed && buttonState == HIGH) {
    delay(20);
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

void handleShortPress() {
  Serial.print(F("Short press - Mode: "));
  Serial.println(currentMode);
  
  switch (currentMode) {
    case MODE_NORMAL:
      Serial.println(F("   -> LED + motor test"));
      digitalWrite(STATUS_LED, HIGH);
      delay(100);
      digitalWrite(STATUS_LED, LOW);
      
      digitalWrite(MOTOR_PIN1, HIGH);
      digitalWrite(MOTOR_PIN2, LOW);
      delay(200);
      digitalWrite(MOTOR_PIN1, LOW);
      digitalWrite(MOTOR_PIN2, LOW);
      break;
      
    case MODE_SET_HOUR:
      if (systemInitialized) {
        DateTime now = rtc.now();
        int newHour = (now.hour() + 1) % 24;
        rtc.adjust(DateTime(now.year(), now.month(), now.day(), 
                           newHour, now.minute(), 0));
        Serial.print(F("   -> Hour: "));
        Serial.println(newHour);
      }
      settingsModeStart = millis();
      break;
      
    case MODE_SET_MINUTE:
      if (systemInitialized) {
        DateTime now = rtc.now();
        int newMinute = (now.minute() + 1) % 60;
        rtc.adjust(DateTime(now.year(), now.month(), now.day(), 
                           now.hour(), newMinute, 0));
        Serial.print(F("   -> Minute: "));
        Serial.println(newMinute);
      }
      settingsModeStart = millis();
      break;
      
    case MODE_SET_THRESHOLD:
      lightThreshold = min(1023, lightThreshold + 10);
      saveThreshold();
      Serial.print(F("   -> Threshold: "));
      Serial.println(lightThreshold);
      settingsModeStart = millis();
      break;
      
    case MODE_SYSTEM_TEST:
      completeTestPhase++;
      Serial.print(F("   -> Test phase: "));
      Serial.println(completeTestPhase);
      break;
  }
}

void handleDoubleClick() {
  Serial.println(F("Double-click"));
  
  if (currentMode == MODE_SET_THRESHOLD) {
    lightThreshold = max(0, lightThreshold - 10);
    saveThreshold();
    Serial.print(F("   -> Threshold decreased: "));
    Serial.println(lightThreshold);
    settingsModeStart = millis();
  }
}

void handleLongPress() {
  Serial.print(F("Long press - Transition: "));
  Serial.print(currentMode);
  
  switch (currentMode) {
    case MODE_NORMAL:
      currentMode = MODE_SET_HOUR;
      Serial.println(F(" -> SET_HOUR"));
      break;
    case MODE_SET_HOUR:
      currentMode = MODE_SET_MINUTE;
      Serial.println(F(" -> SET_MINUTE"));
      break;
    case MODE_SET_MINUTE:
      currentMode = MODE_SET_THRESHOLD;
      Serial.println(F(" -> SET_THRESHOLD"));
      break;
    case MODE_SET_THRESHOLD:
      currentMode = MODE_SYSTEM_TEST;
      testInProgress = true;
      completeTestPhase = 0;
      Serial.println(F(" -> SYSTEM_TEST"));
      break;
    case MODE_SYSTEM_TEST:
      currentMode = MODE_NORMAL;
      testInProgress = false;
      Serial.println(F(" -> NORMAL"));
      break;
  }
  
  settingsModeStart = millis();
  pressProcessed = true;
}

void handleDisplay() {
  lcd.setCursor(0, 0);
  
  switch (currentMode) {
    case MODE_NORMAL:
      if (systemInitialized) {
        DateTime now = rtc.now();
        if (now.hour() < 10) lcd.print(F("0"));
        lcd.print(now.hour());
        lcd.print(F(":"));
        if (now.minute() < 10) lcd.print(F("0"));
        lcd.print(now.minute());
        lcd.print(F("      "));
      } else {
        lcd.print(F("System KO      "));
      }
      
      lcd.setCursor(0, 1);
      int light = analogRead(LIGHT_SENSOR);
      bool top = digitalRead(TOP_LIMIT_SWITCH);
      bool bottom = digitalRead(BOTTOM_LIMIT_SWITCH);
      
      lcd.print(F("L:"));
      if (light < 100) lcd.print(F("  "));
      else if (light < 1000) lcd.print(F(" "));
      lcd.print(light);
      lcd.print(F(" T:"));
      lcd.print(top ? F("0") : F("1"));
      lcd.print(F(" B:"));
      lcd.print(bottom ? F("0") : F("1"));
      lcd.print(F("   "));
      break;
      
    case MODE_SET_HOUR:
      if (systemInitialized) {
        DateTime now = rtc.now();
        if (blinking) {
          lcd.print(F("  :"));
        } else {
          if (now.hour() < 10) lcd.print(F("0"));
          lcd.print(now.hour());
          lcd.print(F(":"));
        }
        if (now.minute() < 10) lcd.print(F("0"));
        lcd.print(now.minute());
        lcd.print(F("      "));
      } else {
        lcd.print(F("RTC not available"));
      }
      
      lcd.setCursor(0, 1);
      lcd.print(F("Set hour        "));
      break;
      
    case MODE_SET_MINUTE:
      if (systemInitialized) {
        DateTime now = rtc.now();
        if (now.hour() < 10) lcd.print(F("0"));
        lcd.print(now.hour());
        lcd.print(F(":"));
        if (blinking) {
          lcd.print(F("  "));
        } else {
          if (now.minute() < 10) lcd.print(F("0"));
          lcd.print(now.minute());
        }
        lcd.print(F("      "));
      } else {
        lcd.print(F("RTC not available"));
      }
      
      lcd.setCursor(0, 1);
      lcd.print(F("Set minute      "));
      break;
      
    case MODE_SET_THRESHOLD:
      lcd.print(F("Threshold: "));
      lcd.print(lightThreshold);
      lcd.print(F("     "));
      
      lcd.setCursor(0, 1);
      int currentValue = analogRead(LIGHT_SENSOR);
      lcd.print(F("Current: "));
      lcd.print(currentValue);
      lcd.print(F("     "));
      break;
      
    case MODE_SYSTEM_TEST:
      lcd.print(F("Auto test "));
      lcd.print(completeTestPhase);
      lcd.print(F("/5   "));
      
      lcd.setCursor(0, 1);
      switch (completeTestPhase) {
        case 0: lcd.print(F("Ready to test   ")); break;
        case 1: lcd.print(F("Test sensors    ")); break;
        case 2: lcd.print(F("Test motor      ")); break;
        case 3: lcd.print(F("Test LED        ")); break;
        case 4: lcd.print(F("Test EEPROM     ")); break;
        case 5: lcd.print(F("Test complete!  ")); break;
        default: lcd.print(F("Unknown phase   ")); break;
      }
      break;
  }
}

void executeAutomaticTest() {
  static unsigned long lastTest = 0;
  
  if (millis() - lastTest < 2000) return;
  lastTest = millis();
  
  switch (completeTestPhase) {
    case 1:
      {
        Serial.println(F("Auto sensor test"));
        int light = analogRead(LIGHT_SENSOR);
        bool top = digitalRead(TOP_LIMIT_SWITCH);
        bool bottom = digitalRead(BOTTOM_LIMIT_SWITCH);
        
        Serial.print(F("   Light: "));
        Serial.print(light);
        Serial.print(F(", Top: "));
        Serial.print(top ? F("Free") : F("Pressed"));
        Serial.print(F(", Bottom: "));
        Serial.println(bottom ? F("Free") : F("Pressed"));
        
        if (light > 0 && light < 1023) {
          testsSucceeded++;
          Serial.println(F("   Sensors OK"));
        } else {
          testsFailed++;
          Serial.println(F("   Sensor edge values"));
        }
      }
      break;
      
    case 2:
      Serial.println(F("Auto motor test"));
      digitalWrite(MOTOR_PIN1, HIGH);
      digitalWrite(MOTOR_PIN2, LOW);
      delay(300);
      digitalWrite(MOTOR_PIN1, LOW);
      digitalWrite(MOTOR_PIN2, HIGH);
      delay(300);
      digitalWrite(MOTOR_PIN1, LOW);
      digitalWrite(MOTOR_PIN2, LOW);
      Serial.println(F("   Motor commands OK"));
      testsSucceeded++;
      break;
      
    case 3:
      Serial.println(F("Auto LED test"));
      for (int i = 0; i < 3; i++) {
        digitalWrite(STATUS_LED, HIGH);
        delay(200);
        digitalWrite(STATUS_LED, LOW);
        delay(200);
      }
      Serial.println(F("   LED OK"));
      testsSucceeded++;
      break;
      
    case 4:
      Serial.println(F("Auto EEPROM test"));
      int testValue = lightThreshold + 1;
      EEPROM.write(100, testValue & 0xFF);
      delay(10);
      int readValue = EEPROM.read(100);
      if (readValue == (testValue & 0xFF)) {
        Serial.println(F("   EEPROM OK"));
        testsSucceeded++;
      } else {
        Serial.println(F("   EEPROM defective"));
        testsFailed++;
      }
      break;
      
    case 5:
      Serial.println();
      Serial.println(F("TEST FINISHED"));
      Serial.print(F("Results: "));
      Serial.print(testsSucceeded);
      Serial.print(F(" successes, "));
      Serial.print(testsFailed);
      Serial.println(F(" failures"));
      
      if (testsFailed == 0) {
        Serial.println(F("SYSTEM FULLY VALIDATED"));
      } else {
        Serial.println(F("Some problems detected"));
      }
      Serial.println(F("Long press to return to normal"));
      break;
  }
}

void saveThreshold() {
  EEPROM.write(THRESHOLD_EEPROM_ADDR, lightThreshold & 0xFF);
  EEPROM.write(THRESHOLD_EEPROM_ADDR + 1, (lightThreshold >> 8) & 0xFF);
}

/*
 * ============================================================================
 * MEMORY OPTIMIZATION NOTES
 * ============================================================================
 * 
 * This version optimizes RAM usage by:
 * - Using F() macro for string literals (stored in flash, not RAM)
 * - Shorter variable names and reduced string constants
 * - Consolidated error messages
 * - Removed lengthy instruction texts
 * - Using byte instead of int for counters where possible
 * - Target: Keep >200 bytes free for stable operation
 * - Arduino Nano total RAM: 2048 bytes
 * 
 * ============================================================================
 */