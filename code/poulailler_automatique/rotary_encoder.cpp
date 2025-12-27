/*
 * ============================================================================
 * ROTARY_ENCODER.CPP - Implémentation encodeur KY-040 (Version procédurale)
 * ============================================================================
 * 
 * Basé sur la bibliothèque Encoder de Paul Stoffregen
 * Adapté du code testé et fonctionnel de F. Baillon
 * 
 * Auteur / Author: Frédéric BAILLON
 * Version: 1.3.0
 * Date: 2025
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
const uint8_t ENCODER_CLK = 2;  // Pin CLK (A) - DOIT être sur interruption
const uint8_t ENCODER_DT = 4;   // Pin DT (B) - DOIT être sur interruption  
const uint8_t ENCODER_SW = 5;   // Pin SW (bouton)

// ============================================================================
// CONSTANTES / CONSTANTS
// ============================================================================
const int32_t ENCODER_DIVISOR = 4;      // Diviseur: 1 clic = 1 position (4 états par détente)
const uint32_t BUTTON_DEBOUNCE = 50;    // Anti-rebond bouton (ms)
const uint32_t LONG_PRESS_TIME = 3000;  // Durée appui long (ms) - 3 secondes

// ============================================================================
// VARIABLES PRIVÉES / PRIVATE VARIABLES
// ============================================================================
static Encoder *encoder = nullptr;      // Objet Encoder (bibliothèque Paul Stoffregen)

// État rotation
static int32_t position = 0;            // Position actuelle (divisée)
static int32_t lastPosition = 0;        // Dernière position

// État bouton
static bool buttonState = false;        // État actuel du bouton (true = appuyé)
static bool lastButtonState = false;    // État précédent
static uint32_t lastButtonChange = 0;   // Timestamp dernier changement
static uint32_t buttonPressTime = 0;    // Timestamp début appui
static bool longPressDetected = false;  // Flag appui long détecté

// Variables externes
extern RTC_DS3231 rtc;
extern LiquidCrystal_I2C lcd;

// ============================================================================
// DÉCLARATIONS FORWARD / FORWARD DECLARATIONS
// ============================================================================
static void readEncoderPosition();
static void processButton();
static void onEncoderRotation(int direction);
static void onEncoderButtonClick();
static void onEncoderButtonLongPress();

// ============================================================================
// FONCTIONS PRIVÉES / PRIVATE FUNCTIONS
// ============================================================================

/**
 * Lit la position de l'encodeur et détecte les changements
 */
static void readEncoderPosition() {
  if (!encoder) return;
  
  // Lire position brute de la bibliothèque Encoder
  int32_t rawPosition = encoder->read();
  
  // Diviser pour obtenir 1 clic = 1 position
  int32_t newPosition = rawPosition / ENCODER_DIVISOR;
  
  // Vérifier si la position a changé
  if (newPosition != position) {
    int direction = (newPosition > position) ? -1 : 1;
    position = newPosition;
    
    // Traiter la rotation
    onEncoderRotation(direction);
  }
}

/**
 * Traite les événements du bouton
 */
static void processButton() {
  uint32_t now = millis();
  bool currentState = (digitalRead(ENCODER_SW) == LOW);  // Actif bas (pull-up)
  
  // Anti-rebond
  if (now - lastButtonChange < BUTTON_DEBOUNCE) {
    return;
  }
  
  // Changement d'état détecté
  if (currentState != buttonState) {
    lastButtonChange = now;
    buttonState = currentState;
    
    if (buttonState) {
      // Bouton appuyé
      buttonPressTime = now;
      longPressDetected = false;
    } else {
      // Bouton relâché
      uint32_t pressDuration = now - buttonPressTime;
      
      if (longPressDetected) {
        // Appui long déjà traité pendant l'appui
        // Ne rien faire
      } else if (pressDuration >= LONG_PRESS_TIME) {
        // Appui long détecté au relâchement (cas rare)
        onEncoderButtonLongPress();
      } else {
        // Clic court
        onEncoderButtonClick();
      }
    }
  }
  
  // Détection appui long en cours (pendant que le bouton est enfoncé)
  if (buttonState && !longPressDetected) {
    if (now - buttonPressTime >= LONG_PRESS_TIME) {
      longPressDetected = true;
      onEncoderButtonLongPress();
    }
  }
}

/**
 * Gère la rotation de l'encodeur
 * @param direction: 1 = CW (horaire), -1 = CCW (anti-horaire)
 */
static void onEncoderRotation(int direction) {
  Serial.print(F("Rotation: "));
  Serial.println(direction > 0 ? F("CW (+1)") : F("CCW (-1)"));
  
  derniereActivite = millis();
  dernierRafraichissementLCD = 0; // Forcer rafraîchissement immédiat
  
  switch (modeActuel) {
    case MODE_REGLAGE_HEURE:
      {
        DateTime maintenant = rtc.now();
        int nouvelleHeure = maintenant.hour() + direction;
        if (nouvelleHeure < 0) nouvelleHeure = 23;
        if (nouvelleHeure > 23) nouvelleHeure = 0;
        rtc.adjust(DateTime(maintenant.year(), maintenant.month(), maintenant.day(), 
                           nouvelleHeure, maintenant.minute(), maintenant.second()));
        debutModeReglage = millis();
        Serial.print(F("  Heure: "));
        Serial.println(nouvelleHeure);
      }
      break;
      
    case MODE_REGLAGE_MINUTE:
      {
        DateTime maintenant = rtc.now();
        int nouvelleMinute = maintenant.minute() + direction;
        if (nouvelleMinute < 0) nouvelleMinute = 59;
        if (nouvelleMinute > 59) nouvelleMinute = 0;
        rtc.adjust(DateTime(maintenant.year(), maintenant.month(), maintenant.day(), 
                           maintenant.hour(), nouvelleMinute, 0));
        debutModeReglage = millis();
        Serial.print(F("  Minute: "));
        Serial.println(nouvelleMinute);
      }
      break;
      
    case MODE_REGLAGE_SEUIL:
      seuilLumiere = constrain(seuilLumiere + direction, 0, 1023);
      sauvegarderSeuil();
      debutModeReglage = millis();
      Serial.print(F("  Seuil: "));
      Serial.println(seuilLumiere);
      break;
      
    case MODE_REGLAGE_TIMEOUT_OUVERTURE:
      timeoutOuverture = constrain(timeoutOuverture + direction, 5, 60);
      sauvegarderTimeoutOuverture();
      debutModeReglage = millis();
      Serial.print(F("  Timeout ouv: "));
      Serial.println(timeoutOuverture);
      break;
      
    case MODE_REGLAGE_TIMEOUT_FERMETURE:
      timeoutFermeture = constrain(timeoutFermeture + direction, 5, 60);
      sauvegarderTimeoutFermeture();
      debutModeReglage = millis();
      Serial.print(F("  Timeout fer: "));
      Serial.println(timeoutFermeture);
      break;
      
    case MODE_NORMAL:
      // Pas d'action en mode normal
      Serial.println(F("  (Mode normal, rotation ignorée)"));
      break;
  }
}

/**
 * Gère le clic court sur le bouton
 */
static void onEncoderButtonClick() {
  Serial.println(F("Bouton: CLICK"));
  
  // Si LCD éteint, juste le rallumer
  if (!lcdAllume) {
    Serial.println(F("  → Rallumage LCD"));
    allumerLCD();
    dernierAllumageMinute = millis();
    return;
  }
  
  derniereActivite = millis();
  
  // En mode normal : ouvrir/fermer la porte
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
  // En mode réglage : pas d'action sur clic court
  else {
    Serial.println(F("  (Mode réglage, clic ignoré)"));
  }
}

/**
 * Gère l'appui long sur le bouton
 */
static void onEncoderButtonLongPress() {
  Serial.println(F("Bouton: APPUI LONG"));
  
  // Si LCD éteint, le rallumer d'abord
  if (!lcdAllume) {
    Serial.println(F("  → Rallumage LCD"));
    allumerLCD();
    dernierAllumageMinute = millis();
    return;
  }
  
  derniereActivite = millis();
  dernierRafraichissementLCD = 0;
  
  switch (modeActuel) {
    case MODE_NORMAL:
      if (etatActuel == ERREUR_OBSTACLE) {
        Serial.println(F("  → Reset erreur obstacle"));
        etatActuel = ARRET;
      } else {
        Serial.println(F("  → MODE_REGLAGE_HEURE"));
        modeActuel = MODE_REGLAGE_HEURE;
        debutModeReglage = millis();
        lcd.clear();
      }
      break;
      
    case MODE_REGLAGE_HEURE:
      Serial.println(F("  → MODE_REGLAGE_MINUTE"));
      modeActuel = MODE_REGLAGE_MINUTE;
      debutModeReglage = millis();
      break;
      
    case MODE_REGLAGE_MINUTE:
      Serial.println(F("  → MODE_REGLAGE_SEUIL"));
      modeActuel = MODE_REGLAGE_SEUIL;
      debutModeReglage = millis();
      lcd.clear();
      break;
      
    case MODE_REGLAGE_SEUIL:
      Serial.println(F("  → MODE_TIMEOUT_OUVERTURE"));
      modeActuel = MODE_REGLAGE_TIMEOUT_OUVERTURE;
      debutModeReglage = millis();
      lcd.clear();
      break;
      
    case MODE_REGLAGE_TIMEOUT_OUVERTURE:
      Serial.println(F("  → MODE_TIMEOUT_FERMETURE"));
      modeActuel = MODE_REGLAGE_TIMEOUT_FERMETURE;
      debutModeReglage = millis();
      lcd.clear();
      break;
      
    case MODE_REGLAGE_TIMEOUT_FERMETURE:
      Serial.println(F("  → MODE_NORMAL (sortie)"));
      modeActuel = MODE_NORMAL;
      lcd.clear();
      eteindreLCD(); // Éteindre le LCD en sortant
      break;
  }
}

// ============================================================================
// FONCTIONS PUBLIQUES / PUBLIC FUNCTIONS
// ============================================================================

/**
 * Initialise l'encodeur rotatif avec la bibliothèque Encoder
 */
void initRotaryEncoder() {
  // Créer l'objet Encoder (gère automatiquement les interruptions)
  encoder = new Encoder(ENCODER_CLK, ENCODER_DT);
  
  if (!encoder) {
    Serial.println(F("ERREUR: Impossible de créer l'objet Encoder!"));
    return;
  }
  
  // Configuration du bouton
  pinMode(ENCODER_SW, INPUT_PULLUP);
  buttonState = (digitalRead(ENCODER_SW) == LOW);
  
  Serial.println(F("Encodeur KY-040 initialisé (bibliothèque Encoder)"));
  Serial.print(F("  CLK="));
  Serial.print(ENCODER_CLK);
  Serial.print(F(" DT="));
  Serial.print(ENCODER_DT);
  Serial.print(F(" SW="));
  Serial.println(ENCODER_SW);
}

/**
 * Met à jour l'état de l'encodeur (à appeler dans loop)
 */
void updateRotaryEncoder() {
  readEncoderPosition();
  processButton();
}
