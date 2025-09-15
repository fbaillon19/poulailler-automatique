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
  Serial.println("============================================");
  Serial.println("ARDUINO NANO TEST - Automatic Chicken Coop");
  Serial.println("============================================");
  Serial.println("Version: 1.0");
  Serial.println("Test: 2.1 - Arduino Basic Functionality");
  Serial.println("");
  
  // System information
  Serial.print("CPU Frequency: ");
  Serial.print(F_CPU / 1000000);
  Serial.println(" MHz");
  
  Serial.print("Reference Voltage: ");
  Serial.print(5.0);
  Serial.println(" V");
  
  Serial.println("");
  Serial.println("Starting test...");
  Serial.println("Expected: Messages every second + LED blinks");
  Serial.println("");
  
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
    Serial.print("Test #");
    Serial.print(counter);
    Serial.print(" | Time: ");
    Serial.print(millis() / 1000);
    Serial.print("s | LED: ");
    Serial.print(ledState ? "ON " : "OFF");
    Serial.print(" | Pin2: ");
    Serial.print(digitalRead(2) ? "HIGH" : "LOW");
    Serial.print(" | Pin3: ");
    Serial.print(digitalRead(3) ? "HIGH" : "LOW");
    Serial.print(" | Free RAM: ");
    Serial.print(getFreeMemory());
    Serial.println(" bytes");
    
    // Periodic status messages
    if (counter % 10 == 0) {
      Serial.println("");
      Serial.print("✅ Arduino running for ");
      Serial.print(counter);
      Serial.println(" seconds");
      Serial.println("");
    }
    
    // Test completed after 30 seconds
    if (counter >= 30) {
      Serial.println("");
      Serial.println("============================================");
      Serial.println("✅ ARDUINO NANO TEST COMPLETED SUCCESSFULLY");
      Serial.println("============================================");
      Serial.println("");
      Serial.println("Results:");
      Serial.print("- Test duration: ");
      Serial.print(counter);
      Serial.println(" seconds");
      Serial.println("- Serial communication: OK");
      Serial.println("- Main loop: OK");
      Serial.println("- Pin control: OK");
      Serial.println("- Built-in LED: OK");
      Serial.println("");
      Serial.println("➡️  Ready for next test: RTC DS3231");
      Serial.println("");
      
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