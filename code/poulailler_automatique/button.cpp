/*
 * ============================================================================
 * BUTTON.CPP - Implémentation gestion bouton multifonction
 * ============================================================================
 * 
 * Gestion du bouton avec la bibliothèque OneButton
 * Button management implementation using OneButton library
 * 
 * Auteur / Author: Frédéric BAILLON
 * Version: 1.1.0
 * Date: 2025
 * Licence / License: MIT
 * 
 * ============================================================================
 */

#include "button.h"
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

// Variables externes / External variables
extern RTC_DS3231 rtc;
extern LiquidCrystal_I2C lcd;

// Constantes / Constants
const int BOUTON_PIN = 5;
const unsigned long APPUI_LONG_MS = 3000; // 3 secondes pour appui long / 3 seconds for long press

// Objet OneButton / OneButton object
OneButton bouton(BOUTON_PIN, true); // true = INPUT_PULLUP

// ============================================================================
// INITIALISATION / INITIALIZATION
// ============================================================================
/**
 * Initialise le bouton avec OneButton
 * Initializes button with OneButton
 */
void initButton() {
  // Configuration des callbacks / Configure callbacks
  bouton.attachClick(onButtonClick);
  bouton.attachDoubleClick(onButtonDoubleClick);
  bouton.attachLongPressStart(onButtonLongPressStart);
  
  // Configuration du timing / Configure timing
  bouton.setPressTicks(APPUI_LONG_MS); // Durée pour appui long / Long press duration
  bouton.setClickTicks(250);           // Durée max pour un clic / Max duration for click
  bouton.setIdleMs(500);               // Temps entre deux clics pour double-clic / Time between clicks for double-click
  
  Serial.println(F("Bouton initialisé avec OneButton / Button initialized with OneButton"));
}

// ============================================================================
// MISE À JOUR / UPDATE
// ============================================================================
/**
 * Fonction à appeler dans loop() pour mettre à jour l'état du bouton
 * Function to call in loop() to update button state
 */
void updateButton() {
  bouton.tick();
}

// ============================================================================
// CALLBACK: SIMPLE CLIC / SINGLE CLICK
// ============================================================================
/**
 * Callback pour un simple clic sur le bouton
 * Callback for single button click
 */
void onButtonClick() {
  Serial.println(F("Click détecté / Click detected"));
  
  // Si le LCD est éteint, le rallumer uniquement
  // If LCD is off, only turn it back on
  if (!lcdAllume) {
    allumerLCD();
    dernierAllumageMinute = millis();
    return; // Ne pas exécuter d'autres actions / Don't execute other actions
  }
  
  derniereActivite = millis(); // Réinitialiser le timer d'inactivité / Reset inactivity timer
  
  switch (modeActuel) {
    case MODE_NORMAL:
      // Ouvrir/fermer la porte ou réessayer après erreur
      // Open/close door or retry after error
      if (etatActuel == ERREUR_OBSTACLE) {
        // Réessayer le mouvement / Retry movement
        Serial.println(F("Réessai après obstacle / Retry after obstacle"));
        etatActuel = ARRET;
        if (porteOuverte) {
          fermerPorte();
        } else {
          ouvrirPorte();
        }
      } else if (porteOuverte) {
        Serial.println(F("Fermeture manuelle demandée / Manual closing requested"));
        fermerPorte();
      } else {
        Serial.println(F("Ouverture manuelle demandée / Manual opening requested"));
        ouvrirPorte();
      }
      break;
      
    case MODE_REGLAGE_HEURE:
      // Incrémenter l'heure / Increment hour
      {
        DateTime maintenant = rtc.now();
        int nouvelleHeure = (maintenant.hour() + 1) % 24;
        rtc.adjust(DateTime(maintenant.year(), maintenant.month(), maintenant.day(), 
                           nouvelleHeure, maintenant.minute(), maintenant.second()));
        debutModeReglage = millis(); // Reset timeout
        Serial.print(F("Heure réglée: "));
        Serial.println(nouvelleHeure);
      }
      break;
      
    case MODE_REGLAGE_MINUTE:
      // Incrémenter les minutes / Increment minutes
      {
        DateTime maintenant = rtc.now();
        int nouvelleMinute = (maintenant.minute() + 1) % 60;
        rtc.adjust(DateTime(maintenant.year(), maintenant.month(), maintenant.day(), 
                           maintenant.hour(), nouvelleMinute, 0));
        debutModeReglage = millis(); // Reset timeout
        Serial.print(F("Minute réglée: "));
        Serial.println(nouvelleMinute);
      }
      break;
      
    case MODE_REGLAGE_SEUIL:
      // Augmenter le seuil par pas de 5 / Increase threshold by steps of 5
      seuilLumiere = min(1023, seuilLumiere + 5);
      sauvegarderSeuil();
      debutModeReglage = millis(); // Reset timeout
      Serial.print(F("Seuil augmenté: "));
      Serial.println(seuilLumiere);
      break;
      
    case MODE_REGLAGE_TIMEOUT_OUVERTURE:
      // Augmenter timeout ouverture par pas de 1s / Increase opening timeout by 1s steps
      timeoutOuverture = min(60, timeoutOuverture + 1);
      sauvegarderTimeoutOuverture();
      debutModeReglage = millis();
      Serial.print(F("Timeout ouverture: "));
      Serial.println(timeoutOuverture);
      break;
      
    case MODE_REGLAGE_TIMEOUT_FERMETURE:
      // Augmenter timeout fermeture par pas de 1s / Increase closing timeout by 1s steps
      timeoutFermeture = min(60, timeoutFermeture + 1);
      sauvegarderTimeoutFermeture();
      debutModeReglage = millis();
      Serial.print(F("Timeout fermeture: "));
      Serial.println(timeoutFermeture);
      break;
  }
}

// ============================================================================
// CALLBACK: DOUBLE CLIC / DOUBLE CLICK
// ============================================================================
/**
 * Callback pour un double-clic sur le bouton
 * Callback for button double-click
 */
void onButtonDoubleClick() {
  Serial.println(F("Double-click détecté / Double-click detected"));
  
  // Si le LCD est éteint, ne rien faire (le premier clic l'a déjà rallumé)
  // If LCD is off, do nothing (first click already turned it on)
  if (!lcdAllume) {
    return;
  }
  
  derniereActivite = millis(); // Réinitialiser le timer d'inactivité / Reset inactivity timer
  
  switch (modeActuel) {
    case MODE_NORMAL:
      // Double-clic en mode normal : éteindre le LCD
      // Double-click in normal mode: turn off LCD
      Serial.println(F("Extinction LCD / LCD off"));
      eteindreLCD();
      break;
      
    case MODE_REGLAGE_SEUIL:
      // Diminuer le seuil par pas de 5 / Decrease threshold by steps of 5
      seuilLumiere = max(0, seuilLumiere - 5);
      sauvegarderSeuil();
      debutModeReglage = millis(); // Reset timeout
      Serial.print(F("Seuil diminué: "));
      Serial.println(seuilLumiere);
      break;
      
    case MODE_REGLAGE_TIMEOUT_OUVERTURE:
      // Diminuer timeout ouverture par pas de 1s / Decrease opening timeout by 1s steps
      timeoutOuverture = max(5, timeoutOuverture - 1);
      sauvegarderTimeoutOuverture();
      debutModeReglage = millis();
      Serial.print(F("Timeout ouverture: "));
      Serial.println(timeoutOuverture);
      break;
      
    case MODE_REGLAGE_TIMEOUT_FERMETURE:
      // Diminuer timeout fermeture par pas de 1s / Decrease closing timeout by 1s steps
      timeoutFermeture = max(5, timeoutFermeture - 1);
      sauvegarderTimeoutFermeture();
      debutModeReglage = millis();
      Serial.print(F("Timeout fermeture: "));
      Serial.println(timeoutFermeture);
      break;
      
    default:
      // Autres modes : ignorer le double-clic
      // Other modes: ignore double-click
      break;
  }
}

// ============================================================================
// CALLBACK: APPUI LONG / LONG PRESS
// ============================================================================
/**
 * Callback pour un appui long sur le bouton
 * Callback for long button press
 */
void onButtonLongPressStart() {
  Serial.println(F("Appui long détecté / Long press detected"));
  
  // Si le LCD est éteint, le rallumer d'abord
  // If LCD is off, turn it on first
  if (!lcdAllume) {
    allumerLCD();
    dernierAllumageMinute = millis();
    return;
  }
  
  derniereActivite = millis(); // Réinitialiser le timer d'inactivité / Reset inactivity timer
  
  switch (modeActuel) {
    case MODE_NORMAL:
      // En cas d'erreur obstacle, un appui long relance
      // In case of obstacle error, long press restarts
      if (etatActuel == ERREUR_OBSTACLE) {
        Serial.println(F("Reset erreur obstacle / Reset obstacle error"));
        etatActuel = ARRET;
      } else {
        Serial.println(F("Passage MODE_REGLAGE_HEURE / Switch to hour setting"));
        modeActuel = MODE_REGLAGE_HEURE;
        debutModeReglage = millis();
        lcd.clear();
      }
      break;
      
    case MODE_REGLAGE_HEURE:
      Serial.println(F("Passage MODE_REGLAGE_MINUTE / Switch to minute setting"));
      modeActuel = MODE_REGLAGE_MINUTE;
      debutModeReglage = millis();
      allumerLCD();
      break;
      
    case MODE_REGLAGE_MINUTE:
      Serial.println(F("Passage MODE_REGLAGE_SEUIL / Switch to threshold setting"));
      modeActuel = MODE_REGLAGE_SEUIL;
      debutModeReglage = millis();
      allumerLCD();
      lcd.clear();
      break;
      
    case MODE_REGLAGE_SEUIL:
      Serial.println(F("Passage MODE_REGLAGE_TIMEOUT_OUVERTURE / Switch to opening timeout setting"));
      modeActuel = MODE_REGLAGE_TIMEOUT_OUVERTURE;
      debutModeReglage = millis();
      allumerLCD(); // Forcer le rallumage du LCD
      lcd.clear();
      break;
      
    case MODE_REGLAGE_TIMEOUT_OUVERTURE:
      Serial.println(F("Passage MODE_REGLAGE_TIMEOUT_FERMETURE / Switch to closing timeout setting"));
      modeActuel = MODE_REGLAGE_TIMEOUT_FERMETURE;
      debutModeReglage = millis();
      allumerLCD(); // Forcer le rallumage du LCD
      lcd.clear();
      break;
      
    case MODE_REGLAGE_TIMEOUT_FERMETURE:
      Serial.println(F("Retour MODE_NORMAL / Back to normal mode"));
      modeActuel = MODE_NORMAL;
      lcd.clear();
      break;
  }
}
