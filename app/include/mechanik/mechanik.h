#pragma once

// Mechanik: Förderband & Motoren
enum class BandZustand {
    BEREIT,
    LAUFEND,
    GESTOPPT,
    FEHLER
};

// Mechanik: Sortierweiche / Klappe
enum class KlappeZustand {
    AUF,
    ZU,
    FEHLER
};

// Mechanik: Kompressor
enum class KompressorZustand {
    BEREIT,
    AKTIV,
    FEHLER,
    WARTUNG
};

