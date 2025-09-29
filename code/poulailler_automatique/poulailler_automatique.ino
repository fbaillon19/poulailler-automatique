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
 * Repo GitHub: https://github.com/[votre-username]/poulailler-automatique
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
const int MOTEUR_PIN1 = 7;        // Contrôle moteur direction 1 / Motor control direction 1
const int MOTEUR_PIN2 = 6;        // Contrôle moteur direction 2 / Motor control direction 2
const int CAPTEUR_LUMIERE = A0;   // Capteur de luminosité / Light sensor
const int BOUTON_PIN = 5;         // Bouton multifonction / Multi-function button
const int FIN_COURSE_HAUT = 8;    // Capteur fin de course haut / TOP limit switch
const int FIN_COURSE_BAS = 9;     // Capteur fin de course bas / BOTTOM limit switch
const int LED_COUPURE = 3;        // LED clignotante coupure courant / Power outage blinking LED

// ============================================================================
// CONSTANTES / CONSTANTS
// ============================================================================
const unsigned long TEMPO_FERMETURE = 600000; // 10 minutes en millisecondes / 10 minutes in milliseconds
const unsigned long APPUI_LONG = 3000;        // 3 secondes pour appui long / 3 seconds for long press
const unsigned long TIMEOUT_REGLAGE = 10000;  // 10 secondes timeout mode réglage / 10 seconds settings timeout
const unsigned long DOUBLE_CLIC = 500;        // 500ms pour détecter double-clic / 500ms to detect double-click

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

// Variables pour la gestion du bouton multifonction / Multi-function button variables
unsigned long debutAppui = 0;               // Début d'appui bouton / Button press start
bool boutonEnfonce = false;                  // État bouton enfoncé / Button pressed state
bool appuiTraite = false;                    // Appui déjà traité / Press already processed
unsigned long dernierRelachement = 0;       // Dernier relâchement bouton / Last button release
bool enAttenteDoubleClick = false;          // Attente double-clic / Waiting for double-click

// Variables pour les modes de réglage / Settings mode variables
enum ModeReglage {
  MODE_NORMAL,
  MODE_REGLAGE_HEURE,
  MODE_REGLAGE_MINUTE,
  MODE_REGLAGE_SEUIL,
  MODE_REGLAGE_TIMEOUT_OUVERTURE,
  MODE_REGLAGE_TIMEOUT_FERMETURE
};
ModeReglage modeActuel = MODE_NORMAL;        // Mode actuel / Current mode
unsigned long debutModeReglage = 0;         // Début mode réglage / Settings mode start
bool clignotement = false;                   // État clignotement / Blinking state
unsigned long dernierClignotement = 0;      // Dernier clignotement / Last blink

// États de la porte / Door states
enum EtatPorte {
  ARRET,
  OUVERTURE,
  FERMETURE,
  ERREUR_OBSTACLE
};
EtatPorte etatActuel = ARRET;               // État actuel porte / Current door state

// Variables pour la gestion des timeouts moteur / Motor timeout variables
unsigned long debutMouvementPorte = 0;     // Début mouvement porte / Door movement start

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
  pinMode(MOTEUR_PIN1, OUTPUT);
  pinMode(MOTEUR_PIN2, OUTPUT);
  pinMode(BOUTON_PIN, INPUT_PULLUP);
  pinMode(FIN_COURSE_HAUT, INPUT_PULLUP);
  pinMode(FIN_COURSE_BAS, INPUT_PULLUP);
  pinMode(LED_COUPURE, OUTPUT);
  
  // Arrêt du moteur au démarrage / Stop motor at startup
  arreterMoteur();
  
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
  chargerParametresEEPROM();
  
  // Vérification si le RTC a perdu l'alimentation / Check if RTC lost power
  if (rtc.lostPower()) {
    coupureCourant = true;
    Serial.println(F("RTC a perdu l'alimentation / RTC lost power"));
    // Décommentez la ligne suivante pour régler l'heure automatiquement
    // Uncomment next line to set time automatically
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
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
  DateTime maintenant = rtc.now();
  
  // Gestion du bouton multifonction / Multi-function button management
  gererBoutonMultifonction();
  
  // Gestion du timeout pour sortir du mode réglage / Settings mode timeout management
  if (modeActuel != MODE_NORMAL && millis() - debutModeReglage > TIMEOUT_REGLAGE) {
    modeActuel = MODE_NORMAL;
  }
  
  // Gestion du clignotement en mode réglage / Blinking management in settings mode
  if (millis() - dernierClignotement > 500) {
    dernierClignotement = millis();
    clignotement = !clignotement;
  }
  
  // Gestion de l'affichage LCD / LCD display management
  gererAffichageLCD(maintenant);
  
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
// GESTION DU BOUTON MULTIFONCTION / MULTI-FUNCTION BUTTON MANAGEMENT
// ============================================================================
/**
 * Gère les interactions avec le bouton multifonction
 * Manages multi-function button interactions
 * 
 * Paramètres / Parameters: Aucun / None
 * Retour / Return: void
 */
void gererBoutonMultifonction() {
  bool etatBouton = digitalRead(BOUTON_PIN);
  
  // Détection du début d'appui / Press start detection
  if (!boutonEnfonce && etatBouton == LOW) {
    delay(20); // Anti-rebond / Debounce
    if (digitalRead(BOUTON_PIN) == LOW) {
      boutonEnfonce = true;
      debutAppui = millis();
      appuiTraite = false;
    }
  }
  
  // Détection du relâchement / Release detection
  if (boutonEnfonce && etatBouton == HIGH) {
    delay(20); // Anti-rebond / Debounce
    if (digitalRead(BOUTON_PIN) == HIGH) {
      boutonEnfonce = false;
      unsigned long dureeAppui = millis() - debutAppui;
      
      if (!appuiTraite) {
        if (dureeAppui >= APPUI_LONG) {
          // Appui long / Long press
          gererAppuiLong();
        } else {
          // Appui bref / Short press
          if (enAttenteDoubleClick && millis() - dernierRelachement < DOUBLE_CLIC) {
            // Double-clic détecté / Double-click detected
            gererDoubleClick();
            enAttenteDoubleClick = false;
          } else {
            // Premier appui bref, attendre pour voir s'il y a un double-clic
            // First short press, wait to see if there's a double-click
            dernierRelachement = millis();
            enAttenteDoubleClick = true;
          }
        }
      }
    }
  }
  
  // Gestion du timeout pour le double-clic / Double-click timeout management
  if (enAttenteDoubleClick && millis() - dernierRelachement > DOUBLE_CLIC) {
    gererAppuiBref();
    enAttenteDoubleClick = false;
  }
}

/**
 * Traite les appuis brefs du bouton
 * Processes short button presses
 * 
 * Paramètres / Parameters: Aucun / None
 * Retour / Return: void
 */
void gererAppuiBref() {
  switch (modeActuel) {
    case MODE_NORMAL:
      // Ouvrir/fermer la porte ou réessayer après erreur
      // Open/close door or retry after error
      if (etatActuel == ERREUR_OBSTACLE) {
        // Réessayer le mouvement / Retry movement
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
      }
      break;
      
    case MODE_REGLAGE_SEUIL:
      // Augmenter le seuil par pas de 5 / Increase threshold by steps of 5
      seuilLumiere = min(1023, seuilLumiere + 5);
      sauvegarderSeuil();
      debutModeReglage = millis(); // Reset timeout
      break;
      
    case MODE_REGLAGE_TIMEOUT_OUVERTURE:
      // Augmenter timeout ouverture par pas de 1s / Increase opening timeout by 1s steps
      timeoutOuverture = min(60, timeoutOuverture + 1);
      sauvegarderTimeoutOuverture();
      debutModeReglage = millis();
      break;
      
    case MODE_REGLAGE_TIMEOUT_FERMETURE:
      // Augmenter timeout fermeture par pas de 1s / Increase closing timeout by 1s steps
      timeoutFermeture = min(60, timeoutFermeture + 1);
      sauvegarderTimeoutFermeture();
      debutModeReglage = millis();
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
void gererDoubleClick() {
  switch (modeActuel) {
    case MODE_REGLAGE_SEUIL:
      // Diminuer le seuil par pas de 5 / Decrease threshold by steps of 5
      seuilLumiere = max(0, seuilLumiere - 5);
      sauvegarderSeuil();
      debutModeReglage = millis(); // Reset timeout
      break;
      
    case MODE_REGLAGE_TIMEOUT_OUVERTURE:
      // Diminuer timeout ouverture par pas de 1s / Decrease opening timeout by 1s steps
      timeoutOuverture = max(5, timeoutOuverture - 1);
      sauvegarderTimeoutOuverture();
      debutModeReglage = millis();
      break;
      
    case MODE_REGLAGE_TIMEOUT_FERMETURE:
      // Diminuer timeout fermeture par pas de 1s / Decrease closing timeout by 1s steps
      timeoutFermeture = max(5, timeoutFermeture - 1);
      sauvegarderTimeoutFermeture();
      debutModeReglage = millis();
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
void gererAppuiLong() {
  switch (modeActuel) {
    case MODE_NORMAL:
      // En cas d'erreur obstacle, un appui long relance
      // In case of obstacle error, long press restarts
      if (etatActuel == ERREUR_OBSTACLE) {
        etatActuel = ARRET;
      } else {
        modeActuel = MODE_REGLAGE_HEURE;
        debutModeReglage = millis();
      }
      break;
      
    case MODE_REGLAGE_HEURE:
      modeActuel = MODE_REGLAGE_MINUTE;
      debutModeReglage = millis();
      break;
      
    case MODE_REGLAGE_MINUTE:
      modeActuel = MODE_REGLAGE_SEUIL;
      debutModeReglage = millis();
      break;
      
    case MODE_REGLAGE_SEUIL:
      modeActuel = MODE_REGLAGE_TIMEOUT_OUVERTURE;
      debutModeReglage = millis();
      break;
      
    case MODE_REGLAGE_TIMEOUT_OUVERTURE:
      modeActuel = MODE_REGLAGE_TIMEOUT_FERMETURE;
      debutModeReglage = millis();
      break;
      
    case MODE_REGLAGE_TIMEOUT_FERMETURE:
      modeActuel = MODE_NORMAL;
      break;
  }
  appuiTraite = true;
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
void gererAffichageLCD(DateTime maintenant) {
  lcd.setCursor(0, 0);
  
  switch (modeActuel) {
    case MODE_NORMAL:
      // Ligne 1: Heure / Line 1: Time
      if (maintenant.hour() < 10) lcd.print(F("0"));
      lcd.print(maintenant.hour());
      lcd.print(F(":"));
      if (maintenant.minute() < 10) lcd.print(F("0"));
      lcd.print(maintenant.minute());
      lcd.print(F("      ")); // Effacer le reste / Clear remainder
      
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
        if (maintenant.hour() < 10) lcd.print(F("0"));
        lcd.print(maintenant.hour());
        lcd.print(F(":"));
      }
      if (maintenant.minute() < 10) lcd.print(F("0"));
      lcd.print(maintenant.minute());
      lcd.print(F("      "));
      
      lcd.setCursor(0, 1);
      lcd.print(F("Reglage heure   "));
      break;
      
    case MODE_REGLAGE_MINUTE:
      if (maintenant.hour() < 10) lcd.print(F("0"));
      lcd.print(maintenant.hour());
      lcd.print(F(":"));
      if (clignotement) {
        lcd.print(F("  "));
      } else {
        if (maintenant.minute() < 10) lcd.print(F("0"));
        lcd.print(maintenant.minute());
      }
      lcd.print(F("      "));
      
      lcd.setCursor(0, 1);
      lcd.print(F("Reglage minute  "));
      break;
      
    case MODE_REGLAGE_SEUIL:
      lcd.print(F("Seuil: "));
      lcd.print(seuilLumiere);
      lcd.print(F("     "));
      
      lcd.setCursor(0, 1);
      int valeurActuelle = analogRead(CAPTEUR_LUMIERE);
      lcd.print(F("Actuel: "));
      lcd.print(valeurActuelle);
      lcd.print(F("     "));
      break;
      
    case MODE_REGLAGE_TIMEOUT_OUVERTURE:
      lcd.print(F("Timeout ouv: "));
      lcd.print(timeoutOuverture);
      lcd.print(F("s "));
      
      lcd.setCursor(0, 1);
      lcd.print(F("Bref:+1 Dbl:-1  "));
      break;
      
    case MODE_REGLAGE_TIMEOUT_FERMETURE:
      lcd.print(F("Timeout fer: "));
      lcd.print(timeoutFermeture);
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
void sauvegarderSeuil() {
  EEPROM.write(SEUIL_EEPROM_ADDR, seuilLumiere & 0xFF);
  EEPROM.write(SEUIL_EEPROM_ADDR + 1, (seuilLumiere >> 8) & 0xFF);
}

/**
 * Sauvegarde le timeout d'ouverture en EEPROM
 * Saves opening timeout to EEPROM
 * 
 * Paramètres / Parameters: Aucun / None
 * Retour / Return: void
 */
void sauvegarderTimeoutOuverture() {
  EEPROM.write(TIMEOUT_OUVERTURE_ADDR, timeoutOuverture);
}

/**
 * Sauvegarde le timeout de fermeture en EEPROM
 * Saves closing timeout to EEPROM
 * 
 * Paramètres / Parameters: Aucun / None
 * Retour / Return: void
 */
void sauvegarderTimeoutFermeture() {
  EEPROM.write(TIMEOUT_FERMETURE_ADDR, timeoutFermeture);
}

/**
 * Charge tous les paramètres depuis l'EEPROM
 * Loads all parameters from EEPROM
 * 
 * Paramètres / Parameters: Aucun / None
 * Retour / Return: void
 */
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
/**
 * Démarre l'ouverture de la porte
 * Starts door opening
 * 
 * Paramètres / Parameters: Aucun / None
 * Retour / Return: void
 */
void ouvrirPorte() {
  if (!porteOuverte && (etatActuel == ARRET || etatActuel == ERREUR_OBSTACLE)) {
    Serial.println(F("Début ouverture porte / Starting door opening"));
    etatActuel = OUVERTURE;
    debutMouvementPorte = millis();
    digitalWrite(MOTEUR_PIN1, HIGH);
    digitalWrite(MOTEUR_PIN2, LOW);
  }
}

/**
 * Démarre la fermeture de la porte
 * Starts door closing
 * 
 * Paramètres / Parameters: Aucun / None
 * Retour / Return: void
 */
void fermerPorte() {
  if (porteOuverte && (etatActuel == ARRET || etatActuel == ERREUR_OBSTACLE)) {
    Serial.println(F("Début fermeture porte / Starting door closing"));
    etatActuel = FERMETURE;
    debutMouvementPorte = millis();
    digitalWrite(MOTEUR_PIN1, LOW);
    digitalWrite(MOTEUR_PIN2, HIGH);
  }
}

/**
 * Arrête le moteur
 * Stops the motor
 * 
 * Paramètres / Parameters: Aucun / None
 * Retour / Return: void
 */
void arreterMoteur() {
  digitalWrite(MOTEUR_PIN1, LOW);
  digitalWrite(MOTEUR_PIN2, LOW);
  etatActuel = ARRET;
}

/**
 * Gère le mouvement de la porte et la détection d'obstacles
 * Manages door movement and obstacle detection
 * 
 * Paramètres / Parameters: Aucun / None
 * Retour / Return: void
 */
void gererMouvementPorte() {
  switch (etatActuel) {
    case OUVERTURE:
      // Vérifier timeout / Check timeout
      if (millis() - debutMouvementPorte > (unsigned long)timeoutOuverture * 1000) {
        Serial.println(F("Timeout ouverture - Obstacle détecté / Opening timeout - Obstacle detected"));
        arreterMoteur();
        etatActuel = ERREUR_OBSTACLE;
      }
      // Vérifier fin de course / Check limit switch
      else if (digitalRead(FIN_COURSE_HAUT) == LOW) {
        Serial.println(F("Porte complètement ouverte / Door fully open"));
        arreterMoteur();
        porteOuverte = true;
      }
      break;
      
    case FERMETURE:
      // Vérifier timeout / Check timeout
      if (millis() - debutMouvementPorte > (unsigned long)timeoutFermeture * 1000) {
        Serial.println(F("Timeout fermeture - Obstacle détecté / Closing timeout - Obstacle detected"));
        arreterMoteur();
        etatActuel = ERREUR_OBSTACLE;
      }
      // Vérifier fin de course / Check limit switch
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
/**
 * Gère la fermeture automatique basée sur la luminosité avec temporisation
 * Manages automatic closing based on light level with delay
 * 
 * Paramètres / Parameters:
 *   - valeurLumiere: int - Valeur actuelle du capteur / Current sensor value
 *   - heureActuelle: int - Heure actuelle / Current hour
 * Retour / Return: void
 */
void gererFermetureLumiere(int valeurLumiere, int heureActuelle) {
  // Vérifier si conditions sont réunies / Check if conditions are met
  if (!porteOuverte || heureActuelle <= 7) {
    lumiereFaibleDetectee = false;
    debutLumiereFaible = 0;
    return;
  }
  
  if (valeurLumiere < seuilLumiere) {
    if (!lumiereFaibleDetectee) {
      // Début de la temporisation / Start of delay
      lumiereFaibleDetectee = true;
      debutLumiereFaible = millis();
      Serial.println(F("Début détection lumière faible - temporisation 10 minutes / Start low light detection - 10 minutes delay"));
    } else if (millis() - debutLumiereFaible >= TEMPO_FERMETURE) {
      // Temporisation écoulée / Delay elapsed
      Serial.println(F("Temporisation écoulée - Fermeture automatique / Delay elapsed - Automatic closing"));
      fermerPorte();
      lumiereFaibleDetectee = false;
    }
  } else {
    // La lumière est revenue / Light has returned
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
/**
 * Gère le clignotement de la LED en cas de coupure de courant
 * Manages LED blinking in case of power outage
 * 
 * Paramètres / Parameters: Aucun / None
 * Retour / Return: void
 */
void gererLEDCoupure() {
  if (coupureCourant) {
    // Faire clignoter la LED toutes les 500ms / Blink LED every 500ms
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
 * GitHub: https://github.com/[votre-username]/poulailler-automatique
 * Issues: Pour les bugs et suggestions / For bugs and suggestions
 * Wiki: Documentation détaillée / Detailed documentation
 * 
 * Licence MIT - Libre d'utilisation et modification
 * MIT License - Free to use and modify
 * 
 * ============================================================================
 */