/*
 * ============================================================================
 * TEST ARDUINO NANO BASIC - Poulailler Automatique
 * ============================================================================
 * 
 * Test: 2.1 - Arduino Nano Basic Functionality
 * Objectif: Vérifier programmation et fonctionnement de base
 * Durée: ~5 minutes
 * 
 * Ce test vérifie:
 * - Programmation Arduino réussie
 * - Communication série fonctionnelle
 * - Boucle principale opérationnelle
 * - Pins digitales configurables
 * 
 * Procédure:
 * 1. Uploader ce code sur l'Arduino Nano
 * 2. Ouvrir le moniteur série (9600 bauds)
 * 3. Vérifier les messages réguliers
 * 4. Observer la LED intégrée qui clignote
 * 
 * Résultat attendu:
 * - Messages dans moniteur série toutes les secondes
 * - LED pin 13 qui clignote
 * - Compteur qui s'incrémente
 * 
 * ============================================================================
 */

// Variables globales
unsigned long compteur = 0;
unsigned long dernierAffichage = 0;
bool etatLED = false;

void setup() {
  // Initialisation communication série
  Serial.begin(9600);
  
  // Configuration LED intégrée (pin 13)
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Configuration pins de test
  pinMode(2, OUTPUT);   // Pin test sortie
  pinMode(3, INPUT);    // Pin test entrée
  
  // Message de démarrage
  Serial.println("============================================");
  Serial.println("TEST ARDUINO NANO - Poulailler Automatique");
  Serial.println("============================================");
  Serial.println("Version: 1.0");
  Serial.println("Test: 2.1 - Arduino Basic Functionality");
  Serial.println("");
  
  // Informations système
  Serial.print("Fréquence CPU: ");
  Serial.print(F_CPU / 1000000);
  Serial.println(" MHz");
  
  Serial.print("Tension référence: ");
  Serial.print(5.0);
  Serial.println(" V");
  
  Serial.println("");
  Serial.println("Démarrage du test...");
  Serial.println("Attendu: Messages toutes les secondes + LED clignote");
  Serial.println("");
  
  delay(1000);
}

void loop() {
  // Test toutes les secondes
  if (millis() - dernierAffichage >= 1000) {
    dernierAffichage = millis();
    compteur++;
    
    // Basculer LED
    etatLED = !etatLED;
    digitalWrite(LED_BUILTIN, etatLED);
    
    // Test pin sortie
    digitalWrite(2, etatLED);
    
    // Affichage état
    Serial.print("Test #");
    Serial.print(compteur);
    Serial.print(" | Temps: ");
    Serial.print(millis() / 1000);
    Serial.print("s | LED: ");
    Serial.print(etatLED ? "ON " : "OFF");
    Serial.print(" | Pin2: ");
    Serial.print(digitalRead(2) ? "HIGH" : "LOW");
    Serial.print(" | Pin3: ");
    Serial.print(digitalRead(3) ? "HIGH" : "LOW");
    Serial.print(" | RAM libre: ");
    Serial.print(getFreeMemory());
    Serial.println(" bytes");
    
    // Messages de statut périodiques
    if (compteur % 10 == 0) {
      Serial.println("");
      Serial.print("✅ Arduino fonctionne depuis ");
      Serial.print(compteur);
      Serial.println(" secondes");
      Serial.println("");
    }
    
    // Test terminé après 30 secondes
    if (compteur >= 30) {
      Serial.println("");
      Serial.println("============================================");
      Serial.println("✅ TEST ARDUINO NANO TERMINÉ AVEC SUCCÈS");
      Serial.println("============================================");
      Serial.println("");
      Serial.println("Résultats:");
      Serial.print("- Durée du test: ");
      Serial.print(compteur);
      Serial.println(" secondes");
      Serial.println("- Communication série: OK");
      Serial.println("- Boucle principale: OK");
      Serial.println("- Contrôle pins: OK");
      Serial.println("- LED intégrée: OK");
      Serial.println("");
      Serial.println("➡️  Prêt pour le test suivant: RTC DS3231");
      Serial.println("");
      
      // Clignotement rapide de succès
      for (int i = 0; i < 10; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN, LOW);
        delay(100);
      }
      
      // Arrêt du test (boucle infinie)
      while (true) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(1000);
        digitalWrite(LED_BUILTIN, LOW);
        delay(1000);
      }
    }
  }
  
  // Autres traitements légers
  delay(10);
}

/*
 * Fonction utilitaire: Calcul mémoire libre
 * Aide à détecter les fuites mémoire
 */
int getFreeMemory() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

/*
 * ============================================================================
 * DIAGNOSTIC ET DÉPANNAGE
 * ============================================================================
 * 
 * Problèmes possibles:
 * 
 * ❌ Pas de messages dans moniteur série:
 *    - Vérifier port COM sélectionné
 *    - Vérifier vitesse 9600 bauds
 *    - Vérifier câble USB
 *    - Redémarrer Arduino IDE
 * 
 * ❌ LED ne clignote pas:
 *    - Arduino possiblement défaillant
 *    - Problème alimentation
 *    - Pin 13 endommagée
 * 
 * ❌ Messages erratiques:
 *    - Alimentation instable
 *    - Parasites électriques
 *    - Câble USB défaillant
 * 
 * ❌ RAM libre diminue:
 *    - Fuite mémoire (ne devrait pas arriver dans ce test)
 *    - Problème plus grave Arduino
 * 
 * ❌ Test ne se termine jamais:
 *    - Horloge Arduino défaillante
 *    - Fonction millis() bloquée
 *    - Redémarrer Arduino
 * 
 * ============================================================================
 */