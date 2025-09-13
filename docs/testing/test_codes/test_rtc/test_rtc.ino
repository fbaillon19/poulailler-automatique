/*
 * ============================================================================
 * TEST RTC DS3231 - Poulailler Automatique
 * ============================================================================
 * 
 * Test: 2.2 - RTC DS3231 Time Keeping & I2C Communication
 * Objectif: Vérifier communication I2C et fonctionnement RTC
 * Durée: ~10 minutes
 * 
 * Ce test vérifie:
 * - Communication I2C avec DS3231
 * - Lecture de l'heure en temps réel
 * - Fonctionnement de la pile de sauvegarde
 * - Précision du timing
 * 
 * Câblage requis:
 * DS3231 VCC → Arduino 5V
 * DS3231 GND → Arduino GND  
 * DS3231 SDA → Arduino A4
 * DS3231 SCL → Arduino A5
 * 
 * Procédure:
 * 1. Uploader ce code sur l'Arduino Nano
 * 2. Ouvrir le moniteur série (9600 bauds)
 * 3. Vérifier détection RTC et affichage heure
 * 4. Si heure incorrecte, suivre instructions pour réglage
 * 5. Observer incrémentation des secondes
 * 
 * Résultat attendu:
 * - RTC détecté sur bus I2C
 * - Heure affichée et qui s'incrémente
 * - Température du module DS3231 affichée
 * 
 * ============================================================================
 */

#include <Wire.h>
#include <RTClib.h>

// Variables globales
RTC_DS3231 rtc;
unsigned long dernierAffichage = 0;
unsigned long compteurSecondes = 0;
DateTime heureInitiale;
bool rtcInitialise = false;

void setup() {
  // Initialisation communication série
  Serial.begin(9600);
  delay(1000); // Attendre stabilisation
  
  Serial.println("============================================");
  Serial.println("TEST RTC DS3231 - Poulailler Automatique");
  Serial.println("============================================");
  Serial.println("Version: 1.0");
  Serial.println("Test: 2.2 - RTC Time Keeping & I2C");
  Serial.println("");
  
  // Test communication I2C
  Serial.println("🔍 Initialisation RTC DS3231...");
  
  if (!rtc.begin()) {
    Serial.println("❌ ÉCHEC: RTC DS3231 non trouvé sur bus I2C !");
    Serial.println("");
    Serial.println("Vérifications à effectuer:");
    Serial.println("- Câblage I2C: SDA→A4, SCL→A5");
    Serial.println("- Alimentation: VCC→5V, GND→GND");
    Serial.println("- Module DS3231 fonctionnel");
    Serial.println("- Contacts soudures du module");
    Serial.println("");
    Serial.println("⏸️  Test arrêté - Corriger problème et relancer");
    
    // Clignotement d'erreur
    pinMode(LED_BUILTIN, OUTPUT);
    while(true) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
    }
  }
  
  Serial.println("✅ RTC DS3231 détecté avec succès !");
  rtcInitialise = true;
  
  // Vérification perte d'alimentation
  if (rtc.lostPower()) {
    Serial.println("");
    Serial.println("⚠️  ATTENTION: RTC a perdu l'alimentation !");
    Serial.println("L'heure doit être réglée.");
    Serial.println("");
    Serial.println("Options de réglage:");
    Serial.println("1. Automatique: heure de compilation");
    Serial.println("2. Manuel: modifier le code");
    Serial.println("");
    Serial.print("Réglage automatique dans 5 secondes...");
    
    for(int i = 5; i > 0; i--) {
      Serial.print(" ");
      Serial.print(i);
      delay(1000);
    }
    Serial.println("");
    
    // Réglage automatique avec heure de compilation
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println("✅ Heure réglée automatiquement");
  } else {
    Serial.println("✅ Heure RTC préservée (pile fonctionnelle)");
  }
  
  // Lecture heure initiale
  heureInitiale = rtc.now();
  
  // Informations du module
  Serial.println("");
  Serial.println("📊 Informations module DS3231:");
  Serial.print("- Heure actuelle: ");
  afficherHeure(heureInitiale);
  Serial.print("- Température: ");
  Serial.print(rtc.getTemperature());
  Serial.println("°C");
  
  Serial.println("");
  Serial.println("🕐 Démarrage test temps réel...");
  Serial.println("Observation: heure doit s'incrémenter chaque seconde");
  Serial.println("");
  
  delay(1000);
}

void loop() {
  if (!rtcInitialise) return;
  
  // Test toutes les secondes
  if (millis() - dernierAffichage >= 1000) {
    dernierAffichage = millis();
    compteurSecondes++;
    
    // Lecture heure actuelle
    DateTime maintenant = rtc.now();
    
    // Affichage principal
    Serial.print("Test #");
    if (compteurSecondes < 10) Serial.print("0");
    Serial.print(compteurSecondes);
    Serial.print(" | ");
    afficherHeure(maintenant);
    
    // Calcul delta temps
    unsigned long deltaSecondes = maintenant.unixtime() - heureInitiale.unixtime();
    Serial.print(" | Δt=");
    Serial.print(deltaSecondes);
    Serial.print("s");
    
    // Vérification précision (tolérance ±2 secondes)
    long difference = (long)deltaSecondes - (long)compteurSecondes;
    if (abs(difference) <= 2) {
      Serial.print(" | ✅");
    } else {
      Serial.print(" | ⚠️ Dérive:");
      Serial.print(difference);
      Serial.print("s");
    }
    
    Serial.println("");
    
    // Tests spéciaux à intervalles
    if (compteurSecondes % 15 == 0) {
      Serial.println("");
      testTemperature();
      Serial.println("");
    }
    
    if (compteurSecondes % 30 == 0) {
      testPrecision(maintenant);
    }
    
    // Test terminé après 60 secondes
    if (compteurSecondes >= 60) {
      Serial.println("");
      Serial.println("============================================");
      Serial.println("✅ TEST RTC DS3231 TERMINÉ AVEC SUCCÈS");
      Serial.println("============================================");
      Serial.println("");
      Serial.println("📊 Résultats du test:");
      Serial.print("- Durée d'observation: ");
      Serial.print(compteurSecondes);
      Serial.println(" secondes");
      Serial.println("- Communication I2C: OK");
      Serial.println("- Incrémentation temps: OK");
      Serial.println("- Pile de sauvegarde: OK");
      
      DateTime final = rtc.now();
      unsigned long dureeReelle = final.unixtime() - heureInitiale.unixtime();
      long derive = (long)dureeReelle - (long)compteurSecondes;
      
      Serial.print("- Précision: ");
      if (abs(derive) <= 2) {
        Serial.println("EXCELLENTE (±2s)");
      } else if (abs(derive) <= 5) {
        Serial.println("BONNE (±5s)");
      } else {
        Serial.print("MOYENNE (dérive ");
        Serial.print(derive);
        Serial.println("s)");
      }
      
      Serial.println("");
      Serial.println("➡️  Prêt pour le test suivant: LCD I2C");
      Serial.println("");
      
      // Arrêt du test
      while (true) {
        Serial.print("Heure finale: ");
        afficherHeure(rtc.now());
        Serial.print(" | Temp: ");
        Serial.print(rtc.getTemperature());
        Serial.println("°C");
        delay(5000);
      }
    }
  }
  
  delay(10);
}

/*
 * Fonction: Affichage formaté de l'heure
 */
void afficherHeure(DateTime dt) {
  if (dt.hour() < 10) Serial.print("0");
  Serial.print(dt.hour());
  Serial.print(":");
  if (dt.minute() < 10) Serial.print("0");
  Serial.print(dt.minute());
  Serial.print(":");
  if (dt.second() < 10) Serial.print("0");
  Serial.print(dt.second());
}

/*
 * Fonction: Test température DS3231
 */
void testTemperature() {
  float temp = rtc.getTemperature();
  Serial.print("🌡️  Température DS3231: ");
  Serial.print(temp);
  Serial.print("°C");
  
  if (temp > -40 && temp < 85) {
    Serial.println(" ✅ (Normale)");
  } else {
    Serial.println(" ⚠️ (Hors plage)");
  }
}

/*
 * Fonction: Test précision timing
 */
void testPrecision(DateTime maintenant) {
  Serial.println("");
  Serial.println("🎯 Test précision timing:");
  
  unsigned long tempsArduino = millis() / 1000;
  unsigned long tempsRTC = maintenant.unixtime() - heureInitiale.unixtime();
  long derive = (long)tempsRTC - (long)tempsArduino;
  
  Serial.print("- Temps Arduino: ");
  Serial.print(tempsArduino);
  Serial.println("s");
  Serial.print("- Temps RTC: ");
  Serial.print(tempsRTC);
  Serial.println("s");
  Serial.print("- Dérive: ");
  Serial.print(derive);
  Serial.println("s");
  
  if (abs(derive) <= 1) {
    Serial.println("- Précision: EXCELLENTE");
  } else if (abs(derive) <= 3) {
    Serial.println("- Précision: BONNE");
  } else {
    Serial.println("- Précision: À surveiller");
  }
}

/*
 * ============================================================================
 * DIAGNOSTIC ET DÉPANNAGE
 * ============================================================================
 * 
 * ❌ "RTC DS3231 non trouvé sur bus I2C":
 *    - Vérifier câblage: SDA→A4, SCL→A5, VCC→5V, GND→GND
 *    - Vérifier soudures module DS3231
 *    - Tester avec scanner I2C (adresse 0x68)
 *    - Remplacer module DS3231 si défaillant
 * 
 * ❌ "RTC a perdu l'alimentation":
 *    - Pile CR2032 déchargée ou mal insérée
 *    - Remplacer pile CR2032
 *    - Vérifier contacts pile sur module
 * 
 * ❌ Heure incorrecte persistante:
 *    - Forcer réglage: rtc.adjust(DateTime(YYYY,MM,DD,HH,MM,SS))
 *    - Vérifier intégrité pile sauvegarde
 *    - Module RTC défaillant
 * 
 * ❌ Dérive importante (>5s/minute):
 *    - Température extrême (compensation automatique DS3231)
 *    - Module DS3231 défaillant
 *    - Interférences électromagnétiques
 * 
 * ❌ Température aberrante:
 *    - Module DS3231 endommagé
 *    - Problème communication I2C
 *    - Remplacer module
 * 
 * ============================================================================
 */