#!/usr/bin/env python3
"""
Einfaches Script zum Aufteilen der MDVSP-Datei in CSV-Dateien
"""

import os
import csv
from pathlib import Path

def parse_mdvsp_file():
    """Teilt die MDVSP-Datei in separate CSV-Dateien auf"""
    
    # Eingabe- und Ausgabepfade
    input_file = "/workspace/data/short.txt"
    output_dir = Path("/workspace/data/short")
    
    # Erstelle Ausgabeverzeichnis
    output_dir.mkdir(exist_ok=True)
    print(f"Ausgabeverzeichnis erstellt: {output_dir}")
    
    # Lese die Datei
    print(f"Lese Datei: {input_file}")
    
    with open(input_file, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    
    current_section = None
    current_header = None
    current_data = []
    sections_created = 0
    
    for line_num, line in enumerate(lines, 1):
        line = line.strip()
        
        # Überspringe leere Zeilen und Kommentare
        if not line or line.startswith('*'):
            continue
        
        # Neue Sektion gefunden
        if line.startswith('$'):
            # Speichere vorherige Sektion als CSV
            if current_section and current_header and current_data:
                save_section_as_csv(output_dir, current_section, current_header, current_data)
                sections_created += 1
                current_data = []
            
            # Parse neue Sektion
            section_parts = line[1:].split(':')
            current_section = section_parts[0]
            current_header = section_parts[1].split(';') if len(section_parts) > 1 else []
            
            print(f"Verarbeite Sektion: {current_section} (Zeile {line_num})")
            
        else:
            # Datenzeile
            if current_section and ';' in line:
                data_row = line.split(';')
                current_data.append(data_row)
    
    # Speichere letzte Sektion
    if current_section and current_header and current_data:
        save_section_as_csv(output_dir, current_section, current_header, current_data)
        sections_created += 1
    
    print(f"\n✅ Erfolgreich abgeschlossen!")
    print(f"📁 {sections_created} CSV-Dateien erstellt in: {output_dir}")


def save_section_as_csv(output_dir, section_name, header, data):
    """Speichert eine Sektion als CSV-Datei"""
    
    # Bereinige Sektionsname für Dateiname
    safe_filename = section_name.lower().replace(' ', '_')
    csv_file = output_dir / f"{safe_filename}.csv"
    
    with open(csv_file, 'w', newline='', encoding='utf-8') as f:
        writer = csv.writer(f, delimiter=';')
        
        # Schreibe Header
        writer.writerow(header)
        
        # Schreibe Daten
        for row in data:
            writer.writerow(row)
    
    print(f"  → {csv_file.name}: {len(data)} Zeilen")


if __name__ == "__main__":
    parse_mdvsp_file()
