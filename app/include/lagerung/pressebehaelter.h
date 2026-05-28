#pragma once
#include "lagerung/behaelter.h"

namespace Pfandautomat {

class Pressebehaelter : public Behaelter {
public:
    void inhaltHinzufuegen(int menge) override;
    BehaelterZustand statusAktualisieren() override;
    void statusZuruecksetzen() override;
};

} // namespace Pfandautomat