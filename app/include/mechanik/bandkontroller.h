#pragma once
#include "mechanik/foerderband.h"
#include "mechanik/mechanik.h"
#include "observer.h"

namespace Pfandautomat {

/**
 * @class BandController
 * @brief Steuert die Logik und die Bewegung des Förderbandes.
 * * DESIGN-ENTSCHEIDUNG: Das physische Förderband wird nicht mehr im Konstruktor
 * erzwungen, um die Schichten flexibler testen und initialisieren zu können.
 * Bewegungen in verschiedene Richtungen (vorwärts/rückwärts) werden einheitlich
 * über das Vorzeichen des Parameters delta in bewegeStueck() gesteuert.
 */
class BandController : public Observer {
private:
    FoerderBand* m_band = nullptr;
    BandZustand  m_zustand;

public:
    BandController();
    virtual ~BandController() override = default;

    /**
     * @brief Injiziert die Referenz auf das reale/simulierte Förderband.
     */
    void setFoerderBand(FoerderBand* hardwareBand) { m_band = hardwareBand; }

    /**
     * @brief Implementierung des Observer-Interfaces.
     * Reagiert auf das notify() der Dreheinheit bei einem Scan-Fehlversuch.
     * Lässt das Band ein kleines Stück vorwärtsruckeln.
     */
    void update() override;

    /**
     * @brief Startet den kontinuierlichen Transport (z.B. für den Weg zur Sortierung).
     */
    void starteTransport();

    /**
     * @brief Bewegt das Band um ein definiertes Stück.
     * @param delta Positiv für Vorwärtsbewegung, negativ für Rückwärtsbewegung.
     */
    void bewegeStueck(double delta);

    /**
     * @brief Stoppt den Motor des Bandes regulär.
     */
    void stoppen();

    /**
     * @brief Versetzt das Band in einen sicheren Fehlerzustand (Notaus).
     */
    void notfallStopp();

    BandZustand getZustand() const { return m_zustand; }
};

} // namespace Pfandautomat