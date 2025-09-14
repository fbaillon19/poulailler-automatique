# 💻 Software Testing Guide - Automatic Chicken Coop

## 🎯 Testing Objectives

These tests validate the **software behavior** of the complete system after hardware validation. They cover the user interface, automatic logic, and integration of all components.

> ⚠️ **Prerequisites**: All hardware tests (1-5) must be validated before proceeding.

## 🖥️ Test 6 - Complete User Interface

### 🎯 Objective
Validate LCD + button interface functionality and all setting modes.

### 🛠️ Required Materials
- Complete assembled system
- All components connected according to wiring diagram

### 📋 Procedure

#### 6.1 Normal Display Test
```
STEPS:
1. Upload main code poulailler_automatique.ino
2. Wait for complete initialization
3. Observe LCD display

EXPECTED RESULTS:
- Line 1: Time in HH:MM format
- Line 2: Door state ("Door open" or "Door closed")
- Smooth refresh every second

VALIDATION:
☐ Correct time display
☐ Door state consistent with real position
☐ No unwanted blinking
☐ Well-formed characters
```

#### 6.2 Button Test - Short Presses
```
STEPS:
1. Perform repeated short presses (<1s)
2. Observe door state changes
3. Verify display/movement consistency

EXPECTED RESULTS:
- Short press → Open command if closed
- Short press → Close command if open
- LCD displays "Opening..." or "Closing..."
- Motor starts in correct direction

VALIDATION:
☐ Reliable short press detection
☐ Open/close alternation
☐ Display consistent with action
☐ No double triggering
```

#### 6.3 Button Test - Long Press (Settings Mode)
```
STEPS:
1. Hold button for 3+ seconds
2. Release and observe settings mode entry
3. Navigate through all modes with long presses

MODE SEQUENCE:
Normal → Hour Setting → Minute Setting → Threshold Setting → Opening Timeout → Closing Timeout → Normal

VALIDATION FOR EACH MODE:
☐ Mode entry by long press
☐ Blinking of modifiable value
☐ Modification by short press (+1)
☐ Modification by double-click (-1) for threshold and timeouts
☐ Automatic timeout after 10s inactivity
```

#### 6.4 Detailed Settings Test

**Hour Setting Mode:**
```
STEPS:
1. Enter hour setting mode
2. Short presses to increment
3. Verify 23h → 0h transition
4. Exit by timeout or long press

VALIDATION:
☐ Hour blinking only
☐ Correct incrementation
☐ Midnight transition handling
☐ Immediate save to RTC
```

**Light Threshold Setting Mode:**
```
STEPS:
1. Enter threshold setting mode
2. Note current sensor value
3. Test short presses (+5) and double-clicks (-5)
4. Verify 0-1023 limits

VALIDATION:
☐ Display threshold + current value
☐ Modification in steps of 5
☐ Limits respected
☐ Immediate EEPROM save
```

**Timeout Settings Mode:**
```
STEPS:
1. Test opening timeout (default 15s)
2. Test closing timeout (default 30s)
3. Verify 5-60 second limits
4. Validate saving

VALIDATION:
☐ Current timeout display
☐ ±1 second modification
☐ Limits respected
☐ EEPROM save
```

### ✅ Test 6 Validation Criteria
- [ ] Stable and readable LCD display
- [ ] Reliable button detection (short/long/double-click)
- [ ] Complete navigation through all modes
- [ ] Persistent parameter saving
- [ ] Functional settings exit timeout

---

## 🤖 Test 7 - Automatic Logic and Integration

### 🎯 Objective
Validate complete automatic system behavior in different scenarios.

### 🛠️ Required Materials
- Complete operational system
- Ability to simulate light conditions (lamp, shading)
- Stopwatch for timeout measurements

### 📋 Procedure

#### 7.1 Morning Automatic Opening Test
```
PREPARATION:
1. Set RTC to 06:58
2. Ensure door is closed
3. Monitor LCD and motor

SEQUENCE:
06:58 → 06:59 → 07:00 → 07:01

EXPECTED RESULTS:
- 06:59: Nothing, door stays closed
- 07:00 exactly: Automatic opening starts
- LCD displays "Opening..."
- Motor runs until TOP limit switch

VALIDATION:
☐ Precise 7:00 AM trigger
☐ Complete automatic opening
☐ Stop at top limit switch
☐ Final LCD "Door open"
☐ No re-trigger at 7:01
```

#### 7.2 Light-Triggered Closing with Delay Test
```
PREPARATION:
1. Door open, time > 7:00 AM
2. Ambient light > configured threshold
3. Prepare sensor shading

SEQUENCE:
1. Shade sensor (value < threshold)
2. Observe 10-minute delay start
3. Wait a few minutes
4. Re-illuminate sensor
5. Shade again and wait for delay completion

EXPECTED RESULTS:
- Shading → LCD "Closes in 10min"
- Minute-by-minute countdown
- Re-illumination → Cancellation "Door open"
- Final shading → Closing after 10 minutes

VALIDATION:
☐ Light change detection
☐ 10-min delay start
☐ Correct countdown display
☐ Cancellation if light returns
☐ Automatic closing after delay
```

#### 7.3 Forced Night Closing Test
```
PREPARATION:
1. Set RTC to 22:58
2. Door open, high luminosity
3. Monitor 23:00 transition

SEQUENCE:
22:58 → 22:59 → 23:00 → 23:01

EXPECTED RESULTS:
- 22:59: Door stays open despite lighting
- 23:00: Immediate forced closing
- Absolute priority over light sensor

VALIDATION:
☐ Precise 23:00 closing
☐ Priority over luminosity
☐ Complete closing
☐ No re-triggering
```

#### 7.4 Anti-Jam System Test
```
PREPARATION:
1. Prepare physical obstacle on path
2. Configure short timeouts for testing (5-10s)
3. Trigger door movement

OPENING TEST:
1. Place obstacle before top limit switch
2. Trigger manual opening
3. Let motor run until timeout

CLOSING TEST:
1. Place obstacle before bottom limit switch
2. Trigger manual closing
3. Observe timeout

EXPECTED RESULTS:
- Motor stops after configured timeout
- LCD displays "OBSTACLE ERROR"
- Short press allows retry
- System remains stable

VALIDATION:
☐ Opening timeout detection
☐ Closing timeout detection
☐ Correct error display
☐ Manual retry possibility
☐ No motor overheating
```

#### 7.5 Power Outage Management Test (RTC)
```
PREPARATION:
1. Functional system with correct time
2. Disconnect power for 5+ minutes
3. Reconnect and observe

RTC LOSS SIMULATION:
// Temporary code to force detection
if (rtc.lostPower()) {
  coupureCourant = true;
}

EXPECTED RESULTS:
- Red LED blinks after restart
- System continues operating
- Time may be lost (depending on outage duration)

VALIDATION:
☐ Blinking LED activated
☐ System remains operational
☐ Ability to reconfigure time
☐ No loss of other EEPROM parameters
```

#### 7.6 EEPROM Parameter Persistence Test
```
PREPARATION:
1. Modify all parameters (threshold, timeouts)
2. Note configured values
3. Disconnect/reconnect power

VERIFICATION:
1. Enter settings mode after restart
2. Verify all parameters are preserved

VALUES TO CHECK:
- Light threshold: _____ (configured) = _____ (read)
- Opening timeout: _____ = _____
- Closing timeout: _____ = _____

VALIDATION:
☐ Light threshold preserved
☐ Opening timeout preserved
☐ Closing timeout preserved
☐ RTC time preserved (if battery OK)
```

#### 7.7 Limit Switch Integration Test
```
PREPARATION:
1. Door in intermediate position
2. Check initial sensor states

COMPLETE MOVEMENT TEST:
1. Trigger manual opening
2. Verify precise stop at TOP limit switch
3. Trigger manual closing
4. Verify precise stop at BOTTOM limit switch

ELECTRICAL VALIDATION:
- TOP limit switch at rest: ☐ HIGH ☐ LOW
- TOP limit switch activated: ☐ HIGH ☐ LOW
- BOTTOM limit switch at rest: ☐ HIGH ☐ LOW
- BOTTOM limit switch activated: ☐ HIGH ☐ LOW

VALIDATION:
☐ Precise stop at top limit switch
☐ Precise stop at bottom limit switch
☐ No overshoot
☐ Reliable position detection
```

### ✅ Test 7 Validation Criteria
- [ ] Precise 7:00 AM automatic opening
- [ ] Functional light delay closing
- [ ] Priority 11:00 PM forced closing
- [ ] Effective anti-jam system
- [ ] Correct power outage management
- [ ] EEPROM parameter persistence
- [ ] Perfect limit switch integration

---

## 📊 Final System Validation

### 🏁 Endurance Test (24h)
```
OBJECTIVE: Validate stability over a complete day

PROTOCOL:
1. Let system run for 24h
2. Simulate several day/night cycles
3. Perform some manual interventions
4. Monitor stability and accuracy

MONITORING:
- Number of complete cycles: _____
- Detected errors: _____
- RTC drift: _____ seconds
- Abnormal behavior: _____

VALIDATION:
☐ No system errors
☐ Regular automatic cycles
☐ Stable user interface
☐ Acceptable time accuracy
```

### 📋 Final Checklist

| Functionality | Status | Notes |
|---------------|--------|-------|
| LCD Interface | ☐ OK ☐ FAIL | |
| Multi-function Button | ☐ OK ☐ FAIL | |
| Complete Settings | ☐ OK ☐ FAIL | |
| 7:00 AM Opening | ☐ OK ☐ FAIL | |
| Light Closing | ☐ OK ☐ FAIL | |
| 11:00 PM Closing | ☐ OK ☐ FAIL | |
| Anti-jam System | ☐ OK ☐ FAIL | |
| Limit Switches | ☐ OK ☐ FAIL | |
| EEPROM Backup | ☐ OK ☐ FAIL | |
| Power Outage Management | ☐ OK ☐ FAIL | |
| 24h Stability | ☐ OK ☐ FAIL | |

### 🔧 Corrective Actions if Failed

**Test 6 failed**:
- LCD Interface: Check I2C address, contrast, power supply
- Button: Check debouncing, pull-up resistor, solder joints
- Settings: Debug code with Serial.print(), check EEPROM

**Test 7 failed**:
- Automatic logic: Check RTC accuracy, configured thresholds
- Light sensor: Clean, recalibrate threshold, check divider resistor
- Limit switches: Test continuity, adjust mechanical position

### 💾 Test Parameter Backup

```cpp
// Useful code for parameter dump after tests
void dumpParameters() {
  Serial.println("=== SYSTEM PARAMETERS ===");
  Serial.print("Light threshold: "); Serial.println(seuilLumiere);
  Serial.print("Opening timeout: "); Serial.println(timeoutOuverture);
  Serial.print("Closing timeout: "); Serial.println(timeoutFermeture);
  
  DateTime now = rtc.now();
  Serial.print("RTC time: ");
  Serial.print(now.hour()); Serial.print(":"); Serial.println(now.minute());
  
  Serial.print("Current light: "); Serial.println(analogRead(CAPTEUR_LUMIERE));
  Serial.print("Limit switches - Top: "); Serial.print(digitalRead(FIN_COURSE_HAUT));
  Serial.print(" / Bottom: "); Serial.println(digitalRead(FIN_COURSE_BAS));
}
```

### 📈 Recommended Post-Test Optimizations

**If tests successful but improvements possible:**

1. **Delay adjustment**: If 10 minutes too long/short for your usage
2. **Light calibration**: Fine-tune according to actual sensor exposure
3. **Motor timeouts**: Adapt to final mechanics (height, load)
4. **Schedule**: Modify 7h/23h according to season and latitude

**Code for seasonal adjustments**:
```cpp
// Example schedule adaptation (integrate if desired)
int getOpeningHour(int month) {
  // Later opening in winter
  if (month >= 11 || month <= 2) return 8;  // Nov-Feb: 8 AM
  if (month >= 3 && month <= 4) return 7;   // Mar-Apr: 7 AM
  if (month >= 5 && month <= 8) return 6;   // May-Aug: 6 AM
  return 7; // Sep-Oct: 7 AM
}
```

---

## 📋 Final Validation Report

### ✅ System Validated for Production

**Validation Conditions:**
- [ ] All hardware tests (1-5) successful
- [ ] All software tests (6-7) successful
- [ ] 24h endurance test without errors
- [ ] Complete documentation available
- [ ] Parameters optimized for installation

**Validation Signature:** _________________ **Date:** _________

### 🎯 Recommended Commissioning

1. **Final mechanical installation** according to assembly guide
2. **Weather protection** of all outdoor components
3. **User training** on interface and basic troubleshooting
4. **Initial monitoring** 1 week with daily checks
5. **Preventive maintenance** scheduled per maintenance guide

---

*Software Testing Guide - Version 1.0 - Automatic Chicken Coop Project*