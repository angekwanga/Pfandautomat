#include "hauptkontroller.h"
#include <iostream>

using namespace std;

namespace Pfandautomat {

HauptController::HauptController()
    : m_aktuellerZustand(AutomatZustand::BEREIT)
    , m_sensorController()
    , m_mechanikController()
    , m_aktuelleFlasche{}
    , m_rueckweisungGemeldet(false)
{}

void HauptController::reset()
{
    m_mechanikController.notausTriggern();
    m_mechanikController.resetKomponenten();
    m_sensorController.resetSensoren();
    m_aktuelleFlasche        = Domain::LogoDaten{};
    m_rueckweisungGemeldet   = false;
    m_aktuellerZustand       = AutomatZustand::BEREIT;
    // kein Signal hier – TransaktionController emittiert zustandGeaendert selbst
}

void HauptController::update() {
    m_mechanikController.update();

    switch (m_aktuellerZustand) {

        case AutomatZustand::BEREIT:
            if (m_sensorController.getEinwurfZustand() == LichtschrankeZustand::UNTERBROCHEN ||
                m_sensorController.istEinwurfErkannt()) {
                m_mechanikController.resetDreheinheit();
                m_rueckweisungGemeldet = false;
                zustandWechseln(AutomatZustand::ERKENNUNG);
            }
            break;

        case AutomatZustand::ERKENNUNG:
            verarbeiteZustandErkennung();
            break;

        case AutomatZustand::TRANSPORT:
            if (!m_sensorController.istSortierbereichFrei()) {
                m_mechanikController.stoppeFlaschenTransport();
                zustandWechseln(AutomatZustand::SORTIERUNG);
            }
            break;

        case AutomatZustand::SORTIERUNG: {
            bool weicheOk = m_mechanikController.stelleWeicheFuerPfand(m_aktuelleFlasche.material);
            zustandWechseln(weicheOk ? AutomatZustand::RECHNEND : AutomatZustand::BLOCKIERT);
            break;
        }

        // RECHNEND: Pfandbetrag berechnen – Signal an TransaktionController.
        case AutomatZustand::RECHNEND:
            emit flascheErfolgreichAngenommen(
                m_aktuelleFlasche.material,
                m_aktuelleFlasche.pfandwertInCent / 100.0);
            zustandWechseln(AutomatZustand::LAGERUNG);
            break;

        // LAGERUNG: Nur physische Einlagerung.
        case AutomatZustand::LAGERUNG:
            if (m_sensorController.istBehaelterVoll()) {
                zustandWechseln(AutomatZustand::BLOCKIERT);
                std::cout << "Container Voll! Bitte leeren\n";
            } else {
                m_mechanikController.verarbeiteLagerung(m_aktuelleFlasche.material);
                zustandWechseln(AutomatZustand::DONE);
            }
            break;

        case AutomatZustand::RUECKWEISUNG:
            // Signal genau einmal werfen – nicht jeden Takt.
            if (!m_rueckweisungGemeldet) {
                m_rueckweisungGemeldet = true;
                emit gebindeZurueckgewiesen();
            }
            m_mechanikController.bewegeFlascheRueckwaerts();
            if (m_sensorController.getEinwurfZustand() == LichtschrankeZustand::FREI) {
                m_mechanikController.stoppeFlaschenTransport();
                zustandWechseln(AutomatZustand::DONE);
            }
            break;

        case AutomatZustand::DONE:
            m_mechanikController.resetKomponenten();
            m_sensorController.resetSensoren();
            m_rueckweisungGemeldet = false;
            if (m_sensorController.getEinwurfZustand() == LichtschrankeZustand::FREI) {
                zustandWechseln(AutomatZustand::BEREIT);
            }
            break;

        case AutomatZustand::BLOCKIERT:
            m_mechanikController.notausTriggern();
            break;

        default:
            break;
    }
}

void HauptController::verarbeiteZustandErkennung() {
    Domain::LogoDaten scanErgebnis = m_sensorController.starteScanning();

    if (m_sensorController.getScannerZustand() == ScannerZustand::ERFOLGREICH) {
        if (!scanErgebnis.istPfandpflichtig) {
            zustandWechseln(AutomatZustand::RUECKWEISUNG);
            return;
        }
        m_aktuelleFlasche = scanErgebnis;
        if (m_sensorController.getGewichtSensorZustand() == GewichtSensorZustand::ERFOLGREICH) {
            m_mechanikController.starteFlaschenTransport();
            zustandWechseln(AutomatZustand::TRANSPORT);
        } else {
            m_sensorController.starteGewichtsmessung();
            if (m_sensorController.getGewichtSensorZustand() == GewichtSensorZustand::ERFOLGREICH) {
                m_mechanikController.starteFlaschenTransport();
                zustandWechseln(AutomatZustand::TRANSPORT);
            } else {
                zustandWechseln(AutomatZustand::RUECKWEISUNG);
            }
        }
    } else {
        m_mechanikController.triggerFehlversuchDrehung();
        if (m_mechanikController.maximaleDrehVersucheErreicht()) {
            zustandWechseln(AutomatZustand::RUECKWEISUNG);
        }
    }
}

void HauptController::zustandWechseln(AutomatZustand neuerZustand) {
    if (m_aktuellerZustand == neuerZustand) return;
    m_aktuellerZustand = neuerZustand;
    emit zustandGeaendert(m_aktuellerZustand);
}

} // namespace Pfandautomat