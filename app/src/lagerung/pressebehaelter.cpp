#include "lagerung/pressebehaelter.h"

namespace Pfandautomat {

void Pressebehaelter::inhaltHinzufuegen(int menge) {
    m_fuellstand += menge;
}

BehaelterZustand Pressebehaelter::statusAktualisieren() {
    if (m_fuellstand >= m_groesse)
        return BehaelterZustand::VOLL;
    return BehaelterZustand::BEREIT;
}

void Pressebehaelter::statusZuruecksetzen() {
    m_fuellstand = 0;
}

} // namespace Pfandautomat