#pragma once

namespace Pfandautomat {

// Gemeinsame Sensor-Basisschnittstelle
// Jeder Sensor hat einen Zustand der gelesen und gesetzt werden kann.
template <typename TZustand>
class ISensor {
public:
    virtual ~ISensor() = default;
    virtual void     setzeZustand(TZustand zustand) = 0;
    virtual TZustand getZustand()                   const = 0;
};

// Lichtschranke – bleibt bewusst binär (physikalisches On/Off-Signal)
enum class LichtschrankeZustand {
    FREI,        // Lichtstrahl nicht unterbrochen – kein Objekt erkannt
    UNTERBROCHEN,  // Objekt im Strahlengang erkannt
    BEREIT,         // Wartet auf Einwurf
    SCANNT,         // Scan läuft aktiv
    ERFOLGREICH,    // Barcode / Logo erkannt und ausgelesen
    FEHLGESCHLAGEN  // Kein verwertbares Ergebnis (unleserlich, kein Pfand)
};

// Scanner – Phasen eines Scan-Vorgangs
enum class ScannerZustand {
    BEREIT,         // Wartet auf Einwurf
    SCANNT,         // Scan läuft aktiv
    ERFOLGREICH,    // Barcode / Logo erkannt und ausgelesen
    FEHLGESCHLAGEN  // Kein verwertbares Ergebnis (unleserlich, kein Pfand)
};

// GewichtSensor – Phasen einer Gewichtsmessung
// MESSEND statt SCANNT – der Sensor misst, er scannt nicht
enum class GewichtSensorZustand {
    BEREIT,         // Wartet auf Objekt
    MESSEND,        // Messung läuft aktiv
    ERFOLGREICH,    // Gewicht plausibel ermittelt
    FEHLGESCHLAGEN  // Messung fehlerhaft (Über-/Untergewicht, kein Signal)
};

} // namespace Pfandautomat
