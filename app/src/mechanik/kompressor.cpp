#include "mechanik/kompressor.h"

namespace Pfandautomat {

void Kompressor::flaschePressen() {
    m_zustand = KompressorZustand::AKTIV;
    m_druck = 0;

    for (m_zyklusZaehler = 0; m_zyklusZaehler < 5; ++m_zyklusZaehler) {
        m_druck += 20; // Simulierter linearer Druckaufbau
    }

    m_druck = 0;
    m_zustand = KompressorZustand::BEREIT;
}

void Kompressor::stoppen() {
    m_druck = 0;
    m_zustand = KompressorZustand::BEREIT;
}

} // namespace Pfandautomat