/*
 * ============================================================================
 * POULAILLER AUTOMATIQUE v1.3.0 - Avec Encodeur Rotatif KY-040
 * ============================================================================
 * 
 * Description: Système automatisé de porte de poulailler avec interface
 *              encodeur rotatif pour configuration intuitive
 * 
 * Auteur / Author: Frédéric BAILLON
 * Version: 1.3.0
 * Date: Décembre 2025
 * Licence / License: MIT
 * 
 * ============================================================================
 * NOUVEAUTÉS v1.3.0:
 * ============================================================================
 * 
 * - Encodeur rotatif KY-040 pour navigation intuitive
 * - Rotation pour modifier les valeurs (heure, seuil, timeouts)
 * - Click bref : ouvrir/fermer porte (ou rallumer LCD)
 * - Click long : navigation dans les modes de réglage
 * - Timeout réglage 30s avec extinction automatique LCD
 * - Simplification du code et meilleure réactivité
 * 
 * ============================================================================
 * BIBLIOTHÈQUES REQUISES:
 * ============================================================================
 * 
 * - RTClib (Adafruit)
 * - LiquidCrystal_I2C
 * 
 * ============================================================================
 */

#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include "rotary_encoder.h"

// ============================================================================
// CONFIGURATION DES BROCHES / PIN CONFIGURATION
// ============================================================================
const int MOTEUR_PIN1 = 7;        // Contrôle moteur direction 1
const int MOTEUR_PIN2 = 6;        // Contrôle moteur direction 2
const int CAPTEUR_LUMIERE = A0;   // Capteur de luminosité
const int FIN_COURSE_HAUT = 8;    // Fin de course HAUT
const int FIN_COURSE_BAS = 9;     // Fin de course BAS
const int LED_COUPURE = 3;        // LED alerte coupure courant

// Pins encodeur rotatif (définis dans rotary_encoder.cpp):
// ENCODER_CLK = 2 (interruption)
// ENCODER_DT = 4
// ENCODER_SW = 5

// ============================================================================
// CONSTANTES / CONSTANTS
// ============================================================================
const unsigned long TEMPO_FERMETURE = 600000;  // 10 minutes
const unsigned long TIMEOUT_REGLAGE = 30000;   // 30 secondes timeout réglage
const unsigned long INTERVAL_AFFICHAGE_MINUTE = 60000; // 1 minute réveil LCD
const unsigned long TIMEOUT_LCD = 30000;       // 30 secondes avant extinction LCD
const unsigned long INTERVALLE_RAFRAICHISSEMENT_LCD = 200; // Rafraîchir LCD toutes les 200ms

// Adresses EEPROM
const int SEUIL_EEPROM_ADDR = 0;
const int TIMEOUT_OUVERTURE_ADDR = 2;
const int TIMEOUT_FERMETURE_ADDR = 4;

// Valeurs par défaut
const int SEUIL_DEFAULT = 300;
const int TIMEOUT_OUVERTURE_DEFAULT = 15;
const int TIMEOUT_FERMETURE_DEFAULT = 30;

// ============================================================================
// VARIABLES GLOBALES / GLOBAL VARIABLES
// ============================================================================
RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);

bool porteOuverte = false;
unsigned long dernierTempsLED = 0;
bool etatLED = false;
bool coupureCourant = false;
unsigned long dernierTimeCheck = 0;

int seuilLumiere = SEUIL_DEFAULT;
int timeoutOuverture = TIMEOUT_OUVERTURE_DEFAULT;
int timeoutFermeture = TIMEOUT_FERMETURE_DEFAULT;

// Temporisation fermeture
unsigned long debutLumiereFaible = 0;
bool lumiereFaibleDetectee = false;

// Modes de réglage
ModeReglage modeActuel = MODE_NORMAL;
unsigned long debutModeReglage = 0;
bool clignotement = false;
unsigned long dernierClignotement = 0;

// Gestion LCD
bool lcdAllume = true;
unsigned long dernierAllumageMinute = 0;
unsigned long derniereActivite = 0;
unsigned long dernierRafraichissementLCD = 0;

// États porte
EtatPorte etatActuel = ARRET;
unsigned long debutMouvementPorte = 0;

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  Serial.begin(9600);
  
  // Configuration pins
  pinMode(MOTEUR_PIN1, OUTPUT);
  pinMode(MOTEUR_PIN2, OUTPUT);
  pinMode(FIN_COURSE_HAUT, INPUT_PULLUP);
  pinMode(FIN_COURSE_BAS, INPUT_PULLUP);
  pinMode(LED_COUPURE, OUTPUT);
  
  arreterMoteur();
  
  // Initialisation LCD
  lcd.init();
  lcd.backlight();
  lcdAllume = true;
  lcd.setCursor(0, 0);
  lcd.print(F("Poulailler v1.3"));
  lcd.setCursor(0, 1);
  lcd.print(F("Encodeur KY-040"));
  delay(2000);
  
  // Initialisation encodeur rotatif
  initRotaryEncoder();
  
  lcd.setCursor(0, 1);
  lcd.print(F("Init RTC...     "));
  
  // Initialisation RTC
  if (!rtc.begin()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Erreur RTC!"));
    Serial.println(F("RTC non trouvé !"));
    while (1);
  }
  
  // Charger paramètres EEPROM
  chargerParametresEEPROM();
  
  // Vérifier perte alimentation RTC
  if (rtc.lostPower()) {
    coupureCourant = true;
    Serial.println(F("RTC a perdu l'alimentation"));
  }
  
  // Déterminer état initial porte
  if (digitalRead(FIN_COURSE_HAUT) == LOW) {
    porteOuverte = true;
    Serial.println(F("Porte ouverte au démarrage"));
  } else if (digitalRead(FIN_COURSE_BAS) == LOW) {
    porteOuverte = false;
    Serial.println(F("Porte fermée au démarrage"));
  }
  
  Serial.println(F("Système initialisé"));
  lcd.clear();
  derniereActivite = millis();
}

// ============================================================================
// LOOP
// ============================================================================
void loop() {
  DateTime maintenant = rtc.now();
  
  // Mise à jour encodeur rotatif
  updateRotaryEncoder();
  
  // Timeout mode réglage → retour mode normal + extinction LCD
  if (modeActuel != MODE_NORMAL && millis() - debutModeReglage > TIMEOUT_REGLAGE) {
    Serial.println(F("Timeout réglage → MODE_NORMAL"));
    modeActuel = MODE_NORMAL;
    lcd.clear();
    eteindreLCD();
  }
  
  // Clignotement en mode réglage
  if (millis() - dernierClignotement > 500) {
    dernierClignotement = millis();
    clignotement = !clignotement;
  }
  
  // Réveil automatique LCD pendant temporisation fermeture
  if (lumiereFaibleDetectee && !lcdAllume) {
    if (millis() - dernierAllumageMinute >= INTERVAL_AFFICHAGE_MINUTE) {
      allumerLCD();
      dernierAllumageMinute = millis();
    }
  }
  
  // Extinction automatique LCD (seulement en mode normal)
  if (lcdAllume && 
      modeActuel == MODE_NORMAL && 
      etatActuel == ARRET && 
      !lumiereFaibleDetectee && 
      (millis() - derniereActivite > TIMEOUT_LCD)) {
    Serial.println(F("Extinction auto LCD"));
    eteindreLCD();
  }
  
  // Rafraîchissement LCD (throttlé à 200ms)
  if (lcdAllume && (millis() - dernierRafraichissementLCD >= INTERVALLE_RAFRAICHISSEMENT_LCD)) {
    dernierRafraichissementLCD = millis();
    gererAffichageLCD(maintenant);
  }
  
  // Gestion LED coupure
  gererLEDCoupure();
  
  // Logique automatique (seulement en mode normal)
  if (modeActuel == MODE_NORMAL) {
    if (millis() - dernierTimeCheck > 1000) {
      dernierTimeCheck = millis();
      
      int valeurLumiere = analogRead(CAPTEUR_LUMIERE);
      
      // Ouverture automatique 7h00
      if (maintenant.hour() == 7 && maintenant.minute() == 0 && !porteOuverte) {
        Serial.println(F("7h00 - Ouverture automatique"));
        ouvrirPorte();
      }
      
      // Fermeture avec temporisation
      gererFermetureLumiere(valeurLumiere, maintenant.hour());
      
      // Fermeture forcée 23h00
      if (maintenant.hour() == 23 && maintenant.minute() == 0 && porteOuverte) {
        Serial.println(F("23h00 - Fermeture forcée"));
        fermerPorte();
      }
    }
    
    // Gestion mouvement porte
    gererMouvementPorte();
  }
  
  delay(10); // Loop rapide pour encoder rotatif
}

// ============================================================================
// FONCTIONS LCD
// ============================================================================
void allumerLCD() {
  if (!lcdAllume) {
    lcd.backlight();
    lcdAllume = true;
    Serial.println(F("LCD allumé"));
  }
  derniereActivite = millis();
}

void eteindreLCD() {
  if (lcdAllume) {
    lcd.noBacklight();
    lcdAllume = false;
    Serial.println(F("LCD éteint"));
  }
}

void gererAffichageLCD(DateTime maintenant) {
  lcd.setCursor(0, 0);
  
  switch (modeActuel) {
    case MODE_NORMAL:
      // Ligne 1: Heure
      if (maintenant.hour() < 10) lcd.print("0");
      lcd.print(maintenant.hour());
      lcd.print(":");
      if (maintenant.minute() < 10) lcd.print("0");
      lcd.print(maintenant.minute());
      lcd.print(F("      "));
      
      // Ligne 2: Statut porte
      lcd.setCursor(0, 1);
      if (etatActuel == OUVERTURE) {
        lcd.print(F("Ouverture...    "));
      } else if (etatActuel == FERMETURE) {
        lcd.print(F("Fermeture...    "));
      } else if (lumiereFaibleDetectee) {
        unsigned long tempsRestant = (TEMPO_FERMETURE - (millis() - debutLumiereFaible)) / 60000;
        lcd.print(F("Ferme dans "));
        lcd.print(tempsRestant + 1);
        lcd.print(F("mn "));
      } else if (etatActuel == ERREUR_OBSTACLE) {
        lcd.print(F("ERREUR OBSTACLE "));
      } else {
        lcd.print(porteOuverte ? F("Porte ouverte   ") : F("Porte fermee    "));
      }
      break;
      
    case MODE_REGLAGE_HEURE:
      if (clignotement) {
        lcd.print(F("  :"));
      } else {
        if (maintenant.hour() < 10) lcd.print("0");
        lcd.print(maintenant.hour());
        lcd.print(":");
      }
      if (maintenant.minute() < 10) lcd.print("0");
      lcd.print(maintenant.minute());
      lcd.print(F("      "));
      lcd.setCursor(0, 1);
      lcd.print(F("Reglage heure   "));
      break;
      
    case MODE_REGLAGE_MINUTE:
      if (maintenant.hour() < 10) lcd.print("0");
      lcd.print(maintenant.hour());
      lcd.print(":");
      if (clignotement) {
        lcd.print(F("  "));
      } else {
        if (maintenant.minute() < 10) lcd.print("0");
        lcd.print(maintenant.minute());
      }
      lcd.print(F("      "));
      lcd.setCursor(0, 1);
      lcd.print(F("Reglage minute  "));
      break;
      
    case MODE_REGLAGE_SEUIL:
      {
        lcd.print(F("Seuil: "));
        lcd.print(seuilLumiere);
        lcd.print(F("     "));
        lcd.setCursor(0, 1);
        int valeurActuelle = analogRead(CAPTEUR_LUMIERE);
        lcd.print(F("Actuel: "));
        lcd.print(valeurActuelle);
        lcd.print(F("     "));
      }
      break;
      
    case MODE_REGLAGE_TIMEOUT_OUVERTURE:
      lcd.print(F("T.ouv: "));
      if (timeoutOuverture < 10) lcd.print(" ");
      lcd.print(timeoutOuverture);
      lcd.print(F("s      "));
      lcd.setCursor(0, 1);
      lcd.print(F("Tourner encodeur"));
      break;
      
    case MODE_REGLAGE_TIMEOUT_FERMETURE:
      lcd.print(F("T.fer: "));
      if (timeoutFermeture < 10) lcd.print(" ");
      lcd.print(timeoutFermeture);
      lcd.print(F("s      "));
      lcd.setCursor(0, 1);
      lcd.print(F("Tourner encodeur"));
      break;
  }
}

// ============================================================================
// FONCTIONS EEPROM
// ============================================================================
void sauvegarderSeuil() {
  EEPROM.write(SEUIL_EEPROM_ADDR, seuilLumiere & 0xFF);
  EEPROM.write(SEUIL_EEPROM_ADDR + 1, (seuilLumiere >> 8) & 0xFF);
}

void sauvegarderTimeoutOuverture() {
  EEPROM.write(TIMEOUT_OUVERTURE_ADDR, timeoutOuverture);
}

void sauvegarderTimeoutFermeture() {
  EEPROM.write(TIMEOUT_FERMETURE_ADDR, timeoutFermeture);
}

void chargerParametresEEPROM() {
  int seuilSauve = EEPROM.read(SEUIL_EEPROM_ADDR) | (EEPROM.read(SEUIL_EEPROM_ADDR + 1) << 8);
  if (seuilSauve >= 0 && seuilSauve <= 1023) {
    seuilLumiere = seuilSauve;
  }
  
  int timeoutOuv = EEPROM.read(TIMEOUT_OUVERTURE_ADDR);
  if (timeoutOuv >= 5 && timeoutOuv <= 60) {
    timeoutOuverture = timeoutOuv;
  }
  
  int timeoutFer = EEPROM.read(TIMEOUT_FERMETURE_ADDR);
  if (timeoutFer >= 5 && timeoutFer <= 60) {
    timeoutFermeture = timeoutFer;
  }
}

// ============================================================================
// FONCTIONS MOTEUR
// ============================================================================
void ouvrirPorte() {
  if (!porteOuverte && (etatActuel == ARRET || etatActuel == ERREUR_OBSTACLE)) {
    Serial.println(F("Début ouverture porte"));
    allumerLCD();
    etatActuel = OUVERTURE;
    debutMouvementPorte = millis();
    digitalWrite(MOTEUR_PIN1, HIGH);
    digitalWrite(MOTEUR_PIN2, LOW);
  }
}

void fermerPorte() {
  if (porteOuverte && (etatActuel == ARRET || etatActuel == ERREUR_OBSTACLE)) {
    Serial.println(F("Début fermeture porte"));
    allumerLCD();
    etatActuel = FERMETURE;
    debutMouvementPorte = millis();
    digitalWrite(MOTEUR_PIN1, LOW);
    digitalWrite(MOTEUR_PIN2, HIGH);
  }
}

void arreterMoteur() {
  digitalWrite(MOTEUR_PIN1, LOW);
  digitalWrite(MOTEUR_PIN2, LOW);
  etatActuel = ARRET;
}

void gererMouvementPorte() {
  switch (etatActuel) {
    case OUVERTURE:
      if (millis() - debutMouvementPorte > (unsigned long)timeoutOuverture * 1000) {
        Serial.println(F("Timeout ouverture - Obstacle"));
        arreterMoteur();
        etatActuel = ERREUR_OBSTACLE;
        allumerLCD();
      }
      else if (digitalRead(FIN_COURSE_HAUT) == LOW) {
        Serial.println(F("Porte ouverte"));
        arreterMoteur();
        porteOuverte = true;
      }
      break;
      
    case FERMETURE:
      if (millis() - debutMouvementPorte > (unsigned long)timeoutFermeture * 1000) {
        Serial.println(F("Timeout fermeture - Obstacle"));
        arreterMoteur();
        etatActuel = ERREUR_OBSTACLE;
        allumerLCD();
      }
      else if (digitalRead(FIN_COURSE_BAS) == LOW) {
        Serial.println(F("Porte fermée"));
        arreterMoteur();
        porteOuverte = false;
      }
      break;
      
    case ARRET:
    case ERREUR_OBSTACLE:
      break;
  }
}

// ============================================================================
// GESTION LUMINOSITÉ
// ============================================================================
void gererFermetureLumiere(int valeurLumiere, int heureActuelle) {
  if (!porteOuverte || heureActuelle <= 7) {
    lumiereFaibleDetectee = false;
    debutLumiereFaible = 0;
    return;
  }
  
  if (valeurLumiere < seuilLumiere) {
    if (!lumiereFaibleDetectee) {
      lumiereFaibleDetectee = true;
      debutLumiereFaible = millis();
      dernierAllumageMinute = millis();
      allumerLCD();
      Serial.println(F("Début temporisation fermeture 10 min"));
    } else if (millis() - debutLumiereFaible >= TEMPO_FERMETURE) {
      Serial.println(F("Temporisation écoulée - Fermeture auto"));
      fermerPorte();
      lumiereFaibleDetectee = false;
    }
  } else {
    if (lumiereFaibleDetectee) {
      Serial.println(F("Lumière revenue - Annulation"));
      lumiereFaibleDetectee = false;
      debutLumiereFaible = 0;
    }
  }
}

// ============================================================================
// LED COUPURE
// ============================================================================
void gererLEDCoupure() {
  if (coupureCourant) {
    if (millis() - dernierTempsLED > 500) {
      dernierTempsLED = millis();
      etatLED = !etatLED;
      digitalWrite(LED_COUPURE, etatLED);
    }
  } else {
    digitalWrite(LED_COUPURE, LOW);
  }
}
