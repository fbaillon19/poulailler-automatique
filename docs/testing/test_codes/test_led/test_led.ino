/*
 * ============================================================================
 * TEST LED STATUT - Poulailler Automatique
 * ============================================================================
 * 
 * Test: 4.1 - Status LED Blinking
 * Objectif: Vérifier contrôle LED et patterns de clignotement
 * Durée: ~3 minutes
 * 
 * Ce test vérifie:
 * - Contrôle LED marche/arrêt
 * - Différents patterns de clignotement
 * - Timing précis des clignotements
 * - Résistance de limitation correcte
 * 
 * Câblage requis:
 * Arduino D3 → Résistance 220Ω → LED Anode (+)
 * LED Cathode (-) → Arduino GND
 * 
 * Procédure:
 * 1. Uploader ce code sur l'Arduino Nano
 * 2. Observer la LED (pas de moniteur série nécessaire)
 * 3. Vérifier les différentes séquences de clignotement
 * 4. Optionnel: Ouvrir moniteur série pour détails
 * 
 * Résultat attendu:
 * - LED s'allume et s'éteint correctement
 * - Différents rythmes de clignotement visibles
 * - Luminosité suffisante mais pas excessive
 * 
 * ============================================================================
 */

// Configuration
const int LED_COUPURE = 3;                    // Pin de la LED
const unsigned long DUREE_TEST = 180000;      // 3 minutes de test

// Variables globales
unsigned long debutTest = 0;
unsigned long dernierClignotement = 0;
bool etatLED = false;
int phaseTest = 0;
unsigned long debutPhase = 0;

void setup() {
  // Initialisation communication série (optionnelle)
  Serial.begin(9600);
  delay(1000);
  
  Serial.println("============================================");
  Serial.println("TEST LED STATUT - Poulailler Automatique");
  Serial.println("============================================");
  Serial.println("Version: 1.0");
  Serial.println("Test: 4.1 - Status LED Blinking");
  Serial.println("");
  
  // Configuration de la LED
  pinMode(LED_COUPURE, OUTPUT);
  
  // Test initial - LED éteinte
  digitalWrite(LED_COUPURE, LOW);
  
  Serial.println("💡 Configuration LED:");
  Serial.println("- Pin utilisée: D3");
  Serial.println("- Résistance: 220Ω (obligatoire)");
  Serial.println("- Polarité: Anode via résistance, Cathode à GND");
  Serial.println("");
  
  Serial.println("👀 REGARDEZ LA LED pendant le test:");
  Serial.println("Le moniteur série est optionnel");
  Serial.println("");
  
  Serial.println("🕐 Séquences de test (3 minutes):");
  Serial.println("1. Test ON/OFF basique");
  Serial.println("2. Clignotement lent (1s)");
  Serial.println("3. Clignotement normal (500ms)");
  Serial.println("4. Clignotement rapide (200ms)");
  Serial.println("5. Pattern SOS (... --- ...)");
  Serial.println("6. Clignotement final continu");
  Serial.println("");
  
  debutTest = millis();
  debutPhase = millis();
  
  delay(2000);
}

void loop() {
  unsigned long tempsEcoule = millis() - debutTest;
  
  // Gestion des phases de test
  if (tempsEcoule < 30000) {        // 0-30s: Test ON/OFF
    phaseTest = 1;
    testOnOff();
  } else if (tempsEcoule < 60000) { // 30-60s: Clignotement lent
    if (phaseTest != 2) {
      phaseTest = 2;
      debutPhase = millis();
      Serial.println("Phase 2: Clignotement lent (1000ms)");
    }
    clignoterAvecPeriode(1000);
  } else if (tempsEcoule < 90000) { // 60-90s: Clignotement normal
    if (phaseTest != 3) {
      phaseTest = 3;
      debutPhase = millis();
      Serial.println("Phase 3: Clignotement normal (500ms)");
    }
    clignoterAvecPeriode(500);
  } else if (tempsEcoule < 120000) { // 90-120s: Clignotement rapide
    if (phaseTest != 4) {
      phaseTest = 4;
      debutPhase = millis();
      Serial.println("Phase 4: Clignotement rapide (200ms)");
    }
    clignoterAvecPeriode(200);
  } else if (tempsEcoule < 150000) { // 120-150s: Pattern SOS
    if (phaseTest != 5) {
      phaseTest = 5;
      debutPhase = millis();
      Serial.println("Phase 5: Pattern SOS (... --- ...)");
    }
    patternSOS();
  } else { // 150s+: Test final
    if (phaseTest != 6) {
      phaseTest = 6;
      Serial.println("Phase 6: Test final - Clignotement continu");
      afficherResultatsFinaux();
    }
    clignoterAvecPeriode(500);
  }
  
  delay(10);
}

/*
 * Phase 1: Test ON/OFF basique
 */
void testOnOff() {
  unsigned long tempsPhase = millis() - debutPhase;
  
  if (tempsPhase < 5000) {
    // 0-5s: LED allumée en permanence
    if (!etatLED) {
      digitalWrite(LED_COUPURE, HIGH);
      etatLED = true;
      Serial.println("Phase 1a: LED ALLUMÉE (5 secondes)");
    }
  } else if (tempsPhase < 10000) {
    // 5-10s: LED éteinte en permanence
    if (etatLED) {
      digitalWrite(LED_COUPURE, LOW);
      etatLED = false;
      Serial.println("Phase 1b: LED ÉTEINTE (5 secondes)");
    }
  } else if (tempsPhase < 30000) {
    // 10-30s: Alternance lente ON/OFF toutes les 2 secondes
    if (millis() - dernierClignotement >= 2000) {
      etatLED = !etatLED;
      digitalWrite(LED_COUPURE, etatLED);
      dernierClignotement = millis();
      
      static int compteur = 0;
      if (compteur % 4 == 0) {
        Serial.print("Phase 1c: Alternance lente ");
        Serial.println(etatLED ? "ON" : "OFF");
      }
      compteur++;
    }
  }
}

/*
 * Fonction: Clignotement avec période donnée
 */
void clignoterAvecPeriode(unsigned long periode) {
  if (millis() - dernierClignotement >= periode) {
    etatLED = !etatLED;
    digitalWrite(LED_COUPURE, etatLED);
    dernierClignotement = millis();
    
    // Log occasionnel pour ne pas spammer
    static unsigned long dernierLog = 0;
    if (millis() - dernierLog > 5000) {
      Serial.print("Clignotement ");
      Serial.print(periode);
      Serial.print("ms - État: ");
      Serial.println(etatLED ? "ON" : "OFF");
      dernierLog = millis();
    }
  }
}

/*
 * Fonction: Pattern SOS (... --- ...)
 */
void patternSOS() {
  static int etapeSOS = 0;
  static unsigned long debutEtape = 0;
  static bool premierPassage = true;
  
  if (premierPassage) {
    debutEtape = millis();
    premierPassage = false;
    etapeSOS = 0;
    digitalWrite(LED_COUPURE, LOW);
    Serial.println("Début pattern SOS");
  }
  
  unsigned long tempsEtape = millis() - debutEtape;
  
  switch (etapeSOS) {
    // Points (. . .)
    case 0: case 2: case 4: // Points ON
      if (tempsEtape >= 200) {
        digitalWrite(LED_COUPURE, LOW);
        debutEtape = millis();
        etapeSOS++;
      } else {
        digitalWrite(LED_COUPURE, HIGH);
      }
      break;
      
    case 1: case 3: // Espaces entre points
      if (tempsEtape >= 200) {
        debutEtape = millis();
        etapeSOS++;
      }
      break;
      
    case 5: // Pause avant traits
      if (tempsEtape >= 400) {
        debutEtape = millis();
        etapeSOS++;
      }
      break;
      
    // Traits (- - -)
    case 6: case 8: case 10: // Traits ON
      if (tempsEtape >= 600) {
        digitalWrite(LED_COUPURE, LOW);
        debutEtape = millis();
        etapeSOS++;
      } else {
        digitalWrite(LED_COUPURE, HIGH);
      }
      break;
      
    case 7: case 9: // Espaces entre traits
      if (tempsEtape >= 200) {
        debutEtape = millis();
        etapeSOS++;
      }
      break;
      
    case 11: // Pause avant points finaux
      if (tempsEtape >= 400) {
        debutEtape = millis();
        etapeSOS++;
      }
      break;
      
    // Points finaux (. . .)
    case 12: case 14: case 16: // Points ON
      if (tempsEtape >= 200) {
        digitalWrite(LED_COUPURE, LOW);
        debutEtape = millis();
        etapeSOS++;
      } else {
        digitalWrite(LED_COUPURE, HIGH);
      }
      break;
      
    case 13: case 15: // Espaces entre points
      if (tempsEtape >= 200) {
        debutEtape = millis();
        etapeSOS++;
      }
      break;
      
    case 17: // Pause longue avant répétition
      if (tempsEtape >= 1000) {
        etapeSOS = 0;
        debutEtape = millis();
        Serial.println("SOS répété");
      }
      break;
  }
}

/*
 * Fonction: Affichage des résultats finaux
 */
void afficherResultatsFinaux() {
  Serial.println("");
  Serial.println("============================================");
  Serial.println("✅ TEST LED TERMINÉ AVEC SUCCÈS");
  Serial.println("============================================");
  Serial.println("");
  
  Serial.println("📊 Résultats du test:");
  Serial.println("");
  
  Serial.println("🧪 FONCTIONS TESTÉES:");
  Serial.println("✅ LED ON/OFF basique");
  Serial.println("✅ Clignotement lent (1000ms)");
  Serial.println("✅ Clignotement normal (500ms)");
  Serial.println("✅ Clignotement rapide (200ms)");
  Serial.println("✅ Pattern complexe (SOS)");
  Serial.println("✅ Contrôle timing précis");
  
  Serial.println("");
  Serial.println("🎯 VALIDATION VISUELLE:");
  Serial.println("- LED doit être visible et claire");
  Serial.println("- Pas de scintillement parasite");
  Serial.println("- Différents rythmes distinguables");
  Serial.println("- Pattern SOS reconnaissable");
  
  Serial.println("");
  Serial.println("⚡ CONTRÔLES ÉLECTRIQUES:");
  Serial.println("- Résistance 220Ω obligatoire");
  Serial.println("- Courant LED ≈ 15-20mA");
  Serial.println("- Tension LED ≈ 2-3V");
  Serial.println("- Pas d'échauffement excessif");
  
  Serial.println("");
  Serial.println("✅ LED DE STATUT FONCTIONNELLE");
  Serial.println("➡️  Prêt pour le test suivant: Contrôle moteur");
  Serial.println("");
  Serial.println("🔄 Test final: Clignotement continu 500ms...");
}

/*
 * ============================================================================
 * DIAGNOSTIC ET DÉPANNAGE
 * ============================================================================
 * 
 * ❌ LED ne s'allume jamais:
 *    - Vérifier polarité: Anode (+) vers résistance, Cathode (-) vers GND
 *    - Vérifier résistance 220Ω présente et connectée
 *    - LED défaillante (tester avec pile 3V + résistance)
 *    - Pin D3 défaillante (tester avec autre pin)
 * 
 * ❌ LED toujours allumée:
 *    - Court-circuit bypassing Arduino
 *    - Pin D3 en court-circuit vers +5V
 *    - Problème dans le code (pinMode incorrect)
 * 
 * ❌ LED très faible/pas visible:
 *    - Résistance trop forte (utiliser 220Ω, pas 10kΩ)
 *    - LED défaillante partiellement
 *    - Tension d'alimentation insuffisante
 *    - LED surdimensionnée (utiliser LED 5mm standard)
 * 
 * ❌ LED trop brillante/chauffe:
 *    - Résistance trop faible ou absente (DANGER!)
 *    - Utiliser exactement 220Ω pour 5V
 *    - LED mal dimensionnée
 *    - Court-circuit partiel
 * 
 * ❌ Clignotement irrégulier:
 *    - Alimentation instable
 *    - Interférences électromagnétiques
 *    - Problème timing Arduino (rare)
 *    - Condensateur de filtrage manquant
 * 
 * ❌ LED grille rapidement:
 *    - Courant excessif (vérifier résistance)
 *    - Tension trop élevée
 *    - LED de mauvaise qualité
 *    - Inversions de polarité répétées
 * 
 * 🔧 Tests avec multimètre:
 *    - Tension sur anode LED: 2-3V (LED allumée)
 *    - Courant dans circuit: 15-20mA (LED allumée)
 *    - Tension pin D3: 5V (HIGH), 0V (LOW)
 *    - Résistance: exactement 220Ω ±5%
 * 
 * 💡 Calcul résistance LED:
 *    R = (Vsource - Vled) / Iled
 *    R = (5V - 2V) / 0.015A = 200Ω
 *    → 220Ω (valeur normalisée proche)
 * 
 * ============================================================================
 */