/*
 * ============================================================================
 * BUTTON.H - Gestion du bouton multifonction
 * ============================================================================
 * 
 * Gestion du bouton avec la bibliothèque OneButton
 * Provides button management using OneButton library
 * 
 * Auteur / Author: Frédéric BAILLON
 * Version: 1.1.0
 * Date: 2025
 * Licence / License: MIT
 * 
 * ============================================================================
 */

#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include <OneButton.h>

// Déclarations externes des variables globales / External global variables declarations
extern bool lcdAllume;
extern unsigned long derniereActivite;
extern unsigned long dernierAllumageMinute;

// Modes de réglage / Settings modes
enum ModeReglage : uint8_t {
  MODE_NORMAL = 0,
  MODE_REGLAGE_HEURE = 1,
  MODE_REGLAGE_MINUTE = 2,
  MODE_REGLAGE_SEUIL = 3,
  MODE_REGLAGE_TIMEOUT_OUVERTURE = 4,
  MODE_REGLAGE_TIMEOUT_FERMETURE = 5
};

extern ModeReglage modeActuel;
extern unsigned long debutModeReglage;

// États de la porte / Door states
enum EtatPorte : uint8_t {
  ARRET = 0,
  OUVERTURE = 1,
  FERMETURE = 2,
  ERREUR_OBSTACLE = 3
};

extern EtatPorte etatActuel;
extern bool porteOuverte;

// Déclarations des fonctions externes / External function declarations
extern void allumerLCD();
extern void eteindreLCD();
extern void ouvrirPorte();
extern void fermerPorte();
extern void sauvegarderSeuil();
extern void sauvegarderTimeoutOuverture();
extern void sauvegarderTimeoutFermeture();

// Variables globales pour réglages / Global variables for settings
extern int seuilLumiere;
extern int timeoutOuverture;
extern int timeoutFermeture;

// Objet OneButton / OneButton object
extern OneButton bouton;

// Fonctions publiques / Public functions
void initButton();
void updateButton();

// Callbacks du bouton / Button callbacks
void onButtonClick();
void onButtonDoubleClick();
void onButtonLongPressStart();

#endif // BUTTON_H
