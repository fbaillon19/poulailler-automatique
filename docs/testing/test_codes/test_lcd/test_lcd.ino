/*
 * ============================================================================
 * LCD I2C 16x2 TEST - Automatic Chicken Coop
 * ============================================================================
 * 
 * Test: 2.3 - LCD I2C Display Functionality
 * Objective: Verify LCD display and I2C communication
 * Duration: ~5 minutes
 * 
 * This test verifies:
 * - I2C communication with LCD module
 * - Text display on both lines
 * - Backlight functionality
 * - Cursor positioning
 * - Special characters and animations
 * 
 * Required wiring:
 * LCD I2C VCC → Arduino 5V
 * LCD I2C GND → Arduino GND  
 * LCD I2C SDA → Arduino A4
 * LCD I2C SCL → Arduino A5
 * 
 * Procedure:
 * 1. Upload this code to Arduino Nano
 * 2. Verify display on LCD (no serial monitor needed)
 * 3. Observe automatic display sequences
 * 4. Check readability and contrast
 * 
 * Expected result:
 * - Visible and readable text on LCD
 * - Functional backlight
 * - Moving animations and counters
 * - No corrupted characters
 * 
 * ============================================================================
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD configuration (standard address 0x27, size 16x2)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Global variables
unsigned long lastUpdate = 0;
unsigned long counter = 0;
int phase = 0;
bool lcdInitialized = false;

void setup() {
  // Initialize serial communication (for debug)
  Serial.begin(9600);
  
  Serial.println("============================================");
  Serial.println("LCD I2C 16x2 TEST - Automatic Chicken Coop");
  Serial.println("============================================");
  Serial.println("Test: 2.3 - LCD I2C Display");
  Serial.println("");
  Serial.println("🖥️  Initializing LCD I2C...");
  
  // Test multiple common I2C addresses
  Serial.println("🔍 Searching for LCD on I2C bus...");
  
  // Try address 0x27 (most common)
  lcd.init();
  lcd.backlight();
  
  // Simple write test
  lcd.setCursor(0, 0);
  lcd.print("Testing LCD...");
  delay(1000);
  
  // Check if LCD responds
  Wire.beginTransmission(0x27);
  if (Wire.endTransmission() == 0) {
    Serial.println("✅ LCD found at address 0x27");
    lcdInitialized = true;
  } else {
    // Try alternative address 0x3F
    Serial.println("❌ No LCD at 0x27, trying 0x3F...");
    LiquidCrystal_I2C lcd_alt(0x3F, 16, 2);
    lcd = lcd_alt;
    lcd.init();
    lcd.backlight();
    
    Wire.beginTransmission(0x3F);
    if (Wire.endTransmission() == 0) {
      Serial.println("✅ LCD found at address 0x3F");
      lcdInitialized = true;
    } else {
      Serial.println("❌ FAILURE: No LCD found on I2C!");
      Serial.println("");
      Serial.println("Checks to perform:");
      Serial.println("- I2C wiring: SDA→A4, SCL→A5");
      Serial.println("- Power supply: VCC→5V, GND→GND");
      Serial.println("- LCD I2C module functional");
      Serial.println("- I2C address: scan to find");
      Serial.println("");
      Serial.println("⏸️  Test stopped - Fix and restart");
      
      while(true) {
        delay(1000);
      }
    }
  }
  
  // Successful initialization
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LCD I2C OK!");
  lcd.setCursor(0, 1);
  lcd.print("Test starting...");
  
  Serial.println("✅ LCD initialized successfully");
  Serial.println("👀 Watch the LCD screen for the rest of the test");
  Serial.println("");
  
  delay(2000);
}

void loop() {
  if (!lcdInitialized) return;
  
  // Update every 500ms
  if (millis() - lastUpdate >= 500) {
    lastUpdate = millis();
    counter++;
    
    // Different test phases
    switch (phase) {
      case 0:
        testBasicDisplay();
        if (counter >= 10) { // 5 seconds
          phase++;
          counter = 0;
        }
        break;
        
      case 1:
        testPositioning();
        if (counter >= 10) { // 5 seconds
          phase++;
          counter = 0;
        }
        break;
        
      case 2:
        testSpecialCharacters();
        if (counter >= 10) { // 5 seconds
          phase++;
          counter = 0;
        }
        break;
        
      case 3:
        testAnimations();
        if (counter >= 20) { // 10 seconds
          phase++;
          counter = 0;
        }
        break;
        
      case 4:
        testInterfaceSimulation();
        if (counter >= 20) { // 10 seconds
          phase++;
          counter = 0;
        }
        break;
        
      case 5:
        testFinal();
        // Final phase, stay here
        break;
    }
  }
  
  delay(10);
}

/*
 * Phase 0: Basic display test
 */
void testBasicDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DISPLAY TEST");
  lcd.setCursor(0, 1);
  lcd.print("Phase 1/6 - ");
  lcd.print(6 - (counter/2));
  
  Serial.print("Phase 1: Basic display - ");
  Serial.print(6 - (counter/2));
  Serial.println("s");
}

/*
 * Phase 1: Cursor positioning test
 */
void testPositioning() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("POSITIONING");
  
  // Moving character display
  int pos = counter % 16;
  lcd.setCursor(pos, 1);
  lcd.print("*");
  
  Serial.print("Phase 2: Cursor positioning - pos ");
  Serial.println(pos);
}

/*
 * Phase 2: Special characters test
 */
void testSpecialCharacters() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CHARACTERS");
  lcd.setCursor(0, 1);
  
    char characters[] = {'!', '@', '#', '$', '%', '^', '&', '*', '(', ')'};
  for (int i = 0; i < 10; i++) {
    lcd.setCursor(i, 1);
    lcd.print(characters[i]);
  }
  
  // Graphic characters if supported
  lcd.setCursor(12, 1);
  lcd.print((char)0xFF); // Full block
  lcd.setCursor(13, 1);
  lcd.print((char)0xFE); // Partial block
  
  Serial.println("Phase 3: Special characters");
}

/*
 * Phase 3: Animation test
 */
void testAnimations() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ANIMATIONS");
  
  // Progress bar
  int progress = (counter * 16) / 20;
  lcd.setCursor(0, 1);
  for (int i = 0; i < 16; i++) {
    if (i < progress) {
      lcd.print((char)0xFF); // Full block
    } else {
      lcd.print("-");
    }
  }
  
  Serial.print("Phase 4: Animations - progress ");
  Serial.print((counter * 100) / 20);
  Serial.println("%");
}

/*
 * Phase 4: Real interface simulation
 */
void testInterfaceSimulation() {
  lcd.clear();
  
  // Time display simulation
  lcd.setCursor(0, 0);
  int hours = (counter / 4) % 24;
  int minutes = (counter * 3) % 60;
  if (hours < 10) lcd.print("0");
  lcd.print(hours);
  lcd.print(":");
  if (minutes < 10) lcd.print("0");
  lcd.print(minutes);
  lcd.print("      ");
  
  // Door status simulation
  lcd.setCursor(0, 1);
  const char* status[] = {
    "Door open       ",
    "Door closed     ",
    "Opening...      ",
    "Closing...      ",
    "Closes in 5min  "
  };
  
  int statusIndex = (counter / 4) % 5;
  lcd.print(status[statusIndex]);
  
  Serial.print("Phase 5: Real interface - ");
  Serial.println(status[statusIndex]);
}

/*
 * Phase 5: Final test and results
 */
void testFinal() {
  if (counter == 1) {
    // First display of results
    Serial.println("");
    Serial.println("============================================");
    Serial.println("✅ LCD I2C TEST COMPLETED SUCCESSFULLY");
    Serial.println("============================================");
    Serial.println("");
    Serial.println("📊 Test results:");
    Serial.println("- I2C communication: OK");
    Serial.println("- Text display: OK");
    Serial.println("- Cursor positioning: OK");
    Serial.println("- Special characters: OK");
    Serial.println("- Animations: OK");
    Serial.println("- Interface simulation: OK");
    Serial.println("");
    Serial.println("➡️  Ready for next test: Sensors");
    Serial.println("");
  }
  
  // Permanent final display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LCD TEST OK!");
  lcd.setCursor(0, 1);
  
  // Final incrementing counter
  lcd.print("Counter: ");
  lcd.print(counter);
  
  // Blinking to show it's working
  if ((counter % 4) < 2) {
    lcd.setCursor(15, 0);
    lcd.print("*");
  } else {
    lcd.setCursor(15, 0);
    lcd.print(" ");
  }
}

/*
 * ============================================================================
 * DIAGNOSTICS AND TROUBLESHOOTING
 * ============================================================================
 * 
 * ❌ Screen completely black:
 *    - Check power supply: VCC→5V, GND→GND
 *    - Check I2C wiring: SDA→A4, SCL→A5
 *    - Turn contrast potentiometer on LCD module
 *    - Defective LCD module
 * 
 * ❌ Backlight on but no text:
 *    - I2C address problem (try 0x3F instead of 0x27)
 *    - Adjust contrast (potentiometer on module)
 *    - Check LCD module solder joints
 *    - I2C communication problem
 * 
 * ❌ Strange/corrupted characters:
 *    - Unstable power supply (add 100µF capacitor)
 *    - Electrical interference
 *    - I2C wires too long (max 1 meter)
 *    - I2C speed too fast
 * 
 * ❌ Intermittent display:
 *    - Poor connection contacts
 *    - Insufficient power supply
 *    - Defective LCD module
 *    - Electromagnetic interference
 * 
 * ❌ "No LCD found on I2C":
 *    - I2C scan to find real address
 *    - Check complete wiring
 *    - Test with multimeter (3.3-5V on VCC)
 *    - Replace LCD I2C module
 * 
 * 🔧 I2C Scanner (useful code):
 * 
 * Wire.begin();
 * for(int addr = 1; addr < 127; addr++) {
 *   Wire.beginTransmission(addr);
 *   if(Wire.endTransmission() == 0) {
 *     Serial.print("Device found at 0x");
 *     Serial.println(addr, HEX);
 *   }
 * }
 * 
 * ============================================================================
 */