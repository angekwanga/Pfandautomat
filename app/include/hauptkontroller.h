#ifndef HAUPTKONTROLLER_H
#define HAUPTKONTROLLER_H

#pragma once
#include "enumtype.h"
#include "domain.h"
#include "sensorik/sensorkontroller.h"
#include "mechanik/mechanikkontroller.h"
#include <QObject>

namespace Pfandautomat {

class HauptController: public QObject {
    Q_OBJECT
private:
    AutomatZustand     m_aktuellerZustand;
    SensorController   m_sensorController;
    MechanikController m_mechanikController;
    Domain::LogoDaten  m_aktuelleFlasche;

    // Verhindert, dass gebindeZurueckgewiesen() jeden Takt neu geworfen wird.
    bool m_rueckweisungGemeldet = false;

    void verarbeiteZustandErkennung();
    void zustandWechseln(AutomatZustand neuerZustand);

signals:
    void zustandGeaendert(AutomatZustand neuerZustand);
    void flascheErfolgreichAngenommen(Pfandautomat::Domain::MaterialTyp material, double preis);

    // Einmalig geworfen beim ersten Takt in RUECKWEISUNG.
    // TransaktionController reagiert darauf mit artikelAbgelehnt → GUI zeigt Fehlermeldung.
    void gebindeZurueckgewiesen();

public:
    HauptController();
    ~HauptController() = default;

    void update();

    // Vollständiger Reset der FSM + Sub-Controller.
    // Muss bei Stop→Start aufgerufen werden.
    void reset();
    void simuliereTransport();

    AutomatZustand getAktuellerZustand() const { return m_aktuellerZustand; }

    const SensorController& getSensorController() const { return m_sensorController; }
    SensorController&       getSensorController()       { return m_sensorController; }

    const MechanikController& getMechanikController() const { return m_mechanikController; }
};

} // namespace Pfandautomat

#endif // HAUPTKONTROLLER_H