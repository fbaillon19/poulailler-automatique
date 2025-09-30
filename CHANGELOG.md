# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.1.0] - 2025-01-XX

### Added
- 🔆 Automatic LCD backlight management with configurable timeouts
- 🔆 Night mode for backlight (longer timeout between 10pm-6am)
- 📊 Light sensor hysteresis to avoid oscillations (`LIGHT_HYSTERESIS` constant)
- 📝 Enhanced Serial debug messages with sensor values
- 💾 F() macro on all string literals for RAM optimization

### Fixed
- 🐛 Settings mode timeout now properly resets on each user action
- 🐛 Display freeze when entering timeout configuration modes
- 🐛 Added mandatory braces in switch/case with local variable declarations
- 🐛 Light sensor oscillations near threshold value

### Changed
- ⚡ Backlight turns off after 30s inactivity (60s at night)
- ⚡ Settings timeout now uses `lastActivity` instead of `settingsModeStart`
- 📊 Light closing logic now uses hysteresis thresholds

## [1.0.0] - 2025-01-XX

### Added
- ✅ Automatic opening at 7:00 AM
- ✅ Light-based closing with 10-minute anti-cloud delay
- ✅ Forced closing at 11:00 PM
- ✅ Complete LCD 16x2 interface
- ✅ Multi-function button management
- ✅ Anti-jam system with configurable timeouts
- ✅ EEPROM parameter storage
- ✅ Power outage alert LED
- ✅ Complete bilingual documentation (FR/EN)
- ✅ Wiring diagrams and component lists
- ✅ Mechanical assembly guide

[1.1.0]: https://github.com/fbaillon19/poulailler-automatique/compare/v1.0.0...v1.1.0
[1.0.0]: https://github.com/fbaillon19/poulailler-automatique/releases/tag/v1.0.0