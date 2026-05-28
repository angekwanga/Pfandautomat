#include "lagerung/sammelbehaelter.h"

namespace Pfandautomat {

void Sammelbehaelter::inhaltHinzufuegen(int menge) {
    m_fuellstand += menge;
}

BehaelterZustand Sammelbehaelter::statusAktualisieren() {
    if (m_fuellstand >= m_groesse)
        return BehaelterZustand::VOLL;
    return BehaelterZustand::BEREIT;
}

void Sammelbehaelter::statusZuruecksetzen() {
    m_fuellstand = 0;
}

} // namespace Pfandautomat