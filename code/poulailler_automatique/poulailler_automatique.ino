/*
 * ============================================================================
 * POULAILLER AUTOMATIQUE / AUTOMATIC CHICKEN COOP DOOR
 * ============================================================================
 * 
 * Description FR: Système automatisé d'ouverture/fermeture de porte de 
 *                 poulailler basé sur Arduino Nano avec gestion de l'heure 
 *                 et de la luminosité
 * 
 * Description EN: Automated chicken coop door system based on Arduino Nano 
 *                 with time and light level management
 * 
 * Auteur / Author: Frédéric BAILLON
 * Version: 1.0.0
 * Date: 2025
 * Licence / License: MIT
 * 
 * Repo GitHub: https://github.com/fbaillon19/poulailler-automatique
 * 
 * ============================================================================
 * FONCTIONNALITÉS / FEATURES:
 * ============================================================================
 * 
 * FR:
 * - Ouverture automatique à 7h00
 * - Fermeture par détection luminosité (temporisation 10min anti-nuages)
 * - Fermeture forcée à 23h00
 * - Interface LCD 16x2 + bouton multifonction
 * - Système anti-obstacle avec timeout configurable
 * - Sauvegarde EEPROM des paramètres
 * - LED d'alerte coupure courant
 * 
 * EN:
 * - Automatic opening at 7:00 AM
 * - Light-triggered closing (10min delay anti-cloud)
 * - Forced closing at 11:00 PM  
 * - 16x2 LCD interface + multi-function button
 * - Anti-jam system with configurable timeout
 * - EEPROM parameter backup
 * - Power outage alert LED
 * 
 * ============================================================================
 * MATÉRIEL REQUIS / REQUIRED HARDWARE:
 * ============================================================================
 * 
 * - Arduino Nano (ATmega328P)
 * - Module RTC DS3231 (I2C)
 * - Écran LCD 16x2 I2C / 16x2 LCD I2C Display
 * - Module contrôleur moteur L298N / L298N Motor Driver
 * - Moteur CC 12V / 12V DC Motor
 * - Capteur de luminosité (LDR) / Light sensor (LDR)
 * - 2x Capteurs fin de course / 2x Limit switches
 * - 1x Bouton poussoir / 1x Push button
 * - 1x LED rouge / 1x Red LED
 * - Résistances / Resistors: 220Ω, 10kΩ
 * - Alimentation 12V 2A / 12V 2A Power supply
 * 
 * ============================================================================
 * CÂBLAGE / WIRING:
 * ============================================================================
 * 
 * Arduino Pin  →  Composant / Component
 * ────────────────────────────────────────
 * A0           →  Capteur luminosité / Light sensor
 * A4 (SDA)     →  RTC DS3231 + LCD I2C
 * A5 (SCL)     →  RTC DS3231 + LCD I2C  
 * D3           →  LED coupure courant / Power outage LED
 * D5           →  Bouton multifonction / Multi-function button
 * D6           →  L298N IN2
 * D7           →  L298N IN1
 * D8           →  Fin de course HAUT / TOP limit switch
 * D9           →  Fin de course BAS / BOTTOM limit switch
 * 5V           →  Alimentation modules / Module power
 * GND          →  Masse commune / Common ground
 * 
 * ============================================================================
 */

#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

// ============================================================================
// CONFIGURATION DES BROCHES / PIN CONFIGURATION
// ============================================================================
const int MOTOR_PIN1 = 7;              // Contrôle moteur direction 1 / Motor control direction 1
const int MOTOR_PIN2 = 6;              // Contrôle moteur direction 2 / Motor control direction 2
const int LIGHT_SENSOR = A0;           // Capteur de luminosité / Light sensor
const int BUTTON_PIN = 5;              // Bouton multifonction / Multi-function button
const int TOP_LIMIT_SWITCH = 8;        // Capteur fin de course haut / TOP limit switch
const int BOTTOM_LIMIT_SWITCH = 9;     // Capteur fin de course bas / BOTTOM limit switch
const int POWER_OUTAGE_LED = 3;        // LED clignotante coupure courant / Power outage blinking LED

// ============================================================================
// CONSTANTES / CONSTANTS
// ============================================================================
const unsigned long CLOSING_DELAY = 600000;        // 10 minutes en millisecondes / 10 minutes in milliseconds
const unsigned long LONG_PRESS_TIME = 3000;        // 3 secondes pour appui long / 3 seconds for long press
const unsigned long SETTINGS_TIMEOUT = 10000;      // 10 secondes timeout mode réglage / 10 seconds settings timeout
const unsigned long DOUBLE_CLICK_TIME = 500;       // 500ms pour détecter double-clic / 500ms to detect double-click

// Adresses EEPROM / EEPROM addresses
const int THRESHOLD_EEPROM_ADDR = 0;               // Adresse EEPROM seuil / EEPROM address for threshold
const int OPENING_TIMEOUT_ADDR = 2;                // Adresse EEPROM timeout ouverture / EEPROM address opening timeout
const int CLOSING_TIMEOUT_ADDR = 4;                // Adresse EEPROM timeout fermeture / EEPROM address closing timeout

// Valeurs par défaut / Default values
const int DEFAULT_THRESHOLD = 300;                 // Valeur par défaut seuil / Default threshold value
const int DEFAULT_OPENING_TIMEOUT = 15;            // 15 secondes par défaut / 15 seconds default
const int DEFAULT_CLOSING_TIMEOUT = 30;            // 30 secondes par défaut / 30 seconds default

// ============================================================================
// VARIABLES GLOBALES / GLOBAL VARIABLES
// ============================================================================
RTC_DS3231 rtc;                                    // Objet RTC / RTC object
LiquidCrystal_I2C lcd(0x27, 16, 2);               // Écran LCD I2C / I2C LCD display
bool doorOpen = false;                             // État de la porte / Door state
unsigned long lastLEDTime = 0;                    // Dernier changement LED / Last LED change
bool ledState = false;                             // État LED / LED state
bool powerOutage = false;                          // Détection coupure courant / Power outage detection
unsigned long lastTimeCheck = 0;                  // Dernière vérification temporelle / Last time check
int lightThreshold = DEFAULT_THRESHOLD;           // Seuil de luminosité / Light threshold
int openingTimeout = DEFAULT_OPENING_TIMEOUT;     // Timeout ouverture en secondes / Opening timeout in seconds
int closingTimeout = DEFAULT_CLOSING_TIMEOUT;     // Timeout fermeture en secondes / Closing timeout in seconds

// Variables pour la temporisation de fermeture / Variables for closing delay
unsigned long lowLightStartTime = 0;              // Début détection lumière faible / Start of low light detection
bool lowLightDetected = false;                    // Flag lumière faible / Low light flag

// Variables pour la gestion du bouton multifonction / Multi-function button variables
unsigned long pressStartTime = 0;                 // Début d'appui bouton / Button press start
bool buttonPressed = false;                        // État bouton enfoncé / Button pressed state
bool pressProcessed = false;                       // Appui déjà traité / Press already processed
unsigned long lastReleaseTime = 0;                // Dernier relâchement bouton / Last button release
bool waitingDoubleClick = false;                  // Attente double-clic / Waiting for double-click

// Variables pour les modes de réglage / Settings mode variables
enum SettingsMode {
  MODE_NORMAL,
  MODE_SET_HOUR,
  MODE_SET_MINUTE,
  MODE_SET_THRESHOLD,
  MODE_SET_OPENING_TIMEOUT,
  MODE_SET_CLOSING_TIMEOUT
};
SettingsMode currentMode = MODE_NORMAL;           // Mode actuel / Current mode
unsigned long settingsModeStart = 0;              // Début mode réglage / Settings mode start
bool blinking = false;                             // État clignotement / Blinking state
unsigned long lastBlinkTime = 0;                  // Dernier clignotement / Last blink

// États de la porte / Door states
enum DoorState {
  STOPPED,
  OPENING,
  CLOSING,
  OBSTACLE_ERROR
};
DoorState currentDoorState = STOPPED;             // État actuel porte / Current door state

// Variables pour la gestion des timeouts moteur / Motor timeout variables
unsigned long doorMovementStart = 0;              // Début mouvement porte / Door movement start

// ============================================================================
// FONCTION SETUP - INITIALISATION / SETUP FUNCTION - INITIALIZATION
// ============================================================================
/**
 * Fonction d'initialisation du système
 * System initialization function
 * 
 * Paramètres / Parameters: Aucun / None
 * Retour / Return: void
 */
void setup() {
  Serial.begin(9600);
  
  // Configuration des broches / Pin configuration
  pinMode(MOTOR_PIN1, OUTPUT);
  pinMode(MOTOR_PIN2, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(TOP_LIMIT_SWITCH, INPUT_PULLUP);
  pinMode(BOTTOM_LIMIT_SWITCH, INPUT_PULLUP);
  pinMode(POWER_OUTAGE_LED, OUTPUT);
  
  // Arrêt du moteur au démarrage / Stop motor at startup
  stopMotor();
  
  // Initialisation du LCD / LCD initialization
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(F("Poulailler Auto"));
  lcd.setCursor(0, 1);
  lcd.print(F("Initialisation.."));
  delay(2000);
  
  // Initialisation du RTC / RTC initialization
  if (!rtc.begin()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Erreur RTC!"));
    Serial.println(F("RTC non trouvé ! / RTC not found!"));
    while (1);
  }
  
  // Chargement des paramètres depuis l'EEPROM / Load parameters from EEPROM
  loadParametersFromEEPROM();
  
  // Vérification si le RTC a perdu l'alimentation / Check if RTC lost power
  if (rtc.lostPower()) {
    powerOutage = true;
    Serial.println(F("RTC a perdu l'alimentation / RTC lost power"));
    // Décommentez la ligne suivante pour régler l'heure automatiquement
    // Uncomment next line to set time automatically
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  // Déterminer l'état initial de la porte / Determine initial door state
  if (digitalRead(TOP_LIMIT_SWITCH) == LOW) {
    doorOpen = true;
    Serial.println(F("Porte détectée ouverte au démarrage / Door detected open at startup"));
  } else if (digitalRead(BOTTOM_LIMIT_SWITCH) == LOW) {
    doorOpen = false;
    Serial.println(F("Porte détectée fermée au démarrage / Door detected closed at startup"));
  }
  
  Serial.println(F("Système initialisé / System initialized"));
  lcd.clear();
}

// ============================================================================
// BOUCLE PRINCIPALE / MAIN LOOP
// ============================================================================
/**
 * Boucle principale du programme
 * Main program loop
 * 
 * Paramètres / Parameters: Aucun / None
 * Retour / Return: void
 */
void loop() {
  DateTime now = rtc.now();
  
  // Gestion du bouton multifonction / Multi-function button management
  handleMultiFunctionButton();
  
  // Gestion du timeout pour sortir du mode réglage / Settings mode timeout management
  if (currentMode != MODE_NORMAL && millis() - settingsModeStart > SETTINGS_TIMEOUT) {
    currentMode = MODE_NORMAL;
  }
  
  // Gestion du clignotement en mode réglage / Blinking management in settings mode
  if (millis() - lastBlinkTime > 500) {
    lastBlinkTime = millis();
    blinking = !blinking;
  }
  
  // Gestion de l'affichage LCD / LCD display management
  handleLCDDisplay(now);
  
  // Gestion de la LED de coupure / Power outage LED management
  handlePowerOutageLED();
  
  // Logique automatique seulement en mode normal / Automatic logic only in normal mode
  if (currentMode == MODE_NORMAL) {
    // Vérifications automatiques toutes les secondes / Automatic checks every second
    if (millis() - lastTimeCheck > 1000) {
      lastTimeCheck = millis();
      
      int lightValue = analogRead(LIGHT_SENSOR);
      
      // Condition 1: Ouverture automatique à 7h00 / Automatic opening at 7:00 AM
      if (now.hour() == 7 && now.minute() == 0 && !doorOpen) {
        Serial.println(F("7h00 - Ouverture automatique / 7:00 AM - Automatic opening"));
        openDoor();
      }
      
      // Condition 2: Fermeture avec temporisation / Closing with delay
      handleLightClosing(lightValue, now.hour());
      
      // Condition 3: Fermeture forcée à 23h00 / Forced closing at 11:00 PM
      if (now.hour() == 23 && now.minute() == 0 && doorOpen) {
        Serial.println(F("23h00 - Fermeture forcée / 11:00 PM - Forced closing"));
        closeDoor();
      }
    }
    
    // Gestion du mouvement de la porte / Door movement management
    handleDoorMovement();
  }
  
  delay(50);
}

// ============================================================================
// GESTION DU BOUTON MULTIFONCTION / MULTI-FUNCTION BUTTON MANAGEMENT
// ============================================================================
/**
 * Gère les interactions avec le bouton multifonction
 * Manages multi-function button interactions
 * 
 * Paramètres / Parameters: Aucun / None
 * Retour / Return: void
 */
void handleMultiFunctionButton() {
  bool buttonState = digitalRead(BUTTON_PIN);
  
  // Détection du début d'appui / Press start detection
  if (!buttonPressed && buttonState == LOW) {
    delay(20); // Anti-rebond / Debounce
    if (digitalRead(BUTTON_PIN) == LOW) {
      buttonPressed = true;
      pressStartTime = millis();
      pressProcessed = false;
    }
  }
  
  // Détection du relâchement / Release detection
  if (buttonPressed && buttonState == HIGH) {
    delay(20); // Anti-rebond / Debounce
    if (digitalRead(BUTTON_PIN) == HIGH) {
      buttonPressed = false;
      unsigned long pressDuration = millis() - pressStartTime;
      
      if (!pressProcessed) {
        if (pressDuration >= LONG_PRESS_TIME) {
          // Appui long / Long press
          handleLongPress();
        } else {
          // Appui bref / Short press
          if (waitingDoubleClick && millis() - lastReleaseTime < DOUBLE_CLICK_TIME) {
            // Double-clic détecté / Double-click detected
            handleDoubleClick();
            waitingDoubleClick = false;
          } else {
            // Premier appui bref, attendre pour voir s'il y a un double-clic
            // First short press, wait to see if there's a double-click
            lastReleaseTime = millis();
            waitingDoubleClick = true;
          }
        }
      }
    }
  }
  
  // Gestion du timeout pour le double-clic / Double-click timeout management
  if (waitingDoubleClick && millis() - lastReleaseTime > DOUBLE_CLICK_TIME) {
    handleShortPress();
    waitingDoubleClick = false;
  }
}

/**
 * Traite les appuis brefs du bouton
 * Processes short button presses
 * 
 * Paramètres / Parameters: Aucun / None
 * Retour / Return: void
 */
void handleShortPress() {
  switch (currentMode) {
    case MODE_NORMAL:
      // Ouvrir/fermer la porte ou réessayer après erreur
      // Open/close door or retry after error
      if (currentDoorState == OBSTACLE_ERROR) {
        // Réessayer le mouvement / Retry movement
        currentDoorState = STOPPED;
        if (doorOpen) {
          closeDoor();
        } else {
          openDoor();
        }
      } else if (doorOpen) {
        Serial.println(F("Fermeture manuelle demandée / Manual closing requested"));
        closeDoor();
      } else {
        Serial.println(F("Ouverture manuelle demandée / Manual opening requested"));
        openDoor();
      }
      break;
      
    case MODE_SET_HOUR:
      // Incrémenter l'heure / Increment hour
      {
        DateTime now = rtc.now();
        int newHour = (now.hour() + 1) % 24;
        rtc.adjust(DateTime(now.year(), now.month(), now.day(), 
                           newHour, now.minute(), now.second()));
        settingsModeStart = millis(); // Reset timeout
      }
      break;
      
    case MODE_SET_MINUTE:
      // Incrémenter les minutes / Increment minutes
      {
        DateTime now = rtc.now();
        int newMinute = (now.minute() + 1) % 60;
        rtc.adjust(DateTime(now.year(), now.month(), now.day(), 
                           now.hour(), newMinute, 0));
        settingsModeStart = millis(); // Reset timeout
      }
      break;
      
    case MODE_SET_THRESHOLD:
      // Augmenter le seuil par pas de 5 / Increase threshold by steps of 5
      lightThreshold = min(1023, lightThreshold + 5);
      saveThreshold();
      settingsModeStart = millis(); // Reset timeout
      break;
      
    case MODE_SET_OPENING_TIMEOUT:
      // Augmenter timeout ouverture par pas de 1s / Increase opening timeout by 1s steps
      openingTimeout = min(60, openingTimeout + 1);
      saveOpeningTimeout();
      settingsModeStart = millis();
      break;
      
    case MODE_SET_CLOSING_TIMEOUT:
      // Augmenter timeout fermeture par pas de 1s / Increase closing timeout by 1s steps
      closingTimeout = min(60, closingTimeout + 1);
      saveClosingTimeout();
      settingsModeStart = millis();
      break;
  }
}

/**
 * Traite les double-clics du bouton
 * Processes button double-clicks
 * 
 * Paramètres / Parameters: Aucun / None
 * Retour / Return: void
 */
void handleDoubleClick() {
  switch (currentMode) {
    case MODE_SET_THRESHOLD:
      // Diminuer le seuil par pas de 5 / Decrease threshold by steps of 5
      lightThreshold = max(0, lightThreshold - 5);
      saveThreshold();
      settingsModeStart = millis(); // Reset timeout
      break;
      
    case MODE_SET_OPENING_TIMEOUT:
      // Diminuer timeout ouverture par pas de 1s / Decrease opening timeout by 1s steps
      openingTimeout = max(5, openingTimeout - 1);
      saveOpeningTimeout();
      settingsModeStart = millis();
      break;
      
    case MODE_SET_CLOSING_TIMEOUT:
      // Diminuer timeout fermeture par pas de 1s / Decrease closing timeout by 1s steps
      closingTimeout = max(5, closingTimeout - 1);
      saveClosingTimeout();
      settingsModeStart = millis();
      break;
  }
}

/**
 * Traite les appuis longs du bouton
 * Processes long button presses
 * 
 * Paramètres / Parameters: Aucun / None
 * Retour / Return: void
 */
void handleLongPress() {
  switch (currentMode) {
    case MODE_NORMAL:
      // En cas d'erreur obstacle, un appui long relance
      // In case of obstacle error, long press restarts
      if (currentDoorState == OBSTACLE_ERROR) {
        currentDoorState = STOPPED;
      } else {
        currentMode = MODE_SET_HOUR;
        settingsModeStart = millis();
      }
      break;
      
    case MODE_SET_HOUR:
      currentMode = MODE_SET_MINUTE;
      settingsModeStart = millis();
      break;
      
    case MODE_SET_MINUTE:
      currentMode = MODE_SET_THRESHOLD;
      settingsModeStart = millis();
      break;
      
    case MODE_SET_THRESHOLD:
      currentMode = MODE_SET_OPENING_TIMEOUT;
      settingsModeStart = millis();
      break;
      
    case MODE_SET_OPENING_TIMEOUT:
      currentMode = MODE_SET_CLOSING_TIMEOUT;
      settingsModeStart = millis();
      break;
      
    case MODE_SET_CLOSING_TIMEOUT:
      currentMode = MODE_NORMAL;
      break;
  }
  pressProcessed = true;
}

// ============================================================================
// GESTION DE L'AFFICHAGE LCD / LCD DISPLAY MANAGEMENT
// ============================================================================
/**
 * Gère l'affichage sur l'écran LCD selon le mode
 * Manages LCD display according to current mode
 * 
 * Paramètres / Parameters: 
 *   - maintenant: DateTime - Heure actuelle / Current time
 * Retour / Return: void
 */
void handleLCDDisplay(DateTime now) {
  lcd.setCursor(0, 0);
  
  switch (currentMode) {
    case MODE_NORMAL:
      // Ligne 1: Heure / Line 1: Time
      if (now.hour() < 10) lcd.print(F("0"));
      lcd.print(now.hour());
      lcd.print(F(":"));
      if (now.minute() < 10) lcd.print(F("0"));
      lcd.print(now.minute());
      lcd.print(F("      "));
      
      // Ligne 2: Statut de la porte / Line 2: Door status
      lcd.setCursor(0, 1);
      if (currentDoorState == OPENING) {
        lcd.print(F("Ouverture...    "));
      } else if (currentDoorState == CLOSING) {
        lcd.print(F("Fermeture...    "));
      } else if (lowLightDetected) {
        unsigned long timeRemaining = (CLOSING_DELAY - (millis() - lowLightStartTime)) / 60000;
        lcd.print(F("Ferme dans "));
        lcd.print(timeRemaining + 1);
        lcd.print(F("mn "));
      } else if (currentDoorState == OBSTACLE_ERROR) {
        lcd.print(F("ERREUR OBSTACLE "));
      } else {
        lcd.print(doorOpen ? F("Porte ouverte   ") : F("Porte fermee    "));
      }
      break;
      
    case MODE_SET_HOUR:
      if (blinking) {
        lcd.print(F("  :"));
      } else {
        if (now.hour() < 10) lcd.print(F("0"));
        lcd.print(now.hour());
        lcd.print(F(":"));
      }
      if (now.minute() < 10) lcd.print(F("0"));
      lcd.print(now.minute());
      lcd.print(F("      "));
      
      lcd.setCursor(0, 1);
      lcd.print(F("Reglage heure   "));
      break;
      
    case MODE_SET_MINUTE:
      if (now.hour() < 10) lcd.print(F("0"));
      lcd.print(now.hour());
      lcd.print(F(":"));
      if (blinking) {
        lcd.print(F("  "));
      } else {
        if (now.minute() < 10) lcd.print(F("0"));
        lcd.print(now.minute());
      }
      lcd.print(F("      "));
      
      lcd.setCursor(0, 1);
      lcd.print(F("Reglage minute  "));
      break;
      
    case MODE_SET_THRESHOLD:
      lcd.print(F("Seuil: "));
      lcd.print(lightThreshold);
      lcd.print(F("     "));
      
      lcd.setCursor(0, 1);
      int currentValue = analogRead(LIGHT_SENSOR);
      lcd.print(F("Actuel: "));
      lcd.print(currentValue);
      lcd.print(F("     "));
      break;
      
    case MODE_SET_OPENING_TIMEOUT:
      lcd.print(F("Timeout ouv: "));
      lcd.print(openingTimeout);
      lcd.print(F("s "));
      
      lcd.setCursor(0, 1);
      lcd.print(F("Bref:+1 Dbl:-1  "));
      break;
      
    case MODE_SET_CLOSING_TIMEOUT:
      lcd.print(F("Timeout fer: "));
      lcd.print(closingTimeout);
      lcd.print(F("s "));
      
      lcd.setCursor(0, 1);
      lcd.print(F("Bref:+1 Dbl:-1  "));
      break;
  }
}

// ============================================================================
// GESTION DE LA SAUVEGARDE EEPROM / EEPROM BACKUP MANAGEMENT
// ============================================================================
/**
 * Sauvegarde le seuil de luminosité en EEPROM
 * Saves light threshold to EEPROM
 * 
 * Paramètres / Parameters: Aucun / None
 * Retour / Return: void
 */
void saveThreshold() {
  EEPROM.write(THRESHOLD_EEPROM_ADDR, lightThreshold & 0xFF);
  EEPROM.write(THRESHOLD_EEPROM_ADDR + 1, (lightThreshold >> 8) & 0xFF);
}

/**
 * Sauvegarde le timeout d'ouverture en EEPROM
 * Saves opening timeout to EEPROM
 * 
 * Paramètres / Parameters: Aucun / None
 * Retour / Return: void
 */
void saveOpeningTimeout() {
  EEPROM.write(OPENING_TIMEOUT_ADDR, openingTimeout);
}

/**
 * Sauvegarde le timeout de fermeture en EEPROM
 * Saves closing timeout to EEPROM
 * 
 * Paramètres / Parameters: Aucun / None
 * Retour / Return: void
 */
void saveClosingTimeout() {
  EEPROM.write(CLOSING_TIMEOUT_ADDR, closingTimeout);
}

/**
 * Charge tous les paramètres depuis l'EEPROM
 * Loads all parameters from EEPROM
 * 
 * Paramètres / Parameters: Aucun / None
 * Retour / Return: void
 */
void loadParametersFromEEPROM() {
  // Chargement du seuil / Loading threshold
  int savedThreshold = EEPROM.read(THRESHOLD_EEPROM_ADDR) | (EEPROM.read(THRESHOLD_EEPROM_ADDR + 1) << 8);
  if (savedThreshold >= 0 && savedThreshold <= 1023) {
    lightThreshold = savedThreshold;
  }
  
  // Chargement timeout ouverture / Loading opening timeout
  int savedOpeningTimeout = EEPROM.read(OPENING_TIMEOUT_ADDR);
  if (savedOpeningTimeout >= 5 && savedOpeningTimeout <= 60) {
    openingTimeout = savedOpeningTimeout;
  }
  
  // Chargement timeout fermeture / Loading closing timeout
  int savedClosingTimeout = EEPROM.read(CLOSING_TIMEOUT_ADDR);
  if (savedClosingTimeout >= 5 && savedClosingTimeout <= 60) {
    closingTimeout = savedClosingTimeout;
  }
}

// ============================================================================
// CONTRÔLE DU MOTEUR / MOTOR CONTROL
// ============================================================================
/**
 * Démarre l'ouverture de la porte
 * Starts door opening
 * 
 * Paramètres / Parameters: Aucun / None
 * Retour / Return: void
 */
void openDoor() {
  if (!doorOpen && (currentDoorState == STOPPED || currentDoorState == OBSTACLE_ERROR)) {
    Serial.println(F("Début ouverture porte / Starting door opening"));
    currentDoorState = OPENING;
    doorMovementStart = millis();
    digitalWrite(MOTOR_PIN1, HIGH);
    digitalWrite(MOTOR_PIN2, LOW);
  }
}

/**
 * Démarre la fermeture de la porte
 * Starts door closing
 * 
 * Paramètres / Parameters: Aucun / None
 * Retour / Return: void
 */
void closeDoor() {
  if (doorOpen && (currentDoorState == STOPPED || currentDoorState == OBSTACLE_ERROR)) {
    Serial.println(F("Début fermeture porte / Starting door closing"));
    currentDoorState = CLOSING;
    doorMovementStart = millis();
    digitalWrite(MOTOR_PIN1, LOW);
    digitalWrite(MOTOR_PIN2, HIGH);
  }
}

/**
 * Arrête le moteur
 * Stops the motor
 * 
 * Paramètres / Parameters: Aucun / None
 * Retour / Return: void
 */
void stopMotor() {
  digitalWrite(MOTOR_PIN1, LOW);
  digitalWrite(MOTOR_PIN2, LOW);
  currentDoorState = STOPPED;
}
//--------------------------------- END TRANSLATION ---------------------------------------------------------------
/**
 * Gère le mouvement de la porte et la détection d'obstacles
 * Manages door movement and obstacle detection
 * 
 * Paramètres / Parameters: Aucun / None
 * Retour / Return: void
 */
void handleDoorMovement() {
  switch (currentDoorState) {
    case OPENING:
      // Vérifier timeout / Check timeout
      if (millis() - doorMovementStart > (unsigned long)openingTimeout * 1000) {
        Serial.println("Timeout ouverture - Obstacle détecté / Opening timeout - Obstacle detected");
        stopMotor();
        currentDoorState = OBSTACLE_ERROR;
      }
      // Vérifier fin de course / Check limit switch
      else if (digitalRead(TOP_LIMIT_SWITCH) == LOW) {
        Serial.println("Porte complètement ouverte / Door fully open");
        stopMotor();
        doorOpen = true;
      }
      break;
      
    case CLOSING:
      // Vérifier timeout / Check timeout
      if (millis() - doorMovementStart > (unsigned long)closingTimeout * 1000) {
        Serial.println("Timeout fermeture - Obstacle détecté / Closing timeout - Obstacle detected");
        stopMotor();
        currentDoorState = OBSTACLE_ERROR;
      }
      // Vérifier fin de course / Check limit switch
      else if (digitalRead(BOTTOM_LIMIT_SWITCH) == LOW) {
        Serial.println("Porte complètement fermée / Door fully closed");
        stopMotor();
        doorOpen = false;
      }
      break;
      
    case STOPPED:
    case OBSTACLE_ERROR:
      break;
  }
}

// ============================================================================
// GESTION DE LA LUMINOSITÉ / LIGHT MANAGEMENT
// ============================================================================
/**
 * Gère la fermeture automatique basée sur la luminosité avec temporisation
 * Manages automatic closing based on light level with delay
 * 
 * Paramètres / Parameters:
 *   - valeurLumiere: int - Valeur actuelle du capteur / Current sensor value
 *   - heureActuelle: int - Heure actuelle / Current hour
 * Retour / Return: void
 */
void handleLightClosing(int lightValue, int currentHour) {
  // Vérifier si conditions sont réunies / Check if conditions are met
  if (!doorOpen || currentHour <= 7) {
    lowLightDetected = false;
    lowLightStartTime = 0;
    return;
  }
  
  if (lightValue < lightThreshold) {
    if (!lowLightDetected) {
      // Début de la temporisation / Start of delay
      lowLightDetected = true;
      lowLightStartTime = millis();
      Serial.println("Début détection lumière faible - temporisation 10 minutes / Start low light detection - 10 minutes delay");
    } else if (millis() - lowLightStartTime >= CLOSING_DELAY) {
      // Temporisation écoulée / Delay elapsed
      Serial.println("Temporisation écoulée - Fermeture automatique / Delay elapsed - Automatic closing");
      closeDoor();
      lowLightDetected = false;
    }
  } else {
    // La lumière est revenue / Light has returned
    if (lowLightDetected) {
      Serial.println("Lumière revenue - Annulation temporisation fermeture / Light returned - Closing delay cancelled");
      lowLightDetected = false;
      lowLightStartTime = 0;
    }
  }
}

// ============================================================================
// GESTION DE LA LED DE COUPURE / POWER OUTAGE LED MANAGEMENT
// ============================================================================
/**
 * Gère le clignotement de la LED en cas de coupure de courant
 * Manages LED blinking in case of power outage
 * 
 * Paramètres / Parameters: Aucun / None
 * Retour / Return: void
 */
void handlePowerOutageLED() {
  if (powerOutage) {
    // Faire clignoter la LED toutes les 500ms / Blink LED every 500ms
    if (millis() - lastLEDTime > 500) {
      lastLEDTime = millis();
      ledState = !ledState;
      digitalWrite(POWER_OUTAGE_LED, ledState);
    }
  } else {
    digitalWrite(POWER_OUTAGE_LED, LOW);
  }
}

// ============================================================================
// FIN DU PROGRAMME / END OF PROGRAM
// ============================================================================
/*
 * ============================================================================
 * NOTES DE DÉVELOPPEMENT / DEVELOPMENT NOTES
 * ============================================================================
 * 
 * Version 1.0.0 - Fonctionnalités implémentées / Implemented features:
 * - ✅ Ouverture/fermeture automatique / Automatic opening/closing
 * - ✅ Interface LCD complète / Complete LCD interface
 * - ✅ Gestion bouton multifonction / Multi-function button management
 * - ✅ Système anti-obstacle / Anti-jam system
 * - ✅ Sauvegarde EEPROM / EEPROM backup
 * - ✅ Temporisation anti-nuages / Anti-cloud delay
 * - ✅ LED d'alerte coupure / Power outage alert LED
 * 
 * Améliorations futures possibles / Possible future improvements:
 * - 📡 Module WiFi pour monitoring distant / WiFi module for remote monitoring
 * - ☀️ Capteur de température / Temperature sensor
 * - 🔋 Gestion alimentation solaire / Solar power management
 * - 📱 Application mobile / Mobile app
 * - 🔔 Notifications push / Push notifications
 * 
 * ============================================================================
 * CONTACT ET SUPPORT / CONTACT AND SUPPORT
 * ============================================================================
 * 
 * GitHub: https://github.com/fbaillon19/poulailler-automatique
 * Issues: Pour les bugs et suggestions / For bugs and suggestions
 * Wiki: Documentation détaillée / Detailed documentation
 * 
 * Licence MIT - Libre d'utilisation et modification
 * MIT License - Free to use and modify
 * 
 * ============================================================================
 */