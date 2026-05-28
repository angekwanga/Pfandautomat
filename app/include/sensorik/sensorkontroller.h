#pragma once
#include "sensorik/lichtschranke.h"
#include "sensorik/scanner.h"
#include "sensorik/gewichtsensor.h"
#include "domain.h"
#include <vector>

namespace Pfandautomat {

class SensorController: public Observer {
private:
    Scanner       m_scanner;
    Lichtschranke m_flaschenLichtschranke;
    Lichtschranke m_kastenLichtschranke;
    Lichtschranke m_sortierLichtschranke;
    Lichtschranke m_behaelterLichtschranke;
    GewichtSensor m_gewichtSensor;

    Domain::LogoDaten m_letzteLogoDaten;
    double            m_letztesGewicht = 0.0;

    bool m_hatSimulierteDaten          = false;
    bool m_letzteValidierungErfolgreich = false;

    std::vector<bool> m_zufallsVerteilung;

public:
    SensorController();
    ~SensorController() = default;

    void update() override;

    bool istEinwurfErkannt()     const;
    bool istSortierbereichFrei() const;
    bool istBehaelterVoll()      const;
    bool istKastenErkannt()      const;

    Domain::LogoDaten starteScanning();
    double            starteGewichtsmessung();

    bool warValidierungErfolgreich() const { return m_letzteValidierungErfolgreich; }

    ScannerZustand       getScannerZustand()   const;
    GewichtSensorZustand getGewichtSensorZustand() const;
    LichtschrankeZustand getEinwurfZustand()   const;
    LichtschrankeZustand getSortierZustand()   const;
    LichtschrankeZustand getBehaelterZustand() const;
    LichtschrankeZustand getKastenZustand()    const;

    const Domain::LogoDaten& getLetzteLogoDaten() const;
    double                   getLetzteGewicht()   const;

    Lichtschranke& getFlaschenLichtschranke() { return m_flaschenLichtschranke; }
    Scanner&       getScanner()               { return m_scanner; }
    GewichtSensor& getGewichtSensor()         { return m_gewichtSensor; }
    Lichtschranke& getSortierLichtschranke()  { return m_sortierLichtschranke; }

    void setzeSimulierteLogoDaten(const Domain::LogoDaten& daten, bool sollErfolgreichSein);

    // Setzt alle Sensor-Zustände auf BEREIT/FREI zurück.
    // Muss nach jedem abgeschlossenen Einwurf-Zyklus (im Zustand DONE)
    // aufgerufen werden, damit der GewichtSensor-Zustand ERFOLGREICH
    // vom vorherigen Durchlauf nicht den nächsten Einwurf beeinflusst.
    void resetSensoren();
};

} // namespace Pfandautomat