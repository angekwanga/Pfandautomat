#pragma once
#include "observer.h"

namespace Pfandautomat {

/**
 * @class DrehEinheit
 * @brief Repräsentiert die mechanische Dreheinheit im Einwurfsbereich.
 * Um zyklische Abhängigkeiten und enge Kopplung zu vermeiden,
 * ist diese Komponente "blind". Sie aggregiert keine Sensorik (Lichtschranke/Scanner).
 * Die Ablaufsteuerung wird vom HauptController delegiert. Die Dreheinheit agiert
 * als Observable, um dem BandController asynchron mitzuteilen, wann ein diskreter
 * Drehratenschritt (Zucken des Bandes) mechanisch erforderlich ist.
 */
class DrehEinheit : public Observable {
private:
    static constexpr int MAXVERSUCHE = 5;
    double m_winkel = 0.0;
    int m_scanVersuche = 0;
    bool m_drehungErforderlich = false;

public:
    // Standard-Konstruktor – benötigt keinerlei Sensor-Pointer mehr
    DrehEinheit() = default;
    virtual ~DrehEinheit() = default;

    /**
     * Registriert einen fehlgeschlagenen Scan-Versuch und leitet die Mechanik ein.
     * Erhöht den internen Zähler und setzt das Flag für den Bandcontroller,
     * falls das Limit noch nicht erreicht wurde. Triggert notify().
     */
    void verarbeiteFehlversuch();

    /**
     * Setzt die Dreheinheit in den Ausgangszustand zurück (z.B. bei neuer Flasche).
     */
    void reset();

    // --- Getters & Setters für die Ablaufsteuerung und den Observer ---

    /**
     * Gibt an, ob das Band ein Stück vorwärtszucken muss, um die Flasche zu drehen.
     */
    bool istDrehungErforderlich() const { return m_drehungErforderlich; }

    /**
     * Quittiert die erfolgte Drehung (wird vom BandController nach dem Zucken aufgerufen).
     */
    void drehungQuittieren() { m_drehungErforderlich = false; }

    /**
     * Prüft, ob die maximal zulässige Anzahl an Rotations- und Scanversuchen erreicht ist.
     */
    bool maximaleVersucheErreicht() const { return m_scanVersuche >= MAXVERSUCHE; }

    // Diagnose- und GUI-Schnittstellen
    double getLetzterWinkel() const { return m_winkel; }
    int getScanVersuche() const     { return m_scanVersuche; }
};

} // namespace Pfandautomat