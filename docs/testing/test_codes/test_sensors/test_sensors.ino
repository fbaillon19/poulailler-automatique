/*
 * ============================================================================
 * SENSORS TEST - Automatic Chicken Coop
 * ============================================================================
 * 
 * Test: 3.1 - Light Sensor + Limit Switches
 * Objective: Verify all sensor functionality
 * Duration: ~10 minutes
 * 
 * This test verifies:
 * - Light sensor (photoresistor + voltage divider)
 * - TOP limit switch sensor
 * - BOTTOM limit switch sensor
 * - Value variations according to conditions
 * 
 * Required wiring:
 * Photoresistor:
 *   LDR pin1 → Arduino 5V
 *   LDR pin2 → Arduino A0 + 10kΩ Resistor pin1
 *   10kΩ Resistor pin2 → Arduino GND
 * 
 * Limit switch sensors:
 *   TOP sensor NO → Arduino D8
 *   TOP sensor Common → Arduino GND
 *   BOTTOM sensor NO → Arduino D9
 *   BOTTOM sensor Common → Arduino GND
 * 
 * Procedure:
 * 1. Upload this code to Arduino Nano
 * 2. Open serial monitor (9600 baud)
 * 3. Cover/uncover light sensor
 * 4. Press limit switch sensors
 * 5. Observe variations in serial monitor
 * 
 * Expected result:
 * - Variable light values (50-900)
 * - Limit switches change state
 * - Immediate response to user actions
 * 
 * ============================================================================
 */

// Pin configuration
const int LIGHT_SENSOR = A0;
const int TOP_LIMIT_SWITCH = 8;
const int BOTTOM_LIMIT_SWITCH = 9;

// Global variables
unsigned long lastDisplay = 0;
unsigned long testCounter = 0;

// Variables for change detection
int previousLightValue = -1;
bool previousTopState = true;
bool previousBottomState = true;

// Test statistics
int lightValueMin = 1023;
int lightValueMax = 0;
int topChanges = 0;
int bottomChanges = 0;
bool topTested = false;
bool bottomTested = false;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  delay(1000);
  
  Serial.println("============================================");
  Serial.println("SENSORS TEST - Automatic Chicken Coop");
  Serial.println("============================================");
  Serial.println("Version: 1.0");
  Serial.println("Test: 3.1 - Light Sensor + Limit Switches");
  Serial.println("");
  
  // Pin configuration
  pinMode(TOP_LIMIT_SWITCH, INPUT_PULLUP);
  pinMode(BOTTOM_LIMIT_SWITCH, INPUT_PULLUP);
  
  // Initial sensor test
  Serial.println("🔍 Initial sensor test...");
  Serial.println("");
  
  // Test light sensor
  int initValue = analogRead(LIGHT_SENSOR);
  Serial.print("💡 Light sensor (A0): ");
  Serial.print(initValue);
  Serial.print(" (");
  if (initValue > 800) Serial.println("VERY BRIGHT)");
  else if (initValue > 400) Serial.println("BRIGHT)");
  else if (initValue > 200) Serial.println("DIM)");
  else Serial.println("VERY DIM)");
  
  // Test limit switches
  bool topInitState = digitalRead(TOP_LIMIT_SWITCH);
  bool bottomInitState = digitalRead(BOTTOM_LIMIT_SWITCH);
  
  Serial.print("🔘 TOP limit switch (D8): ");
  Serial.println(topInitState ? "FREE" : "PRESSED");
  Serial.print("🔘 BOTTOM limit switch (D9): ");
  Serial.println(bottomInitState ? "FREE" : "PRESSED");
  
  Serial.println("");
  Serial.println("📋 Test instructions:");
  Serial.println("1. LIGHT: Cover/uncover sensor with your hand");
  Serial.println("2. LIGHTING: Use your phone's flashlight");
  Serial.println("3. LIMIT SWITCHES: Physically press each sensor");
  Serial.println("");
  Serial.println("🕐 Automatic test for 60 seconds...");
  Serial.println("Observe values and test sensors manually");
  Serial.println("");
  
  // Initialize reference values
  previousLightValue = initValue;
  previousTopState = topInitState;
  previousBottomState = bottomInitState;
  
  delay(2000);
}

void loop() {
  // Update every 200ms for responsiveness
  if (millis() - lastDisplay >= 200) {
    lastDisplay = millis();
    testCounter++;
    
    // Read all sensors
    int lightValue = analogRead(LIGHT_SENSOR);
    bool topState = digitalRead(TOP_LIMIT_SWITCH);
    bool bottomState = digitalRead(BOTTOM_LIMIT_SWITCH);
    
    // Update light statistics
    if (lightValue < lightValueMin) lightValueMin = lightValue;
    if (lightValue > lightValueMax) lightValueMax = lightValue;
    
    // Detect limit switch changes
    if (topState != previousTopState) {
      topChanges++;
      topTested = true;
      Serial.print("🔄 TOP: ");
      Serial.println(topState ? "FREE" : "PRESSED ✅");
      previousTopState = topState;
    }
    
    if (bottomState != previousBottomState) {
      bottomChanges++;
      bottomTested = true;
      Serial.print("🔄 BOTTOM: ");
      Serial.println(bottomState ? "FREE" : "PRESSED ✅");
      previousBottomState = bottomState;
    }
    
    // Main display every second
    if (testCounter % 5 == 0) {
      Serial.print("Test ");
      Serial.print(testCounter / 5);
      Serial.print("s | Light: ");
      
      // Display light value with visual bar
      if (lightValue < 100) Serial.print("  ");
      else if (lightValue < 1000) Serial.print(" ");
      Serial.print(lightValue);
      
      // Visual progress bar
      Serial.print(" [");
      int bars = map(lightValue, 0, 1023, 0, 10);
      for (int i = 0; i < 10; i++) {
        if (i < bars) Serial.print("█");
        else Serial.print("-");
      }
      Serial.print("]");
      
      // Limit switch states
      Serial.print(" | T:");
      Serial.print(topState ? "○" : "●");
      Serial.print(" B:");
      Serial.print(bottomState ? "○" : "●");
      
      // Detect significant light variation
      int variation = abs(lightValue - previousLightValue);
      if (variation > 50) {
        Serial.print(" 🔄");
        previousLightValue = lightValue;
      }
      
      Serial.println("");
    }
    
    // Encouragement messages
    if (testCounter == 50) { // 10 seconds
      Serial.println("");
      Serial.println("💡 Try covering the light sensor!");
      Serial.println("");
    }
    
    if (testCounter == 150) { // 30 seconds
      Serial.println("");
      Serial.println("🔘 Don't forget to test the limit switches!");
      Serial.println("");
    }
    
    // Test completed after 60 seconds (300 * 200ms)
    if (testCounter >= 300) {
      displayFinalResults(lightValue, topState, bottomState);
      
      // Final loop with continuous display
      while (true) {
        delay(500);
        
        int light = analogRead(LIGHT_SENSOR);
        bool t = digitalRead(TOP_LIMIT_SWITCH);
        bool b = digitalRead(BOTTOM_LIMIT_SWITCH);
        
        Serial.print("Final | Light:");
        Serial.print(light);
        Serial.print(" T:");
        Serial.print(t ? "○" : "●");
        Serial.print(" B:");
        Serial.print(b ? "○" : "●");
        Serial.print(" | Light range: ");
        Serial.print(lightValueMin);
        Serial.print("-");
        Serial.println(lightValueMax);
      }
    }
  }
  
  delay(10);
}

/*
 * Function: Display final results
 */
void displayFinalResults(int finalLight, bool finalTop, bool finalBottom) {
  Serial.println("");
  Serial.println("============================================");
  Serial.println("✅ SENSORS TEST COMPLETED");
  Serial.println("============================================");
  Serial.println("");
  
  Serial.println("📊 Test results:");
  Serial.println("");
  
  // Light sensor results
  Serial.println("💡 LIGHT SENSOR:");
  Serial.print("- Final value: ");
  Serial.println(finalLight);
  Serial.print("- Observed range: ");
  Serial.print(lightValueMin);
  Serial.print(" - ");
  Serial.println(lightValueMax);
  
  int range = lightValueMax - lightValueMin;
  Serial.print("- Total variation: ");
  Serial.print(range);
  if (range > 300) {
    Serial.println(" ✅ EXCELLENT");
  } else if (range > 100) {
    Serial.println(" ✅ GOOD");
  } else if (range > 50) {
    Serial.println(" ⚠️ WEAK (check wiring)");
  } else {
    Serial.println(" ❌ VERY WEAK (sensor problem)");
  }
  
  // Limit switch results
  Serial.println("");
  Serial.println("🔘 LIMIT SWITCHES:");
  Serial.print("- TOP sensor: ");
  Serial.print(finalTop ? "FREE" : "PRESSED");
  Serial.print(" (");
  Serial.print(topChanges);
  Serial.print(" changes) ");
  Serial.println(topTested ? "✅" : "⚠️ Not tested");
  
  Serial.print("- BOTTOM sensor: ");
  Serial.print(finalBottom ? "FREE" : "PRESSED");
  Serial.print(" (");
  Serial.print(bottomChanges);
  Serial.print(" changes) ");
  Serial.println(bottomTested ? "✅" : "⚠️ Not tested");
  
  // Overall evaluation
  Serial.println("");
  Serial.println("🎯 OVERALL EVALUATION:");
  
  bool lightOK = (range > 100);
  bool sensorsOK = (topTested && bottomTested);
  
  if (lightOK && sensorsOK) {
    Serial.println("✅ ALL SENSORS FUNCTIONAL");
    Serial.println("➡️  Ready for next test: Multi-function button");
  } else {
    Serial.println("⚠️  SOME PROBLEMS DETECTED:");
    if (!lightOK) Serial.println("- Light sensor: insufficient variation");
    if (!sensorsOK) Serial.println("- Limit switches: incomplete test");
    Serial.println("➡️  Check wiring before next test");
  }
  
  Serial.println("");
}

/*
 * ============================================================================
 * DIAGNOSTICS AND TROUBLESHOOTING
 * ============================================================================
 * 
 * ❌ Light sensor fixed value 0:
 *    - Short circuit: check resistor divider wiring
 *    - Defective or poorly connected LDR
 *    - 10kΩ resistor poorly connected
 * 
 * ❌ Light sensor fixed value 1023:
 *    - Open circuit: check all connections
 *    - LDR disconnected from A0
 *    - 10kΩ resistor not connected to GND
 * 
 * ❌ Light sensor doesn't vary:
 *    - Defective LDR (test resistance with multimeter)
 *    - Constant ambient lighting
 *    - Incorrect resistor divider wiring
 * 
 * ❌ Limit switch always "FREE":
 *    - Defective or poorly wired sensor
 *    - Incorrect NO (Normally Open) connection
 *    - Internal pull-up not activated (INPUT_PULLUP)
 * 
 * ❌ Limit switch always "PRESSED":
 *    - Short circuit to GND
 *    - Mechanically stuck sensor
 *    - Reversed wiring (use NC instead of NO)
 * 
 * 🔧 Additional manual tests:
 *    - Multimeter on A0: should vary 0-5V with light
 *    - Multimeter on D8/D9: 5V free, 0V pressed
 *    - Test LDR resistance: varies 1kΩ-100kΩ with light
 * 
 * ============================================================================
 */