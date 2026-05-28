#pragma once
#include "enumtype.h"
#include "mechanik/foerderband.h"
#include "mechanik/bandkontroller.h"
#include "mechanik/dreheinheit.h"
#include "mechanik/sortierweiche.h"
#include "mechanik/kompressor.h"

namespace Pfandautomat {

class MechanikController {
private:
    FoerderBand     m_hardwareBand;
    BandController  m_bandController;
    DrehEinheit     m_drehEinheit;
    Sortierweiche   m_sortierWeiche;
    Kompressor      m_kompressor;

public:
    MechanikController();
    ~MechanikController() = default;

    // Wird zyklisch vom HauptController aufgerufen, um die internen Ketten (z.B. Bandzucken) zu triggern
    void update();

    // --- High-Level Befehle für den HauptController ---
    void starteFlaschenTransport();
    void stoppeFlaschenTransport();
    void bewegeFlascheRueckwaerts();

    void triggerFehlversuchDrehung();
    bool maximaleDrehVersucheErreicht() const;
    void resetDreheinheit();

    bool stelleWeicheFuerPfand(Domain::MaterialTyp material);
    void verarbeiteLagerung(Domain::MaterialTyp material);

    void notausTriggern();
    void resetKomponenten();

    // Getter für Diagnose / GUI
    const BandController& getBandController() const { return m_bandController; }
    const DrehEinheit&    getDrehEinheit()    const { return m_drehEinheit; }
};

} // namespace Pfandautomat