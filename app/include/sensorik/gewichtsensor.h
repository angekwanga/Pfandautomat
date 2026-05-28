#pragma once
#include <vector>
#include "sensorik/sensorik.h"

namespace Pfandautomat {

// Forward declarations – GewichtSensor braucht diese zur Laufzeit,
// aber der Header soll keine transitiven Abhängigkeiten ziehen
class Lichtschranke;
class FoerderBand;

// Misst das Gewicht des eingeworfenen Artikels.
// Benötigt die Einwurf-Lichtschranke um zu prüfen ob überhaupt ein
// Objekt im Messbereich liegt (Null-Guard vor jeder Messung).
//
// Dependency Injection via Konstruktor:
//   - Im Produktionsbetrieb: echte Lichtschranke & FoerderBand übergeben
//   - Im Test/Simulation:    nullptr → Sensor arbeitet mit Testgewichten
class GewichtSensor : public ISensor<GewichtSensorZustand> {
public:
    explicit GewichtSensor(Lichtschranke* einwurfLichtschranke = nullptr,
                           FoerderBand*  foerderBand          = nullptr);

    // ISensor-Interface
    void               setzeZustand(GewichtSensorZustand zustand) override;
    GewichtSensorZustand getZustand() const override;

    // Hauptoperation: führt eine Gewichtsmessung durch und aktualisiert Zustand.
    // Gibt 0.0 zurück wenn kein Objekt erkannt (Lichtschranke frei)
    //      oder Lichtschranke nicht angeschlossen (Testmodus).
    double gewichtMessen();

    // Letztes gemessenes Gewicht ohne erneute Messung abrufen
    double letztesMessergebnis() const;

    // Semantische Helfer
    bool istErfolgreich()    const;
    bool istFehlgeschlagen() const;

private:
    GewichtSensorZustand m_zustand;
    double               m_gewicht;

    Lichtschranke* m_einwurfLichtschranke; // kann nullptr sein (Testmodus)
    FoerderBand*   m_foerderBand;          // aktuell ungenutzt, für spätere Steuerung

    // Simulierte Testwerte (Gramm):
    //   15.0  – leere PET-Flasche        (gültig)
    //  120.0  – Glasflasche              (gültig)
    //  350.0  – Flasche mit Restinhalt   (zu schwer → FEHLGESCHLAGEN)
    //    5.0  – zerknülltes Plastik      (zu leicht → FEHLGESCHLAGEN)
    const std::vector<double> m_testGewichte = { 15.0, 120.0, 350.0, 15.0, 120.0, 5.0 };
    mutable std::size_t m_testIndex = 0; // zyklisch durch Testgewichte iterieren
};

} // namespace Pfandautomat
