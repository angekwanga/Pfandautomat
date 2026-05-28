#include "sensorik/scanner.h"

namespace Pfandautomat {

Scanner::Scanner()
    : m_zustand(ScannerZustand::BEREIT)
    , m_letztesDaten{}
{}

void Scanner::setzeZustand(ScannerZustand zustand) {
    m_zustand = zustand;
}

ScannerZustand Scanner::getZustand() const {
    return m_zustand;
}

Domain::LogoDaten Scanner::scannen() {
    setzeZustand(ScannerZustand::SCANNT);

    m_letztesDaten = simuliereScan();

    if (m_letztesDaten.istPfandpflichtig) {
        setzeZustand(ScannerZustand::ERFOLGREICH);
    } else {
        setzeZustand(ScannerZustand::FEHLGESCHLAGEN);
    }

    return m_letztesDaten;
}

bool Scanner::istErfolgreich() const {
    return m_zustand == ScannerZustand::ERFOLGREICH;
}

// Simulation – liefert reihum verschiedene Szenarien.
// Wird ersetzt sobald echte Hardware (USB-Scanner, GPIO) angebunden ist.
Domain::LogoDaten Scanner::simuliereScan() const {
    // Simulierte Szenarien: 5 gültige Artikel, 1 ungültiger
    static const Domain::LogoDaten szenarien[] = {
        { 25, Domain::MaterialTyp::PET,      "Flasche", true,  1001 },
        { 25, Domain::MaterialTyp::ALLU,     "Dose",    true,  1002 },
        { 15, Domain::MaterialTyp::GLAS,     "Flasche", true,  1003 },
        { 25, Domain::MaterialTyp::PET,      "Flasche", true,  1004 },
        {  8, Domain::MaterialTyp::HDPE,     "Kasten",  true,  1005 },
        {  0, Domain::MaterialTyp::SONSTIGES,"Unbekannt",false, 0   }  // ungültig
    };
    static std::size_t index = 0;
    Domain::LogoDaten ergebnis = szenarien[index];
    index = (index + 1) % (sizeof(szenarien) / sizeof(szenarien[0]));
    return ergebnis;
}

} // namespace Pfandautomat
