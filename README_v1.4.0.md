# 📋 Version 1.4.0 - Guide complet

## 🎯 Version Production - Testée et validée sur le terrain

Cette version intègre **tous les retours d'expérience terrain** après plusieurs jours d'utilisation réelle.

---

## ✨ Nouveautés principales

### 1️⃣ **Heure minimum de fermeture** ⭐ **CRITIQUE**

**Problème résolu :**
> En hiver, la porte s'ouvre à 7h mais il fait encore nuit. Sans cette fonctionnalité, la porte se refermerait immédiatement après ouverture !

**Solution :**
- Paramétrage heure minimum fermeture (15h-17h par défaut 16h00)
- La porte ne fermera JAMAIS avant cette heure, même si luminosité faible
- Évite le problème : ouverture 7h → nuit → fermeture immédiate ❌

**Configuration :**
```
Mode 3: Heure min fermeture → 16h
Mode 4: Minute min fermeture → 00
```

### 2️⃣ **Temporisation paramétrable** ⭐ **IMPORTANT**

**Avant :** Temporisation fixe 10 minutes  
**Maintenant :** Paramétrable 10-30 minutes

**Utilité :**
- Ajuster selon météo locale (nuages fréquents = tempo plus longue)
- Par défaut : 10 minutes (compromis)

**Configuration :**
```
Mode 8: Tempo fermeture → 10-30 min
```

### 3️⃣ **Affichage luminosité temps réel** ⭐ **PRATIQUE**

**Avant :** Impossible de voir la valeur sans entrer en mode réglage  
**Maintenant :** Affichage automatique qui alterne toutes les 3s

**Affichage LCD ligne 1 :**
```
07:15        → Heure
(3 secondes)
Lum:45       → Luminosité actuelle
(3 secondes)
07:15        → Heure
...
```

**Utilité :** Vérifier rapidement si seuil configuré est bon

### 4️⃣ **Calibration automatique au démarrage** ⭐ **SÉCURITÉ**

**Problème détecté :**
> Un matin, capteur fin de course HAUT coincé par le froid → système pensait porte ouverte → pas d'ouverture automatique !

**Solution :**
Au démarrage, le système :
1. Ferme complètement la porte (vérif capteur BAS)
2. Ouvre complètement la porte (vérif capteur HAUT)
3. Détecte erreurs capteurs dès le démarrage

**Messages Serial :**
```
=== CALIBRATION PORTE AU DÉMARRAGE ===
État initial: HAUT=OFF BAS=ON
Descente vers position BAS...
Position BAS atteinte
Montée vers position HAUT...
Position HAUT atteinte
=== CALIBRATION RÉUSSIE ===
```

### 5️⃣ **Gestion d'erreurs complète** ⭐ **DIAGNOSTIC**

**Erreurs critiques (arrêt automatique) :**
- Capteurs incohérents (les 2 activés)
- Capteur fin de course HS
- RTC aberrant ou figé

**Alertes (fonctionnement dégradé) :**
- Capteur luminosité HS
- Tension alimentation faible

**LED différenciée :**
- Clignotement rapide 200ms = Critique
- Clignotement lent 1s = Alerte
- Clignotement moyen 500ms = Pile RTC

**Acquittement :**
- Appui TRÈS long 5s pour reset erreur

### 6️⃣ **Interface plus rapide** ⭐ **CONFORT**

**Avant v1.4.0 :**
- Appui long 3s pour entrer en réglage → LONG
- Appui long pour passer entre modes → PÉNIBLE

**Maintenant v1.4.0 :**
- Appui long 1.5s pour entrer en réglage → RAPIDE ✅
- Appui bref pour passer entre modes → FACILE ✅

**Navigation :**
```
MODE_NORMAL
  ↓ [Appui long 1.5s]
MODE_HEURE_OUVERTURE
  ↓ [Appui bref]
MODE_MINUTE_OUVERTURE
  ↓ [Appui bref]
MODE_HEURE_MIN_FERMETURE
  ↓ [Appui bref]
...
  ↓ [Appui bref sur dernier mode]
MODE_NORMAL (sauvegarde + extinction LCD)
```

---

## 🔧 Configuration recommandée (terrain validé)

### Paramètres par défaut v1.4.0

| Paramètre | Valeur défaut | Plage | Notes terrain |
|-----------|---------------|-------|---------------|
| **Heure ouverture** | 7h | 6h-9h | OK hiver/été |
| **Minute ouverture** | 0 | 0-59 | - |
| **Heure min fermeture** | 16h | 15h-17h | **Critique hiver !** |
| **Minute min fermeture** | 0 | 0-59 | - |
| **Heure max fermeture** | 23h | 0-23 | Sécurité |
| **Minute max fermeture** | 0 | 0-59 | - |
| **Seuil luminosité** | 100 | 0-1023 | **Utilisateur à 30** |
| **Temporisation** | 10 min | 10-30 min | **Selon météo** |
| **Timeout ouverture** | 15s | 5-60s | OK |
| **Timeout fermeture** | 30s | 5-60s | OK |

### Ajustements recommandés selon environnement

**Environnement ombragé (forêt, nord) :**
```
Seuil luminosité: 50-100
Temporisation: 15-20 min
```

**Environnement dégagé (champ, sud) :**
```
Seuil luminosité: 20-50
Temporisation: 10-15 min
```

**Météo changeante (montagne) :**
```
Temporisation: 20-30 min
```

---

## 🐛 Bugs corrigés v1.4.0

### Bug 1 : Erreur RTC figé (faux positif)

**Symptôme :** Message "ERREUR RTC figé" alors que l'heure fonctionne

**Cause :** Vérification dès le démarrage avec valeur initiale incorrecte

**Solution :** 
- Ajout flag `rtc_verification_initialisee`
- Première vérification après 2 min : juste enregistrer
- Vérifications suivantes : comparer et détecter
- **Plus de faux positif !** ✅

### Bug 2 : Clignotement désagréable heures

**Symptôme :** Heure "saute" lors du clignotement en mode réglage

**Cause :** Nombre de caractères différent entre clignotement ON/OFF
```
ON:  "Ouv:   :" = 8 caractères
OFF: "Ouv:07:" = 7 caractères → DÉCALAGE
```

**Solution :** 
```cpp
if (clignotement) {
  lcd.print(F("  :"));  // 2 espaces = toujours 7 car
} else {
  if (heure < 10) lcd.print("0");
  lcd.print(heure);
  lcd.print(":");        // Toujours 7 caractères
}
```
**Clignotement fluide !** ✅

---

## 📊 Validation terrain

### Tests effectués (plusieurs jours)

✅ **Ouverture automatique 7h** : OK  
✅ **Heure min fermeture 16h** : OK (pas de fermeture avant même si sombre)  
✅ **Fermeture par luminosité** : OK  
✅ **Temporisation 10 min** : OK  
✅ **Calibration démarrage** : OK (détecté capteur coincé)  
✅ **Seuil 100 → ajusté à 30** : OK pour environnement utilisateur  
✅ **Navigation encodeur** : Fluide et intuitive  
✅ **Affichage luminosité** : Pratique pour contrôle visuel  
✅ **Gestion erreurs** : LED différenciée fonctionne  

### Observations terrain

**Point positif :**
- Navigation beaucoup plus rapide (appui bref entre modes)
- Heure min fermeture évite problème hiver
- Affichage luminosité très pratique
- Calibration démarrage rassurante

**Ajustements faits :**
- Seuil abaissé de 100 à 30 (environnement spécifique)
- Temporisation laissée à 10 min (suffisant)

---

## 🚀 Migration depuis v1.3.0

### ⚠️ Attention : EEPROM réorganisée

**Les adresses EEPROM ont changé !**

**Action requise après téléversement v1.4.0 :**
1. ✅ Tous les paramètres seront réinitialisés aux valeurs par défaut
2. ✅ Reconfigurer TOUS les paramètres via encodeur
3. ✅ Vérifier seuil luminosité (100 par défaut, ajuster selon terrain)
4. ✅ Configurer heure min fermeture (IMPORTANT pour hiver)

### Procédure de migration

```
1. Téléverser v1.4.0
2. Au démarrage : calibration automatique (attendre fin)
3. Entrer en mode réglage (appui long 1.5s)
4. Configurer dans l'ordre :
   - Heure ouverture (ex: 7h)
   - Minute ouverture (ex: 0)
   - Heure min fermeture (ex: 16h) ← IMPORTANT
   - Minute min fermeture (ex: 0)
   - Heure max fermeture (ex: 23h)
   - Minute max fermeture (ex: 0)
   - Seuil luminosité (tester avec affichage temps réel)
   - Temporisation (10-30 min selon météo)
   - Timeouts (laisser par défaut)
5. Dernier appui bref → sauvegarde + sortie
6. Vérifier fonctionnement 24-48h
```

---

## 📞 Support et questions

**En cas de problème :**
1. Vérifier messages Serial (9600 bauds)
2. Noter LED clignotement (200ms/1s/500ms)
3. Vérifier configuration heure min fermeture
4. Créer une issue GitHub avec détails

**Questions fréquentes :**

**Q : La porte se referme juste après ouverture le matin ?**  
R : Configurer heure min fermeture à 16h (ou plus tard)

**Q : Quelle valeur seuil luminosité ?**  
R : Regarder affichage temps réel et ajuster. Démarrer à 100, descendre si besoin.

**Q : Temporisation 10 min suffisante ?**  
R : Oui pour météo stable. Augmenter à 20-30 min si nuages fréquents.

**Q : Erreur capteurs au démarrage ?**  
R : Vérifier câblage. Appui 5s pour acquitter et vérifier manuellement.

---

**Version 1.4.0 - Production validée ✅**  
**Date : Janvier 2026**  
**Auteur : Frédéric BAILLON**
