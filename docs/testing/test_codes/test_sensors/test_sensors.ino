/*
 * ============================================================================
 * TEST CAPTEURS - Poulailler Automatique
 * ============================================================================
 * 
 * Test: 3.1 - Light Sensor + Limit Switches
 * Objectif: Vérifier fonctionnement de tous les capteurs
 * Durée: ~10 minutes
 * 
 * Ce test vérifie:
 * - Capteur de luminosité (photorésistance + diviseur)
 * - Capteur fin de course HAUT
 * - Capteur fin de course BAS
 * - Variations des valeurs selon conditions
 * 
 * Câblage requis:
 * Photorésistance:
 *   LDR pin1 → Arduino 5V
 *   LDR pin2 → Arduino A0 + Résistance 10kΩ pin1
 *   Résistance 10kΩ pin2 → Arduino GND
 * 
 * Capteurs fin de course:
 *   Capteur HAUT NO → Arduino D8
 *   Capteur HAUT Common → Arduino GND
 *   Capteur BAS NO → Arduino D9
 *   Capteur BAS Common → Arduino GND
 * 
 * Procédure:
 * 1. Uploader ce code sur l'Arduino Nano
 * 2. Ouvrir moniteur série (9600 bauds)
 * 3. Couvrir/découvrir capteur luminosité
 * 4. Appuyer sur capteurs fin de course
 * 5. Observer variations dans moniteur série
 * 
 * Résultat attendu:
 * - Valeurs luminosité variables (50-900)
 * - Capteurs fin de course changent d'état
 * - Réponse immédiate aux actions utilisateur
 * 
 * ============================================================================
 */

// Configuration des pins
const int CAPTEUR_LUMIERE = A0;
const int FIN_COURSE_HAUT = 8;
const int FIN_COURSE_BAS = 9;

// Variables globales
unsigned long dernierAffichage = 0;
unsigned long compteurTest = 0;

// Variables pour détection changements
int ancienneValeurLumiere = -1;
bool ancienEtatHaut = true;
bool ancienEtatBas = true;

// Statistiques pour le test
int valeurLumiereMin = 1023;
int valeurLumiereMax = 0;
int nombreChangementsHaut = 0;
int nombreChangementsBas = 0;
bool testHautEffectue = false;
bool testBasEffectue = false;

void setup() {
  // Initialisation communication série
  Serial.begin(9600);
  delay(1000);
  
  Serial.println("============================================");
  Serial.println("TEST CAPTEURS - Poulailler Automatique");
  Serial.println("============================================");
  Serial.println("Version: 1.0");
  Serial.println("Test: 3.1 - Light Sensor + Limit Switches");
  Serial.println("");
  
  // Configuration des pins
  pinMode(FIN_COURSE_HAUT, INPUT_PULLUP);
  pinMode(FIN_COURSE_BAS, INPUT_PULLUP);
  
  // Test initial des capteurs
  Serial.println("🔍 Test initial des capteurs...");
  Serial.println("");
  
  // Test capteur luminosité
  int valeurInit = analogRead(CAPTEUR_LUMIERE);
  Serial.print("💡 Capteur luminosité (A0): ");
  Serial.print(valeurInit);
  Serial.print(" (");
  if (valeurInit > 800) Serial.println("TRÈS CLAIR)");
  else if (valeurInit > 400) Serial.println("CLAIR)");
  else if (valeurInit > 200) Serial.println("SOMBRE)");
  else Serial.println("TRÈS SOMBRE)");
  
  // Test capteurs fin de course
  bool etatHautInit = digitalRead(FIN_COURSE_HAUT);
  bool etatBasInit = digitalRead(FIN_COURSE_BAS);
  
  Serial.print("🔘 Capteur fin course HAUT (D8): ");
  Serial.println(etatHautInit ? "LIBRE" : "ACTIONNÉ");
  Serial.print("🔘 Capteur fin course BAS (D9): ");
  Serial.println(etatBasInit ? "LIBRE" : "ACTIONNÉ");
  
  Serial.println("");
  Serial.println("📋 Instructions de test:");
  Serial.println("1. LUMINOSITÉ: Couvrez/découvrez le capteur avec votre main");
  Serial.println("2. ÉCLAIRAGE: Utilisez la lampe de votre téléphone");
  Serial.println("3. FIN COURSE: Appuyez physiquement sur chaque capteur");
  Serial.println("");
  Serial.println("🕐 Test automatique pendant 60 secondes...");
  Serial.println("Observer les valeurs et tester les capteurs manuellement");
  Serial.println("");
  
  // Initialisation valeurs de référence
  ancienneValeurLumiere = valeurInit;
  ancienEtatHaut = etatHautInit;
  ancienEtatBas = etatBasInit;
  
  delay(2000);
}

void loop() {
  // Mise à jour toutes les 200ms pour réactivité
  if (millis() - dernierAffichage >= 200) {
    dernierAffichage = millis();
    compteurTest++;
    
    // Lecture de tous les capteurs
    int valeurLumiere = analogRead(CAPTEUR_LUMIERE);
    bool etatHaut = digitalRead(FIN_COURSE_HAUT);
    bool etatBas = digitalRead(FIN_COURSE_BAS);
    
    // Mise à jour statistiques luminosité
    if (valeurLumiere < valeurLumiereMin) valeurLumiereMin = valeurLumiere;
    if (valeurLumiere > valeurLumiereMax) valeurLumiereMax = valeurLumiere;
    
    // Détection changements capteurs fin de course
    if (etatHaut != ancienEtatHaut) {
      nombreChangementsHaut++;
      testHautEffectue = true;
      Serial.print("🔄 HAUT: ");
      Serial.println(etatHaut ? "LIBRE" : "ACTIONNÉ ✅");
      ancienEtatHaut = etatHaut;
    }
    
    if (etatBas != ancienEtatBas) {
      nombreChangementsBas++;
      testBasEffectue = true;
      Serial.print("🔄 BAS: ");
      Serial.println(etatBas ? "LIBRE" : "ACTIONNÉ ✅");
      ancienEtatBas = etatBas;
    }
    
    // Affichage principal toutes les secondes
    if (compteurTest % 5 == 0) {
      Serial.print("Test ");
      Serial.print(compteurTest / 5);
      Serial.print("s | Lum: ");
      
      // Affichage valeur luminosité avec barre visuelle
      if (valeurLumiere < 100) Serial.print("  ");
      else if (valeurLumiere < 1000) Serial.print(" ");
      Serial.print(valeurLumiere);
      
      // Barre de progression visuelle
      Serial.print(" [");
      int barres = map(valeurLumiere, 0, 1023, 0, 10);
      for (int i = 0; i < 10; i++) {
        if (i < barres) Serial.print("█");
        else Serial.print("-");
      }
      Serial.print("]");
      
      // États capteurs fin de course
      Serial.print(" | H:");
      Serial.print(etatHaut ? "○" : "●");
      Serial.print(" B:");
      Serial.print(etatBas ? "○" : "●");
      
      // Détection variation significative luminosité
      int variation = abs(valeurLumiere - ancienneValeurLumiere);
      if (variation > 50) {
        Serial.print(" 🔄");
        ancienneValeurLumiere = valeurLumiere;
      }
      
      Serial.println("");
    }
    
    // Messages d'encouragement
    if (compteurTest == 50) { // 10 secondes
      Serial.println("");
      Serial.println("💡 Essayez de couvrir le capteur de luminosité!");
      Serial.println("");
    }
    
    if (compteurTest == 150) { // 30 secondes
      Serial.println("");
      Serial.println("🔘 N'oubliez pas de tester les capteurs fin de course!");
      Serial.println("");
    }
    
    // Test terminé après 60 secondes (300 * 200ms)
    if (compteurTest >= 300) {
      afficherResultatsFinaux(valeurLumiere, etatHaut, etatBas);
      
      // Boucle finale avec affichage continu
      while (true) {
        delay(500);
        
        int lum = analogRead(CAPTEUR_LUMIERE);
        bool h = digitalRead(FIN_COURSE_HAUT);
        bool b = digitalRead(FIN_COURSE_BAS);
        
        Serial.print("Final | Lum:");
        Serial.print(lum);
        Serial.print(" H:");
        Serial.print(h ? "○" : "●");
        Serial.print(" B:");
        Serial.print(b ? "○" : "●");
        Serial.print(" | Plage lum: ");
        Serial.print(valeurLumiereMin);
        Serial.print("-");
        Serial.println(valeurLumiereMax);
      }
    }
  }
  
  delay(10);
}

/*
 * Fonction: Affichage des résultats finaux
 */
void afficherResultatsFinaux(int lumFinale, bool hautFinal, bool basFinal) {
  Serial.println("");
  Serial.println("============================================");
  Serial.println("✅ TEST CAPTEURS TERMINÉ");
  Serial.println("============================================");
  Serial.println("");
  
  Serial.println("📊 Résultats du test:");
  Serial.println("");
  
  // Résultats capteur luminosité
  Serial.println("💡 CAPTEUR LUMINOSITÉ:");
  Serial.print("- Valeur finale: ");
  Serial.println(lumFinale);
  Serial.print("- Plage observée: ");
  Serial.print(valeurLumiereMin);
  Serial.print(" - ");
  Serial.println(valeurLumiereMax);
  
  int plage = valeurLumiereMax - valeurLumiereMin;
  Serial.print("- Variation totale: ");
  Serial.print(plage);
  if (plage > 300) {
    Serial.println(" ✅ EXCELLENTE");
  } else if (plage > 100) {
    Serial.println(" ✅ BONNE");
  } else if (plage > 50) {
    Serial.println(" ⚠️ FAIBLE (vérifier câblage)");
  } else {
    Serial.println(" ❌ TRÈS FAIBLE (problème capteur)");
  }
  
  // Résultats capteurs fin de course
  Serial.println("");
  Serial.println("🔘 CAPTEURS FIN DE COURSE:");
  Serial.print("- Capteur HAUT: ");
  Serial.print(hautFinal ? "LIBRE" : "ACTIONNÉ");
  Serial.print(" (");
  Serial.print(nombreChangementsHaut);
  Serial.print(" changements) ");
  Serial.println(testHautEffectue ? "✅" : "⚠️ Non testé");
  
  Serial.print("- Capteur BAS: ");
  Serial.print(basFinal ? "LIBRE" : "ACTIONNÉ");
  Serial.print(" (");
  Serial.print(nombreChangementsBas);
  Serial.print(" changements) ");
  Serial.println(testBasEffectue ? "✅" : "⚠️ Non testé");
  
  // Évaluation globale
  Serial.println("");
  Serial.println("🎯 ÉVALUATION GLOBALE:");
  
  bool luminositeOK = (plage > 100);
  bool capteursOK = (testHautEffectue && testBasEffectue);
  
  if (luminositeOK && capteursOK) {
    Serial.println("✅ TOUS CAPTEURS FONCTIONNELS");
    Serial.println("➡️  Prêt pour le test suivant: Bouton multifonction");
  } else {
    Serial.println("⚠️  QUELQUES PROBLÈMES DÉTECTÉS:");
    if (!luminositeOK) Serial.println("- Capteur luminosité: variation insuffisante");
    if (!capteursOK) Serial.println("- Capteurs fin course: test incomplet");
    Serial.println("➡️  Vérifier câblage avant test suivant");
  }
  
  Serial.println("");
}

/*
 * ============================================================================
 * DIAGNOSTIC ET DÉPANNAGE
 * ============================================================================
 * 
 * ❌ Capteur luminosité valeur fixe 0:
 *    - Court-circuit: vérifier câblage diviseur résistif
 *    - LDR défaillante ou mal connectée
 *    - Résistance 10kΩ mal connectée
 * 
 * ❌ Capteur luminosité valeur fixe 1023:
 *    - Circuit ouvert: vérifier toutes connexions
 *    - LDR déconnectée de A0
 *    - Résistance 10kΩ non connectée à GND
 * 
 * ❌ Capteur luminosité ne varie pas:
 *    - LDR défaillante (tester résistance avec multimètre)
 *    - Éclairage ambiant constant
 *    - Câblage incorrect du diviseur résistif
 * 
 * ❌ Capteur fin course toujours "LIBRE":
 *    - Capteur défaillant ou mal câblé
 *    - Connexion NO (Normalement Ouvert) incorrecte
 *    - Pull-up interne non activé (INPUT_PULLUP)
 * 
 * ❌ Capteur fin course toujours "ACTIONNÉ":
 *    - Court-circuit vers GND
 *    - Capteur mécaniquement bloqué
 *    - Câblage inversé (utiliser NC au lieu de NO)
 * 
 * 🔧 Tests manuels complémentaires:
 *    - Multimètre sur A0: doit varier 0-5V selon lumière
 *    - Multimètre sur D8/D9: 5V libre, 0V actionné
 *    - Test résistance LDR: varie 1kΩ-100kΩ selon lumière
 * 
 * ============================================================================
 */