#pragma once
#include "observer.h"
#include "sensorik/sensorik.h"

namespace Pfandautomat {

class Lichtschranke : public Observable, public ISensor<LichtschrankeZustand> {
private:
    LichtschrankeZustand m_zustand;

public:
    Lichtschranke();

    void setzeZustand(LichtschrankeZustand zustand) override;

    LichtschrankeZustand getZustand() const override { return m_zustand; }
    bool istUnterbrochen() const;
    bool istFrei() const;
};

} // namespace Pfandautomat