#pragma once
#include "mechanik/mechanik.h" // Für KompressorZustand

namespace Pfandautomat {

class Kompressor {
private:
    int m_druck = 0;
    int m_zyklusZaehler = 0;
    KompressorZustand m_zustand = KompressorZustand::BEREIT;

public:
    Kompressor() = default;
    ~Kompressor() = default;

    void flaschePressen();
    void stoppen();

    bool istBereit() const            { return m_zustand == KompressorZustand::BEREIT; }
    KompressorZustand getZustand() const { return m_zustand; }
};

} // namespace Pfandautomat