# Greedy MDVSP Solver - Schritt-für-Schritt Erklärung

## Überblick

Der Greedy MDVSP (Multi-Depot Vehicle Scheduling Problem) Solver löst das Problem der optimalen Zuordnung von Servicefahrten zu Fahrzeugblöcken bei mehreren Depots. Das Ziel ist die Minimierung der Gesamtkosten bei gleichzeitiger Erfüllung aller zeitlichen und operativen Beschränkungen.

## Was ist ein Fahrzeugblock?

Ein **Fahrzeugblock** repräsentiert eine komplette Arbeitsschicht eines einzelnen Fahrzeugs:

```
Depot → Servicefahrt 1 → Servicefahrt 2 → ... → Servicefahrt n → Depot
```

### Komponenten eines Fahrzeugblocks:
- **Startdepot**: Wo das Fahrzeug seine Schicht beginnt
- **Servicefahrten**: Aufeinanderfolgende Fahrten mit Fahrgästen
- **Leerfahrten**: Verbindungen zwischen Servicefahrten ohne Fahrgäste
- **Enddepot**: Wo das Fahrzeug seine Schicht beendet (meist gleiches Depot)

### Kostenbestandteile:
- **Fahrzeugfixkosten**: Grundkosten für das Fahrzeug pro Tag
- **Anfahrtskosten**: Depot → erste Servicefahrt
- **Servicefahrtkosten**: Betriebskosten aller Kundenfahrten
- **Leerfahrtkosten**: Verbindungen zwischen Servicefahrten
- **Rückfahrtkosten**: Letzte Servicefahrt → Depot

## Schritt-für-Schritt Ablauf des Greedy-Algorithmus

### 1. Vorbereitung und Initialisierung

- **Zeitmessung starten** für Performance-Tracking
- **Datenstrukturen leeren**: 
  - Alle Listen für Fahrzeugblöcke zurücksetzen
  - Zugewiesene Fahrten löschen
  - Depot-Kapazitätszähler auf null setzen
- **Logging aktivieren** für Debugging und Monitoring

### 2. Sortierung der Servicefahrten

- **Alle Servicefahrten nach Abfahrtszeit sortieren**
- **Grund**: Frühere Fahrten haben mehr Flexibilität für spätere Anschlussverbindungen
- **Greedy-Prinzip**: Behandle zeitkritische Entscheidungen zuerst

### 3. Hauptschleife - Fahrt für Fahrt bearbeiten

Für jede Servicefahrt (in chronologischer Reihenfolge):

#### 3a. Versuch: Zuordnung zu existierendem Fahrzeugblock

**Für jeden bestehenden Fahrzeugblock prüfen:**

1. **Kompatibilität checken**:
   - Ist der Fahrzeugtyp für diese Fahrt geeignet?
   - Ist zeitlich eine Verbindung möglich?
   - Prüfung: Ankunft + Mindestpause + Leerfahrtzeit ≤ neue Abfahrt

2. **Kosten berechnen** (falls kompatibel):
   - Leerfahrtkosten zwischen letzter und neuer Fahrt
   - Servicefahrtkosten der neuen Fahrt

3. **Besten Block auswählen**: Der Block mit den niedrigsten Zusatzkosten gewinnt

4. **Fahrt anhängen**: Falls passender Block gefunden, Fahrt hinzufügen

#### 3b. Fallback: Neuen Fahrzeugblock erstellen

**Falls keine Zuordnung möglich war:**

1. **Fahrzeugtyp finden**:
   - Alle kompatiblen Fahrzeugtypen durchgehen
   - Günstigsten auswählen (Kombination aus Fix-, Kilometer- und Stundenkosten)
   - Falls kein kompatibler Typ: Fehler

2. **Depot auswählen**:
   - Alle verfügbaren Depots prüfen
   - Depot mit niedrigsten Anfahrtskosten wählen
   - Kapazitätsbeschränkungen beachten
   - Falls kein Depot verfügbar: Fehler

3. **Depot-Kapazität prüfen**:
   - Ist noch Platz für diesen Fahrzeugtyp?
   - Falls nicht: Fehler

4. **Neuen Block erstellen**:
   - Fahrzeugtyp und Depot zuweisen
   - Erste Servicefahrt hinzufügen

5. **Gesamtkosten berechnen**:
   - Fahrzeugfixkosten
   - Anfahrt: Depot → erste Servicefahrt
   - Servicefahrtkosten
   - Rückfahrt: letzte Servicefahrt → Depot

6. **Block zur Lösung hinzufügen**:
   - Block in die Liste der aktuellen Blöcke einfügen
   - Depot-Kapazitätszähler aktualisieren

#### 3c. Erfolgskontrolle

- **Bei erfolgreicher Zuordnung**: Fahrt als "zugewiesen" markieren
- **Bei Fehlschlag**: Warnung loggen und Lösung als "nicht machbar" markieren

### 4. Lösungsauswertung

- **Gesamtkosten berechnen**: Summe aller Fahrzeugblockkosten
- **Fahrzeuganzahl zählen**: Anzahl der erstellten Blöcke
- **Machbarkeit prüfen**: Wurden alle Fahrten zugewiesen?
- **Laufzeit messen**: Ende der Zeitmessung

### 5. Ergebnis-Logging

- Gesamtkosten ausgeben
- Anzahl verwendeter Fahrzeuge
- Machbarkeit (JA/NEIN)
- Berechnungszeit in Sekunden

## Greedy-Charakteristika

### Das "Gierige" am Algorithmus:

- **Keine Vorausschau**: Jede Entscheidung wird nur basierend auf aktuellen Informationen getroffen
- **Lokal optimal**: Wählt immer die momentan beste Option
- **Keine Rücknahme**: Einmal getroffene Entscheidungen werden nicht revidiert

### Entscheidungskriterien (in dieser Reihenfolge):

1. **Zeit vor Kosten**: Frühere Fahrten werden zuerst behandelt
2. **Bestehende vor neuen Blöcken**: Versucht zuerst, in existierende Blöcke einzufügen
3. **Niedrigste Kosten**: Bei mehreren Optionen wird die günstigste gewählt

### Kostenberechnung im Detail:

**Für existierende Blöcke:**
- Kosten = Leerfahrtkosten + Servicefahrtkosten
- Leerfahrt: Kilometer-Kosten × Distanz/100 + Stunden-Kosten × Zeit/3600
- Service: Kilometer-Kosten × Fahrtdistanz/100 + Stunden-Kosten × Fahrtzeit/3600

**Für neue Blöcke:**
- Gesamtkosten = Fahrzeugfixkosten + Depot→Start + Servicefahrt + Ende→Depot

## Vor- und Nachteile

### Vorteile:
- **Schnell**: Einfache und effiziente Implementierung
- **Garantierte Lösung**: Findet immer eine Lösung (falls möglich)
- **Natürliche Zeitordnung**: Berücksichtigt zeitliche Reihenfolge automatisch
- **Gute Startlösung**: Kann als Basis für weitere Optimierungen dienen

### Nachteile:
- **Nur lokal optimal**: Berücksichtigt keine globalen Auswirkungen
- **Keine Rücksicht auf Zukunft**: Frühe Entscheidungen können spätere Optionen verschlechtern
- **Suboptimale Gesamtlösung**: Kann weit vom globalen Optimum entfernt sein
- **Keine Flexibilität**: Einmal getroffene Entscheidungen sind endgültig

## Rückkehr zum Depot

**Ja, die Fahrzeuge müssen wieder zum Startdepot zurückkehren.**

Dies ist notwendig für:
- **Realistische Modellierung**: Fahrzeuge müssen am Ende des Tages zum Depot zurück
- **Kostenwahrheit**: Die Rückfahrtskosten sind Teil der Gesamtbetriebskosten
- **Depot-Management**: Fahrzeuge müssen für Wartung, Reinigung und den nächsten Tag verfügbar sein
- **Personalplanung**: Fahrer müssen ihre Schicht am Depot beenden

Die Rückfahrtskosten werden sowohl bei der Erstellung neuer Blöcke als auch bei der Kostenvergleichung berücksichtigt, um eine realistische und vollständige Kostenkalkulation zu gewährleisten.

## Fazit

Der Greedy-Algorithmus ist eine klassische Heuristik, die in jedem Schritt die lokal beste Entscheidung trifft. Er ist schnell und einfach zu verstehen, kann aber aufgrund seiner kurzsichtigen Natur zu suboptimalen Gesamtlösungen führen. Trotzdem ist er ein wertvoller Ansatz für das MDVSP, besonders als Startlösung für komplexere Optimierungsverfahren.
