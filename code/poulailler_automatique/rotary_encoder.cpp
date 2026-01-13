/*
 * ============================================================================
 * ROTARY_ENCODER.CPP v1.4.0 - Implémentation encodeur KY-040
 * ============================================================================
 * 
 * Basé sur la bibliothèque Encoder de Paul Stoffregen
 * 
 * Nouveautés v1.4.0:
 * - Appui long réduit à 1.5s (au lieu de 3s)
 * - Appui très long 5s pour acquittement erreurs
 * - Navigation par appui BREF entre modes
 * - Nouveaux modes de réglage (10 modes au total)
 * 
 * Auteur / Author: Frédéric BAILLON
 * Version: 1.4.0
 * Date: Janvier 2026
 * Licence / License: MIT
 * 
 * ============================================================================
 */

#include "rotary_encoder.h"
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

// ============================================================================
// CONFIGURATION DES BROCHES / PIN CONFIGURATION
// ============================================================================
const uint8_t ENCODER_CLK = 2;
const uint8_t ENCODER_DT = 4;
const uint8_t ENCODER_SW = 5;

// ============================================================================
// CONSTANTES / CONSTANTS
// ============================================================================
const int32_t ENCODER_DIVISOR = 4;
const uint32_t BUTTON_DEBOUNCE = 50;
const uint32_t LONG_PRESS_TIME = 1500;      // 1.5s (réduit de 3s)
const uint32_t VERY_LONG_PRESS_TIME = 5000; // 5s pour acquittement erreur

// ============================================================================
// VARIABLES PRIVÉES / PRIVATE VARIABLES
// ============================================================================
static Encoder *encoder = nullptr;

static int32_t position = 0;
static int32_t lastPosition = 0;

static bool buttonState = false;
static bool lastButtonState = false;
static uint32_t lastButtonChange = 0;
static uint32_t buttonPressTime = 0;
static bool longPressDetected = false;
static bool veryLongPressDetected = false;

extern RTC_DS3231 rtc;
extern LiquidCrystal_I2C lcd;

// Variables externes pour gestion erreurs
extern uint8_t erreurActuelle;

// ============================================================================
// DÉCLARATIONS FORWARD / FORWARD DECLARATIONS
// ============================================================================
static void readEncoderPosition();
static void processButton();
static void onEncoderRotation(int direction);
static void onEncoderButtonClick();
static void onEncoderButtonLongPress();
static void onEncoderButtonVeryLongPress();

// ============================================================================
// FONCTIONS PRIVÉES / PRIVATE FUNCTIONS
// ============================================================================

static void readEncoderPosition() {
  if (!encoder) return;
  
  int32_t rawPosition = encoder->read();
  int32_t newPosition = rawPosition / ENCODER_DIVISOR;
  
  if (newPosition != position) {
    int direction = (newPosition > position) ? -1 : 1;
    position = newPosition;
    onEncoderRotation(direction);
  }
}

static void processButton() {
  uint32_t now = millis();
  bool currentState = (digitalRead(ENCODER_SW) == LOW);
  
  if (now - lastButtonChange < BUTTON_DEBOUNCE) {
    return;
  }
  
  if (currentState != buttonState) {
    lastButtonChange = now;
    buttonState = currentState;
    
    if (buttonState) {
      buttonPressTime = now;
      longPressDetected = false;
      veryLongPressDetected = false;
    } else {
      uint32_t pressDuration = now - buttonPressTime;
      
      if (veryLongPressDetected || longPressDetected) {
        // Déjà traité
      } else if (pressDuration < LONG_PRESS_TIME) {
        onEncoderButtonClick();
      }
    }
  }
  
  // Détection appui long (1.5s)
  if (buttonState && !longPressDetected && !veryLongPressDetected) {
    if (now - buttonPressTime >= LONG_PRESS_TIME && now - buttonPressTime < VERY_LONG_PRESS_TIME) {
      longPressDetected = true;
      onEncoderButtonLongPress();
    }
  }
  
  // Détection appui TRÈS long (5s)
  if (buttonState && !veryLongPressDetected) {
    if (now - buttonPressTime >= VERY_LONG_PRESS_TIME) {
      veryLongPressDetected = true;
      onEncoderButtonVeryLongPress();
    }
  }
}

static void onEncoderRotation(int direction) {
  Serial.print(F("Rotation: "));
  Serial.println(direction > 0 ? F("CW (+1)") : F("CCW (-1)"));
  
  derniereActivite = millis();
  dernierRafraichissementLCD = 0;
  
  switch (modeActuel) {
    case MODE_REGLAGE_HEURE_OUVERTURE:
      heureOuverture = constrain(heureOuverture + direction, 6, 9);
      debutModeReglage = millis();
      Serial.print(F("  Heure ouverture: "));
      Serial.println(heureOuverture);
      break;
      
    case MODE_REGLAGE_MINUTE_OUVERTURE:
      minuteOuverture += direction;
      if (minuteOuverture < 0) minuteOuverture = 59;
      if (minuteOuverture > 59) minuteOuverture = 0;
      debutModeReglage = millis();
      Serial.print(F("  Minute ouverture: "));
      Serial.println(minuteOuverture);
      break;
      
    case MODE_REGLAGE_HEURE_MIN_FERMETURE:
      heureMinFermeture = constrain(heureMinFermeture + direction, 15, 17);
      debutModeReglage = millis();
      Serial.print(F("  Heure min fermeture: "));
      Serial.println(heureMinFermeture);
      break;
      
    case MODE_REGLAGE_MINUTE_MIN_FERMETURE:
      minuteMinFermeture += direction;
      if (minuteMinFermeture < 0) minuteMinFermeture = 59;
      if (minuteMinFermeture > 59) minuteMinFermeture = 0;
      debutModeReglage = millis();
      Serial.print(F("  Minute min fermeture: "));
      Serial.println(minuteMinFermeture);
      break;
      
    case MODE_REGLAGE_HEURE_MAX_FERMETURE:
      heureMaxFermeture += direction;
      if (heureMaxFermeture < 0) heureMaxFermeture = 23;
      if (heureMaxFermeture > 23) heureMaxFermeture = 0;
      debutModeReglage = millis();
      Serial.print(F("  Heure max fermeture: "));
      Serial.println(heureMaxFermeture);
      break;
      
    case MODE_REGLAGE_MINUTE_MAX_FERMETURE:
      minuteMaxFermeture += direction;
      if (minuteMaxFermeture < 0) minuteMaxFermeture = 59;
      if (minuteMaxFermeture > 59) minuteMaxFermeture = 0;
      debutModeReglage = millis();
      Serial.print(F("  Minute max fermeture: "));
      Serial.println(minuteMaxFermeture);
      break;
      
    case MODE_REGLAGE_SEUIL:
      seuilLumiere = constrain(seuilLumiere + direction, 0, 1023);
      debutModeReglage = millis();
      Serial.print(F("  Seuil: "));
      Serial.println(seuilLumiere);
      break;
      
    case MODE_REGLAGE_TEMPO_FERMETURE:
      tempoFermetureMinutes = constrain(tempoFermetureMinutes + direction, 10, 30);
      debutModeReglage = millis();
      Serial.print(F("  Tempo fermeture: "));
      Serial.print(tempoFermetureMinutes);
      Serial.println(F(" min"));
      break;
      
    case MODE_REGLAGE_TIMEOUT_OUVERTURE:
      timeoutOuverture = constrain(timeoutOuverture + direction, 5, 60);
      debutModeReglage = millis();
      Serial.print(F("  Timeout ouv: "));
      Serial.println(timeoutOuverture);
      break;
      
    case MODE_REGLAGE_TIMEOUT_FERMETURE:
      timeoutFermeture = constrain(timeoutFermeture + direction, 5, 60);
      debutModeReglage = millis();
      Serial.print(F("  Timeout fer: "));
      Serial.println(timeoutFermeture);
      break;
      
    case MODE_NORMAL:
      Serial.println(F("  (Mode normal, rotation ignorée)"));
      break;
  }
}

static void onEncoderButtonClick() {
  Serial.println(F("Bouton: CLICK"));
  
  if (!lcdAllume) {
    Serial.println(F("  → Rallumage LCD"));
    allumerLCD();
    dernierAllumageMinute = millis();
    return;
  }
  
  derniereActivite = millis();
  
  // En mode normal : ouvrir/fermer porte
  if (modeActuel == MODE_NORMAL) {
    if (etatActuel == ERREUR_OBSTACLE) {
      Serial.println(F("  → Réessai après obstacle"));
      etatActuel = ARRET;
      if (porteOuverte) {
        fermerPorte();
      } else {
        ouvrirPorte();
      }
    } else if (porteOuverte) {
      Serial.println(F("  → Fermeture manuelle"));
      fermerPorte();
    } else {
      Serial.println(F("  → Ouverture manuelle"));
      ouvrirPorte();
    }
  }
  // En mode réglage : passer au mode suivant (NOUVEAU v1.4.0)
  else {
    Serial.print(F("  → Mode suivant: "));
    
    switch (modeActuel) {
      case MODE_REGLAGE_HEURE_OUVERTURE:
        modeActuel = MODE_REGLAGE_MINUTE_OUVERTURE;
        Serial.println(F("MINUTE_OUVERTURE"));
        break;
      case MODE_REGLAGE_MINUTE_OUVERTURE:
        modeActuel = MODE_REGLAGE_HEURE_MIN_FERMETURE;
        Serial.println(F("HEURE_MIN_FERMETURE"));
        break;
      case MODE_REGLAGE_HEURE_MIN_FERMETURE:
        modeActuel = MODE_REGLAGE_MINUTE_MIN_FERMETURE;
        Serial.println(F("MINUTE_MIN_FERMETURE"));
        break;
      case MODE_REGLAGE_MINUTE_MIN_FERMETURE:
        modeActuel = MODE_REGLAGE_HEURE_MAX_FERMETURE;
        Serial.println(F("HEURE_MAX_FERMETURE"));
        break;
      case MODE_REGLAGE_HEURE_MAX_FERMETURE:
        modeActuel = MODE_REGLAGE_MINUTE_MAX_FERMETURE;
        Serial.println(F("MINUTE_MAX_FERMETURE"));
        break;
      case MODE_REGLAGE_MINUTE_MAX_FERMETURE:
        modeActuel = MODE_REGLAGE_SEUIL;
        Serial.println(F("SEUIL"));
        break;
      case MODE_REGLAGE_SEUIL:
        modeActuel = MODE_REGLAGE_TEMPO_FERMETURE;
        Serial.println(F("TEMPO_FERMETURE"));
        break;
      case MODE_REGLAGE_TEMPO_FERMETURE:
        modeActuel = MODE_REGLAGE_TIMEOUT_OUVERTURE;
        Serial.println(F("TIMEOUT_OUVERTURE"));
        break;
      case MODE_REGLAGE_TIMEOUT_OUVERTURE:
        modeActuel = MODE_REGLAGE_TIMEOUT_FERMETURE;
        Serial.println(F("TIMEOUT_FERMETURE"));
        break;
      case MODE_REGLAGE_TIMEOUT_FERMETURE:
        Serial.println(F("NORMAL (sortie + sauvegarde)"));
        modeActuel = MODE_NORMAL;
        sauvegarderParametres();
        lcd.clear();
        eteindreLCD();
        break;
    }
    
    debutModeReglage = millis();
    dernierRafraichissementLCD = 0;
  }
}

static void onEncoderButtonLongPress() {
  Serial.println(F("Bouton: APPUI LONG (1.5s)"));
  
  if (!lcdAllume) {
    Serial.println(F("  → Rallumage LCD"));
    allumerLCD();
    dernierAllumageMinute = millis();
    return;
  }
  
  derniereActivite = millis();
  dernierRafraichissementLCD = 0;
  
  // En mode normal : entrer en mode réglage
  if (modeActuel == MODE_NORMAL) {
    if (etatActuel == ERREUR_OBSTACLE) {
      Serial.println(F("  → Reset erreur obstacle"));
      etatActuel = ARRET;
    } else {
      Serial.println(F("  → MODE_REGLAGE_HEURE_OUVERTURE"));
      modeActuel = MODE_REGLAGE_HEURE_OUVERTURE;
      debutModeReglage = millis();
      lcd.clear();
    }
  }
  // En mode réglage : appui long ne fait rien (navigation par appui bref)
}

static void onEncoderButtonVeryLongPress() {
  Serial.println(F("Bouton: APPUI TRÈS LONG (5s) - ACQUITTEMENT ERREUR"));
  
  if (erreurActuelle != 0) {  // AUCUNE_ERREUR = 0
    Serial.print(F("  → Acquittement erreur: "));
    Serial.println(erreurActuelle);
    erreurActuelle = 0;  // AUCUNE_ERREUR
    lcd.clear();
    allumerLCD();
  } else {
    Serial.println(F("  → Aucune erreur à acquitter"));
  }
}

// ============================================================================
// FONCTIONS PUBLIQUES / PUBLIC FUNCTIONS
// ============================================================================

void initRotaryEncoder() {
  encoder = new Encoder(ENCODER_CLK, ENCODER_DT);
  
  if (!encoder) {
    Serial.println(F("ERREUR: Impossible de créer l'objet Encoder!"));
    return;
  }
  
  pinMode(ENCODER_SW, INPUT_PULLUP);
  buttonState = (digitalRead(ENCODER_SW) == LOW);
  
  Serial.println(F("Encodeur KY-040 initialisé (bibliothèque Encoder)"));
  Serial.print(F("  CLK="));
  Serial.print(ENCODER_CLK);
  Serial.print(F(" DT="));
  Serial.print(ENCODER_DT);
  Serial.print(F(" SW="));
  Serial.println(ENCODER_SW);
  Serial.println(F("  Appui long: 1.5s, Appui très long: 5s"));
}

void updateRotaryEncoder() {
  readEncoderPosition();
  processButton();
}
