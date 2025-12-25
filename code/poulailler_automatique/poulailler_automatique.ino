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
 * Version: 1.2.0 (refactorisée avec OneButton / refactored with OneButton)
 * Date: 2025
 * Licence / License: MIT
 * 
 * Repo GitHub: https://github.com/[votre-username]/poulailler-automatique
 * 
 * ============================================================================
 * BIBLIOTHÈQUES REQUISES / REQUIRED LIBRARIES:
 * ============================================================================
 * 
 * - RTClib (Adafruit)
 * - LiquidCrystal_I2C
 * - OneButton (Matthias Hertel)
 * 
 * ============================================================================
 */

#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <OneButton.h>
#include "button.h"

// ============================================================================
// CONFIGURATION DES BROCHES / PIN CONFIGURATION
// ============================================================================
const int MOTEUR_PIN1 = 7;        // Contrôle moteur direction 1 / Motor control direction 1
const int MOTEUR_PIN2 = 6;        // Contrôle moteur direction 2 / Motor control direction 2
const int CAPTEUR_LUMIERE = A0;   // Capteur de luminosité / Light sensor
const int FIN_COURSE_HAUT = 8;    // Capteur fin de course haut / TOP limit switch
const int FIN_COURSE_BAS = 9;     // Capteur fin de course bas / BOTTOM limit switch
const int LED_COUPURE = 3;        // LED clignotante coupure courant / Power outage blinking LED

// ============================================================================
// CONSTANTES / CONSTANTS
// ============================================================================
const unsigned long TEMPO_FERMETURE = 600000; // 10 minutes en millisecondes / 10 minutes in milliseconds
const unsigned long TIMEOUT_REGLAGE = 10000;  // 10 secondes timeout mode réglage / 10 seconds settings timeout
const unsigned long INTERVAL_AFFICHAGE_MINUTE = 60000; // 1 minute pour réveil LCD / 1 minute for LCD wake-up
const unsigned long TIMEOUT_LCD = 30000;      // 30 secondes d'inactivité avant extinction LCD / 30 seconds inactivity before LCD off

// Adresses EEPROM / EEPROM addresses
const int SEUIL_EEPROM_ADDR = 0;              // Adresse EEPROM seuil / EEPROM address for threshold
const int TIMEOUT_OUVERTURE_ADDR = 2;        // Adresse EEPROM timeout ouverture / EEPROM address opening timeout
const int TIMEOUT_FERMETURE_ADDR = 4;        // Adresse EEPROM timeout fermeture / EEPROM address closing timeout

// Valeurs par défaut / Default values
const int SEUIL_DEFAULT = 300;                // Valeur par défaut seuil / Default threshold value
const int TIMEOUT_OUVERTURE_DEFAULT = 15;    // 15 secondes par défaut / 15 seconds default
const int TIMEOUT_FERMETURE_DEFAULT = 30;    // 30 secondes par défaut / 30 seconds default

// ============================================================================
// VARIABLES GLOBALES / GLOBAL VARIABLES
// ============================================================================
RTC_DS3231 rtc;                              // Objet RTC / RTC object
LiquidCrystal_I2C lcd(0x27, 16, 2);         // Écran LCD I2C / I2C LCD display
bool porteOuverte = false;                   // État de la porte / Door state
unsigned long dernierTempsLED = 0;          // Dernier changement LED / Last LED change
bool etatLED = false;                        // État LED / LED state
bool coupureCourant = false;                 // Détection coupure courant / Power outage detection
unsigned long dernierTimeCheck = 0;         // Dernière vérification temporelle / Last time check
int seuilLumiere = SEUIL_DEFAULT;           // Seuil de luminosité / Light threshold
int timeoutOuverture = TIMEOUT_OUVERTURE_DEFAULT;  // Timeout ouverture en secondes / Opening timeout in seconds
int timeoutFermeture = TIMEOUT_FERMETURE_DEFAULT;  // Timeout fermeture en secondes / Closing timeout in seconds

// Variables pour la temporisation de fermeture / Variables for closing delay
unsigned long debutLumiereFaible = 0;       // Début détection lumière faible / Start of low light detection
bool lumiereFaibleDetectee = false;         // Flag lumière faible / Low light flag

// Variables pour les modes de réglage / Settings mode variables
ModeReglage modeActuel = MODE_NORMAL;        // Mode actuel / Current mode
unsigned long debutModeReglage = 0;         // Début mode réglage / Settings mode start
bool clignotement = false;                   // État clignotement / Blinking state
unsigned long dernierClignotement = 0;      // Dernier clignotement / Last blink
ModeReglage dernierModeAffiche = MODE_NORMAL; // Pour debug: dernier mode affiché

// Variables pour la gestion du rétroéclairage LCD / LCD backlight management variables
bool lcdAllume = true;                       // État rétroéclairage LCD / LCD backlight state
unsigned long dernierAllumageMinute = 0;    // Dernier allumage minute temporisation / Last minute display activation
unsigned long derniereActivite = 0;         // Dernière activité utilisateur ou système / Last user or system activity

// États de la porte / Door states
EtatPorte etatActuel = ARRET;               // État actuel porte / Current door state

// Variables pour la gestion des timeouts moteur / Motor timeout variables
unsigned long debutMouvementPorte = 0;     // Début mouvement porte / Door movement start

// ============================================================================
// FONCTION SETUP - INITIALISATION / SETUP FUNCTION - INITIALIZATION
// ============================================================================
void setup() {
  Serial.begin(9600);
  
  // Configuration des broches / Pin configuration
  pinMode(MOTEUR_PIN1, OUTPUT);
  pinMode(MOTEUR_PIN2, OUTPUT);
  pinMode(FIN_COURSE_HAUT, INPUT_PULLUP);
  pinMode(FIN_COURSE_BAS, INPUT_PULLUP);
  pinMode(LED_COUPURE, OUTPUT);
  
  // Arrêt du moteur au démarrage / Stop motor at startup
  arreterMoteur();
  
  // Initialisation du LCD / LCD initialization
  lcd.init();
  lcd.backlight();
  lcdAllume = true;
  lcd.setCursor(0, 0);
  lcd.print(F("Poulailler Auto"));
  lcd.setCursor(0, 1);
  lcd.print(F("Init OneButton.."));
  delay(2000);
  
  // Initialisation du bouton avec OneButton / Button initialization with OneButton
  initButton();
  
  lcd.setCursor(0, 1);
  lcd.print(F("Initialisation.."));
  delay(1000);
  
  // Initialisation du RTC / RTC initialization
  if (!rtc.begin()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Erreur RTC!"));
    Serial.println(F("RTC non trouvé ! / RTC not found!"));
    while (1);
  }
  
  // Chargement des paramètres depuis l'EEPROM / Load parameters from EEPROM
  chargerParametresEEPROM();
  
  // Vérification si le RTC a perdu l'alimentation / Check if RTC lost power
  if (rtc.lostPower()) {
    coupureCourant = true;
    Serial.println(F("RTC a perdu l'alimentation / RTC lost power"));
  }
  
  // Déterminer l'état initial de la porte / Determine initial door state
  if (digitalRead(FIN_COURSE_HAUT) == LOW) {
    porteOuverte = true;
    Serial.println(F("Porte détectée ouverte au démarrage / Door detected open at startup"));
  } else if (digitalRead(FIN_COURSE_BAS) == LOW) {
    porteOuverte = false;
    Serial.println(F("Porte détectée fermée au démarrage / Door detected closed at startup"));
  }
  
  Serial.println(F("Système initialisé / System initialized"));
  lcd.clear();
  derniereActivite = millis(); // Initialiser le timer d'activité
}

// ============================================================================
// BOUCLE PRINCIPALE / MAIN LOOP
// ============================================================================
void loop() {
  DateTime maintenant = rtc.now();
  
  // Gestion du bouton avec OneButton / Button management with OneButton
  updateButton();
  
  // Gestion du timeout pour sortir du mode réglage / Settings mode timeout management
  if (modeActuel != MODE_NORMAL && millis() - debutModeReglage > TIMEOUT_REGLAGE) {
    modeActuel = MODE_NORMAL;
    lcd.clear();
  }
  
  // Gestion du clignotement en mode réglage / Blinking management in settings mode
  if (millis() - dernierClignotement > 500) {
    dernierClignotement = millis();
    clignotement = !clignotement;
  }
  
  // Gestion du réveil automatique du LCD pendant la temporisation
  // Automatic LCD wake-up management during closing delay
  if (lumiereFaibleDetectee && !lcdAllume) {
    if (millis() - dernierAllumageMinute >= INTERVAL_AFFICHAGE_MINUTE) {
      allumerLCD();
      dernierAllumageMinute = millis();
    }
  }
  
  // Extinction automatique du LCD après inactivité
  // Automatic LCD shutdown after inactivity
  if (lcdAllume && 
      modeActuel == MODE_NORMAL && 
      etatActuel == ARRET && 
      !lumiereFaibleDetectee && 
      (millis() - derniereActivite > TIMEOUT_LCD)) {
    Serial.println(F(">>> Extinction auto LCD (inactivité)"));
    eteindreLCD();
  }
  
  // Gestion de l'affichage LCD / LCD display management
  if (lcdAllume) {
    gererAffichageLCD(maintenant);
  }
  
  // Gestion de la LED de coupure / Power outage LED management
  gererLEDCoupure();
  
  // Logique automatique seulement en mode normal / Automatic logic only in normal mode
  if (modeActuel == MODE_NORMAL) {
    // Vérifications automatiques toutes les secondes / Automatic checks every second
    if (millis() - dernierTimeCheck > 1000) {
      dernierTimeCheck = millis();
      
      int valeurLumiere = analogRead(CAPTEUR_LUMIERE);
      
      // Condition 1: Ouverture automatique à 7h00 / Automatic opening at 7:00 AM
      if (maintenant.hour() == 7 && maintenant.minute() == 0 && !porteOuverte) {
        Serial.println(F("7h00 - Ouverture automatique / 7:00 AM - Automatic opening"));
        ouvrirPorte();
      }
      
      // Condition 2: Fermeture avec temporisation / Closing with delay
      gererFermetureLumiere(valeurLumiere, maintenant.hour());
      
      // Condition 3: Fermeture forcée à 23h00 / Forced closing at 11:00 PM
      if (maintenant.hour() == 23 && maintenant.minute() == 0 && porteOuverte) {
        Serial.println(F("23h00 - Fermeture forcée / 11:00 PM - Forced closing"));
        fermerPorte();
      }
    }
    
    // Gestion du mouvement de la porte / Door movement management
    gererMouvementPorte();
  }
  
  delay(50);
}

// ============================================================================
// GESTION DU RÉTROÉCLAIRAGE LCD / LCD BACKLIGHT MANAGEMENT
// ============================================================================
void allumerLCD() {
  if (!lcdAllume) {
    lcd.backlight();
    lcdAllume = true;
    Serial.println(F("LCD rallumé / LCD turned on"));
  }
  derniereActivite = millis(); // Mettre à jour l'activité
}

void eteindreLCD() {
  if (lcdAllume) {
    lcd.noBacklight();
    lcdAllume = false;
    Serial.println(F("LCD éteint / LCD turned off"));
  }
}

// ============================================================================
// GESTION DE L'AFFICHAGE LCD / LCD DISPLAY MANAGEMENT
// ============================================================================
void gererAffichageLCD(DateTime maintenant) {
  lcd.setCursor(0, 0);
  
  switch (modeActuel) {
    case MODE_NORMAL:
      // Ligne 1: Heure / Line 1: Time
      if (maintenant.hour() < 10) lcd.print("0");
      lcd.print(maintenant.hour());
      lcd.print(":");
      if (maintenant.minute() < 10) lcd.print("0");
      lcd.print(maintenant.minute());
      lcd.print(F("      "));
      
      // Ligne 2: Statut de la porte / Line 2: Door status
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
      lcd.print(F("Bref:+1 Dbl:-1  "));
      break;
      
    case MODE_REGLAGE_TIMEOUT_FERMETURE:
      lcd.print(F("T.fer: "));
      if (timeoutFermeture < 10) lcd.print(" ");
      lcd.print(timeoutFermeture);
      lcd.print(F("s      "));
      
      lcd.setCursor(0, 1);
      lcd.print(F("Bref:+1 Dbl:-1  "));
      break;
  }
}

// ============================================================================
// GESTION DE LA SAUVEGARDE EEPROM / EEPROM BACKUP MANAGEMENT
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
  // Chargement du seuil / Loading threshold
  int seuilSauve = EEPROM.read(SEUIL_EEPROM_ADDR) | (EEPROM.read(SEUIL_EEPROM_ADDR + 1) << 8);
  if (seuilSauve >= 0 && seuilSauve <= 1023) {
    seuilLumiere = seuilSauve;
  }
  
  // Chargement timeout ouverture / Loading opening timeout
  int timeoutOuv = EEPROM.read(TIMEOUT_OUVERTURE_ADDR);
  if (timeoutOuv >= 5 && timeoutOuv <= 60) {
    timeoutOuverture = timeoutOuv;
  }
  
  // Chargement timeout fermeture / Loading closing timeout
  int timeoutFer = EEPROM.read(TIMEOUT_FERMETURE_ADDR);
  if (timeoutFer >= 5 && timeoutFer <= 60) {
    timeoutFermeture = timeoutFer;
  }
}

// ============================================================================
// CONTRÔLE DU MOTEUR / MOTOR CONTROL
// ============================================================================
void ouvrirPorte() {
  if (!porteOuverte && (etatActuel == ARRET || etatActuel == ERREUR_OBSTACLE)) {
    Serial.println(F("Début ouverture porte / Starting door opening"));
    allumerLCD();
    etatActuel = OUVERTURE;
    debutMouvementPorte = millis();
    digitalWrite(MOTEUR_PIN1, HIGH);
    digitalWrite(MOTEUR_PIN2, LOW);
  }
}

void fermerPorte() {
  if (porteOuverte && (etatActuel == ARRET || etatActuel == ERREUR_OBSTACLE)) {
    Serial.println(F("Début fermeture porte / Starting door closing"));
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
        Serial.println(F("Timeout ouverture - Obstacle détecté / Opening timeout - Obstacle detected"));
        arreterMoteur();
        etatActuel = ERREUR_OBSTACLE;
        allumerLCD();
      }
      else if (digitalRead(FIN_COURSE_HAUT) == LOW) {
        Serial.println(F("Porte complètement ouverte / Door fully open"));
        arreterMoteur();
        porteOuverte = true;
      }
      break;
      
    case FERMETURE:
      if (millis() - debutMouvementPorte > (unsigned long)timeoutFermeture * 1000) {
        Serial.println(F("Timeout fermeture - Obstacle détecté / Closing timeout - Obstacle detected"));
        arreterMoteur();
        etatActuel = ERREUR_OBSTACLE;
        allumerLCD();
      }
      else if (digitalRead(FIN_COURSE_BAS) == LOW) {
        Serial.println(F("Porte complètement fermée / Door fully closed"));
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
// GESTION DE LA LUMINOSITÉ / LIGHT MANAGEMENT
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
      Serial.println(F("Début détection lumière faible - temporisation 10 minutes / Start low light detection - 10 minutes delay"));
    } else if (millis() - debutLumiereFaible >= TEMPO_FERMETURE) {
      Serial.println(F("Temporisation écoulée - Fermeture automatique / Delay elapsed - Automatic closing"));
      fermerPorte();
      lumiereFaibleDetectee = false;
    }
  } else {
    if (lumiereFaibleDetectee) {
      Serial.println(F("Lumière revenue - Annulation temporisation fermeture / Light returned - Closing delay cancelled"));
      lumiereFaibleDetectee = false;
      debutLumiereFaible = 0;
    }
  }
}

// ============================================================================
// GESTION DE LA LED DE COUPURE / POWER OUTAGE LED MANAGEMENT
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

// ============================================================================
// FIN DU PROGRAMME / END OF PROGRAM
// ============================================================================
