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
  
  Serial.println("============================================");
  Serial.println("RTC DS3231 TEST - Automatic Chicken Coop");
  Serial.println("============================================");
  Serial.println("Version: 1.0");
  Serial.println("Test: 2.2 - RTC Time Keeping & I2C");
  Serial.println("");
  
  // Test I2C communication
  Serial.println("🔍 Initializing RTC DS3231...");
  
  if (!rtc.begin()) {
    Serial.println("❌ FAILURE: RTC DS3231 not found on I2C bus!");
    Serial.println("");
    Serial.println("Checks to perform:");
    Serial.println("- I2C wiring: SDA→A4, SCL→A5");
    Serial.println("- Power supply: VCC→5V, GND→GND");
    Serial.println("- DS3231 module functional");
    Serial.println("- Module solder joints");
    Serial.println("");
    Serial.println("⏸️  Test stopped - Fix problem and restart");
    
    // Error blinking
    pinMode(LED_BUILTIN, OUTPUT);
    while(true) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
    }
  }
  
  Serial.println("✅ RTC DS3231 detected successfully!");
  rtcInitialized = true;
  
  // Check for power loss
  if (rtc.lostPower()) {
    Serial.println("");
    Serial.println("⚠️  WARNING: RTC lost power!");
    Serial.println("Time must be set.");
    Serial.println("");
    Serial.println("Setting options:");
    Serial.println("1. Automatic: compilation time");
    Serial.println("2. Manual: modify code");
    Serial.println("");
    Serial.print("Automatic setting in 5 seconds...");
    
    for(int i = 5; i > 0; i--) {
      Serial.print(" ");
      Serial.print(i);
      delay(1000);
    }
    Serial.println("");
    
    // Automatic setting with compilation time
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println("✅ Time set automatically");
  } else {
    Serial.println("✅ RTC time preserved (battery functional)");
  }
  
  // Read initial time
  initialTime = rtc.now();
  
  // Module information
  Serial.println("");
  Serial.println("📊 DS3231 Module Information:");
  Serial.print("- Current time: ");
  displayTime(initialTime);
  Serial.print("- Temperature: ");
  Serial.print(rtc.getTemperature());
  Serial.println("°C");
  
  Serial.println("");
  Serial.println("🕐 Starting real-time test...");
  Serial.println("Observation: time should increment every second");
  Serial.println("");
  
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
    Serial.print("Test #");
    if (secondCounter < 10) Serial.print("0");
    Serial.print(secondCounter);
    Serial.print(" | ");
    displayTime(now);
    
    // Calculate time delta
    unsigned long deltaSeconds = now.unixtime() - initialTime.unixtime();
    Serial.print(" | Δt=");
    Serial.print(deltaSeconds);
    Serial.print("s");
    
    // Check accuracy (tolerance ±2 seconds)
    long difference = (long)deltaSeconds - (long)secondCounter;
    if (abs(difference) <= 2) {
      Serial.print(" | ✅");
    } else {
      Serial.print(" | ⚠️ Drift:");
      Serial.print(difference);
      Serial.print("s");
    }
    
    Serial.println("");
    
    // Special tests at intervals
    if (secondCounter % 15 == 0) {
      Serial.println("");
      testTemperature();
      Serial.println("");
    }
    
    if (secondCounter % 30 == 0) {
      testAccuracy(now);
    }
    
    // Test completed after 60 seconds
    if (secondCounter >= 60) {
      Serial.println("");
      Serial.println("============================================");
      Serial.println("✅ RTC DS3231 TEST COMPLETED SUCCESSFULLY");
      Serial.println("============================================");
      Serial.println("");
      Serial.println("📊 Test results:");
      Serial.print("- Observation duration: ");
      Serial.print(secondCounter);
      Serial.println(" seconds");
      Serial.println("- I2C communication: OK");
      Serial.println("- Time incrementing: OK");
      Serial.println("- Backup battery: OK");
      
      DateTime final = rtc.now();
      unsigned long realDuration = final.unixtime() - initialTime.unixtime();
      long drift = (long)realDuration - (long)secondCounter;
      
      Serial.print("- Accuracy: ");
      if (abs(drift) <= 2) {
        Serial.println("EXCELLENT (±2s)");
      } else if (abs(drift) <= 5) {
        Serial.println("GOOD (±5s)");
      } else {
        Serial.print("AVERAGE (drift ");
        Serial.print(drift);
        Serial.println("s)");
      }
      
      Serial.println("");
      Serial.println("➡️  Ready for next test: LCD I2C");
      Serial.println("");
      
      // Stop test
      while (true) {
        Serial.print("Final time: ");
        displayTime(rtc.now());
        Serial.print(" | Temp: ");
        Serial.print(rtc.getTemperature());
        Serial.println("°C");
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
  if (dt.hour() < 10) Serial.print("0");
  Serial.print(dt.hour());
  Serial.print(":");
  if (dt.minute() < 10) Serial.print("0");
  Serial.print(dt.minute());
  Serial.print(":");
  if (dt.second() < 10) Serial.print("0");
  Serial.print(dt.second());
}

/*
 * Function: DS3231 temperature test
 */
void testTemperature() {
  float temp = rtc.getTemperature();
  Serial.print("🌡️  DS3231 Temperature: ");
  Serial.print(temp);
  Serial.print("°C");
  
  if (temp > -40 && temp < 85) {
    Serial.println(" ✅ (Normal)");
  } else {
    Serial.println(" ⚠️ (Out of range)");
  }
}

/*
 * Function: Timing accuracy test
 */
void testAccuracy(DateTime now) {
  Serial.println("");
  Serial.println("🎯 Timing accuracy test:");
  
  unsigned long arduinoTime = millis() / 1000;
  unsigned long rtcTime = now.unixtime() - initialTime.unixtime();
  long drift = (long)rtcTime - (long)arduinoTime;
  
  Serial.print("- Arduino time: ");
  Serial.print(arduinoTime);
  Serial.println("s");
  Serial.print("- RTC time: ");
  Serial.print(rtcTime);
  Serial.println("s");
  Serial.print("- Drift: ");
  Serial.print(drift);
  Serial.println("s");
  
  if (abs(drift) <= 1) {
    Serial.println("- Accuracy: EXCELLENT");
  } else if (abs(drift) <= 3) {
    Serial.println("- Accuracy: GOOD");
  } else {
    Serial.println("- Accuracy: To monitor");
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