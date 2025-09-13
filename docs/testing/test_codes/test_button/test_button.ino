/*
 * TEST BOUTON MULTIFONCTION - Version Allégée
 * Test: 3.3 - Multi-function Button (Light Version)
 * 
 * Câblage:
 * Bouton pin1 → Arduino D5
 * Bouton pin2 → Arduino GND
 */

const int BOUTON_PIN = 5;
const unsigned long APPUI_LONG = 3000;
const unsigned long DOUBLE_CLIC = 500;

// Variables bouton
unsigned long debutAppui = 0;
unsigned long dernierRelachement = 0;
bool boutonEnfonce = false;
bool appuiTraite = false;
bool enAttenteDoubleClick = false;

// Statistiques
int appuisCourts = 0;
int appuisLongs = 0;
int doubleClics = 0;
int rebonds = 0;
unsigned long compteurTest = 0;

void setup() {
  Serial.begin(9600);
  pinMode(BOUTON_PIN, INPUT_PULLUP);
  
  Serial.println(F("=== TEST BOUTON MULTIFONCTION ==="));
  Serial.print(F("Etat initial: "));
  Serial.println(digitalRead(BOUTON_PIN) ? F("LIBRE") : F("ENFONCE"));
  Serial.println(F("Instructions:"));
  Serial.println(F("- Court: <3s"));
  Serial.println(F("- Long: >3s"));  
  Serial.println(F("- Double-clic: 2 clics rapides"));
  Serial.println(F("Test 60s..."));
  Serial.println();
}

void loop() {
  compteurTest++;
  gererBouton();
  
  if (enAttenteDoubleClick && millis() - dernierRelachement > DOUBLE_CLIC) {
    gererAppuiBref();
    enAttenteDoubleClick = false;
  }
  
  // Stats toutes les 10s
  if (compteurTest % 10000 == 0) {
    Serial.print(F("Stats - Courts:"));
    Serial.print(appuisCourts);
    Serial.print(F(" Longs:"));
    Serial.print(appuisLongs);
    Serial.print(F(" Double:"));
    Serial.print(doubleClics);
    Serial.print(F(" Rebonds:"));
    Serial.println(rebonds);
  }
  
  // Fin test après 60s
  if (compteurTest > 600000) {
    Serial.println(F("\n=== RESULTAT TEST ==="));
    Serial.print(F("Appuis courts: ")); Serial.println(appuisCourts);
    Serial.print(F("Appuis longs: ")); Serial.println(appuisLongs);
    Serial.print(F("Double-clics: ")); Serial.println(doubleClics);
    Serial.print(F("Rebonds: ")); Serial.println(rebonds);
    
    if (appuisCourts > 0 && appuisLongs > 0 && doubleClics > 0) {
      Serial.println(F("BOUTON OK - Test suivant: LED"));
    } else {
      Serial.println(F("Test incomplet - Tester tous types"));
    }
    
    // Monitoring continu
    while (true) {
      gererBouton();
      if (enAttenteDoubleClick && millis() - dernierRelachement > DOUBLE_CLIC) {
        gererAppuiBref();
        enAttenteDoubleClick = false;
      }
      delay(10);
    }
  }
  
  delay(10);
}

void gererBouton() {
  bool etat = digitalRead(BOUTON_PIN);
  
  // Debut appui
  if (!boutonEnfonce && etat == LOW) {
    delay(20); // Anti-rebond
    if (digitalRead(BOUTON_PIN) == LOW) {
      boutonEnfonce = true;
      debutAppui = millis();
      appuiTraite = false;
      Serial.print(F("-> Appui t="));
      Serial.println(debutAppui);
    } else {
      rebonds++;
      Serial.println(F("Rebond filtre"));
    }
  }
  
  // Relachement
  if (boutonEnfonce && etat == HIGH) {
    delay(20);
    if (digitalRead(BOUTON_PIN) == HIGH) {
      boutonEnfonce = false;
      unsigned long duree = millis() - debutAppui;
      
      Serial.print(F("<- Relache "));
      Serial.print(duree);
      Serial.print(F("ms"));
      
      if (!appuiTraite) {
        if (duree >= APPUI_LONG) {
          Serial.println(F(" -> LONG"));
          appuisLongs++;
          gererAppuiLong();
        } else {
          Serial.println(F(" -> Bref"));
          
          if (enAttenteDoubleClick && (millis() - dernierRelachement) < DOUBLE_CLIC) {
            Serial.println(F("DOUBLE-CLIC!"));
            doubleClics++;
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
  
  // Indication appui long
  if (boutonEnfonce && !appuiTraite) {
    unsigned long duree = millis() - debutAppui;
    if (duree >= APPUI_LONG) {
      Serial.print(F("LONG en cours "));
      Serial.print(duree);
      Serial.println(F("ms"));
      appuiTraite = true;
    }
  }
}

void gererAppuiBref() {
  Serial.println(F("APPUI BREF confirme"));
  appuisCourts++;
}

void gererDoubleClick() {
  Serial.println(F("Action: Double-clic"));
}

void gererAppuiLong() {
  Serial.println(F("Action: Appui long"));
}

/*
 * DEPANNAGE:
 * - Pas de reponse: Verifier cablage D5-GND
 * - Rebonds: Bouton de mauvaise qualite
 * - Long non detecte: Maintenir >3s
 * - Double-clic rate: Cliquer plus vite
 */