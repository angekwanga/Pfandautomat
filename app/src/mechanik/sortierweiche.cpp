#include "mechanik/sortierweiche.h"

namespace Pfandautomat {

Sortierweiche::Sortierweiche() : m_zustand(KlappeZustand::ZU) {};

void Sortierweiche::aufklappen() {
    m_zustand = KlappeZustand::AUF;
}

void Sortierweiche::zuklappen() {
    m_zustand = KlappeZustand::ZU;
}

} // namespace Pfandautomat