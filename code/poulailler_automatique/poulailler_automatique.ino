/*
 * ============================================================================
 * POULAILLER AUTOMATIQUE v1.4.0 - Version production terrain
 * ============================================================================
 * 
 * Description: Système automatisé de porte de poulailler avec interface
 *              encodeur rotatif et gestion d'erreurs complète
 * 
 * Auteur / Author: Frédéric BAILLON
 * Version: 1.4.0
 * Date: Janvier 2026
 * Licence / License: MIT
 * 
 * ============================================================================
 * NOUVEAUTÉS v1.4.0:
 * ============================================================================
 * 
 * INTERFACE:
 * - Appui long réduit à 1.5s (au lieu de 3s)
 * - Navigation modes par appui BREF (au lieu d'appui long)
 * - Affichage luminosité en temps réel (alterne avec heure toutes les 3s)
 * - Acquittement erreur par appui TRÈS long (5s)
 * 
 * NOUVEAUX RÉGLAGES:
 * - Heure d'ouverture paramétrable (6h-9h, heure + minutes)
 * - Heure minimum fermeture (15h-17h, ne jamais fermer avant)
 * - Heure fermeture de secours (heure + minutes)
 * - Temporisation fermeture paramétrable (10-30 minutes)
 * - Seuil par défaut abaissé à 100 (au lieu de 300)
 * 
 * GESTION D'ERREURS:
 * - Calibration porte au démarrage (fermeture puis ouverture)
 * - Détection capteurs fin de course incohérents
 * - Détection capteur luminosité HS (valeurs extrêmes)
 * - Détection RTC aberrant ou figé
 * - Détection tension alimentation faible
 * - LED clignotement différencié (200ms=critique, 1s=alerte)
 * - Arrêt automatique en cas d'erreur critique
 * 
 * ============================================================================
 * BIBLIOTHÈQUES REQUISES:
 * ============================================================================
 * 
 * - RTClib (Adafruit)
 * - LiquidCrystal_I2C
 * - Encoder (Paul Stoffregen)
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
const int MOTEUR_PIN1 = 7;
const int MOTEUR_PIN2 = 6;
const int CAPTEUR_LUMIERE = A0;
const int FIN_COURSE_HAUT = 8;
const int FIN_COURSE_BAS = 9;
const int LED_COUPURE = 3;

// ============================================================================
// CONSTANTES / CONSTANTS
// ============================================================================
const unsigned long TIMEOUT_REGLAGE = 30000;   // 30s timeout réglage
const unsigned long INTERVAL_AFFICHAGE_MINUTE = 60000; // 1 min réveil LCD
const unsigned long TIMEOUT_LCD = 30000;       // 30s avant extinction LCD
const unsigned long INTERVALLE_RAFRAICHISSEMENT_LCD = 200; // 200ms rafraîchissement

// Adresses EEPROM (nouvelles adresses pour v1.4.0)
const int HEURE_OUVERTURE_ADDR = 0;         // Heure ouverture
const int MINUTE_OUVERTURE_ADDR = 1;        // Minute ouverture
const int HEURE_MIN_FERMETURE_ADDR = 2;     // Heure minimum fermeture
const int MINUTE_MIN_FERMETURE_ADDR = 3;    // Minute minimum fermeture
const int HEURE_MAX_FERMETURE_ADDR = 4;     // Heure fermeture secours
const int MINUTE_MAX_FERMETURE_ADDR = 5;    // Minute fermeture secours
const int SEUIL_EEPROM_ADDR = 6;            // Seuil luminosité (2 bytes)
const int TEMPO_FERMETURE_ADDR = 8;         // Temporisation fermeture (minutes)
const int TIMEOUT_OUVERTURE_ADDR = 9;       // Timeout ouverture (secondes)
const int TIMEOUT_FERMETURE_ADDR = 10;      // Timeout fermeture (secondes)

// Valeurs par défaut
const int HEURE_OUVERTURE_DEFAULT = 7;
const int MINUTE_OUVERTURE_DEFAULT = 0;
const int HEURE_MIN_FERMETURE_DEFAULT = 16;
const int MINUTE_MIN_FERMETURE_DEFAULT = 0;
const int HEURE_MAX_FERMETURE_DEFAULT = 23;
const int MINUTE_MAX_FERMETURE_DEFAULT = 0;
const int SEUIL_DEFAULT = 100;              // Abaissé de 300 à 100
const int TEMPO_FERMETURE_DEFAULT = 10;     // 10 minutes
const int TIMEOUT_OUVERTURE_DEFAULT = 15;   // 15 secondes
const int TIMEOUT_FERMETURE_DEFAULT = 30;   // 30 secondes

// ============================================================================
// TYPES D'ERREURS / ERROR TYPES
// ============================================================================
enum TypeErreur : uint8_t {
  AUCUNE_ERREUR = 0,
  
  // CRITIQUES (arrêt complet automatique)
  ERREUR_CAPTEURS_INCOHERENTS = 1,
  ERREUR_CAPTEUR_FIN_COURSE_HS = 2,
  ERREUR_RTC_ABERRANT = 3,
  ERREUR_RTC_FIGE = 4,
  
  // ALERTES (fonctionnement dégradé)
  ALERTE_CAPTEUR_LUMIERE = 10,
  ALERTE_TENSION_FAIBLE = 11
};

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

// Paramètres configurables
int heureOuverture = HEURE_OUVERTURE_DEFAULT;
int minuteOuverture = MINUTE_OUVERTURE_DEFAULT;
int heureMinFermeture = HEURE_MIN_FERMETURE_DEFAULT;
int minuteMinFermeture = MINUTE_MIN_FERMETURE_DEFAULT;
int heureMaxFermeture = HEURE_MAX_FERMETURE_DEFAULT;
int minuteMaxFermeture = MINUTE_MAX_FERMETURE_DEFAULT;
int seuilLumiere = SEUIL_DEFAULT;
int tempoFermetureMinutes = TEMPO_FERMETURE_DEFAULT;
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

// Gestion erreurs
TypeErreur erreurActuelle = AUCUNE_ERREUR;
bool modeAutomatiqueActif = true;

// Affichage luminosité
bool afficherLuminosite = false;
unsigned long dernierAlternanceLCD = 0;

// Vérification RTC
DateTime derniere_datetime_rtc;
unsigned long derniere_verification_rtc = 0;
bool rtc_verification_initialisee = false;

// ============================================================================
// FONCTION LECTURE TENSION / VCC READING FUNCTION
// ============================================================================
long readVcc() {
  long result;
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2);
  ADCSRA |= _BV(ADSC);
  while (bit_is_set(ADCSRA, ADSC));
  result = ADCL;
  result |= ADCH << 8;
  result = 1125300L / result;
  return result;
}

// ============================================================================
// CALIBRATION PORTE AU DÉMARRAGE / STARTUP DOOR CALIBRATION
// ============================================================================
void calibrerPorteDemarrage() {
  Serial.println(F("=== CALIBRATION PORTE AU DÉMARRAGE ==="));
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Calibration..."));
  
  // 1. Vérifier état initial capteurs
  bool hautActif = (digitalRead(FIN_COURSE_HAUT) == LOW);
  bool basActif = (digitalRead(FIN_COURSE_BAS) == LOW);
  
  Serial.print(F("État initial: HAUT="));
  Serial.print(hautActif ? F("ON") : F("OFF"));
  Serial.print(F(" BAS="));
  Serial.println(basActif ? F("ON") : F("OFF"));
  
  if (hautActif && basActif) {
    Serial.println(F("ERREUR: Les deux capteurs sont activés!"));
    erreurActuelle = ERREUR_CAPTEURS_INCOHERENTS;
    return;
  }
  
  // 2. Aller en position BAS
  lcd.setCursor(0, 1);
  lcd.print(F("Fermeture...    "));
  Serial.println(F("Descente vers position BAS..."));
  
  digitalWrite(MOTEUR_PIN1, LOW);
  digitalWrite(MOTEUR_PIN2, HIGH);
  
  unsigned long debut = millis();
  while (digitalRead(FIN_COURSE_BAS) == HIGH) {
    if (millis() - debut > (unsigned long)timeoutFermeture * 1000) {
      Serial.println(F("ERREUR: Timeout fermeture lors calibration!"));
      erreurActuelle = ERREUR_CAPTEUR_FIN_COURSE_HS;
      arreterMoteur();
      return;
    }
    delay(10);
  }
  arreterMoteur();
  Serial.println(F("Position BAS atteinte"));
  delay(500);
  
  // 3. Monter en position HAUT
  lcd.setCursor(0, 1);
  lcd.print(F("Ouverture...    "));
  Serial.println(F("Montée vers position HAUT..."));
  
  digitalWrite(MOTEUR_PIN1, HIGH);
  digitalWrite(MOTEUR_PIN2, LOW);
  
  debut = millis();
  while (digitalRead(FIN_COURSE_HAUT) == HIGH) {
    if (millis() - debut > (unsigned long)timeoutOuverture * 1000) {
      Serial.println(F("ERREUR: Timeout ouverture lors calibration!"));
      erreurActuelle = ERREUR_CAPTEUR_FIN_COURSE_HS;
      arreterMoteur();
      return;
    }
    delay(10);
  }
  arreterMoteur();
  Serial.println(F("Position HAUT atteinte"));
  
  porteOuverte = true;
  Serial.println(F("=== CALIBRATION RÉUSSIE ==="));
  lcd.setCursor(0, 1);
  lcd.print(F("Calibr. OK      "));
  delay(1000);
}

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  Serial.begin(9600);
  Serial.println(F("\n\n=========================================="));
  Serial.println(F("POULAILLER AUTOMATIQUE v1.4.0"));
  Serial.println(F("==========================================\n"));
  
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
  lcd.print(F("Poulailler v1.4"));
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
    lcd.print(F("ERREUR RTC!"));
    Serial.println(F("RTC non trouvé !"));
    while (1);
  }
  
  // Charger paramètres EEPROM
  chargerParametresEEPROM();
  
  // Vérifier perte alimentation RTC
  if (rtc.lostPower()) {
    coupureCourant = true;
    Serial.println(F("ATTENTION: RTC a perdu l'alimentation"));
  }
  
  // CALIBRATION PORTE AU DÉMARRAGE
  calibrerPorteDemarrage();
  
  // Si erreur critique lors calibration, bloquer
  if (erreurActuelle >= 1 && erreurActuelle <= 9) {
    Serial.println(F("ERREUR CRITIQUE lors calibration - Arrêt automatique"));
    modeAutomatiqueActif = false;
  }
  
  Serial.println(F("\n=== SYSTÈME INITIALISÉ ==="));
  Serial.println(F("Paramètres:"));
  Serial.print(F("  Heure ouverture: "));
  Serial.print(heureOuverture);
  Serial.print(F(":"));
  Serial.println(minuteOuverture);
  Serial.print(F("  Heure min fermeture: "));
  Serial.print(heureMinFermeture);
  Serial.print(F(":"));
  Serial.println(minuteMinFermeture);
  Serial.print(F("  Seuil luminosité: "));
  Serial.println(seuilLumiere);
  Serial.print(F("  Temporisation: "));
  Serial.print(tempoFermetureMinutes);
  Serial.println(F(" min"));
  
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
  
  // Gestion des erreurs
  gererErreurs();
  
  // Si erreur critique, bloquer automatique
  if (erreurActuelle >= 1 && erreurActuelle <= 9) {
    modeAutomatiqueActif = false;
  }
  
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
      erreurActuelle == AUCUNE_ERREUR &&
      (millis() - derniereActivite > TIMEOUT_LCD)) {
    Serial.println(F("Extinction auto LCD"));
    eteindreLCD();
  }
  
  // Rafraîchissement LCD (throttlé à 200ms)
  if (lcdAllume && (millis() - dernierRafraichissementLCD >= INTERVALLE_RAFRAICHISSEMENT_LCD)) {
    dernierRafraichissementLCD = millis();
    
    if (erreurActuelle != AUCUNE_ERREUR) {
      afficherMessageErreur();
    } else {
      gererAffichageLCD(maintenant);
    }
  }
  
  // Logique automatique (seulement si mode actif et pas d'erreur)
  if (modeActuel == MODE_NORMAL && modeAutomatiqueActif && erreurActuelle == AUCUNE_ERREUR) {
    if (millis() - dernierTimeCheck > 1000) {
      dernierTimeCheck = millis();
      
      int valeurLumiere = analogRead(CAPTEUR_LUMIERE);
      
      // Ouverture automatique à l'heure configurée
      if (maintenant.hour() == heureOuverture && 
          maintenant.minute() == minuteOuverture && 
          !porteOuverte) {
        Serial.print(F("Heure ouverture ("));
        Serial.print(heureOuverture);
        Serial.print(F(":"));
        Serial.print(minuteOuverture);
        Serial.println(F(") - Ouverture automatique"));
        ouvrirPorte();
      }
      
      // Fermeture avec temporisation
      gererFermetureLumiere(valeurLumiere, maintenant.hour(), maintenant.minute());
      
      // Fermeture forcée à l'heure max configurée
      if (maintenant.hour() == heureMaxFermeture && 
          maintenant.minute() == minuteMaxFermeture && 
          porteOuverte) {
        Serial.print(F("Heure fermeture max ("));
        Serial.print(heureMaxFermeture);
        Serial.print(F(":"));
        Serial.print(minuteMaxFermeture);
        Serial.println(F(") - Fermeture forcée"));
        fermerPorte();
      }
    }
    
    // Gestion mouvement porte
    gererMouvementPorte();
  }
  
  delay(10); // Loop rapide pour encodeur
}

// Suite du code dans le prochain message...

// ============================================================================
// FONCTIONS GESTION ERREURS / ERROR MANAGEMENT FUNCTIONS
// ============================================================================
void gererErreurs() {
  verifierCapteursFinCourse();
  verifierCapteurLuminosite();
  verifierRTC();
  verifierTension();
  gererLEDErreur();
}

void verifierCapteursFinCourse() {
  // Vérifier si les deux capteurs sont activés (impossible physiquement)
  if (digitalRead(FIN_COURSE_HAUT) == LOW && 
      digitalRead(FIN_COURSE_BAS) == LOW) {
    if (erreurActuelle != ERREUR_CAPTEURS_INCOHERENTS) {
      Serial.println(F("ERREUR: Capteurs incohérents (les 2 actifs)"));
      erreurActuelle = ERREUR_CAPTEURS_INCOHERENTS;
      arreterMoteur();
    }
  }
}

void verifierCapteurLuminosite() {
  static bool test_en_cours = false;
  static bool premiere_lecture_nulle = false;
  
  DateTime maintenant = rtc.now();
  int heure = maintenant.hour();
  int minute = maintenant.minute();
  int seconde = maintenant.second();
  
  // Test quotidien à 13h avec confirmation 1 minute après
  // On teste UNIQUEMENT lumiere=0 (nuit à 13h = capteur HS critique)
  // lumiere=1023 (soleil) n'est pas dangereux, pas de test
  
  if (heure == 13 && minute == 0 && seconde == 0) {
    // Premier test à 13:00:00
    int lumiere = analogRead(CAPTEUR_LUMIERE);
    
    if (lumiere == 0) {
      Serial.print(F("Test capteur 13h00: lumiere=0 (suspect), confirmation dans 1 min"));
      premiere_lecture_nulle = true;
      test_en_cours = true;
    } else {
      Serial.print(F("Test capteur 13h00: OK (valeur="));
      Serial.print(lumiere);
      Serial.println(F(")"));
      
      // Capteur OK → reset alerte si elle existait
      if (erreurActuelle == ALERTE_CAPTEUR_LUMIERE) {
        Serial.println(F("Capteur luminosité réparé"));
        erreurActuelle = AUCUNE_ERREUR;
      }
      
      premiere_lecture_nulle = false;
      test_en_cours = false;
    }
  }
  
  // Confirmation à 13:01:00 si première lecture était nulle
  if (heure == 13 && minute == 1 && seconde == 0 && test_en_cours && premiere_lecture_nulle) {
    int lumiere = analogRead(CAPTEUR_LUMIERE);
    
    if (lumiere == 0) {
      // Confirmé : capteur indique nuit à 13h → HS !
      if (erreurActuelle != ALERTE_CAPTEUR_LUMIERE) {
        Serial.println(F("ALERTE: Capteur luminosité HS confirmé (nuit à 13h)"));
        erreurActuelle = ALERTE_CAPTEUR_LUMIERE;
      }
    } else {
      // Fausse alerte, c'était juste un glitch
      Serial.print(F("Test capteur 13h01: OK finalement (valeur="));
      Serial.print(lumiere);
      Serial.println(F(")"));
    }
    
    test_en_cours = false;
    premiere_lecture_nulle = false;
  }
  
  // Reset flags si on a raté la fenêtre de test
  if (heure == 13 && minute >= 2) {
    test_en_cours = false;
    premiere_lecture_nulle = false;
  }
}

void verifierRTC() {
  DateTime maintenant = rtc.now();
  
  // Année aberrante
  if (maintenant.year() < 2025 || maintenant.year() > 2030) {
    if (erreurActuelle != ERREUR_RTC_ABERRANT) {
      Serial.print(F("ERREUR: RTC année aberrante ("));
      Serial.print(maintenant.year());
      Serial.println(F(")"));
      erreurActuelle = ERREUR_RTC_ABERRANT;
    }
    return;
  }
  
  // Heure figée (vérifier toutes les 2 minutes)
  if (millis() - derniere_verification_rtc > 120000) {
    // Première vérification : juste enregistrer le DateTime complet
    if (!rtc_verification_initialisee) {
      derniere_datetime_rtc = maintenant;
      rtc_verification_initialisee = true;
      Serial.println(F("Vérification RTC initialisée"));
    } else {
      // Vérifications suivantes : comparer les timestamps complets
      // Si exactement le même timestamp après 2 minutes → RTC figé !
      if (derniere_datetime_rtc.unixtime() == maintenant.unixtime()) {
        if (erreurActuelle != ERREUR_RTC_FIGE) {
          Serial.println(F("ERREUR: RTC figé (timestamp identique après 2 min)"));
          erreurActuelle = ERREUR_RTC_FIGE;
        }
      }
      // Enregistrer le nouveau timestamp pour prochaine comparaison
      derniere_datetime_rtc = maintenant;
    }
    derniere_verification_rtc = millis();
  }
}

void verifierTension() {
  static unsigned long derniere_verification_tension = 0;
  
  if (millis() - derniere_verification_tension > 60000) {
    long vcc = readVcc();
    if (vcc < 4500) {
      if (erreurActuelle != ALERTE_TENSION_FAIBLE) {
        Serial.print(F("ALERTE: Tension faible ("));
        Serial.print(vcc);
        Serial.println(F("mV)"));
        erreurActuelle = ALERTE_TENSION_FAIBLE;
      }
    }
    derniere_verification_tension = millis();
  }
}

void gererLEDErreur() {
  if (erreurActuelle >= 1 && erreurActuelle <= 9) {
    // Erreur critique → LED rapide 200ms
    if (millis() - dernierTempsLED > 200) {
      dernierTempsLED = millis();
      etatLED = !etatLED;
      digitalWrite(LED_COUPURE, etatLED);
    }
  } else if (erreurActuelle >= 10 && erreurActuelle <= 19) {
    // Alerte → LED lente 1000ms
    if (millis() - dernierTempsLED > 1000) {
      dernierTempsLED = millis();
      etatLED = !etatLED;
      digitalWrite(LED_COUPURE, etatLED);
    }
  } else if (coupureCourant) {
    // Coupure RTC → LED 500ms
    if (millis() - dernierTempsLED > 500) {
      dernierTempsLED = millis();
      etatLED = !etatLED;
      digitalWrite(LED_COUPURE, etatLED);
    }
  } else {
    digitalWrite(LED_COUPURE, LOW);
  }
}

void afficherMessageErreur() {
  lcd.clear();
  lcd.setCursor(0, 0);
  
  switch (erreurActuelle) {
    case ERREUR_CAPTEURS_INCOHERENTS:
      lcd.print(F("ERR: CAPTEURS"));
      lcd.setCursor(0, 1);
      lcd.print(F("2 actifs! 5s=OK"));
      break;
      
    case ERREUR_CAPTEUR_FIN_COURSE_HS:
      lcd.print(F("ERR: CAPTEUR"));
      lcd.setCursor(0, 1);
      lcd.print(F("Fin course HS"));
      break;
      
    case ERREUR_RTC_ABERRANT:
      lcd.print(F("ERR: HORLOGE"));
      lcd.setCursor(0, 1);
      lcd.print(F("Heure aberrante"));
      break;
      
    case ERREUR_RTC_FIGE:
      lcd.print(F("ERR: HORLOGE"));
      lcd.setCursor(0, 1);
      lcd.print(F("RTC fige"));
      break;
      
    case ALERTE_CAPTEUR_LUMIERE:
      lcd.print(F("ALERTE LUMIERE"));
      lcd.setCursor(0, 1);
      lcd.print(F("Val extreme"));
      break;
      
    case ALERTE_TENSION_FAIBLE:
      lcd.print(F("ALERTE TENSION"));
      lcd.setCursor(0, 1);
      lcd.print(F("Batterie faible"));
      break;
  }
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
      // Ligne 1: Alterner heure / luminosité toutes les 3s
      if (millis() - dernierAlternanceLCD > 3000) {
        dernierAlternanceLCD = millis();
        afficherLuminosite = !afficherLuminosite;
      }
      
      if (afficherLuminosite) {
        int lumiere = analogRead(CAPTEUR_LUMIERE);
        lcd.print(F("Lum:"));
        if (lumiere < 10) lcd.print("   ");
        else if (lumiere < 100) lcd.print("  ");
        else if (lumiere < 1000) lcd.print(" ");
        lcd.print(lumiere);
        lcd.print(F("        "));
      } else {
        if (maintenant.hour() < 10) lcd.print("0");
        lcd.print(maintenant.hour());
        lcd.print(":");
        if (maintenant.minute() < 10) lcd.print("0");
        lcd.print(maintenant.minute());
        lcd.print(F("        "));
      }
      
      // Ligne 2: Statut porte
      lcd.setCursor(0, 1);
      if (etatActuel == OUVERTURE) {
        lcd.print(F("Ouverture...    "));
      } else if (etatActuel == FERMETURE) {
        lcd.print(F("Fermeture...    "));
      } else if (lumiereFaibleDetectee) {
        unsigned long tempsRestant = (tempoFermetureMinutes * 60000UL - (millis() - debutLumiereFaible)) / 60000UL;
        lcd.print(F("Ferme dans "));
        lcd.print(tempsRestant + 1);
        lcd.print(F("mn "));
      } else if (etatActuel == ERREUR_OBSTACLE) {
        lcd.print(F("ERR OBSTACLE    "));
      } else {
        lcd.print(porteOuverte ? F("Porte ouverte   ") : F("Porte fermee    "));
      }
      break;
      
    case MODE_REGLAGE_HEURE_OUVERTURE:
      lcd.print(F("Ouv:"));
      if (clignotement) {
        lcd.print(F("  :"));  // 2 espaces pour l'heure
      } else {
        if (heureOuverture < 10) lcd.print("0");
        lcd.print(heureOuverture);
        lcd.print(":");
      }
      if (minuteOuverture < 10) lcd.print("0");
      lcd.print(minuteOuverture);
      lcd.print(F("      "));
      lcd.setCursor(0, 1);
      lcd.print(F("Tourner encodeur"));
      break;
      
    case MODE_REGLAGE_MINUTE_OUVERTURE:
      lcd.print(F("Ouv:"));
      if (heureOuverture < 10) lcd.print("0");
      lcd.print(heureOuverture);
      lcd.print(":");
      if (clignotement) {
        lcd.print(F("  "));
      } else {
        if (minuteOuverture < 10) lcd.print("0");
        lcd.print(minuteOuverture);
      }
      lcd.print(F("      "));
      lcd.setCursor(0, 1);
      lcd.print(F("Tourner encodeur"));
      break;
      
    case MODE_REGLAGE_HEURE_MIN_FERMETURE:
      lcd.print(F("MinF:"));
      if (clignotement) {
        lcd.print(F("  :"));  // 2 espaces pour l'heure
      } else {
        if (heureMinFermeture < 10) lcd.print("0");
        lcd.print(heureMinFermeture);
        lcd.print(":");
      }
      if (minuteMinFermeture < 10) lcd.print("0");
      lcd.print(minuteMinFermeture);
      lcd.print(F("     "));
      lcd.setCursor(0, 1);
      lcd.print(F("Heure min ferm. "));
      break;
      
    case MODE_REGLAGE_MINUTE_MIN_FERMETURE:
      lcd.print(F("MinF:"));
      if (heureMinFermeture < 10) lcd.print("0");
      lcd.print(heureMinFermeture);
      lcd.print(":");
      if (clignotement) {
        lcd.print(F("  "));
      } else {
        if (minuteMinFermeture < 10) lcd.print("0");
        lcd.print(minuteMinFermeture);
      }
      lcd.print(F("     "));
      lcd.setCursor(0, 1);
      lcd.print(F("Tourner encodeur"));
      break;
      
    case MODE_REGLAGE_HEURE_MAX_FERMETURE:
      lcd.print(F("MaxF:"));
      if (clignotement) {
        lcd.print(F("  :"));  // 2 espaces pour l'heure
      } else {
        if (heureMaxFermeture < 10) lcd.print("0");
        lcd.print(heureMaxFermeture);
        lcd.print(":");
      }
      if (minuteMaxFermeture < 10) lcd.print("0");
      lcd.print(minuteMaxFermeture);
      lcd.print(F("     "));
      lcd.setCursor(0, 1);
      lcd.print(F("Heure max ferm. "));
      break;
      
    case MODE_REGLAGE_MINUTE_MAX_FERMETURE:
      lcd.print(F("MaxF:"));
      if (heureMaxFermeture < 10) lcd.print("0");
      lcd.print(heureMaxFermeture);
      lcd.print(":");
      if (clignotement) {
        lcd.print(F("  "));
      } else {
        if (minuteMaxFermeture < 10) lcd.print("0");
        lcd.print(minuteMaxFermeture);
      }
      lcd.print(F("     "));
      lcd.setCursor(0, 1);
      lcd.print(F("Tourner encodeur"));
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
      
    case MODE_REGLAGE_TEMPO_FERMETURE:
      lcd.print(F("Tempo: "));
      if (tempoFermetureMinutes < 10) lcd.print(" ");
      lcd.print(tempoFermetureMinutes);
      lcd.print(F("mn     "));
      lcd.setCursor(0, 1);
      lcd.print(F("Anti-nuages     "));
      break;
      
    case MODE_REGLAGE_TIMEOUT_OUVERTURE:
      lcd.print(F("T.ouv: "));
      if (timeoutOuverture < 10) lcd.print(" ");
      lcd.print(timeoutOuverture);
      lcd.print(F("s      "));
      lcd.setCursor(0, 1);
      lcd.print(F("Timeout moteur  "));
      break;
      
    case MODE_REGLAGE_TIMEOUT_FERMETURE:
      lcd.print(F("T.fer: "));
      if (timeoutFermeture < 10) lcd.print(" ");
      lcd.print(timeoutFermeture);
      lcd.print(F("s      "));
      lcd.setCursor(0, 1);
      lcd.print(F("Timeout moteur  "));
      break;
  }
}

// ============================================================================
// FONCTIONS EEPROM
// ============================================================================
void sauvegarderParametres() {
  EEPROM.write(HEURE_OUVERTURE_ADDR, heureOuverture);
  EEPROM.write(MINUTE_OUVERTURE_ADDR, minuteOuverture);
  EEPROM.write(HEURE_MIN_FERMETURE_ADDR, heureMinFermeture);
  EEPROM.write(MINUTE_MIN_FERMETURE_ADDR, minuteMinFermeture);
  EEPROM.write(HEURE_MAX_FERMETURE_ADDR, heureMaxFermeture);
  EEPROM.write(MINUTE_MAX_FERMETURE_ADDR, minuteMaxFermeture);
  EEPROM.write(SEUIL_EEPROM_ADDR, seuilLumiere & 0xFF);
  EEPROM.write(SEUIL_EEPROM_ADDR + 1, (seuilLumiere >> 8) & 0xFF);
  EEPROM.write(TEMPO_FERMETURE_ADDR, tempoFermetureMinutes);
  EEPROM.write(TIMEOUT_OUVERTURE_ADDR, timeoutOuverture);
  EEPROM.write(TIMEOUT_FERMETURE_ADDR, timeoutFermeture);
}

void chargerParametresEEPROM() {
  // Charger et valider chaque paramètre
  uint8_t h = EEPROM.read(HEURE_OUVERTURE_ADDR);
  if (h >= 6 && h <= 9) heureOuverture = h;
  
  uint8_t m = EEPROM.read(MINUTE_OUVERTURE_ADDR);
  if (m <= 59) minuteOuverture = m;
  
  h = EEPROM.read(HEURE_MIN_FERMETURE_ADDR);
  if (h >= 15 && h <= 17) heureMinFermeture = h;
  
  m = EEPROM.read(MINUTE_MIN_FERMETURE_ADDR);
  if (m <= 59) minuteMinFermeture = m;
  
  h = EEPROM.read(HEURE_MAX_FERMETURE_ADDR);
  if (h <= 23) heureMaxFermeture = h;
  
  m = EEPROM.read(MINUTE_MAX_FERMETURE_ADDR);
  if (m <= 59) minuteMaxFermeture = m;
  
  int seuil = EEPROM.read(SEUIL_EEPROM_ADDR) | (EEPROM.read(SEUIL_EEPROM_ADDR + 1) << 8);
  if (seuil >= 0 && seuil <= 1023) seuilLumiere = seuil;
  
  uint8_t tempo = EEPROM.read(TEMPO_FERMETURE_ADDR);
  if (tempo >= 10 && tempo <= 30) tempoFermetureMinutes = tempo;
  
  uint8_t tout = EEPROM.read(TIMEOUT_OUVERTURE_ADDR);
  if (tout >= 5 && tout <= 60) timeoutOuverture = tout;
  
  tout = EEPROM.read(TIMEOUT_FERMETURE_ADDR);
  if (tout >= 5 && tout <= 60) timeoutFermeture = tout;
}

// ============================================================================
// FONCTIONS MOTEUR
// ============================================================================
void ouvrirPorte() {
  if (!porteOuverte && (etatActuel == ARRET || etatActuel == ERREUR_OBSTACLE)) {
    Serial.println(F(">>> OUVERTURE PORTE"));
    allumerLCD();
    etatActuel = OUVERTURE;
    debutMouvementPorte = millis();
    digitalWrite(MOTEUR_PIN1, HIGH);
    digitalWrite(MOTEUR_PIN2, LOW);
  }
}

void fermerPorte() {
  if (porteOuverte && (etatActuel == ARRET || etatActuel == ERREUR_OBSTACLE)) {
    Serial.println(F(">>> FERMETURE PORTE"));
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
        Serial.println(F("Porte ouverte (capteur HAUT)"));
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
        Serial.println(F("Porte fermée (capteur BAS)"));
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
void gererFermetureLumiere(int valeurLumiere, int heure, int minute) {
  if (!porteOuverte || heure <= heureOuverture) {
    lumiereFaibleDetectee = false;
    debutLumiereFaible = 0;
    return;
  }
  
  // Vérifier si on est après l'heure minimum de fermeture
  int minutesCourantes = heure * 60 + minute;
  int minutesMinFermeture = heureMinFermeture * 60 + minuteMinFermeture;
  
  if (minutesCourantes < minutesMinFermeture) {
    // Trop tôt pour fermer
    if (lumiereFaibleDetectee) {
      Serial.println(F("Lumière faible mais trop tôt pour fermer"));
      lumiereFaibleDetectee = false;
      debutLumiereFaible = 0;
    }
    return;
  }
  
  if (valeurLumiere < seuilLumiere) {
    if (!lumiereFaibleDetectee) {
      lumiereFaibleDetectee = true;
      debutLumiereFaible = millis();
      dernierAllumageMinute = millis();
      allumerLCD();
      Serial.print(F("Début temporisation fermeture ("));
      Serial.print(tempoFermetureMinutes);
      Serial.println(F(" min)"));
    } else if (millis() - debutLumiereFaible >= tempoFermetureMinutes * 60000UL) {
      Serial.println(F("Temporisation écoulée - Fermeture auto"));
      fermerPorte();
      lumiereFaibleDetectee = false;
    }
  } else {
    if (lumiereFaibleDetectee) {
      Serial.println(F("Lumière revenue - Annulation fermeture"));
      lumiereFaibleDetectee = false;
      debutLumiereFaible = 0;
    }
  }
}
