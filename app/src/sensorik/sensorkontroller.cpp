#include "sensorik/sensorkontroller.h"
#include <algorithm>
#include <random>

namespace Pfandautomat {

SensorController::SensorController()
    : m_scanner()
    , m_flaschenLichtschranke()
    , m_kastenLichtschranke()
    , m_sortierLichtschranke()
    , m_behaelterLichtschranke()
    , m_gewichtSensor(&m_flaschenLichtschranke)
    , m_letzteLogoDaten{}
    , m_letztesGewicht(0.0)
{
    m_flaschenLichtschranke.subscribe(this);
    m_scanner.subscribe(this);

    m_zufallsVerteilung.assign(9, true);
    m_zufallsVerteilung.push_back(false);
}

void SensorController::update() {}

bool SensorController::istEinwurfErkannt() const {
    return m_flaschenLichtschranke.istUnterbrochen();
}

bool SensorController::istSortierbereichFrei() const {
    return m_sortierLichtschranke.istFrei();
}

bool SensorController::istBehaelterVoll() const {
    return m_behaelterLichtschranke.istUnterbrochen();
}

bool SensorController::istKastenErkannt() const {
    return m_kastenLichtschranke.istUnterbrochen();
}

Domain::LogoDaten SensorController::starteScanning() {
    // Simulierte Daten vorhanden (gesetzt durch HardwareEinwurfTriggern)?
    // → direkt übernehmen, KEINEN Zufallspfad betreten.
    if (m_hatSimulierteDaten) {
        m_hatSimulierteDaten = false;  // Flag sofort zurücksetzen
        m_scanner.setzeZustand(
            m_letzteValidierungErfolgreich
                ? ScannerZustand::ERFOLGREICH
                : ScannerZustand::FEHLGESCHLAGEN);
        return m_letzteLogoDaten;
    }

    // Autonomer Hardware-Modus
    m_letzteLogoDaten = m_scanner.scannen();

    static std::random_device rd;
    static std::mt19937 g(rd());
    std::shuffle(m_zufallsVerteilung.begin(), m_zufallsVerteilung.end(), g);

    bool sensorEntscheidung = m_zufallsVerteilung.front() && m_letzteLogoDaten.istPfandpflichtig;
    m_letzteValidierungErfolgreich = sensorEntscheidung;
    m_scanner.setzeZustand(
        sensorEntscheidung ? ScannerZustand::ERFOLGREICH : ScannerZustand::FEHLGESCHLAGEN);

    return m_letzteLogoDaten;
}

double SensorController::starteGewichtsmessung() {
    m_letztesGewicht = m_gewichtSensor.gewichtMessen();
    return m_letztesGewicht;
}

ScannerZustand SensorController::getScannerZustand() const {
    return m_scanner.getZustand();
}

GewichtSensorZustand SensorController::getGewichtSensorZustand() const {
    return m_gewichtSensor.getZustand();
}

LichtschrankeZustand SensorController::getEinwurfZustand() const {
    return m_flaschenLichtschranke.getZustand();
}

LichtschrankeZustand SensorController::getSortierZustand() const {
    return m_sortierLichtschranke.getZustand();
}

LichtschrankeZustand SensorController::getBehaelterZustand() const {
    return m_behaelterLichtschranke.getZustand();
}

LichtschrankeZustand SensorController::getKastenZustand() const {
    return m_kastenLichtschranke.getZustand();
}

const Domain::LogoDaten& SensorController::getLetzteLogoDaten() const {
    return m_letzteLogoDaten;
}

double SensorController::getLetzteGewicht() const {
    return m_letztesGewicht;
}

void SensorController::setzeSimulierteLogoDaten(const Domain::LogoDaten& daten,
                                                bool sollErfolgreichSein) {
    m_letzteLogoDaten              = daten;
    m_letzteValidierungErfolgreich = sollErfolgreichSein;
    m_hatSimulierteDaten           = true;
}

// ---------------------------------------------------------------------------
// resetSensoren
//
// Setzt alle transienten Sensor-Zustände auf ihren Ausgangswert zurück.
// Wird vom HauptController im Zustand DONE aufgerufen, nachdem ein
// vollständiger Einwurf-Zyklus abgeschlossen ist.
//
// Warum das nötig ist:
// HardwareEinwurfTriggern() setzt den GewichtSensor auf ERFOLGREICH,
// damit verarbeiteZustandErkennung() ihn direkt lesen kann (ohne dass
// starteGewichtsmessung() ihn wegen der freien Lichtschranke auf BEREIT
// zurücksetzt). Ohne dieses Reset bleibt ERFOLGREICH nach dem Zyklus
// stehen → der nächste Einwurf (auch ein ungültiger) findet den Sensor
// bereits auf ERFOLGREICH vor und passiert die Gewichtsprüfung automatisch.
// ---------------------------------------------------------------------------
void SensorController::resetSensoren() {
    m_scanner.setzeZustand(ScannerZustand::BEREIT);
    m_gewichtSensor.setzeZustand(GewichtSensorZustand::BEREIT);
    m_hatSimulierteDaten           = false;
    m_letzteValidierungErfolgreich = false;
    m_letzteLogoDaten              = Domain::LogoDaten{};
    m_letztesGewicht               = 0.0;
    // Lichtschranken NICHT zurücksetzen – die werden durch die
    // Simulation (onTimerTick) und HardwareEinwurfTriggern gesteuert.
}

} // namespace Pfandautomat