/*
 * ============================================================================
 * MULTI-FUNCTION BUTTON TEST - Automatic Chicken Coop
 * ============================================================================
 * 
 * Test: 3.3 - Multi-function Button (Lightweight Version)
 * Objective: Verify button detection and multi-function capabilities
 * Duration: ~5 minutes
 * 
 * This test verifies:
 * - Short press detection (<3s)
 * - Long press detection (>3s)
 * - Double-click detection
 * - Button debouncing
 * - Response timing accuracy
 * 
 * Required wiring:
 * Button pin1 → Arduino D5
 * Button pin2 → Arduino GND
 * (Internal pull-up used)
 * 
 * Procedure:
 * 1. Upload this code to Arduino Nano
 * 2. Open serial monitor (9600 baud)
 * 3. Test different button press types
 * 4. Observe detection in serial monitor
 * 
 * Expected result:
 * - Reliable short press detection
 * - Reliable long press detection (>3s)
 * - Double-click recognition
 * - Clean debouncing without false triggers
 * 
 * ============================================================================
 */
const int BUTTON_PIN = 5;
const unsigned long LONG_PRESS = 3000;
const unsigned long DOUBLE_CLICK = 500;

// Button variables
unsigned long pressStart = 0;
unsigned long lastRelease = 0;
bool buttonPressed = false;
bool pressProcessed = false;
bool waitingDoubleClick = false;

// Statistics
int shortPresses = 0;
int longPresses = 0;
int doubleClicks = 0;
int bounces = 0;
unsigned long testCounter = 0;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  Serial.println(F("=== MULTI-FUNCTION BUTTON TEST ==="));
  Serial.print(F("Initial state: "));
  Serial.println(digitalRead(BUTTON_PIN) ? F("FREE") : F("PRESSED"));
  Serial.println(F("Instructions:"));
  Serial.println(F("- Short: <3s"));
  Serial.println(F("- Long: >3s"));  
  Serial.println(F("- Double-click: 2 fast clicks"));
  Serial.println(F("60s test..."));
  Serial.println();
}

void loop() {
  testCounter++;
  handleButton();
  
  if (waitingDoubleClick && millis() - lastRelease > DOUBLE_CLICK) {
    handleShortPress();
    waitingDoubleClick = false;
  }
  
  // Stats every 10s
  if (testCounter % 10000 == 0) {
    Serial.print(F("Stats - Short:"));
    Serial.print(shortPresses);
    Serial.print(F(" Long:"));
    Serial.print(longPresses);
    Serial.print(F(" Double:"));
    Serial.print(doubleClicks);
    Serial.print(F(" Bounces:"));
    Serial.println(bounces);
  }
  
  // End test after 60s
  if (testCounter > 600000) {
    Serial.println(F("\n=== TEST RESULTS ==="));
    Serial.print(F("Short presses: ")); Serial.println(shortPresses);
    Serial.print(F("Long presses: ")); Serial.println(longPresses);
    Serial.print(F("Double-clicks: ")); Serial.println(doubleClicks);
    Serial.print(F("Bounces: ")); Serial.println(bounces);
    
    if (shortPresses > 0 && longPresses > 0 && doubleClicks > 0) {
      Serial.println(F("BUTTON OK - Next test: LED"));
    } else {
      Serial.println(F("Incomplete test - Test all types"));
    }
    
    // Continuous monitoring
    while (true) {
      handleButton();
      if (waitingDoubleClick && millis() - lastRelease > DOUBLE_CLICK) {
        handleShortPress();
        waitingDoubleClick = false;
      }
      delay(10);
    }
  }
  
  delay(10);
}

void handleButton() {
  bool state = digitalRead(BUTTON_PIN);
  
  // Press start
  if (!buttonPressed && state == LOW) {
    delay(20); // Debounce
    if (digitalRead(BUTTON_PIN) == LOW) {
      buttonPressed = true;
      pressStart = millis();
      pressProcessed = false;
      Serial.print(F("-> Press t="));
      Serial.println(pressStart);
    } else {
      bounces++;
      Serial.println(F("Bounce filtered"));
    }
  }
  
  // Release
  if (buttonPressed && state == HIGH) {
    delay(20);
    if (digitalRead(BUTTON_PIN) == HIGH) {
      buttonPressed = false;
      unsigned long duration = millis() - pressStart;
      
      Serial.print(F("<- Release "));
      Serial.print(duration);
      Serial.print(F("ms"));
      
      if (!pressProcessed) {
        if (duration >= LONG_PRESS) {
          Serial.println(F(" -> LONG"));
          longPresses++;
          handleLongPress();
        } else {
          Serial.println(F(" -> Short"));
          
          if (waitingDoubleClick && (millis() - lastRelease) < DOUBLE_CLICK) {
            Serial.println(F("DOUBLE-CLICK!"));
            doubleClicks++;
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
  
  // Long press indication
  if (buttonPressed && !pressProcessed) {
    unsigned long duration = millis() - pressStart;
    if (duration >= LONG_PRESS) {
      Serial.print(F("LONG in progress "));
      Serial.print(duration);
      Serial.println(F("ms"));
      pressProcessed = true;
    }
  }
}

void handleShortPress() {
  Serial.println(F("SHORT PRESS confirmed"));
  shortPresses++;
}

void handleDoubleClick() {
  Serial.println(F("Action: Double-click"));
}

void handleLongPress() {
  Serial.println(F("Action: Long press"));
}
/*
 * ============================================================================
 * DIAGNOSTICS AND TROUBLESHOOTING
 * ============================================================================
 * 
 * ❌ No button response:
 *    - Check wiring: D5-GND connections
 *    - Verify button type (normally open)
 *    - Test button with multimeter
 *    - Check internal pull-up activation
 * 
 * ❌ Excessive bouncing:
 *    - Poor quality button switch
 *    - Add 100nF capacitor across button
 *    - Increase debounce delay in code
 *    - Replace button with better quality
 * 
 * ❌ Long press not detected:
 *    - Hold button longer than 3 seconds
 *    - Check for interruptions during press
 *    - Verify timing constants in code
 *    - Button may be sticking/unreliable
 * 
 * ❌ Double-click missed:
 *    - Click faster (within 500ms)
 *    - Ensure clean button releases
 *    - Adjust DOUBLE_CLICK timing if needed
 *    - Check for mechanical button issues
 * 
 * ❌ False triggers:
 *    - Electrical interference on pin D5
 *    - Add hardware debouncing circuit
 *    - Check power supply stability
 *    - Shield button wires if necessary
 * 
 * 🔧 Hardware debouncing circuit:
 *    Button → 10kΩ resistor → +5V
 *    Button → 100nF capacitor → GND
 *    Arduino D5 → Junction point
 * 
 * ============================================================================
 */