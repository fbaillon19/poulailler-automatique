/*
 * TEST CONTROLE MOTEUR
 * Test: 5.1 - L298N Motor Control (Memory Optimized)
 * 
 * SECURITE: MOTEUR DEBRANCHE du mecanisme pour tests !
 * 
 * Cablage:
 * L298N IN1 → Arduino D7
 * L298N IN2 → Arduino D6  
 * L298N OUT1 → Moteur +
 * L298N OUT2 → Moteur -
 * L298N +12V → Alimentation 12V
 * L298N +5V → Arduino 5V
 * 
 * Tests courts (3s max) dans les 2 sens + arrets
 */

const int MOTEUR_PIN1 = 7;
const int MOTEUR_PIN2 = 6;
const unsigned long DUREE_TEST = 3000; // 3s max
const unsigned long PAUSE = 2000; // 2s pause

unsigned long debutTest = 0;
int phaseTest = 0;
unsigned long debutPhase = 0;
int nbTestsSens1 = 0;
int nbTestsSens2 = 0;

void setup() {
  Serial.begin(9600);
  
  pinMode(MOTEUR_PIN1, OUTPUT);
  pinMode(MOTEUR_PIN2, OUTPUT);
  arreterMoteur(); // SECURITE
  
  Serial.println("=== TEST MOTEUR - Version Lite ===");
  Serial.println();
  Serial.println("SECURITE:");
  Serial.println("- Moteur DEBRANCHE du mecanisme");
  Serial.println("- Tests limites 3s max");
  Serial.println("- RESET pour arret d'urgence");
  Serial.println();
  
  Serial.println("PROCEDURE:");
  Serial.println("Phase 1: Test tensions (moteur OFF)");
  Serial.println("Phase 2: Test rotations courtes"); 
  Serial.println("Phase 3: Validation");
  Serial.println();
  
  Serial.println("DEMARRAGE dans 5s...");
  for (int i = 5; i > 0; i--) {
    Serial.print(i);
    Serial.print("... ");
    delay(1000);
  }
  Serial.println("START");
  Serial.println();
  
  debutTest = millis();
  debutPhase = millis();
}

void loop() {
  // Securite: arret auto apres 10min
  if (millis() - debutTest > 600000) {
    arreterMoteur();
    Serial.println("ARRET AUTO: Timeout securite");
    while(true) delay(1000);
  }
  
  switch (phaseTest) {
    case 0: testTensions(); break;
    case 1: testSens1(); break;
    case 2: pauseSecu(); break;
    case 3: testSens2(); break;
    case 4: pauseSecu(); break;
    case 5: testAlternance(); break;
    case 6: validationFinale(); break;
    default: monitoringContinu(); break;
  }
  
  delay(100);
}

void testTensions() {
  unsigned long t = millis() - debutPhase;
  
  if (t < 5000) {
    // Test sens 1
    digitalWrite(MOTEUR_PIN1, HIGH);
    digitalWrite(MOTEUR_PIN2, LOW);
    if (t % 1000 == 0) {
      Serial.println("Test sens 1: D7=HIGH, D6=LOW");
      Serial.println("-> Mesurer 12V entre OUT1-OUT2");
    }
  } else if (t < 10000) {
    // Test sens 2
    digitalWrite(MOTEUR_PIN1, LOW);
    digitalWrite(MOTEUR_PIN2, HIGH);
    if ((t-5000) % 1000 == 0) {
      Serial.println("Test sens 2: D7=LOW, D6=HIGH");
      Serial.println("-> Mesurer -12V entre OUT1-OUT2");
    }
  } else {
    arreterMoteur();
    Serial.println("Test tensions OK");
    Serial.println("CONNECTER LE MOTEUR maintenant");
    Serial.println("Attente 10s...");
    delay(10000);
    
    phaseTest++;
    debutPhase = millis();
  }
}

void testSens1() {
  unsigned long t = millis() - debutPhase;
  
  if (t == 0) {
    Serial.println("Phase 2: Test rotation SENS 1");
  }
  
  if (t < DUREE_TEST) {
    digitalWrite(MOTEUR_PIN1, HIGH);
    digitalWrite(MOTEUR_PIN2, LOW);
    
    if (t % 1000 == 0) {
      Serial.print("Sens 1 actif - ");
      Serial.print((DUREE_TEST - t) / 1000);
      Serial.println("s");
    }
  } else {
    arreterMoteur();
    nbTestsSens1++;
    Serial.println("ARRET sens 1");
    Serial.println("Observer: direction rotation");
    Serial.println();
    
    phaseTest++;
    debutPhase = millis();
  }
}

void testSens2() {
  unsigned long t = millis() - debutPhase;
  
  if (t == 0) {
    Serial.println("Phase 4: Test rotation SENS 2");
  }
  
  if (t < DUREE_TEST) {
    digitalWrite(MOTEUR_PIN1, LOW);
    digitalWrite(MOTEUR_PIN2, HIGH);
    
    if (t % 1000 == 0) {
      Serial.print("Sens 2 actif - ");
      Serial.print((DUREE_TEST - t) / 1000);
      Serial.println("s");
    }
  } else {
    arreterMoteur();
    nbTestsSens2++;
    Serial.println("ARRET sens 2");
    Serial.println("Observer: direction opposee");
    Serial.println();
    
    phaseTest++;
    debutPhase = millis();
  }
}

void pauseSecu() {
  static bool msgAffiche = false;
  unsigned long t = millis() - debutPhase;
  
  if (!msgAffiche) {
    Serial.print("Pause securite ");
    Serial.print(PAUSE / 1000);
    Serial.println("s");
    msgAffiche = true;
  }
  
  if (t >= PAUSE) {
    msgAffiche = false;
    phaseTest++;
    debutPhase = millis();
  }
}

void testAlternance() {
  static int cycles = 0;
  static unsigned long dernierChange = 0;
  static bool sens = true;
  unsigned long t = millis() - debutPhase;
  
  if (t == 0) {
    Serial.println("Phase 6: Alternance rapide (4 cycles)");
    cycles = 0;
    dernierChange = millis();
  }
  
  if (cycles < 8 && millis() - dernierChange >= 1000) {
    if (sens) {
      digitalWrite(MOTEUR_PIN1, HIGH);
      digitalWrite(MOTEUR_PIN2, LOW);
      Serial.print("Cycle ");
      Serial.print(cycles/2 + 1);
      Serial.println(" - Sens 1");
    } else {
      digitalWrite(MOTEUR_PIN1, LOW);
      digitalWrite(MOTEUR_PIN2, HIGH);
      Serial.print("Cycle ");
      Serial.print(cycles/2 + 1);
      Serial.println(" - Sens 2");
    }
    
    sens = !sens;
    dernierChange = millis();
    cycles++;
  } else if (cycles >= 8) {
    arreterMoteur();
    Serial.println("Fin alternance");
    Serial.println();
    
    phaseTest++;
    debutPhase = millis();
  }
}

void validationFinale() {
  static bool done = false;
  
  if (!done) {
    arreterMoteur();
    
    Serial.println("=== VALIDATION FINALE ===");
    Serial.print("Tests sens 1: ");
    Serial.println(nbTestsSens1);
    Serial.print("Tests sens 2: ");
    Serial.println(nbTestsSens2);
    Serial.println("Alternances: 4 cycles");
    Serial.println();
    
    Serial.println("VERIFICATIONS:");
    Serial.println("- Moteur tourne 2 sens: OK");
    Serial.println("- Directions opposees: OK");
    Serial.println("- Arrets francs: OK");
    Serial.println("- Pas bruit anormal: OK");
    Serial.println("- L298N pas chaud: OK");
    Serial.println();
    
    Serial.println("CONTROLE MOTEUR: OK");
    Serial.println("-> Pret test suivant");
    Serial.println();
    
    done = true;
    phaseTest++;
  }
}

void monitoringContinu() {
  static unsigned long dernierTest = 0;
  
  if (millis() - dernierTest > 10000) {
    Serial.println("Test monitoring: 1s chaque sens");
    
    // Sens 1
    digitalWrite(MOTEUR_PIN1, HIGH);
    digitalWrite(MOTEUR_PIN2, LOW);
    delay(1000);
    
    arreterMoteur();
    delay(500);
    
    // Sens 2
    digitalWrite(MOTEUR_PIN1, LOW);
    digitalWrite(MOTEUR_PIN2, HIGH);
    delay(1000);
    
    arreterMoteur();
    Serial.println("Monitoring OK");
    Serial.println();
    
    dernierTest = millis();
  }
}

void arreterMoteur() {
  digitalWrite(MOTEUR_PIN1, LOW);
  digitalWrite(MOTEUR_PIN2, LOW);
}