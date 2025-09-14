# 🔧 Mechanical Assembly Guide - Automatic Chicken Coop

## 🎯 Overview

This guide walks you through building the mechanical part of your automatic chicken coop door, from design to final installation.

## 📐 Door Design

### 🚪 Recommended Dimensions
- **Width**: 25-35 cm (according to your chickens' size)
- **Height**: 30-40 cm 
- **Thickness**: 10-15 mm (weight/rigidity compromise)
- **Total weight**: < 2 kg (for standard motor)

### 🛠️ Door Materials
**Option 1: Expanded PVC** ✅ Recommended
- Lightweight, rot-proof, easy to cut
- 10mm thickness, easy to drill

**Option 2: Marine Plywood**
- Heavier but very resistant
- Requires moisture treatment

**Option 3: Aluminum Composite**
- Very lightweight, modern appearance
- More expensive but durable

### 🎨 Door Cutting
```
Required Tools:
- Jigsaw or circular saw
- Drill with wood/metal bits
- File or sander for finishing
- Ruler, square, pencil
```

## 🛤️ Guidance System

### Option A: U-Channel Rails (Recommended)
```
Materials:
- 2x Aluminum U-profiles (door height + 20cm)
- Stainless steel M4 screws for mounting
- Plastic or Teflon spacers

Advantages:
✅ Precise guidance
✅ Low friction  
✅ Weather resistant
```

### Option B: Wood Corner Brackets
```
Materials:
- Treated wood strips 20x20mm
- Stainless wood screws
- Wax or graphite (lubricant)

Advantages:
✅ Economical
✅ Easy to machine
❌ Maintenance required
```

## ⚙️ Motion Transmission

### 🔄 Option 1: Pulley-Cable System
```
Components:
- 1x Pulley Ø60-80mm (mounted on motor shaft)
- Steel cable Ø2mm (textile core)
- 2x Return pulleys Ø40mm
- Cable tensioner
- Cable clamps

Assembly:
1. Mount drive pulley on gearbox
2. Install return pulleys at top
3. Thread cable: motor → return → door → return → counterweight
4. Adjust tension with tensioner
```

### 🦷 Option 2: Rack & Pinion  
```
Components:
- Toothed rack (length = door travel)
- Motor pinion matching pitch
- Rack support

Advantages:
✅ High precision
✅ No slipping
❌ More complex to align
```

## 🏋️ Counterweight (Recommended Option)

```
Weight Calculation:
Counterweight = 70% of door weight

Example:
- 1.5 kg door → 1 kg counterweight
- Reduces motor effort
- Safety in case of failure (door stays open)

Construction:
- Waterproof bag + sand/gravel
- Or closed PVC tube + sand
- Carabiner attachment
```

## 📍 Sensor Positioning

### 🔝 TOP Limit Switch
```
Position: Door fully open + 1cm margin
Mounting: Adjustable bracket on frame
Protection: IP54 minimum waterproof housing

Adjustment:
1. Manually open door fully
2. Position sensor for triggering
3. Test triggering multiple times
```

### 🔻 BOTTOM Limit Switch
```
Position: Door closed at ground level
Protection: Enhanced sealing (ground contact)

Tip:
- Bury sensor slightly
- Or use magnetic proximity sensor
```

### 💡 Light Sensor
```
Ideal Location:
- North facing (avoid direct sun)
- Sheltered from rain
- Clear view of sky
- Height > 1.5m (out of animals' reach)

Protection:
- Small awning or transparent cover
- Regular cleaning
```

## 🏠 Coop Installation

### 🔧 System Mounting
```
Mounting Points:
- Main frame: 4 screws Ø8mm minimum
- Guide rails: screws every 30cm
- Motor: anti-vibration mounting

Mounting Materials:
- Stainless screws (anti-corrosion)
- Appropriate anchors for support
- Distribution washers
```

### ⚡ Cable Routing
```
Required Protection:
- Waterproof sheaths for outdoor cables
- Cable glands on electronic enclosure
- UV-resistant flexible cables

Routing:
- Avoid animal traffic areas
- Secure every 50cm maximum
- Leave slack at articulations
```

## 🔍 Testing and Adjustments

### ✅ Pre-commissioning Checklist

**Mechanical:**
- [ ] Door slides freely by hand
- [ ] No hard spots on travel
- [ ] Limit switches trigger
- [ ] All fixings tightened

**Electrical:**
- [ ] Stable 12V power supply
- [ ] All grounds connected
- [ ] Outdoor connections insulated
- [ ] Sensor testing (multimeter)

**Functional:**
- [ ] Motor turns both directions
- [ ] Stops on limit switches
- [ ] Light threshold adjustment
- [ ] Timeout testing

### 🎛️ Fine Adjustments

**Motor Speed:**
```
- Too fast: jamming risk
- Too slow: possible timeout
- Optimal: 10-15 cm/second
```

**Safety Timeouts:**
```
- Opening: 15-20s (according to height)
- Closing: 20-30s (slower if counterweight)
```

## 🧰 Preventive Maintenance

### 🗓️ Monthly
- Clean guide rails
- Check mounting tightness
- Complete open/close test

### 🗓️ Semi-annually  
- Lubricate mechanisms (silicone grease)
- Check cable condition
- Clean light sensor
- Verify electronic enclosure sealing

### 🗓️ Annually
- Complete mounting revision
- Preventive cable replacement if wear
- Test all operating modes

## 🚨 Mechanical Troubleshooting

| Problem | Probable Cause | Solution |
|---------|----------------|----------|
| **Door jams** | Dirty/deformed rails | Cleaning, realignment |
| **Abnormal noise** | Lack of lubrication | Shaft greasing |
| **Random stops** | Poorly adjusted limit switch | Sensor repositioning |
| **Jerky movement** | Irregular cable tension | Tensioner adjustment |
| **Door falls** | Cable break | Emergency replacement |

## 💡 Possible Improvements

### 🔋 Backup Power
```
- 12V 7Ah battery + charger
- Autonomy: ~50 cycles
- Automatic switching
```

### 🌡️ Winter Protection
```
- Light sensor deicing  
- Special low temperature oil
- Electronic enclosure insulation
```

### 📱 Connectivity
```
- WiFi/GSM module for monitoring
- SMS notification on problems
- Remote control
```

---

## ⚠️ Important Safety

- **Always disconnect power** before intervention
- **Provide manual mode** in case of failure
- **Regularly test** obstacle detection
- **Train all users** on emergency procedures

---

*Guide created by the maker community - Version 1.0*