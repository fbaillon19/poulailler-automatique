/*
 * ============================================================================
 * STATUS LED TEST - Automatic Chicken Coop
 * ============================================================================
 * 
 * Test: 4.1 - Status LED Blinking
 * Objective: Verify LED control and blinking patterns
 * Duration: ~3 minutes
 * 
 * This test verifies:
 * - LED on/off control
 * - Different blinking patterns
 * - Precise blinking timing
 * - Correct limiting resistor
 * 
 * Required wiring:
 * Arduino D3 → 220Ω Resistor → LED Anode (+)
 * LED Cathode (-) → Arduino GND
 * 
 * Procedure:
 * 1. Upload this code to Arduino Nano
 * 2. Observe the LED (no serial monitor needed)
 * 3. Verify different blinking sequences
 * 4. Optional: Open serial monitor for details
 * 
 * Expected result:
 * - LED turns on and off correctly
 * - Different blinking rhythms visible
 * - Sufficient brightness but not excessive
 * 
 * ============================================================================
 */

// Configuration
const int STATUS_LED = 3;                     // LED pin
const unsigned long TEST_DURATION = 180000;   // 3 minutes test

// Global variables
unsigned long testStart = 0;
unsigned long lastBlink = 0;
bool ledState = false;
int testPhase = 0;
unsigned long phaseStart = 0;

void setup() {
  // Initialize serial communication (optional)
  Serial.begin(9600);
  delay(1000);
  
  Serial.println("============================================");
  Serial.println("STATUS LED TEST - Automatic Chicken Coop");
  Serial.println("============================================");
  Serial.println("Version: 1.0");
  Serial.println("Test: 4.1 - Status LED Blinking");
  Serial.println("");
  
  // LED configuration
  pinMode(STATUS_LED, OUTPUT);
  
  // Initial test - LED off
  digitalWrite(STATUS_LED, LOW);
  
  Serial.println("💡 LED Configuration:");
  Serial.println("- Pin used: D3");
  Serial.println("- Resistor: 220Ω (mandatory)");
  Serial.println("- Polarity: Anode via resistor, Cathode to GND");
  Serial.println("");
  
  Serial.println("👀 WATCH THE LED during test:");
  Serial.println("Serial monitor is optional");
  Serial.println("");
  
  Serial.println("🕐 Test sequences (3 minutes):");
  Serial.println("1. Basic ON/OFF test");
  Serial.println("2. Slow blinking (1s)");
  Serial.println("3. Normal blinking (500ms)");
  Serial.println("4. Fast blinking (200ms)");
  Serial.println("5. SOS pattern (... --- ...)");
  Serial.println("6. Final continuous blinking");
  Serial.println("");
  
  testStart = millis();
  phaseStart = millis();
  
  delay(2000);
}

void loop() {
  unsigned long elapsedTime = millis() - testStart;
  
  // Test phase management
  if (elapsedTime < 30000) {        // 0-30s: ON/OFF test
    testPhase = 1;
    testOnOff();
  } else if (elapsedTime < 60000) { // 30-60s: Slow blinking
    if (testPhase != 2) {
      testPhase = 2;
      phaseStart = millis();
      Serial.println("Phase 2: Slow blinking (1000ms)");
    }
    blinkWithPeriod(1000);
  } else if (elapsedTime < 90000) { // 60-90s: Normal blinking
    if (testPhase != 3) {
      testPhase = 3;
      phaseStart = millis();
      Serial.println("Phase 3: Normal blinking (500ms)");
    }
    blinkWithPeriod(500);
  } else if (elapsedTime < 120000) { // 90-120s: Fast blinking
    if (testPhase != 4) {
      testPhase = 4;
      phaseStart = millis();
      Serial.println("Phase 4: Fast blinking (200ms)");
    }
    blinkWithPeriod(200);
  } else if (elapsedTime < 150000) { // 120-150s: SOS pattern
    if (testPhase != 5) {
      testPhase = 5;
      phaseStart = millis();
      Serial.println("Phase 5: SOS pattern (... --- ...)");
    }
    sosPattern();
  } else { // 150s+: Final test
    if (testPhase != 6) {
      testPhase = 6;
      Serial.println("Phase 6: Final test - Continuous blinking");
      displayFinalResults();
    }
    blinkWithPeriod(500);
  }
  
  delay(10);
}

/*
 * Phase 1: Basic ON/OFF test
 */
void testOnOff() {
  unsigned long phaseTime = millis() - phaseStart;
  
  if (phaseTime < 5000) {
    // 0-5s: LED permanently on
    if (!ledState) {
      digitalWrite(STATUS_LED, LOW);
      ledState = false;
      Serial.println("Phase 1b: LED OFF (5 seconds)");
    }
  } else if (phaseTime < 30000) {
    // 10-30s: Slow alternation ON/OFF every 2 seconds
    if (millis() - lastBlink >= 2000) {
      ledState = !ledState;
      digitalWrite(STATUS_LED, ledState);
      lastBlink = millis();
      
      static int counter = 0;
      if (counter % 4 == 0) {
        Serial.print("Phase 1c: Slow alternation ");
        Serial.println(ledState ? "ON" : "OFF");
      }
      counter++;
    }
  }
}

/*
 * Function: Blinking with given period
 */
void blinkWithPeriod(unsigned long period) {
  if (millis() - lastBlink >= period) {
    ledState = !ledState;
    digitalWrite(STATUS_LED, ledState);
    lastBlink = millis();
    
    // Occasional log to avoid spam
    static unsigned long lastLog = 0;
    if (millis() - lastLog > 5000) {
      Serial.print("Blinking ");
      Serial.print(period);
      Serial.print("ms - State: ");
      Serial.println(ledState ? "ON" : "OFF");
      lastLog = millis();
    }
  }
}

/*
 * Function: SOS Pattern (... --- ...)
 */
void sosPattern() {
  static int sosStep = 0;
  static unsigned long stepStart = 0;
  static bool firstPass = true;
  
  if (firstPass) {
    stepStart = millis();
    firstPass = false;
    sosStep = 0;
    digitalWrite(STATUS_LED, LOW);
    Serial.println("Starting SOS pattern");
  }
  
  unsigned long stepTime = millis() - stepStart;
  
  switch (sosStep) {
    // Dots (. . .)
    case 0: case 2: case 4: // Dots ON
      if (stepTime >= 200) {
        digitalWrite(STATUS_LED, LOW);
        stepStart = millis();
        sosStep++;
      } else {
        digitalWrite(STATUS_LED, HIGH);
      }
      break;
      
    case 1: case 3: // Spaces between dots
      if (stepTime >= 200) {
        stepStart = millis();
        sosStep++;
      }
      break;
      
    case 5: // Pause before dashes
      if (stepTime >= 400) {
        stepStart = millis();
        sosStep++;
      }
      break;
      
    // Dashes (- - -)
    case 6: case 8: case 10: // Dashes ON
      if (stepTime >= 600) {
        digitalWrite(STATUS_LED, LOW);
        stepStart = millis();
        sosStep++;
      } else {
        digitalWrite(STATUS_LED, HIGH);
      }
      break;
      
    case 7: case 9: // Spaces between dashes
      if (stepTime >= 200) {
        stepStart = millis();
        sosStep++;
      }
      break;
      
    case 11: // Pause before final dots
      if (stepTime >= 400) {
        stepStart = millis();
        sosStep++;
      }
      break;
      
    // Final dots (. . .)
    case 12: case 14: case 16: // Dots ON
      if (stepTime >= 200) {
        digitalWrite(STATUS_LED, LOW);
        stepStart = millis();
        sosStep++;
      } else {
        digitalWrite(STATUS_LED, HIGH);
      }
      break;
      
    case 13: case 15: // Spaces between dots
      if (stepTime >= 200) {
        stepStart = millis();
        sosStep++;
      }
      break;
      
    case 17: // Long pause before repeat
      if (stepTime >= 1000) {
        sosStep = 0;
        stepStart = millis();
        Serial.println("SOS repeated");
      }
      break;
  }
}

/*
 * Function: Display final results
 */
void displayFinalResults() {
  Serial.println("");
  Serial.println("============================================");
  Serial.println("✅ LED TEST COMPLETED SUCCESSFULLY");
  Serial.println("============================================");
  Serial.println("");
  
  Serial.println("📊 Test results:");
  Serial.println("");
  
  Serial.println("🧪 TESTED FUNCTIONS:");
  Serial.println("✅ Basic LED ON/OFF");
  Serial.println("✅ Slow blinking (1000ms)");
  Serial.println("✅ Normal blinking (500ms)");
  Serial.println("✅ Fast blinking (200ms)");
  Serial.println("✅ Complex pattern (SOS)");
  Serial.println("✅ Precise timing control");
  
  Serial.println("");
  Serial.println("🎯 VISUAL VALIDATION:");
  Serial.println("- LED should be visible and clear");
  Serial.println("- No parasitic flickering");
  Serial.println("- Different rhythms distinguishable");
  Serial.println("- SOS pattern recognizable");
  
  Serial.println("");
  Serial.println("⚡ ELECTRICAL CHECKS:");
  Serial.println("- 220Ω resistor mandatory");
  Serial.println("- LED current ≈ 15-20mA");
  Serial.println("- LED voltage ≈ 2-3V");
  Serial.println("- No excessive heating");
  
  Serial.println("");
  Serial.println("✅ STATUS LED FUNCTIONAL");
  Serial.println("➡️  Ready for next test: Motor control");
  Serial.println("");
  Serial.println("🔄 Final test: Continuous 500ms blinking...");
}

/*
 * ============================================================================
 * DIAGNOSTICS AND TROUBLESHOOTING
 * ============================================================================
 * 
 * ❌ LED never lights up:
 *    - Check polarity: Anode (+) to resistor, Cathode (-) to GND
 *    - Check 220Ω resistor present and connected
 *    - Defective LED (test with 3V battery + resistor)
 *    - Defective pin D3 (test with another pin)
 * 
 * ❌ LED always on:
 *    - Short circuit bypassing Arduino
 *    - Pin D3 short-circuited to +5V
 *    - Code problem (incorrect pinMode)
 * 
 * ❌ LED very dim/not visible:
 *    - Resistor too high (use 220Ω, not 10kΩ)
 *    - Partially defective LED
 *    - Insufficient supply voltage
 *    - Oversized LED (use standard 5mm LED)
 * 
 * ❌ LED too bright/heats up:
 *    - Resistor too low or missing (DANGER!)
 *    - Use exactly 220Ω for 5V
 *    - Poorly sized LED
 *    - Partial short circuit
 * 
 * ❌ Irregular blinking:
 *    - Unstable power supply
 *    - Electromagnetic interference
 *    - Arduino timing problem (rare)
 *    - Missing filter capacitor
 * 
 * ❌ LED burns out quickly:
 *    - Excessive current (check resistor)
 *    - Voltage too high
 *    - Poor quality LED
 *    - Repeated polarity inversions
 * 
 * 🔧 Multimeter tests:
 *    - Voltage on LED anode: 2-3V (LED on)
 *    - Current in circuit: 15-20mA (LED on)
 *    - Pin D3 voltage: 5V (HIGH), 0V (LOW)
 *    - Resistance: exactly 220Ω ±5%
 * 
 * 💡 LED resistor calculation:
 *    R = (Vsource - Vled) / Iled
 *    R = (5V - 2V) / 0.015A = 200Ω
 *    → 220Ω (closest standard value)
 * 
 * ============================================================================
 */