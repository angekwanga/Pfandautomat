#pragma once
#include "lagerung/behaelter.h"

namespace Pfandautomat {

class Sammelbehaelter : public Behaelter {
public:
    void inhaltHinzufuegen(int menge) override;
    BehaelterZustand statusAktualisieren() override;
    void statusZuruecksetzen() override;
};

} // namespace Pfandautomat