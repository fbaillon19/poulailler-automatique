/*
 * ============================================================================
 * ROTARY_ENCODER.H - Gestion encodeur rotatif KY-040 (Version procédurale)
 * ============================================================================
 * 
 * Utilise la bibliothèque Encoder de Paul Stoffregen (ultra-fiable)
 * Version procédurale (pas de POO)
 * 
 * BIBLIOTHÈQUE REQUISE:
 * - Encoder par Paul Stoffregen
 *   Installation: IDE Arduino → Croquis → Inclure bibliothèque → 
 *                Gérer les bibliothèques → Chercher "Encoder"
 * 
 * MATÉRIEL:
 * - Condensateurs 100nF recommandés entre CLK-GND et DT-GND pour filtrage
 * 
 * Auteur / Author: Frédéric BAILLON
 * Version: 1.3.0
 * Date: 2025
 * Licence / License: MIT
 * 
 * ============================================================================
 */

#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <Arduino.h>
#include <Encoder.h>  // Bibliothèque Paul Stoffregen

// ============================================================================
// DÉCLARATIONS EXTERNES / EXTERNAL DECLARATIONS
// ============================================================================

// Modes de réglage / Settings modes
enum ModeReglage : uint8_t {
  MODE_NORMAL = 0,
  MODE_REGLAGE_HEURE_OUVERTURE = 1,
  MODE_REGLAGE_MINUTE_OUVERTURE = 2,
  MODE_REGLAGE_HEURE_MIN_FERMETURE = 3,
  MODE_REGLAGE_MINUTE_MIN_FERMETURE = 4,
  MODE_REGLAGE_HEURE_MAX_FERMETURE = 5,
  MODE_REGLAGE_MINUTE_MAX_FERMETURE = 6,
  MODE_REGLAGE_SEUIL = 7,
  MODE_REGLAGE_TEMPO_FERMETURE = 8,
  MODE_REGLAGE_TIMEOUT_OUVERTURE = 9,
  MODE_REGLAGE_TIMEOUT_FERMETURE = 10
};

// États de la porte / Door states
enum EtatPorte : uint8_t {
  ARRET = 0,
  OUVERTURE = 1,
  FERMETURE = 2,
  ERREUR_OBSTACLE = 3
};

// Variables globales externes / External global variables
extern bool lcdAllume;
extern unsigned long derniereActivite;
extern unsigned long dernierAllumageMinute;
extern unsigned long dernierRafraichissementLCD;

extern ModeReglage modeActuel;
extern unsigned long debutModeReglage;

extern EtatPorte etatActuel;
extern bool porteOuverte;

extern int seuilLumiere;
extern int timeoutOuverture;
extern int timeoutFermeture;

// Nouvelles variables v1.4.0
extern int heureOuverture;
extern int minuteOuverture;
extern int heureMinFermeture;
extern int minuteMinFermeture;
extern int heureMaxFermeture;
extern int minuteMaxFermeture;
extern int tempoFermetureMinutes;

// Fonctions externes / External functions
extern void allumerLCD();
extern void eteindreLCD();
extern void ouvrirPorte();
extern void fermerPorte();
extern void sauvegarderParametres();

// ============================================================================
// FONCTIONS PUBLIQUES / PUBLIC FUNCTIONS
// ============================================================================

/**
 * Initialise l'encodeur rotatif
 * Initializes rotary encoder
 */
void initRotaryEncoder();

/**
 * Met à jour l'état de l'encodeur (à appeler dans loop)
 * Updates encoder state (call in loop)
 */
void updateRotaryEncoder();

#endif // ROTARY_ENCODER_H
