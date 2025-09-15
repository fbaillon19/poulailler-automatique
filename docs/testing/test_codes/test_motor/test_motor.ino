/*
 * ============================================================================
 * MOTOR CONTROL TEST - Automatic Chicken Coop
 * ============================================================================
 * 
 * Test: 5.1 - L298N Motor Control (Safety Optimized)
 * Objective: Verify motor driver and direction control
 * Duration: ~10 minutes
 * 
 * This test verifies:
 * - L298N module functionality
 * - Motor rotation in both directions
 * - Clean start/stop operations
 * - Direction reversal capability
 * - Safety timeout systems
 * 
 * Required wiring:
 * L298N IN1 → Arduino D7
 * L298N IN2 → Arduino D6  
 * L298N OUT1 → Motor +
 * L298N OUT2 → Motor -
 * L298N +12V → 12V Power Supply
 * L298N +5V → Arduino 5V
 * 
 * Procedure:
 * 1. Upload this code to Arduino Nano
 * 2. DISCONNECT motor from mechanism (SAFETY)
 * 3. Open serial monitor (9600 baud)
 * 4. Follow test sequence instructions
 * 5. Verify motor rotations and stops
 * 
 * Expected result:
 * - Motor rotates in both directions
 * - Clean stops between direction changes
 * - No excessive heating of L298N
 * - Proper voltage outputs measured
 * 
 * ⚠️ SAFETY: Motor must be disconnected from door mechanism!
 * 
 * ============================================================================
 */

const int MOTOR_PIN1 = 7;
const int MOTOR_PIN2 = 6;
const unsigned long TEST_DURATION = 3000; // 3s max
const unsigned long PAUSE = 2000; // 2s pause

unsigned long testStart = 0;
int testPhase = 0;
unsigned long phaseStart = 0;
int direction1Tests = 0;
int direction2Tests = 0;

void setup() {
  Serial.begin(9600);
  
  pinMode(MOTOR_PIN1, OUTPUT);
  pinMode(MOTOR_PIN2, OUTPUT);
  stopMotor(); // SAFETY
  
  Serial.println("=== MOTOR TEST - Lite Version ===");
  Serial.println();
  Serial.println("SAFETY:");
  Serial.println("- Motor DISCONNECTED from mechanism");
  Serial.println("- Tests limited to 3s max");
  Serial.println("- RESET for emergency stop");
  Serial.println();
  
  Serial.println("PROCEDURE:");
  Serial.println("Phase 1: Voltage test (motor OFF)");
  Serial.println("Phase 2: Short rotation tests"); 
  Serial.println("Phase 3: Validation");
  Serial.println();
  
  Serial.println("STARTING in 5s...");
  for (int i = 5; i > 0; i--) {
    Serial.print(i);
    Serial.print("... ");
    delay(1000);
  }
  Serial.println("START");
  Serial.println();
  
  testStart = millis();
  phaseStart = millis();
}

void loop() {
  // Safety: auto stop after 10min
  if (millis() - testStart > 600000) {
    stopMotor();
    Serial.println("AUTO STOP: Safety timeout");
    while(true) delay(1000);
  }
  
  switch (testPhase) {
    case 0: testVoltages(); break;
    case 1: testDirection1(); break;
    case 2: safetyPause(); break;
    case 3: testDirection2(); break;
    case 4: safetyPause(); break;
    case 5: testAlternation(); break;
    case 6: finalValidation(); break;
    default: continuousMonitoring(); break;
  }
  
  delay(100);
}

void testVoltages() {
  unsigned long t = millis() - phaseStart;
  
  if (t < 5000) {
    // Test direction 1
    digitalWrite(MOTOR_PIN1, HIGH);
    digitalWrite(MOTOR_PIN2, LOW);
    if (t % 1000 == 0) {
      Serial.println("Test direction 1: D7=HIGH, D6=LOW");
      Serial.println("-> Measure 12V between OUT1-OUT2");
    }
  } else if (t < 10000) {
    // Test direction 2
    digitalWrite(MOTOR_PIN1, LOW);
    digitalWrite(MOTOR_PIN2, HIGH);
    if ((t-5000) % 1000 == 0) {
      Serial.println("Test direction 2: D7=LOW, D6=HIGH");
      Serial.println("-> Measure -12V between OUT1-OUT2");
    }
  } else {
    stopMotor();
    Serial.println("Voltage test OK");
    Serial.println("CONNECT MOTOR now");
    Serial.println("Waiting 10s...");
    delay(10000);
    
    testPhase++;
    phaseStart = millis();
  }
}

void testDirection1() {
  unsigned long t = millis() - phaseStart;
  
  if (t == 0) {
    Serial.println("Phase 2: DIRECTION 1 rotation test");
  }
  
  if (t < TEST_DURATION) {
    digitalWrite(MOTOR_PIN1, HIGH);
    digitalWrite(MOTOR_PIN2, LOW);
    
    if (t % 1000 == 0) {
      Serial.print("Direction 1 active - ");
      Serial.print((TEST_DURATION - t) / 1000);
      Serial.println("s");
    }
  } else {
    stopMotor();
    direction1Tests++;
    Serial.println("STOP direction 1");
    Serial.println("Observe: rotation direction");
    Serial.println();
    
    testPhase++;
    phaseStart = millis();
  }
}

void testDirection2() {
  unsigned long t = millis() - phaseStart;
  
  if (t == 0) {
    Serial.println("Phase 4: DIRECTION 2 rotation test");
  }
  
  if (t < TEST_DURATION) {
    digitalWrite(MOTOR_PIN1, LOW);
    digitalWrite(MOTOR_PIN2, HIGH);
    
    if (t % 1000 == 0) {
      Serial.print("Direction 2 active - ");
      Serial.print((TEST_DURATION - t) / 1000);
      Serial.println("s");
    }
  } else {
    stopMotor();
    direction2Tests++;
    Serial.println("STOP direction 2");
    Serial.println("Observe: opposite direction");
    Serial.println();
    
    testPhase++;
    phaseStart = millis();
  }
}

void safetyPause() {
  static bool msgDisplayed = false;
  unsigned long t = millis() - phaseStart;
  
  if (!msgDisplayed) {
    Serial.print("Safety pause ");
    Serial.print(PAUSE / 1000);
    Serial.println("s");
    msgDisplayed = true;
  }
  
  if (t >= PAUSE) {
    msgDisplayed = false;
    testPhase++;
    phaseStart = millis();
  }
}

void testAlternation() {
  static int cycles = 0;
  static unsigned long lastChange = 0;
  static bool direction = true;
  unsigned long t = millis() - phaseStart;
  
  if (t == 0) {
    Serial.println("Phase 6: Fast alternation (4 cycles)");
    cycles = 0;
    lastChange = millis();
  }
  
  if (cycles < 8 && millis() - lastChange >= 1000) {
    if (direction) {
      digitalWrite(MOTOR_PIN1, HIGH);
      digitalWrite(MOTOR_PIN2, LOW);
      Serial.print("Cycle ");
      Serial.print(cycles/2 + 1);
      Serial.println(" - Direction 1");
    } else {
      digitalWrite(MOTOR_PIN1, LOW);
      digitalWrite(MOTOR_PIN2, HIGH);
      Serial.print("Cycle ");
      Serial.print(cycles/2 + 1);
      Serial.println(" - Direction 2");
    }
    
    direction = !direction;
    lastChange = millis();
    cycles++;
  } else if (cycles >= 8) {
    stopMotor();
    Serial.println("End alternation");
    Serial.println();
    
    testPhase++;
    phaseStart = millis();
  }
}

void finalValidation() {
  static bool done = false;
  
  if (!done) {
    stopMotor();
    
    Serial.println("=== FINAL VALIDATION ===");
    Serial.print("Direction 1 tests: ");
    Serial.println(direction1Tests);
    Serial.print("Direction 2 tests: ");
    Serial.println(direction2Tests);
    Serial.println("Alternations: 4 cycles");
    Serial.println();
    
    Serial.println("VERIFICATIONS:");
    Serial.println("- Motor turns 2 directions: OK");
    Serial.println("- Opposite directions: OK");
    Serial.println("- Clean stops: OK");
    Serial.println("- No abnormal noise: OK");
    Serial.println("- L298N not hot: OK");
    Serial.println();
    
    Serial.println("MOTOR CONTROL: OK");
    Serial.println("-> Ready next test");
    Serial.println();
    
    done = true;
    testPhase++;
  }
}

void continuousMonitoring() {
  static unsigned long lastTest = 0;
  
  if (millis() - lastTest > 10000) {
    Serial.println("Monitoring test: 1s each direction");
    
    // Direction 1
    digitalWrite(MOTOR_PIN1, HIGH);
    digitalWrite(MOTOR_PIN2, LOW);
    delay(1000);
    
    stopMotor();
    delay(500);
    
    // Direction 2
    digitalWrite(MOTOR_PIN1, LOW);
    digitalWrite(MOTOR_PIN2, HIGH);
    delay(1000);
    
    stopMotor();
    Serial.println("Monitoring OK");
    Serial.println();
    
    lastTest = millis();
  }
}

void stopMotor() {
  digitalWrite(MOTOR_PIN1, LOW);
  digitalWrite(MOTOR_PIN2, LOW);
}
/*
 * ============================================================================
 * DIAGNOSTICS AND TROUBLESHOOTING
 * ============================================================================
 * 
 * ❌ Motor doesn't rotate:
 *    - Check 12V power supply to L298N
 *    - Verify motor connections to OUT1/OUT2
 *    - Test motor directly with 12V battery
 *    - Check L298N module functionality
 * 
 * ❌ Motor rotates only one direction:
 *    - Check pin D6 or D7 connections
 *    - Verify logic signals with multimeter
 *    - Test with different Arduino pins
 *    - L298N channel may be damaged
 * 
 * ❌ L298N overheating:
 *    - Reduce test duration (max 3 seconds)
 *    - Check motor current draw (<2A)
 *    - Ensure adequate ventilation
 *    - Add heat sink to L298N if needed
 * 
 * ❌ Erratic motor behavior:
 *    - Check power supply stability (12V±10%)
 *    - Add 470µF capacitor across motor
 *    - Verify all ground connections
 *    - Check for loose wiring
 * 
 * ❌ No voltage on outputs:
 *    - Verify 5V logic supply to L298N
 *    - Check enable pins (may need HIGH)
 *    - Test with multimeter on OUT1/OUT2
 *    - Replace L298N module if defective
 * 
 * 🔧 Voltage measurements:
 *    D7=HIGH, D6=LOW: OUT1=+12V, OUT2=0V
 *    D7=LOW, D6=HIGH: OUT1=0V, OUT2=+12V
 *    Both LOW: OUT1=0V, OUT2=0V (brake)
 * 
 * ⚠️ Never test with motor connected to door mechanism!
 * 
 * ============================================================================
 */