#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

// Configuration des broches
const int MOTEUR_PIN1 = 7;        // Contrôle moteur direction 1
const int MOTEUR_PIN2 = 6;        // Contrôle moteur direction 2
const int CAPTEUR_LUMIERE = A0;   // Capteur de luminosité
const int BOUTON_PIN = 5;         // Bouton multifonction
const int FIN_COURSE_HAUT = 8;    // Capteur fin de course haut
const int FIN_COURSE_BAS = 9;     // Capteur fin de course bas
const int LED_COUPURE = 3;        // LED clignotante coupure courant

// Constantes
const unsigned long TEMPO_FERMETURE = 600000; // 10 minutes en millisecondes
const unsigned long APPUI_LONG = 3000;        // 3 secondes pour appui long
const unsigned long TIMEOUT_REGLAGE = 10000;  // 10 secondes timeout mode réglage
const unsigned long DOUBLE_CLIC = 500;        // 500ms pour détecter double-clic
const int SEUIL_EEPROM_ADDR = 0;              // Adresse EEPROM pour sauver le seuil
const int SEUIL_DEFAULT = 300;                // Valeur par défaut du seuil

// Variables globales
RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);
bool porteOuverte = false;
unsigned long dernierTempsLED = 0;
bool etatLED = false;
bool coupureCourant = false;
unsigned long dernierTimeCheck = 0;
int seuilLumiere = SEUIL_DEFAULT;

// Variables pour la temporisation de fermeture
unsigned long debutLumiereFaible = 0;
bool lumiereFaibleDetectee = false;

// Variables pour la gestion du bouton multi-fonction
unsigned long debutAppui = 0;
bool boutonEnfonce = false;
bool appuiTraite = false;
unsigned long dernierRelachement = 0;
bool enAttenteDoubleClick = false;

// Variables pour les modes de réglage
enum ModeReglage {
  MODE_NORMAL,
  MODE_REGLAGE_HEURE,
  MODE_REGLAGE_MINUTE,
  MODE_REGLAGE_SEUIL
};
ModeReglage modeActuel = MODE_NORMAL;
unsigned long debutModeReglage = 0;
bool clignotement = false;
unsigned long dernierClignotement = 0;

// États de la porte
enum EtatPorte {
  ARRET,
  OUVERTURE,
  FERMETURE
};
EtatPorte etatActuel = ARRET;

void setup() {
  Serial.begin(9600);
  
  // Configuration des broches
  pinMode(MOTEUR_PIN1, OUTPUT);
  pinMode(MOTEUR_PIN2, OUTPUT);
  pinMode(BOUTON_PIN, INPUT_PULLUP);
  pinMode(FIN_COURSE_HAUT, INPUT_PULLUP);
  pinMode(FIN_COURSE_BAS, INPUT_PULLUP);
  pinMode(LED_COUPURE, OUTPUT);
  
  // Arrêt du moteur au démarrage
  arreterMoteur();
  
  // Initialisation du LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Poulailler Auto");
  lcd.setCursor(0, 1);
  lcd.print("Initialisation..");
  delay(2000);
  
  // Initialisation du RTC
  if (!rtc.begin()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Erreur RTC!");
    Serial.println("RTC non trouvé !");
    while (1);
  }
  
  // Chargement du seuil depuis l'EEPROM
  int seuilSauve = EEPROM.read(SEUIL_EEPROM_ADDR) | (EEPROM.read(SEUIL_EEPROM_ADDR + 1) << 8);
  if (seuilSauve >= 0 && seuilSauve <= 1023) {
    seuilLumiere = seuilSauve;
  }
  
  // Vérification si le RTC a perdu l'alimentation
  if (rtc.lostPower()) {
    coupureCourant = true;
    Serial.println("RTC a perdu l'alimentation");
    // Décommentez la ligne suivante pour régler l'heure automatiquement
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  // Déterminer l'état initial de la porte
  if (digitalRead(FIN_COURSE_HAUT) == LOW) {
    porteOuverte = true;
    Serial.println("Porte détectée ouverte au démarrage");
  } else if (digitalRead(FIN_COURSE_BAS) == LOW) {
    porteOuverte = false;
    Serial.println("Porte détectée fermée au démarrage");
  }
  
  Serial.println("Système initialisé");
  lcd.clear();
}

void loop() {
  DateTime maintenant = rtc.now();
  
  // Gestion du bouton multifonction
  gererBoutonMultifonction();
  
  // Gestion du timeout pour sortir du mode réglage
  if (modeActuel != MODE_NORMAL && millis() - debutModeReglage > TIMEOUT_REGLAGE) {
    modeActuel = MODE_NORMAL;
  }
  
  // Gestion du clignotement en mode réglage
  if (millis() - dernierClignotement > 500) {
    dernierClignotement = millis();
    clignotement = !clignotement;
  }
  
  // Gestion de l'affichage LCD
  gererAffichageLCD(maintenant);
  
  // Gestion de la LED de coupure
  gererLEDCoupure();
  
  // Logique automatique seulement en mode normal
  if (modeActuel == MODE_NORMAL) {
    // Vérifications automatiques toutes les secondes
    if (millis() - dernierTimeCheck > 1000) {
      dernierTimeCheck = millis();
      
      int valeurLumiere = analogRead(CAPTEUR_LUMIERE);
      
      // Condition 1: Ouverture automatique à 7h00
      if (maintenant.hour() == 7 && maintenant.minute() == 0 && !porteOuverte) {
        Serial.println("7h00 - Ouverture automatique");
        ouvrirPorte();
      }
      
      // Condition 2: Fermeture avec temporisation
      gererFermetureLumiere(valeurLumiere, maintenant.hour());
      
      // Condition 3: Fermeture forcée à 23h00
      if (maintenant.hour() == 23 && maintenant.minute() == 0 && porteOuverte) {
        Serial.println("23h00 - Fermeture forcée");
        fermerPorte();
      }
    }
    
    // Gestion du mouvement de la porte
    gererMouvementPorte();
  }
  
  delay(50);
}

void gererBoutonMultifonction() {
  bool etatBouton = digitalRead(BOUTON_PIN);
  
  // Détection du début d'appui
  if (!boutonEnfonce && etatBouton == LOW) {
    delay(20); // Anti-rebond
    if (digitalRead(BOUTON_PIN) == LOW) {
      boutonEnfonce = true;
      debutAppui = millis();
      appuiTraite = false;
    }
  }
  
  // Détection du relâchement
  if (boutonEnfonce && etatBouton == HIGH) {
    delay(20); // Anti-rebond
    if (digitalRead(BOUTON_PIN) == HIGH) {
      boutonEnfonce = false;
      unsigned long dureeAppui = millis() - debutAppui;
      
      if (!appuiTraite) {
        if (dureeAppui >= APPUI_LONG) {
          // Appui long
          gererAppuiLong();
        } else {
          // Appui bref
          if (enAttenteDoubleClick && millis() - dernierRelachement < DOUBLE_CLIC) {
            // Double-clic détecté
            gererDoubleClick();
            enAttenteDoubleClick = false;
          } else {
            // Premier appui bref, attendre pour voir s'il y a un double-clic
            dernierRelachement = millis();
            enAttenteDoubleClick = true;
          }
        }
      }
    }
  }
  
  // Gestion du timeout pour le double-clic
  if (enAttenteDoubleClick && millis() - dernierRelachement > DOUBLE_CLIC) {
    gererAppuiBref();
    enAttenteDoubleClick = false;
  }
}

void gererAppuiBref() {
  switch (modeActuel) {
    case MODE_NORMAL:
      // Ouvrir/fermer la porte ou réessayer après erreur
      if (etatActuel == ERREUR_OBSTACLE) {
        // Réessayer le mouvement
        etatActuel = ARRET;
        if (porteOuverte) {
          fermerPorte();
        } else {
          ouvrirPorte();
        }
      } else if (porteOuverte) {
        Serial.println("Fermeture manuelle demandée");
        fermerPorte();
      } else {
        Serial.println("Ouverture manuelle demandée");
        ouvrirPorte();
      }
      break;
      
    case MODE_REGLAGE_HEURE:
      // Incrémenter l'heure
      {
        DateTime maintenant = rtc.now();
        int nouvelleHeure = (maintenant.hour() + 1) % 24;
        rtc.adjust(DateTime(maintenant.year(), maintenant.month(), maintenant.day(), 
                           nouvelleHeure, maintenant.minute(), maintenant.second()));
        debutModeReglage = millis(); // Reset timeout
      }
      break;
      
    case MODE_REGLAGE_MINUTE:
      // Incrémenter les minutes
      {
        DateTime maintenant = rtc.now();
        int nouvelleMinute = (maintenant.minute() + 1) % 60;
        rtc.adjust(DateTime(maintenant.year(), maintenant.month(), maintenant.day(), 
                           maintenant.hour(), nouvelleMinute, 0));
        debutModeReglage = millis(); // Reset timeout
      }
      break;
      
    case MODE_REGLAGE_SEUIL:
      // Augmenter le seuil par pas de 5
      seuilLumiere = min(1023, seuilLumiere + 5);
      sauvegarderSeuil();
      debutModeReglage = millis(); // Reset timeout
      break;
  }
}

void gererDoubleClick() {
  if (modeActuel == MODE_REGLAGE_SEUIL) {
    // Diminuer le seuil par pas de 5
    seuilLumiere = max(0, seuilLumiere - 5);
    sauvegarderSeuil();
    debutModeReglage = millis(); // Reset timeout
  }
}

void gererAppuiLong() {
  switch (modeActuel) {
    case MODE_NORMAL:
      modeActuel = MODE_REGLAGE_HEURE;
      debutModeReglage = millis();
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
      modeActuel = MODE_NORMAL;
      break;
  }
  appuiTraite = true;
}

void gererAffichageLCD(DateTime maintenant) {
  lcd.setCursor(0, 0);
  
  switch (modeActuel) {
    case MODE_NORMAL:
      // Ligne 1: Heure
      if (maintenant.hour() < 10) lcd.print("0");
      lcd.print(maintenant.hour());
      lcd.print(":");
      if (maintenant.minute() < 10) lcd.print("0");
      lcd.print(maintenant.minute());
      lcd.print("      "); // Effacer le reste
      
      // Ligne 2: Statut de la porte
      lcd.setCursor(0, 1);
      if (etatActuel == OUVERTURE) {
        lcd.print("Ouverture...    ");
      } else if (etatActuel == FERMETURE) {
        lcd.print("Fermeture...    ");
      } else if (lumiereFaibleDetectee) {
        unsigned long tempsRestant = (TEMPO_FERMETURE - (millis() - debutLumiereFaible)) / 60000;
        lcd.print("Ferme dans ");
        lcd.print(tempsRestant + 1);
        lcd.print("mn ");
      } else {
        lcd.print(porteOuverte ? "Porte ouverte   " : "Porte fermee    ");
      }
      break;
      
    case MODE_REGLAGE_HEURE:
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
      
      lcd.setCursor(0, 1);
      lcd.print("Reglage heure   ");
      break;
      
    case MODE_REGLAGE_MINUTE:
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
  }
}

void sauvegarderSeuil() {
  EEPROM.write(SEUIL_EEPROM_ADDR, seuilLumiere & 0xFF);
  EEPROM.write(SEUIL_EEPROM_ADDR + 1, (seuilLumiere >> 8) & 0xFF);
}

void ouvrirPorte() {
  if (!porteOuverte && etatActuel == ARRET) {
    Serial.println("Début ouverture porte");
    etatActuel = OUVERTURE;
    digitalWrite(MOTEUR_PIN1, HIGH);
    digitalWrite(MOTEUR_PIN2, LOW);
  }
}

void fermerPorte() {
  if (porteOuverte && etatActuel == ARRET) {
    Serial.println("Début fermeture porte");
    etatActuel = FERMETURE;
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
      if (digitalRead(FIN_COURSE_HAUT) == LOW) {
        Serial.println("Porte complètement ouverte");
        arreterMoteur();
        porteOuverte = true;
      }
      break;
      
    case FERMETURE:
      if (digitalRead(FIN_COURSE_BAS) == LOW) {
        Serial.println("Porte complètement fermée");
        arreterMoteur();
        porteOuverte = false;
      }
      break;
      
    case ARRET:
      break;
  }
}

void gererFermetureLumiere(int valeurLumiere, int heureActuelle) {
  if (!porteOuverte || heureActuelle <= 7) {
    lumiereFaibleDetectee = false;
    debutLumiereFaible = 0;
    return;
  }
  
  if (valeurLumiere < seuilLumiere) {
    if (!lumiereFaibleDetectee) {
      lumiereFaibleDetectee = true;
      debutLumiereFaible = millis();
      Serial.println("Début détection lumière faible - temporisation 10 minutes");
    } else if (millis() - debutLumiereFaible >= TEMPO_FERMETURE) {
      Serial.println("Temporisation écoulée - Fermeture automatique");
      fermerPorte();
      lumiereFaibleDetectee = false;
    }
  } else {
    if (lumiereFaibleDetectee) {
      Serial.println("Lumière revenue - Annulation temporisation fermeture");
      lumiereFaibleDetectee = false;
      debutLumiereFaible = 0;
    }
  }
}

void gererLEDCoupure() {
  if (coupureCourant) {
    if (millis() - dernierTempsLED > 500) {
      dernierTempsLED = millis();
      etatLED = !etatLED;
      digitalWrite(LED_COUPURE, etatLED);
    }
  } else {
    digitalWrite(LED_COUPURE, LOW);
  }
}
