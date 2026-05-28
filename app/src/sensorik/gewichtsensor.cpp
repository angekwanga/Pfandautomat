#include "sensorik/gewichtsensor.h"
#include "sensorik/lichtschranke.h"

namespace Pfandautomat {

// Grenzwerte für die Plausibilitätsprüfung (in Gramm)
static constexpr double MIN_GEWICHT_G =  10.0;  // unter 10g → zerknüllt / leer
static constexpr double MAX_GEWICHT_G = 300.0;  // über 300g → noch befüllt

GewichtSensor::GewichtSensor(Lichtschranke* einwurfLichtschranke,
                             FoerderBand*   foerderBand)
    : m_zustand(GewichtSensorZustand::BEREIT)
    , m_gewicht(0.0)
    , m_einwurfLichtschranke(einwurfLichtschranke)
    , m_foerderBand(foerderBand)
{}

void GewichtSensor::setzeZustand(GewichtSensorZustand zustand) {
    m_zustand = zustand;
}

GewichtSensorZustand GewichtSensor::getZustand() const {
    return m_zustand;
}

double GewichtSensor::gewichtMessen() {
    setzeZustand(GewichtSensorZustand::MESSEND);

    // Lichtschranken-Prüfung: kein Objekt → sofort abbrechen
    if (m_einwurfLichtschranke != nullptr) {
        if (m_einwurfLichtschranke->istFrei()) {
            m_gewicht = 0.0;
            setzeZustand(GewichtSensorZustand::BEREIT);
            return 0.0;
        }
    }
    // Testmodus (kein Lichtschranke angeschlossen): Testgewichte zyklisch liefern
    if (m_testGewichte.empty()) {
        m_gewicht = 0.0;
        setzeZustand(GewichtSensorZustand::FEHLGESCHLAGEN);
        return 0.0;
    }

    double gemessen = m_testGewichte[m_testIndex];
    m_testIndex = (m_testIndex + 1) % m_testGewichte.size();

    // Plausibilitätsprüfung
    if (gemessen < MIN_GEWICHT_G || gemessen > MAX_GEWICHT_G) {
        m_gewicht = gemessen; // Wert trotzdem speichern (für Diagnose)
        setzeZustand(GewichtSensorZustand::FEHLGESCHLAGEN);
    } else {
        m_gewicht = gemessen;
        setzeZustand(GewichtSensorZustand::ERFOLGREICH);
    }

    return m_gewicht;
}

double GewichtSensor::letztesMessergebnis() const {
    return m_gewicht;
}

bool GewichtSensor::istErfolgreich() const {
    return m_zustand == GewichtSensorZustand::ERFOLGREICH;
}

bool GewichtSensor::istFehlgeschlagen() const {
    return m_zustand == GewichtSensorZustand::FEHLGESCHLAGEN;
}

} // namespace Pfandautomat
