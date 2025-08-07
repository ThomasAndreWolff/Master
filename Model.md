# Sprechende Namen für MDVSP-Modell nach Kliewer et al.

## Entscheidungsvariablen

### Schema: `x_D{DepotGroup}V{VehicleType}{ArcType}_{Details}`

### Arc-Typen:
- **S** = Service (Servicefahrten)
- **TW** = TimeWindow (Zeitfenster-Fahrten)
- **W** = Waiting (Wartekanten)
- **DH** = Deadhead (Leerfahrten)
- **L** = Line (Linienkanten)
- **R** = Return (Rückkanten)
- **D** = Depot (Depot Ein-/Ausfahrten)

### Beispiele:
- `x_D0V1S_T123` = Depotgruppe 0, Fahrzeugtyp 1, Servicefahrt Trip 123
- `x_D0V2DH_45_67` = Depotgruppe 0, Fahrzeugtyp 2, Leerfahrt von Haltestelle 45 zu 67
- `x_D1V1W_10_10` = Depotgruppe 1, Fahrzeugtyp 1, Wartekante an Haltestelle 10

## Constraints

### Flusserhaltung:
- `FlowCons_D{DepotGroup}V{VehicleType}_SP{StopPoint}_N{NodeNumber}`
- **Beispiel**: `FlowCons_D0V1_SP5_N123`

### Service Coverage:
- `ServiceCoverage_T{TripID}`
- **Beispiel**: `ServiceCoverage_T456`

### Kapazitäten:
- `Capacity_{ConstraintID}` (Depot-Kapazitäten, Fahrzeugtyp-Limits, etc.)
- **Beispiel**: `Capacity_789`

## Bedeutung:
- **D{Nummer}** = Depotgruppe
- **V{Nummer}** = Fahrzeugtyp (VehicleType)
- **T{Nummer}** = Trip-ID
- **SP{Nummer}** = StopPoint (Haltestelle)
- **N{Nummer}** = Node-Nummer im Netzwerk
