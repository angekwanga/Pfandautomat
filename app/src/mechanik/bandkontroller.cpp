#include "mechanik/bandkontroller.h"
#include <cstdlib>

namespace Pfandautomat {

BandController::BandController()
    : m_band(nullptr)
    , m_zustand(BandZustand::BEREIT)
{}

void BandController::update() {
    // Wenn die Dreheinheit einen Fehlversuch meldet, zuckt das Band
    // diskret um ein kleines Stück vorwärts (positiver Delta-Wert)
    bewegeStueck(0.1);
}

void BandController::starteTransport() {
    if (m_band) {
        m_band->setDrehrichtung(true); // Standard: Vorwärts
        m_band->setMotorSpannung(true);
        m_zustand = BandZustand::LAUFEND;
    }
}

void BandController::bewegeStueck(double delta) {
    if (!m_band) return;

    // Richtung dynamisch anhand des Vorzeichens bestimmen
    // true = vorwärts (delta >= 0), false = rückwärts (delta < 0)
    bool vorwaerts = (delta >= 0.0);
    m_band->setDrehrichtung(vorwaerts);

    m_band->setMotorSpannung(true);

    // Wir übergeben den absoluten Betrag an die physikalische Simulation
    m_band->updatePositionInSimulation(std::abs(delta));

    m_band->setMotorSpannung(false); // Nach dem diskreten Schritt sofort wieder stoppen
    m_zustand = BandZustand::GESTOPPT;
}

void BandController::stoppen() {
    if (m_band) {
        m_band->setMotorSpannung(false);
        m_zustand = BandZustand::GESTOPPT;
    }
}

void BandController::notfallStopp() {
    if (m_band) {
        m_band->setMotorSpannung(false);
        m_zustand = BandZustand::FEHLER;
    }
}

} // namespace Pfandautomat