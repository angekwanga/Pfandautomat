#pragma once
#include "sensorik/sensorik.h"
#include "domain.h"
#include "observer.h"

namespace Pfandautomat {

// Liest Barcode / Pfandlogo vom eingeworfenen Artikel.
// Gibt LogoDaten zurück – oder einen ungültigen Datensatz bei Fehlschlag.
class Scanner : public Observable, public ISensor<ScannerZustand> {
public:
    Scanner();

    // ISensor-Interface
    void         setzeZustand(ScannerZustand zustand) override;
    ScannerZustand getZustand() const override;

    // Hauptoperation: führt einen Scan durch und aktualisiert den Zustand.
    // Gibt die erkannten LogoDaten zurück.
    // Bei Fehlschlag: Zustand = FEHLGESCHLAGEN, LogoDaten.istPfandpflichtig = false
    Domain::LogoDaten scannen();

    // Semantischer Helfer
    bool istErfolgreich() const;

private:
    ScannerZustand    m_zustand;
    Domain::LogoDaten m_letztesDaten; // Ergebnis des letzten Scans

    // Simulierte Testdaten – wird ersetzt sobald echte Hardware angebunden
    Domain::LogoDaten simuliereScan() const;
};

} // namespace Pfandautomat
