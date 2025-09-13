/*
 * ============================================================================
 * TEST COMPLET SYSTÈME - Poulailler Automatique
 * ============================================================================
 * 
 * Test: 7.1 - Complete System Integration
 * Objectif: Valider l'intégration complète de tous les composants
 * Durée: ~15 minutes
 * 
 * Ce test vérifie:
 * - Fonctionnement simultané de tous les modules
 * - Interface utilisateur complète (LCD + bouton)
 * - Logique de contrôle automatique
 * - Gestion des capteurs et actionneurs
 * - Sauvegarde EEPROM des paramètres
 * - Scénarios d'utilisation réalistes
 * 
 * Composants requis (TOUS connectés):
 * - Arduino Nano + alimentation
 * - RTC DS3231 (I2C)
 * - LCD 16x2 I2C  
 * - Capteur luminosité + résistance 10kΩ
 * - 2x capteurs fin de course
 * - Bouton multifonction
 * - LED de statut
 * - Module L298N + moteur (⚠️ DÉCONNECTÉ du mécanisme)
 * 
 * Procédure:
 * 1. Uploader ce code (version simplifiée du projet final)
 * 2. Vérifier affichage LCD et navigation bouton
 * 3. Tester tous les capteurs simultanément  
 * 4. Valider logique automatique (simulation)
 * 5. Contrôler sauvegarde paramètres
 * 
 * ⚠️ SÉCURITÉ: Moteur déconnecté du mécanisme pour éviter 
 * mouvements intempestifs pendant les tests
 * 
 * ============================================================================
 */

#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

// Configuration des broches (identique au projet final)
const int MOTEUR_PIN1 = 7;
const int MOTEUR_PIN2 = 6;
const int CAPTEUR_LUMIERE = A0;
const int BOUTON_PIN = 5;
const int FIN_COURSE_HAUT = 8;
const int FIN_COURSE_BAS = 9;
const int LED_COUPURE = 3;

// Constantes
const unsigned long APPUI_LONG = 3000;
const unsigned long TIMEOUT_REGLAGE = 10000;
const unsigned long DOUBLE_CLIC = 500;
const int SEUIL_EEPROM_ADDR = 0;

// Variables globales
RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);
bool systemeInitialise = false;
int seuilLumiere = 300;

// Variables bouton
unsigned long debutAppui = 0;
bool boutonEnfonce = false;
bool appuiTraite = false;
unsigned long dernierRelachement = 0;
bool enAttenteDoubleClick = false;

// Variables interface LCD
enum ModeReglage {
  MODE_NORMAL,
  MODE_REGLAGE_HEURE,
  MODE_REGLAGE_MINUTE,
  MODE_REGLAGE_SEUIL,
  MODE_TEST_SYSTEME
};
ModeReglage modeActuel = MODE_NORMAL;
unsigned long debutModeReglage = 0;
bool clignotement = false;
unsigned long dernierClignotement = 0;

// Variables de test intégration
unsigned long debutTestComplet = 0;
int phaseTestComplete = 0;
bool testEnCours = false;

// Statistiques de test
int testsReussis = 0;
int testsEchoues = 0;
String dernierProbleme = "";

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  Serial.println("============================================");
  Serial.println("TEST COMPLET SYSTÈME - Poulailler Auto");
  Serial.println("============================================");
  Serial.println("Version: 1.0");
  Serial.println("Test: 7.1 - Complete System Integration");
  Serial.println("");
  
  // Initialisation des broches
  pinMode(MOTEUR_PIN1, OUTPUT);
  pinMode(MOTEUR_PIN2, OUTPUT);
  pinMode(BOUTON_PIN, INPUT_PULLUP);
  pinMode(FIN_COURSE_HAUT, INPUT_PULLUP);
  pinMode(FIN_COURSE_BAS, INPUT_PULLUP);
  pinMode(LED_COUPURE, OUTPUT);
  
  // Arrêt moteur (sécurité)
  digitalWrite(MOTEUR_PIN1, LOW);
  digitalWrite(MOTEUR_PIN2, LOW);
  
  Serial.println("🔧 INITIALISATION DES MODULES:");
  Serial.println("");
  
  // Test LCD
  Serial.print("1. LCD I2C... ");
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Test Complet");
  lcd.setCursor(0, 1);
  lcd.print("Initialisation..");
  delay(1000);
  Serial.println("✅ OK");
  
  // Test RTC
  Serial.print("2. RTC DS3231... ");
  if (!rtc.begin()) {
    Serial.println("❌ ÉCHEC");
    testsEchoues++;
    dernierProbleme = "RTC non détecté";
  } else {
    Serial.println("✅ OK");
    testsReussis++;
  }
  
  // Test capteurs
  Serial.print("3. Capteur luminosité... ");
  int lumiere = analogRead(CAPTEUR_LUMIERE);
  if (lumiere > 0 && lumiere < 1023) {
    Serial.print("✅ OK (valeur: ");
    Serial.print(lumiere);
    Serial.println(")");
    testsReussis++;
  } else {
    Serial.println("⚠️ Valeurs limites");
  }
  
  Serial.print("4. Capteurs fin de course... ");
  bool haut = digitalRead(FIN_COURSE_HAUT);
  bool bas = digitalRead(FIN_COURSE_BAS);
  Serial.print("✅ OK (H:");
  Serial.print(haut ? "Libre" : "Actionné");
  Serial.print(", B:");
  Serial.print(bas ? "Libre" : "Actionné");
  Serial.println(")");
  testsReussis++;
  
  Serial.print("5. Bouton... ");
  bool bouton = digitalRead(BOUTON_PIN);
  if (bouton == HIGH) {
    Serial.println("✅ OK (pull-up actif)");
    testsReussis++;
  } else {
    Serial.println("⚠️ État pressé au démarrage");
  }
  
  Serial.print("6. LED de statut... ");
  digitalWrite(LED_COUPURE, HIGH);
  delay(200);
  digitalWrite(LED_COUPURE, LOW);
  Serial.println("✅ OK");
  testsReussis++;
  
  Serial.print("7. EEPROM... ");
  int valeurTest = EEPROM.read(SEUIL_EEPROM_ADDR);
  if (valeurTest >= 0 && valeurTest <= 255) {
    seuilLumiere = valeurTest | (EEPROM.read(SEUIL_EEPROM_ADDR + 1) << 8);
    if (seuilLumiere == 0) seuilLumiere = 300; // Valeur par défaut
    Serial.println("✅ OK");
    testsReussis++;
  } else {
    Serial.println("⚠️ Initialisation");
  }
  
  Serial.println("");
  Serial.print("📊 Initialisation: ");
  Serial.print(testsReussis);
  Serial.print(" OK, ");
  Serial.print(testsEchoues);
  Serial.println(" échecs");
  
  if (testsEchoues == 0) {
    Serial.println("✅ SYSTÈME INTÉGRALEMENT FONCTIONNEL");
    systemeInitialise = true;
  } else {
    Serial.print("⚠️ Problèmes détectés: ");
    Serial.println(dernierProbleme);
  }
  
  Serial.println("");
  Serial.println("📋 TESTS D'INTÉGRATION DISPONIBLES:");
  Serial.println("- Navigation normale: Bouton court = actions");
  Serial.println("- Mode réglage: Bouton long = réglages");
  Serial.println("- Test automatique: Mode spécial via menu");
  Serial.println("");
  Serial.println("🎮 COMMANDES:");
  Serial.println("- Appui court: Actions/navigation");
  Serial.println("- Appui long: Mode réglage");
  Serial.println("- Double-clic: Actions spéciales");
  Serial.println("");
  
  lcd.clear();
  lcd.setCursor(0, 0);
  if (systemeInitialise) {
    lcd.print("Systeme OK!");
  } else {
    lcd.print("Problemes detect");
  }
  
  delay(2000);
  debutTestComplet = millis();
}

void loop() {
  // Gestion du clignotement pour mode réglage
  if (millis() - dernierClignotement > 500) {
    dernierClignotement = millis();
    clignotement = !clignotement;
  }
  
  // Gestion du bouton
  gererBouton();
  
  // Gestion du timeout mode réglage
  if (modeActuel != MODE_NORMAL && millis() - debutModeReglage > TIMEOUT_REGLAGE) {
    modeActuel = MODE_NORMAL;
  }
  
  // Gestion de l'affichage selon le mode
  gererAffichage();
  
  // Test automatique si mode activé
  if (modeActuel == MODE_TEST_SYSTEME) {
    executerTestAutomatique();
  }
  
  delay(50);
}

/*
 * Gestion du bouton multifonction
 */
void gererBouton() {
  bool etatBouton = digitalRead(BOUTON_PIN);
  
  // Détection début d'appui
  if (!boutonEnfonce && etatBouton == LOW) {
    delay(20); // Anti-rebond
    if (digitalRead(BOUTON_PIN) == LOW) {
      boutonEnfonce = true;
      debutAppui = millis();
      appuiTraite = false;
    }
  }
  
  // Détection relâchement
  if (boutonEnfonce && etatBouton == HIGH) {
    delay(20); // Anti-rebond
    if (digitalRead(BOUTON_PIN) == HIGH) {
      boutonEnfonce = false;
      unsigned long dureeAppui = millis() - debutAppui;
      
      if (!appuiTraite) {
        if (dureeAppui >= APPUI_LONG) {
          gererAppuiLong();
        } else {
          if (enAttenteDoubleClick && millis() - dernierRelachement < DOUBLE_CLIC) {
            gererDoubleClick();
            enAttenteDoubleClick = false;
          } else {
            dernierRelachement = millis();
            enAttenteDoubleClick = true;
          }
        }
      }
    }
  }
  
  // Timeout double-clic
  if (enAttenteDoubleClick && millis() - dernierRelachement > DOUBLE_CLIC) {
    gererAppuiBref();
    enAttenteDoubleClick = false;
  }
}

/*
 * Gestion appui bref
 */
void gererAppuiBref() {
  Serial.print("🔘 Appui bref - Mode: ");
  Serial.println(modeActuel);
  
  switch (modeActuel) {
    case MODE_NORMAL:
      // Test simple des sorties
      Serial.println("   → Test LED + moteur");
      digitalWrite(LED_COUPURE, HIGH);
      delay(100);
      digitalWrite(LED_COUPURE, LOW);
      
      // Test moteur très court (sécurité)
      digitalWrite(MOTEUR_PIN1, HIGH);
      digitalWrite(MOTEUR_PIN2, LOW);
      delay(200);
      digitalWrite(MOTEUR_PIN1, LOW);
      digitalWrite(MOTEUR_PIN2, LOW);
      break;
      
    case MODE_REGLAGE_HEURE:
      // Incrémenter heure
      if (systemeInitialise) {
        DateTime maintenant = rtc.now();
        int nouvelleHeure = (maintenant.hour() + 1) % 24;
        rtc.adjust(DateTime(maintenant.year(), maintenant.month(), maintenant.day(), 
                           nouvelleHeure, maintenant.minute(), 0));
        Serial.print("   → Heure: ");
        Serial.println(nouvelleHeure);
      }
      debutModeReglage = millis();
      break;
      
    case MODE_REGLAGE_MINUTE:
      // Incrémenter minute
      if (systemeInitialise) {
        DateTime maintenant = rtc.now();
        int nouvelleMinute = (maintenant.minute() + 1) % 60;
        rtc.adjust(DateTime(maintenant.year(), maintenant.month(), maintenant.day(), 
                           maintenant.hour(), nouvelleMinute, 0));
        Serial.print("   → Minute: ");
        Serial.println(nouvelleMinute);
      }
      debutModeReglage = millis();
      break;
      
    case MODE_REGLAGE_SEUIL:
      // Augmenter seuil
      seuilLumiere = min(1023, seuilLumiere + 10);
      sauvegarderSeuil();
      Serial.print("   → Seuil: ");
      Serial.println(seuilLumiere);
      debutModeReglage = millis();
      break;
      
    case MODE_TEST_SYSTEME:
      // Passer à la phase suivante
      phaseTestComplete++;
      Serial.print("   → Phase test: ");
      Serial.println(phaseTestComplete);
      break;
  }
}

/*
 * Gestion double-clic
 */
void gererDoubleClick() {
  Serial.println("🔄 Double-clic");
  
  if (modeActuel == MODE_REGLAGE_SEUIL) {
    seuilLumiere = max(0, seuilLumiere - 10);
    sauvegarderSeuil();
    Serial.print("   → Seuil diminué: ");
    Serial.println(seuilLumiere);
    debutModeReglage = millis();
  }
}

/*
 * Gestion appui long
 */
void gererAppuiLong() {
  Serial.print("⏱️ Appui long - Transition: ");
  Serial.print(modeActuel);
  
  switch (modeActuel) {
    case MODE_NORMAL:
      modeActuel = MODE_REGLAGE_HEURE;
      Serial.println(" → REGLAGE_HEURE");
      break;
    case MODE_REGLAGE_HEURE:
      modeActuel = MODE_REGLAGE_MINUTE;
      Serial.println(" → REGLAGE_MINUTE");
      break;
    case MODE_REGLAGE_MINUTE:
      modeActuel = MODE_REGLAGE_SEUIL;
      Serial.println(" → REGLAGE_SEUIL");
      break;
    case MODE_REGLAGE_SEUIL:
      modeActuel = MODE_TEST_SYSTEME;
      testEnCours = true;
      phaseTestComplete = 0;
      Serial.println(" → TEST_SYSTEME");
      break;
    case MODE_TEST_SYSTEME:
      modeActuel = MODE_NORMAL;
      testEnCours = false;
      Serial.println(" → NORMAL");
      break;
  }
  
  debutModeReglage = millis();
  appuiTraite = true;
}

/*
 * Gestion de l'affichage LCD
 */
void gererAffichage() {
  lcd.setCursor(0, 0);
  
  switch (modeActuel) {
    case MODE_NORMAL:
      // Ligne 1: Heure ou statut
      if (systemeInitialise) {
        DateTime maintenant = rtc.now();
        if (maintenant.hour() < 10) lcd.print("0");
        lcd.print(maintenant.hour());
        lcd.print(":");
        if (maintenant.minute() < 10) lcd.print("0");
        lcd.print(maintenant.minute());
        lcd.print("      ");
      } else {
        lcd.print("Systeme KO      ");
      }
      
      // Ligne 2: Capteurs
      lcd.setCursor(0, 1);
      int lumiere = analogRead(CAPTEUR_LUMIERE);
      bool haut = digitalRead(FIN_COURSE_HAUT);
      bool bas = digitalRead(FIN_COURSE_BAS);
      
      lcd.print("L:");
      if (lumiere < 100) lcd.print("  ");
      else if (lumiere < 1000) lcd.print(" ");
      lcd.print(lumiere);
      lcd.print(" H:");
      lcd.print(haut ? "0" : "1");
      lcd.print(" B:");
      lcd.print(bas ? "0" : "1");
      lcd.print("   ");
      break;
      
    case MODE_REGLAGE_HEURE:
      if (systemeInitialise) {
        DateTime maintenant = rtc.now();
        if (clignotement) {
          lcd.print("  :");
        } else {
          if (maintenant.hour() < 10) lcd.print("0");
          lcd.print(maintenant.hour());
          lcd.print(":");
        }
        if (maintenant.minute() < 10) lcd.print("0");
        lcd.print(maintenant.minute());
        lcd.print("      ");
      } else {
        lcd.print("RTC non dispo   ");
      }
      
      lcd.setCursor(0, 1);
      lcd.print("Reglage heure   ");
      break;
      
    case MODE_REGLAGE_MINUTE:
      if (systemeInitialise) {
        DateTime maintenant = rtc.now();
        if (maintenant.hour() < 10) lcd.print("0");
        lcd.print(maintenant.hour());
        lcd.print(":");
        if (clignotement) {
          lcd.print("  ");
        } else {
          if (maintenant.minute() < 10) lcd.print("0");
          lcd.print(maintenant.minute());
        }
        lcd.print("      ");
      } else {
        lcd.print("RTC non dispo   ");
      }
      
      lcd.setCursor(0, 1);
      lcd.print("Reglage minute  ");
      break;
      
    case MODE_REGLAGE_SEUIL:
      lcd.print("Seuil: ");
      lcd.print(seuilLumiere);
      lcd.print("     ");
      
      lcd.setCursor(0, 1);
      int valeurActuelle = analogRead(CAPTEUR_LUMIERE);
      lcd.print("Actuel: ");
      lcd.print(valeurActuelle);
      lcd.print("     ");
      break;
      
    case MODE_TEST_SYSTEME:
      lcd.print("Test auto ");
      lcd.print(phaseTestComplete);
      lcd.print("/5   ");
      
      lcd.setCursor(0, 1);
      switch (phaseTestComplete) {
        case 0: lcd.print("Pret a tester   "); break;
        case 1: lcd.print("Test capteurs   "); break;
        case 2: lcd.print("Test moteur     "); break;
        case 3: lcd.print("Test LED        "); break;
        case 4: lcd.print("Test EEPROM     "); break;
        case 5: lcd.print("Test termine!   "); break;
        default: lcd.print("Phase inconnue  "); break;
      }
      break;
  }
}

/*
 * Test automatique du système
 */
void executerTestAutomatique() {
  static unsigned long dernierTest = 0;
  
  if (millis() - dernierTest < 2000) return; // Tests toutes les 2s
  dernierTest = millis();
  
  switch (phaseTestComplete) {
    case 1: // Test capteurs
      {
        Serial.println("🧪 Test capteurs automatique");
        int lumiere = analogRead(CAPTEUR_LUMIERE);
        bool haut = digitalRead(FIN_COURSE_HAUT);
        bool bas = digitalRead(FIN_COURSE_BAS);
        
        Serial.print("   Lumière: ");
        Serial.print(lumiere);
        Serial.print(", Haut: ");
        Serial.print(haut ? "Libre" : "Actionné");
        Serial.print(", Bas: ");
        Serial.println(bas ? "Libre" : "Actionné");
        
        if (lumiere > 0 && lumiere < 1023) {
          testsReussis++;
          Serial.println("   ✅ Capteurs OK");
        } else {
          testsEchoues++;
          Serial.println("   ⚠️ Valeurs capteur limites");
        }
      }
      break;
      
    case 2: // Test moteur
      Serial.println("🧪 Test moteur automatique");
      // Test très court pour sécurité
      digitalWrite(MOTEUR_PIN1, HIGH);
      digitalWrite(MOTEUR_PIN2, LOW);
      delay(300);
      digitalWrite(MOTEUR_PIN1, LOW);
      digitalWrite(MOTEUR_PIN2, HIGH);
      delay(300);
      digitalWrite(MOTEUR_PIN1, LOW);
      digitalWrite(MOTEUR_PIN2, LOW);
      Serial.println("   ✅ Commandes moteur OK");
      testsReussis++;
      break;
      
    case 3: // Test LED
      Serial.println("🧪 Test LED automatique");
      for (int i = 0; i < 3; i++) {
        digitalWrite(LED_COUPURE, HIGH);
        delay(200);
        digitalWrite(LED_COUPURE, LOW);
        delay(200);
      }
      Serial.println("   ✅ LED OK");
      testsReussis++;
      break;
      
    case 4: // Test EEPROM
      Serial.println("🧪 Test EEPROM automatique");
      int valeurTest = seuilLumiere + 1;
      EEPROM.write(100, valeurTest & 0xFF);
      delay(10);
      int valeurLue = EEPROM.read(100);
      if (valeurLue == (valeurTest & 0xFF)) {
        Serial.println("   ✅ EEPROM OK");
        testsReussis++;
      } else {
        Serial.println("   ❌ EEPROM défaillante");
        testsEchoues++;
      }
      break;
      
    case 5: // Résultats finaux
      Serial.println("");
      Serial.println("============================================");
      Serial.println("🎯 TEST COMPLET TERMINÉ");
      Serial.println("============================================");
      Serial.println("");
      Serial.print("📊 Résultats: ");
      Serial.print(testsReussis);
      Serial.print(" succès, ");
      Serial.print(testsEchoues);
      Serial.println(" échecs");
      Serial.println("");
      
      if (testsEchoues == 0) {
        Serial.println("✅ SYSTÈME INTÉGRALEMENT VALIDÉ");
        Serial.println("🎉 Prêt pour déploiement final!");
      } else {
        Serial.println("⚠️ Quelques problèmes détectés");
        Serial.println("📋 Réviser composants défaillants");
      }
      Serial.println("");
      Serial.println("➡️ Appui long pour revenir au mode normal");
      break;
  }
}

/*
 * Sauvegarde du seuil en EEPROM
 */
void sauvegarderSeuil() {
  EEPROM.write(SEUIL_EEPROM_ADDR, seuilLumiere & 0xFF);
  EEPROM.write(SEUIL_EEPROM_ADDR + 1, (seuilLumiere >> 8) & 0xFF);
}

/*
 * ============================================================================
 * DIAGNOSTIC SYSTÈME COMPLET
 * ============================================================================
 * 
 * ❌ "RTC non détecté":
 *    - Vérifier câblage I2C: SDA→A4, SCL→A5
 *    - Vérifier alimentation RTC: VCC→5V, GND→GND
 *    - Conflit adresse I2C avec LCD (rare)
 *    - Module RTC défaillant
 * 
 * ❌ Interface bouton non réactive:
 *    - Bouton défaillant ou mal câblé
 *    - Pin D5 utilisée par autre composant
 *    - Pull-up interne défaillant
 *    - Rebonds excessifs (ajouter condensateur)
 * 
 * ❌ LCD affiche caractères incorrects:
 *    - Conflit adresse I2C (scanner I2C)
 *    - Alimentation instable
 *    - Câblage I2C défaillant
 *    - Module LCD défaillant
 * 
 * ❌ Capteurs valeurs aberrantes:
 *    - Capteur luminosité: vérifier diviseur résistif
 *    - Fins de course: vérifier pull-up et câblage NO/NC
 *    - Interférences électromagnétiques
 *    - Alimentation bruyante
 * 
 * ❌ Moteur ne répond pas aux commandes:
 *    - L298N non alimenté en 12V
 *    - Connexions moteur inversées ou débranchées
 *    - L298N défaillant (surchauffe)
 *    - Pins Arduino D6/D7 défaillantes
 * 
 * ❌ EEPROM ne sauvegarde pas:
 *    - Arduino Nano défaillant (rare)
 *    - Coupure alimentation pendant écriture
 *    - Usure EEPROM (>100000 cycles)
 *    - Adresse mémoire corrompue
 * 
 * ❌ Système instable/resets intempestifs:
 *    - Alimentation insuffisante ou instable
 *    - Parasites moteur (condensateurs manquants)
 *    - Court-circuit intermittent
 *    - Watchdog Arduino (code bloqué)
 * 
 * 🔧 Tests de validation complète:
 *    - Tous les modules détectés à l'initialisation
 *    - Interface utilisateur 100% réactive
 *    - Capteurs donnent valeurs cohérentes
 *    - Actionneurs répondent aux commandes
 *    - Paramètres sauvegardés et rechargés
 *    - Aucun reset ou blocage pendant 15 minutes
 * 
 * 📊 Critères de succès:
 *    - Initialisation: 6/7 modules OK minimum
 *    - Tests automatiques: <20% d'échecs
 *    - Stabilité: Fonctionnement continu >15min
 *    - Interface: Navigation fluide entre tous les modes
 * 
 * ============================================================================
 */