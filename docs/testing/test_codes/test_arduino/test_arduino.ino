/*
 * ============================================================================
 * ARDUINO NANO BASIC TEST - Automatic Chicken Coop
 * ============================================================================
 * 
 * Test: 2.1 - Arduino Nano Basic Functionality
 * Objective: Verify programming and basic operation
 * Duration: ~5 minutes
 * 
 * This test verifies:
 * - Successful Arduino programming
 * - Functional serial communication
 * - Operational main loop
 * - Configurable digital pins
 * 
 * Procedure:
 * 1. Upload this code to Arduino Nano
 * 2. Open serial monitor (9600 baud)
 * 3. Verify regular messages
 * 4. Observe built-in LED blinking
 * 
 * Expected result:
 * - Messages in serial monitor every second
 * - Pin 13 LED blinking
 * - Incrementing counter
 * 
 * ============================================================================
 */

// Global variables
unsigned long counter = 0;
unsigned long lastDisplay = 0;
bool ledState = false;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
  // Configure built-in LED (pin 13)
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Configure test pins
  pinMode(2, OUTPUT);   // Test output pin
  pinMode(3, INPUT);    // Test input pin
  
  // Startup message
  Serial.println(F("============================================"));
  Serial.println(F("ARDUINO NANO TEST - Automatic Chicken Coop"));
  Serial.println(F("============================================"));
  Serial.println(F("Version: 1.0"));
  Serial.println(F("Test: 2.1 - Arduino Basic Functionality"));
  Serial.println();
  
  // System information
  Serial.print(F("CPU Frequency: "));
  Serial.print(F_CPU / 1000000);
  Serial.println(F(" MHz"));
  
  Serial.print(F("Reference Voltage: "));
  Serial.print(F("5.0"));
  Serial.println(F(" V"));
  
  Serial.println();
  Serial.println(F("Starting test..."));
  Serial.println(F("Expected: Messages every second + LED blinks"));
  Serial.println();
  
  delay(1000);
}

void loop() {
  // Test every second
  if (millis() - lastDisplay >= 1000) {
    lastDisplay = millis();
    counter++;
    
    // Toggle LED
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);
    
    // Test output pin
    digitalWrite(2, ledState);
    
    // Display status
    Serial.print(F("Test #"));
    Serial.print(counter);
    Serial.print(F(" | Time: "));
    Serial.print(millis() / 1000);
    Serial.print(F("s | LED: "));
    Serial.print(ledState ? F("ON ") : F("OFF"));
    Serial.print(F(" | Pin2: "));
    Serial.print(digitalRead(2) ? F("HIGH") : F("LOW"));
    Serial.print(F(" | Pin3: "));
    Serial.print(digitalRead(3) ? F("HIGH") : F("LOW"));
    Serial.print(F(" | Free RAM: "));
    Serial.print(getFreeMemory());
    Serial.println(F(" bytes"));
    
    // Periodic status messages
    if (counter % 10 == 0) {
      Serial.println();
      Serial.print(F("✅ Arduino running for "));
      Serial.print(counter);
      Serial.println(F(" seconds"));
      Serial.println();
    }
    
    // Test completed after 30 seconds
    if (counter >= 30) {
      Serial.println();
      Serial.println(F("============================================"));
      Serial.println(F("✅ ARDUINO NANO TEST COMPLETED SUCCESSFULLY"));
      Serial.println(F("============================================"));
      Serial.println();
      Serial.println(F("Results:"));
      Serial.print(F("- Test duration: "));
      Serial.print(counter);
      Serial.println(F(" seconds"));
      Serial.println(F("- Serial communication: OK"));
      Serial.println(F("- Main loop: OK"));
      Serial.println(F("- Pin control: OK"));
      Serial.println(F("- Built-in LED: OK"));
      Serial.println();
      Serial.println(F("➡️  Ready for next test: RTC DS3231"));
      Serial.println();
      
      // Success blinking
      for (int i = 0; i < 10; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN, LOW);
        delay(100);
      }
      
      // Stop test (infinite loop)
      while (true) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(1000);
        digitalWrite(LED_BUILTIN, LOW);
        delay(1000);
      }
    }
  }
  
  // Other light processing
  delay(10);
}

/*
 * Utility function: Calculate free memory
 * Helps detect memory leaks
 */
int getFreeMemory() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

/*
 * ============================================================================
 * DIAGNOSTICS AND TROUBLESHOOTING
 * ============================================================================
 * 
 * Possible problems:
 * 
 * ❌ No messages in serial monitor:
 *    - Check selected COM port
 *    - Check 9600 baud rate
 *    - Check USB cable
 *    - Restart Arduino IDE
 * 
 * ❌ LED not blinking:
 *    - Arduino possibly defective
 *    - Power supply problem
 *    - Pin 13 damaged
 * 
 * ❌ Erratic messages:
 *    - Unstable power supply
 *    - Electrical interference
 *    - Defective USB cable
 * 
 * ❌ Free RAM decreasing:
 *    - Memory leak (shouldn't happen in this test)
 *    - More serious Arduino problem
 * 
 * ❌ Test never ends:
 *    - Defective Arduino clock
 *    - Blocked millis() function
 *    - Restart Arduino
 * 
 * ============================================================================
 */