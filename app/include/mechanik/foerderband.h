// foerderband.h
#pragma once

namespace Pfandautomat {

class FoerderBand {
private:
    double m_position = 0.0; // Position des Artikel auf der Achse
    bool m_motorLaeuft = false;
    bool m_richtungVorwaerts = true; // Vorwärst oder Rückwärst bewegung?

public:
    FoerderBand() = default;

    void setMotorSpannung(bool an) { m_motorLaeuft = an; }
    void setDrehrichtung(bool vorwaerts) { m_richtungVorwaerts = vorwaerts; }

    bool istMotorAn() const { return m_motorLaeuft; }
    bool getDrehrichtung() const { return m_richtungVorwaerts; }
    double getAktuellePosition() const { return m_position; }

    void updatePositionInSimulation(double delta);
};

} // namespace Pfandautomat