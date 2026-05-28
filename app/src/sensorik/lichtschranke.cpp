#include "sensorik/lichtschranke.h"

namespace Pfandautomat {

Lichtschranke::Lichtschranke()
    : m_zustand(LichtschrankeZustand::FREI)
{}

void Lichtschranke::setzeZustand(LichtschrankeZustand zustand) {
    if (m_zustand != zustand) { // Nur bei echter Änderung feuern
        m_zustand = zustand;
        notify();
    }
}

bool Lichtschranke::istUnterbrochen() const {
    return m_zustand == LichtschrankeZustand::UNTERBROCHEN;
}

bool Lichtschranke::istFrei() const {
    return m_zustand == LichtschrankeZustand::FREI;
}

} // namespace Pfandautomat
