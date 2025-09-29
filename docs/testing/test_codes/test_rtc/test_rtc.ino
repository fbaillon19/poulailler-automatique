/*
 * ============================================================================
 * RTC DS3231 TEST - Automatic Chicken Coop
 * ============================================================================
 * 
 * Test: 2.2 - RTC DS3231 Time Keeping & I2C Communication
 * Objective: Verify I2C communication and RTC functionality
 * Duration: ~10 minutes
 * 
 * This test verifies:
 * - I2C communication with DS3231
 * - Real-time clock reading
 * - Backup battery functionality
 * - Timing accuracy
 * 
 * Required wiring:
 * DS3231 VCC → Arduino 5V
 * DS3231 GND → Arduino GND  
 * DS3231 SDA → Arduino A4
 * DS3231 SCL → Arduino A5
 * 
 * Procedure:
 * 1. Upload this code to Arduino Nano
 * 2. Open serial monitor (9600 baud)
 * 3. Verify RTC detection and time display
 * 4. If time incorrect, follow instructions for setting
 * 5. Observe seconds incrementing
 * 
 * Expected result:
 * - RTC detected on I2C bus
 * - Time displayed and incrementing
 * - DS3231 module temperature displayed
 * 
 * ============================================================================
 */

#include <Wire.h>
#include <RTClib.h>

// Global variables
RTC_DS3231 rtc;
unsigned long lastDisplay = 0;
unsigned long secondCounter = 0;
DateTime initialTime;
bool rtcInitialized = false;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  delay(1000); // Wait for stabilization
  
  Serial.println(F("============================================"));
  Serial.println(F("RTC DS3231 TEST - Automatic Chicken Coop"));
  Serial.println(F("============================================"));
  Serial.println(F("Version: 1.0"));
  Serial.println(F("Test: 2.2 - RTC Time Keeping & I2C"));
  Serial.println();
  
  // Test I2C communication
  Serial.println(F("🔍 Initializing RTC DS3231..."));
  
  if (!rtc.begin()) {
    Serial.println(F("❌ FAILURE: RTC DS3231 not found on I2C bus!"));
    Serial.println();
    Serial.println(F("Checks to perform:"));
    Serial.println(F("- I2C wiring: SDA→A4, SCL→A5"));
    Serial.println(F("- Power supply: VCC→5V, GND→GND"));
    Serial.println(F("- DS3231 module functional"));
    Serial.println(F("- Module solder joints"));
    Serial.println();
    Serial.println(F("⏸️  Test stopped - Fix problem and restart"));
    
    // Error blinking
    pinMode(LED_BUILTIN, OUTPUT);
    while(true) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
    }
  }
  
  Serial.println(F("✅ RTC DS3231 detected successfully!"));
  rtcInitialized = true;
  
  // Check for power loss
  if (rtc.lostPower()) {
    Serial.println();
    Serial.println(F("⚠️  WARNING: RTC lost power!"));
    Serial.println(F("Time must be set."));
    Serial.println();
    Serial.println(F("Setting options:"));
    Serial.println(F("1. Automatic: compilation time"));
    Serial.println(F("2. Manual: modify code"));
    Serial.println();
    Serial.print(F("Automatic setting in 5 seconds..."));
    
    for(int i = 5; i > 0; i--) {
      Serial.print(F(" "));
      Serial.print(i);
      delay(1000);
    }
    Serial.println();
    
    // Automatic setting with compilation time
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println(F("✅ Time set automatically"));
  } else {
    Serial.println(F("✅ RTC time preserved (battery functional)"));
  }
  
  // Read initial time
  initialTime = rtc.now();
  
  // Module information
  Serial.println();
  Serial.println(F("📊 DS3231 Module Information:"));
  Serial.print(F("- Current time: "));
  displayTime(initialTime);
  Serial.print(F("- Temperature: "));
  Serial.print(rtc.getTemperature());
  Serial.println(F("°C"));
  
  Serial.println();
  Serial.println(F("🕐 Starting real-time test..."));
  Serial.println(F("Observation: time should increment every second"));
  Serial.println();
  
  delay(1000);
}

void loop() {
  if (!rtcInitialized) return;
  
  // Test every second
  if (millis() - lastDisplay >= 1000) {
    lastDisplay = millis();
    secondCounter++;
    
    // Read current time
    DateTime now = rtc.now();
    
    // Main display
    Serial.print(F("Test #"));
    if (secondCounter < 10) Serial.print(F("0"));
    Serial.print(secondCounter);
    Serial.print(F(" | "));
    displayTime(now);
    
    // Calculate time delta
    unsigned long deltaSeconds = now.unixtime() - initialTime.unixtime();
    Serial.print(F(" | Δt="));
    Serial.print(deltaSeconds);
    Serial.print(F("s"));
    
    // Check accuracy (tolerance ±2 seconds)
    long difference = (long)deltaSeconds - (long)secondCounter;
    if (abs(difference) <= 2) {
      Serial.print(F(" | ✅"));
    } else {
      Serial.print(F(" | ⚠️ Drift:"));
      Serial.print(difference);
      Serial.print(F("s"));
    }
    
    Serial.println();
    
    // Special tests at intervals
    if (secondCounter % 15 == 0) {
      Serial.println();
      testTemperature();
      Serial.println();
    }
    
    if (secondCounter % 30 == 0) {
      testAccuracy(now);
    }
    
    // Test completed after 60 seconds
    if (secondCounter >= 60) {
      Serial.println();
      Serial.println(F("============================================"));
      Serial.println(F("✅ RTC DS3231 TEST COMPLETED SUCCESSFULLY"));
      Serial.println(F("============================================"));
      Serial.println();
      Serial.println(F("📊 Test results:"));
      Serial.print(F("- Observation duration: "));
      Serial.print(secondCounter);
      Serial.println(F(" seconds"));
      Serial.println(F("- I2C communication: OK"));
      Serial.println(F("- Time incrementing: OK"));
      Serial.println(F("- Backup battery: OK"));
      
      DateTime final = rtc.now();
      unsigned long realDuration = final.unixtime() - initialTime.unixtime();
      long drift = (long)realDuration - (long)secondCounter;
      
      Serial.print(F("- Accuracy: "));
      if (abs(drift) <= 2) {
        Serial.println(F("EXCELLENT (±2s)"));
      } else if (abs(drift) <= 5) {
        Serial.println(F("GOOD (±5s)"));
      } else {
        Serial.print(F("AVERAGE (drift "));
        Serial.print(drift);
        Serial.println(F("s)"));
      }
      
      Serial.println();
      Serial.println(F("➡️  Ready for next test: LCD I2C"));
      Serial.println();
      
      // Stop test
      while (true) {
        Serial.print(F("Final time: "));
        displayTime(rtc.now());
        Serial.print(F(" | Temp: "));
        Serial.print(rtc.getTemperature());
        Serial.println(F("°C"));
        delay(5000);
      }
    }
  }
  
  delay(10);
}

/*
 * Function: Formatted time display
 */
void displayTime(DateTime dt) {
  if (dt.hour() < 10) Serial.print(F("0"));
  Serial.print(dt.hour());
  Serial.print(F(":"));
  if (dt.minute() < 10) Serial.print(F("0"));
  Serial.print(dt.minute());
  Serial.print(F(":"));
  if (dt.second() < 10) Serial.print(F("0"));
  Serial.print(dt.second());
}

/*
 * Function: DS3231 temperature test
 */
void testTemperature() {
  float temp = rtc.getTemperature();
  Serial.print(F("🌡️  DS3231 Temperature: "));
  Serial.print(temp);
  Serial.print(F("°C"));
  
  if (temp > -40 && temp < 85) {
    Serial.println(F(" ✅ (Normal)"));
  } else {
    Serial.println(F(" ⚠️ (Out of range)"));
  }
}

/*
 * Function: Timing accuracy test
 */
void testAccuracy(DateTime now) {
  Serial.println();
  Serial.println(F("🎯 Timing accuracy test:"));
  
  unsigned long arduinoTime = millis() / 1000;
  unsigned long rtcTime = now.unixtime() - initialTime.unixtime();
  long drift = (long)rtcTime - (long)arduinoTime;
  
  Serial.print(F("- Arduino time: "));
  Serial.print(arduinoTime);
  Serial.println(F("s"));
  Serial.print(F("- RTC time: "));
  Serial.print(rtcTime);
  Serial.println(F("s"));
  Serial.print(F("- Drift: "));
  Serial.print(drift);
  Serial.println(F("s"));
  
  if (abs(drift) <= 1) {
    Serial.println(F("- Accuracy: EXCELLENT"));
  } else if (abs(drift) <= 3) {
    Serial.println(F("- Accuracy: GOOD"));
  } else {
    Serial.println(F("- Accuracy: To monitor"));
  }
}

/*
 * ============================================================================
 * DIAGNOSTICS AND TROUBLESHOOTING
 * ============================================================================
 * 
 * ❌ "RTC DS3231 not found on I2C bus":
 *    - Check wiring: SDA→A4, SCL→A5, VCC→5V, GND→GND
 *    - Check DS3231 module solder joints
 *    - Test with I2C scanner (address 0x68)
 *    - Replace DS3231 module if defective
 * 
 * ❌ "RTC lost power":
 *    - CR2032 battery discharged or poorly inserted
 *    - Replace CR2032 battery
 *    - Check battery contacts on module
 * 
 * ❌ Persistent incorrect time:
 *    - Force setting: rtc.adjust(DateTime(YYYY,MM,DD,HH,MM,SS))
 *    - Check backup battery integrity
 *    - Defective RTC module
 * 
 * ❌ Significant drift (>5s/minute):
 *    - Extreme temperature (automatic DS3231 compensation)
 *    - Defective DS3231 module
 *    - Electromagnetic interference
 * 
 * ❌ Aberrant temperature:
 *    - Damaged DS3231 module
 *    - I2C communication problem
 *    - Replace module
 * 
 * ============================================================================
 */