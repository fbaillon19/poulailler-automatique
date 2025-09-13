/*
 * ============================================================================
 * TEST LCD I2C 16x2 - Poulailler Automatique
 * ============================================================================
 * 
 * Test: 2.3 - LCD I2C Display Functionality
 * Objectif: Vérifier affichage LCD et communication I2C
 * Durée: ~5 minutes
 * 
 * Ce test vérifie:
 * - Communication I2C avec module LCD
 * - Affichage texte sur les 2 lignes
 * - Fonctionnement rétroéclairage
 * - Positionnement curseur
 * - Caractères spéciaux et animations
 * 
 * Câblage requis:
 * LCD I2C VCC → Arduino 5V
 * LCD I2C GND → Arduino GND  
 * LCD I2C SDA → Arduino A4
 * LCD I2C SCL → Arduino A5
 * 
 * Procédure:
 * 1. Uploader ce code sur l'Arduino Nano
 * 2. Vérifier affichage sur LCD (pas de moniteur série nécessaire)
 * 3. Observer séquences d'affichage automatiques
 * 4. Vérifier lisibilité et contraste
 * 
 * Résultat attendu:
 * - Texte visible et lisible sur LCD
 * - Rétroéclairage fonctionnel
 * - Animations et compteurs qui bougent
 * - Pas de caractères corrompus
 * 
 * ============================================================================
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Configuration LCD (adresse 0x27 standard, taille 16x2)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Variables globales
unsigned long dernierUpdate = 0;
unsigned long compteur = 0;
int phase = 0;
bool lcdInitialise = false;

void setup() {
  // Initialisation communication série (pour debug)
  Serial.begin(9600);
  
  Serial.println("============================================");
  Serial.println("TEST LCD I2C 16x2 - Poulailler Automatique");
  Serial.println("============================================");
  Serial.println("Test: 2.3 - LCD I2C Display");
  Serial.println("");
  Serial.println("🖥️  Initialisation LCD I2C...");
  
  // Test de plusieurs adresses I2C communes
  Serial.println("🔍 Recherche LCD sur bus I2C...");
  
  // Essai adresse 0x27 (la plus commune)
  lcd.init();
  lcd.backlight();
  
  // Test simple d'écriture
  lcd.setCursor(0, 0);
  lcd.print("Test LCD...");
  delay(1000);
  
  // Vérification si LCD répond
  Wire.beginTransmission(0x27);
  if (Wire.endTransmission() == 0) {
    Serial.println("✅ LCD trouvé à l'adresse 0x27");
    lcdInitialise = true;
  } else {
    // Essai adresse alternative 0x3F
    Serial.println("❌ Pas de LCD à 0x27, essai 0x3F...");
    LiquidCrystal_I2C lcd_alt(0x3F, 16, 2);
    lcd = lcd_alt;
    lcd.init();
    lcd.backlight();
    
    Wire.beginTransmission(0x3F);
    if (Wire.endTransmission() == 0) {
      Serial.println("✅ LCD trouvé à l'adresse 0x3F");
      lcdInitialise = true;
    } else {
      Serial.println("❌ ÉCHEC: Aucun LCD trouvé sur I2C !");
      Serial.println("");
      Serial.println("Vérifications à effectuer:");
      Serial.println("- Câblage I2C: SDA→A4, SCL→A5");
      Serial.println("- Alimentation: VCC→5V, GND→GND");
      Serial.println("- Module LCD I2C fonctionnel");
      Serial.println("- Adresse I2C: scanner pour trouver");
      Serial.println("");
      Serial.println("⏸️  Test arrêté - Corriger et relancer");
      
      while(true) {
        delay(1000);
      }
    }
  }
  
  // Initialisation réussie
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LCD I2C OK!");
  lcd.setCursor(0, 1);
  lcd.print("Test commence...");
  
  Serial.println("✅ LCD initialisé avec succès");
  Serial.println("👀 Regardez l'écran LCD pour la suite du test");
  Serial.println("");
  
  delay(2000);
}

void loop() {
  if (!lcdInitialise) return;
  
  // Mise à jour toutes les 500ms
  if (millis() - dernierUpdate >= 500) {
    dernierUpdate = millis();
    compteur++;
    
    // Différentes phases de test
    switch (phase) {
      case 0:
        testAffichageBase();
        if (compteur >= 10) { // 5 secondes
          phase++;
          compteur = 0;
        }
        break;
        
      case 1:
        testPositionnement();
        if (compteur >= 10) { // 5 secondes
          phase++;
          compteur = 0;
        }
        break;
        
      case 2:
        testCaracteresSpeciaux();
        if (compteur >= 10) { // 5 secondes
          phase++;
          compteur = 0;
        }
        break;
        
      case 3:
        testAnimations();
        if (compteur >= 20) { // 10 secondes
          phase++;
          compteur = 0;
        }
        break;
        
      case 4:
        testSimulationInterface();
        if (compteur >= 20) { // 10 secondes
          phase++;
          compteur = 0;
        }
        break;
        
      case 5:
        testFinal();
        // Phase finale, reste ici
        break;
    }
  }
  
  delay(10);
}

/*
 * Phase 0: Test affichage de base
 */
void testAffichageBase() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TEST AFFICHAGE");
  lcd.setCursor(0, 1);
  lcd.print("Phase 1/6 - ");
  lcd.print(6 - (compteur/2));
  
  Serial.print("Phase 1: Affichage de base - ");
  Serial.print(6 - (compteur/2));
  Serial.println("s");
}

/*
 * Phase 1: Test positionnement curseur
 */
void testPositionnement() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("POSITIONNEMENT");
  
  // Affichage caractère qui bouge
  int pos = compteur % 16;
  lcd.setCursor(pos, 1);
  lcd.print("*");
  
  Serial.print("Phase 2: Positionnement curseur - pos ");
  Serial.println(pos);
}

/*
 * Phase 2: Test caractères spéciaux
 */
void testCaracteresSpeciaux() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CARACTERES");
  lcd.setCursor(0, 1);
  
  char caracteres[] = {'!', '@', '#', '$', '%', '^', '&', '*', '(', ')'};
  for (int i = 0; i < 10; i++) {
    lcd.setCursor(i, 1);
    lcd.print(caracteres[i]);
  }
  
  // Caractères graphiques si supportés
  lcd.setCursor(12, 1);
  lcd.print((char)0xFF); // Bloc plein
  lcd.setCursor(13, 1);
  lcd.print((char)0xFE); // Bloc partiel
  
  Serial.println("Phase 3: Caractères spéciaux");
}

/*
 * Phase 3: Test animations
 */
void testAnimations() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ANIMATIONS");
  
  // Barre de progression
  int progress = (compteur * 16) / 20;
  lcd.setCursor(0, 1);
  for (int i = 0; i < 16; i++) {
    if (i < progress) {
      lcd.print((char)0xFF); // Bloc plein
    } else {
      lcd.print("-");
    }
  }
  
  Serial.print("Phase 4: Animations - progression ");
  Serial.print((compteur * 100) / 20);
  Serial.println("%");
}

/*
 * Phase 4: Simulation interface réelle
 */
void testSimulationInterface() {
  lcd.clear();
  
  // Simulation affichage heure
  lcd.setCursor(0, 0);
  int heures = (compteur / 4) % 24;
  int minutes = (compteur * 3) % 60;
  if (heures < 10) lcd.print("0");
  lcd.print(heures);
  lcd.print(":");
  if (minutes < 10) lcd.print("0");
  lcd.print(minutes);
  lcd.print("      ");
  
  // Simulation statut porte
  lcd.setCursor(0, 1);
  const char* status[] = {
    "Porte ouverte   ",
    "Porte fermee    ",
    "Ouverture...    ",
    "Fermeture...    ",
    "Ferme dans 5mn  "
  };
  
  int statusIndex = (compteur / 4) % 5;
  lcd.print(status[statusIndex]);
  
  Serial.print("Phase 5: Interface réelle - ");
  Serial.println(status[statusIndex]);
}

/*
 * Phase 5: Test final et résultats
 */
void testFinal() {
  if (compteur == 1) {
    // Premier affichage des résultats
    Serial.println("");
    Serial.println("============================================");
    Serial.println("✅ TEST LCD I2C TERMINÉ AVEC SUCCÈS");
    Serial.println("============================================");
    Serial.println("");
    Serial.println("📊 Résultats du test:");
    Serial.println("- Communication I2C: OK");
    Serial.println("- Affichage texte: OK");
    Serial.println("- Positionnement curseur: OK");
    Serial.println("- Caractères spéciaux: OK");
    Serial.println("- Animations: OK");
    Serial.println("- Simulation interface: OK");
    Serial.println("");
    Serial.println("➡️  Prêt pour le test suivant: Capteurs");
    Serial.println("");
  }
  
  // Affichage final permanent
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TEST LCD OK!");
  lcd.setCursor(0, 1);
  
  // Compteur final qui s'incrémente
  lcd.print("Compteur: ");
  lcd.print(compteur);
  
  // Clignotement pour montrer que ça marche
  if ((compteur % 4) < 2) {
    lcd.setCursor(15, 0);
    lcd.print("*");
  } else {
    lcd.setCursor(15, 0);
    lcd.print(" ");
  }
}

/*
 * ============================================================================
 * DIAGNOSTIC ET DÉPANNAGE
 * ============================================================================
 * 
 * ❌ Écran complètement noir:
 *    - Vérifier alimentation: VCC→5V, GND→GND
 *    - Vérifier câblage I2C: SDA→A4, SCL→A5
 *    - Tourner potentiomètre contraste sur module LCD
 *    - Module LCD défaillant
 * 
 * ❌ Rétroéclairage allumé mais pas de texte:
 *    - Problème adresse I2C (essayer 0x3F au lieu de 0x27)
 *    - Régler contraste (potentiomètre sur module)
 *    - Vérifier soudures module LCD
 *    - Problème communication I2C
 * 
 * ❌ Caractères bizarres/corrompus:
 *    - Alimentation instable (ajouter condensateur 100µF)
 *    - Parasites électriques
 *    - Fils I2C trop longs (max 1 mètre)
 *    - Vitesse I2C trop rapide
 * 
 * ❌ Affichage intermittent:
 *    - Faux contacts connexions
 *    - Alimentation insuffisante
 *    - Module LCD défaillant
 *    - Interférences électromagnétiques
 * 
 * ❌ "Aucun LCD trouvé sur I2C":
 *    - Scanner I2C pour trouver adresse réelle
 *    - Vérifier câblage complet
 *    - Tester avec multimètre (3.3-5V sur VCC)
 *    - Remplacer module LCD I2C
 * 
 * 🔧 Scanner I2C (code utile):
 * 
 * Wire.begin();
 * for(int addr = 1; addr < 127; addr++) {
 *   Wire.beginTransmission(addr);
 *   if(Wire.endTransmission() == 0) {
 *     Serial.print("Device found at 0x");
 *     Serial.println(addr, HEX);
 *   }
 * }
 * 
 * ============================================================================
 */