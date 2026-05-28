#pragma once
#include "behaelter.h"
#include "lagerung.h"

namespace Pfandautomat {

class Behaelter {
public:
    virtual ~Behaelter() = default;
    virtual void inhaltHinzufuegen(int menge) = 0;
    virtual BehaelterZustand statusAktualisieren() = 0;
    virtual void statusZuruecksetzen() = 0;

    int getFuellstand() const { return m_fuellstand; }
    int getGroesse() const { return m_groesse; }

protected:
    int m_groesse = 100;
    int m_fuellstand = 0;
    int m_behaelterNr = 0;
};

} // namespace Pfandautomat