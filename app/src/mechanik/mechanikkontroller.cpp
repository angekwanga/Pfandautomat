#include "mechanik/mechanikkontroller.h"
#include <vector>
#include <random>

namespace Pfandautomat {

MechanikController::MechanikController()
    : m_hardwareBand()
    , m_bandController() // Völlig leerer Konstruktoraufruf!
    , m_drehEinheit()
    , m_sortierWeiche()
    , m_kompressor()
{
    // 1. Das Band dynamisch aufeinander abstimmen
    m_bandController.setFoerderBand(&m_hardwareBand);

    // 2. Observer-Verbindung herstellen
    m_drehEinheit.subscribe(&m_bandController);
}

void MechanikController::update() {
    // Aktualisiert die simulationsinterne Physik und reaktive Abläufe
    m_bandController.update();
}

void MechanikController::starteFlaschenTransport() {
    m_bandController.starteTransport();
}

void MechanikController::stoppeFlaschenTransport() {
    m_bandController.stoppen();
}

void MechanikController::bewegeFlascheRueckwaerts() {
    m_hardwareBand.setDrehrichtung(false);
    m_hardwareBand.setMotorSpannung(true);
}

void MechanikController::triggerFehlversuchDrehung() {
    m_drehEinheit.verarbeiteFehlversuch();
}

bool MechanikController::maximaleDrehVersucheErreicht() const {
    return m_drehEinheit.maximaleVersucheErreicht();
}

void MechanikController::resetDreheinheit() {
    m_drehEinheit.reset();
}

bool MechanikController::stelleWeicheFuerPfand(Domain::MaterialTyp material) {
    // 1. Wahrscheinlichkeitspool für die simulation definieren: 90% true , 10% false
    static const std::vector<bool> erfolgsPool = {
        true, true, true, true, true, true, true, true, true, false
    };

    // 2. Statischen Zufallsgenerator initialisieren
    static std::random_device rd;
    static std::mt19937 gen(rd());
    // Index-Verteilung von 0 bis 9
    std::uniform_int_distribution<size_t> dist(0, erfolgsPool.size() - 1);

    // 3. Zufälligen Index würfeln und den Erfolg bestimmen
    size_t zufallsIndex = dist(gen);
    bool erfolg = erfolgsPool[zufallsIndex];

    // 4. Die Weiche wird nur bewegt, wenn die Mechanik in diesem Durchlauf fehlerfrei arbeitet
    if (erfolg) {
        if (material == Domain::MaterialTyp::GLAS) {
            m_sortierWeiche.aufklappen();  // Glas pass through
        } else {
            m_sortierWeiche.zuklappen();   // PET und ALLU nicht
        }
    } else {
        // Hier könnte man optional ein Fehler-Logging einbauen,
        // da die Weiche physisch versagt hat (z.B. blockiert ist).
    }

    return erfolg;
}

void MechanikController::verarbeiteLagerung(Domain::MaterialTyp material) {
    if (material == Domain::MaterialTyp::PET || material == Domain::MaterialTyp::ALLU) {
        m_kompressor.flaschePressen();
    }
}

void MechanikController::notausTriggern() {
    m_bandController.notfallStopp();
    m_kompressor.stoppen();
}

void MechanikController::resetKomponenten() {
    m_bandController.stoppen();
    m_sortierWeiche.zuklappen();
    m_kompressor.stoppen();
    m_drehEinheit.reset();
}

} // namespace Pfandautomat