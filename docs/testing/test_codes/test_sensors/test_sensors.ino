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
  
  Serial.println(F("============================================"));
  Serial.println(F("SENSORS TEST - Automatic Chicken Coop"));
  Serial.println(F("============================================"));
  Serial.println(F("Version: 1.0"));
  Serial.println(F("Test: 3.1 - Light Sensor + Limit Switches"));
  Serial.println();
  
  // Pin configuration
  pinMode(TOP_LIMIT_SWITCH, INPUT_PULLUP);
  pinMode(BOTTOM_LIMIT_SWITCH, INPUT_PULLUP);
  
  // Initial sensor test
  Serial.println(F("🔍 Initial sensor test..."));
  Serial.println();
  
  // Test light sensor
  int initValue = analogRead(LIGHT_SENSOR);
  Serial.print(F("💡 Light sensor (A0): "));
  Serial.print(initValue);
  Serial.print(F(" ("));
  if (initValue > 800) Serial.println(F("VERY BRIGHT)"));
  else if (initValue > 400) Serial.println(F("BRIGHT)"));
  else if (initValue > 200) Serial.println(F("DIM)"));
  else Serial.println(F("VERY DIM)"));
  
  // Test limit switches
  bool topInitState = digitalRead(TOP_LIMIT_SWITCH);
  bool bottomInitState = digitalRead(BOTTOM_LIMIT_SWITCH);
  
  Serial.print(F("🔘 TOP limit switch (D8): "));
  Serial.println(topInitState ? F("FREE") : F("PRESSED"));
  Serial.print(F("🔘 BOTTOM limit switch (D9): "));
  Serial.println(bottomInitState ? F("FREE") : F("PRESSED"));
  
  Serial.println();
  Serial.println(F("📋 Test instructions:"));
  Serial.println(F("1. LIGHT: Cover/uncover sensor with your hand"));
  Serial.println(F("2. LIGHTING: Use your phone's flashlight"));
  Serial.println(F("3. LIMIT SWITCHES: Physically press each sensor"));
  Serial.println();
  Serial.println(F("🕐 Automatic test for 60 seconds..."));
  Serial.println(F("Observe values and test sensors manually"));
  Serial.println();
  
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
      Serial.print(F("🔄 TOP: "));
      Serial.println(topState ? F("FREE") : F("PRESSED ✅"));
      previousTopState = topState;
    }
    
    if (bottomState != previousBottomState) {
      bottomChanges++;
      bottomTested = true;
      Serial.print(F("🔄 BOTTOM: "));
      Serial.println(bottomState ? F("FREE") : F("PRESSED ✅"));
      previousBottomState = bottomState;
    }
    
    // Main display every second
    if (testCounter % 5 == 0) {
      Serial.print(F("Test "));
      Serial.print(testCounter / 5);
      Serial.print(F("s | Light: "));
      
      // Display light value with visual bar
      if (lightValue < 100) Serial.print(F("  "));
      else if (lightValue < 1000) Serial.print(F(" "));
      Serial.print(lightValue);
      
      // Visual progress bar
      Serial.print(F(" ["));
      int bars = map(lightValue, 0, 1023, 0, 10);
      for (int i = 0; i < 10; i++) {
        if (i < bars) Serial.print(F("█"));
        else Serial.print(F("-"));
      }
      Serial.print(F("]"));
      
      // Limit switch states
      Serial.print(F(" | T:"));
      Serial.print(topState ? F("○") : F("●"));
      Serial.print(F(" B:"));
      Serial.print(bottomState ? F("○") : F("●"));
      
      // Detect significant light variation
      int variation = abs(lightValue - previousLightValue);
      if (variation > 50) {
        Serial.print(F(" 🔄"));
        previousLightValue = lightValue;
      }
      
      Serial.println();
    }
    
    // Encouragement messages
    if (testCounter == 50) { // 10 seconds
      Serial.println();
      Serial.println(F("💡 Try covering the light sensor!"));
      Serial.println();
    }
    
    if (testCounter == 150) { // 30 seconds
      Serial.println();
      Serial.println(F("🔘 Don't forget to test the limit switches!"));
      Serial.println();
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
        
        Serial.print(F("Final | Light:"));
        Serial.print(light);
        Serial.print(F(" T:"));
        Serial.print(t ? F("○") : F("●"));
        Serial.print(F(" B:"));
        Serial.print(b ? F("○") : F("●"));
        Serial.print(F(" | Light range: "));
        Serial.print(lightValueMin);
        Serial.print(F("-"));
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
  Serial.println();
  Serial.println(F("============================================"));
  Serial.println(F("✅ SENSORS TEST COMPLETED"));
  Serial.println(F("============================================"));
  Serial.println();
  
  Serial.println(F("📊 Test results:"));
  Serial.println();
  
  // Light sensor results
  Serial.println(F("💡 LIGHT SENSOR:"));
  Serial.print(F("- Final value: "));
  Serial.println(finalLight);
  Serial.print(F("- Observed range: "));
  Serial.print(lightValueMin);
  Serial.print(F(" - "));
  Serial.println(lightValueMax);
  
  int range = lightValueMax - lightValueMin;
  Serial.print(F("- Total variation: "));
  Serial.print(range);
  if (range > 300) {
    Serial.println(F(" ✅ EXCELLENT"));
  } else if (range > 100) {
    Serial.println(F(" ✅ GOOD"));
  } else if (range > 50) {
    Serial.println(F(" ⚠️ WEAK (check wiring)"));
  } else {
    Serial.println(F(" ❌ VERY WEAK (sensor problem)"));
  }
  
  // Limit switch results
  Serial.println();
  Serial.println(F("🔘 LIMIT SWITCHES:"));
  Serial.print(F("- TOP sensor: "));
  Serial.print(finalTop ? F("FREE") : F("PRESSED"));
  Serial.print(F(" ("));
  Serial.print(topChanges);
  Serial.print(F(" changes) "));
  Serial.println(topTested ? F("✅") : F("⚠️ Not tested"));
  
  Serial.print(F("- BOTTOM sensor: "));
  Serial.print(finalBottom ? F("FREE") : F("PRESSED"));
  Serial.print(F(" ("));
  Serial.print(bottomChanges);
  Serial.print(F(" changes) "));
  Serial.println(bottomTested ? F("✅") : F("⚠️ Not tested"));
  
  // Overall evaluation
  Serial.println();
  Serial.println(F("🎯 OVERALL EVALUATION:"));
  
  bool lightOK = (range > 100);
  bool sensorsOK = (topTested && bottomTested);
  
  if (lightOK && sensorsOK) {
    Serial.println(F("✅ ALL SENSORS FUNCTIONAL"));
    Serial.println(F("➡️  Ready for next test: Multi-function button"));
  } else {
    Serial.println(F("⚠️  SOME PROBLEMS DETECTED:"));
    if (!lightOK) Serial.println(F("- Light sensor: insufficient variation"));
    if (!sensorsOK) Serial.println(F("- Limit switches: incomplete test"));
    Serial.println(F("➡️  Check wiring before next test"));
  }
  
  Serial.println();
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