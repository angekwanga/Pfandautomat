#include "mechanik/dreheinheit.h"

namespace Pfandautomat {

void DrehEinheit::verarbeiteFehlversuch() {
    // Falls das Limit bereits erreicht wurde, ignorieren wir weitere Schritte
    if (maximaleVersucheErreicht()) {
        return;
    }

    m_scanVersuche++;

    // Falls wir nach dem Fehlversuch das Limit noch nicht gesprengt haben,
    // leiten wir den nächsten mechanischen Drehschritt ein
    if (m_scanVersuche < MAXVERSUCHE) {
        m_winkel += 5.0;            // Theoretischen Rotationswinkel für die GUI/Diagnose erhöhen
        m_drehungErforderlich = true; // Flag setzen, das vom BandController abgefragt wird

        // Den Observer (BandController) asynchron alarmieren,
        // damit er in seinem update() das Band ein kleines Stück bewegt
        notify();
    }
}

void DrehEinheit::reset() {
    m_winkel = 0.0;
    m_scanVersuche = 0;
    m_drehungErforderlich = false;
}

} // namespace Pfandautomat