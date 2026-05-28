// foerderband.cpp
#include "mechanik/foerderband.h"

namespace Pfandautomat {

void FoerderBand::updatePositionInSimulation(double delta) {
    if (m_motorLaeuft) {
        if (m_richtungVorwaerts) {
            m_position += delta;
        } else {
            m_position -= delta;
        }
        // Begrenzung der Position (simulierte Enden)
        if (m_position < 0.0) m_position = 0.0;
        if (m_position > 100.0) m_position = 100.0;
    }
}

} // namespace Pfandautomat