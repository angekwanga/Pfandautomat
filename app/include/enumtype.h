#ifndef ENUMTYPE_H
#define ENUMTYPE_H

#pragma once

namespace Pfandautomat {

    namespace Kern {
    enum class Modus {
        KUNDE,
        PERSONAL,
        TECHNISCHER
    };

    enum class ExportFormat {
        TXT,
        CSV,
        PDF
    };

    // Fix 4: proper BonTyp enum – no more string comparison
    enum class BonTyp {
        PAPIER,
        DIGITAL
    };
    }

    namespace Domain {
    enum class MaterialTyp {
        GLAS,
        PET,
        HDPE,
        ALLU,
        SONSTIGES
    };

    enum class PfandTyp {
        EINWEG,
        MEHRWEG
    };
    }

    enum class DruckerZustand {
        BEREIT,
        DRUCKT,
        PAPIERLEER,
        FEHLER
    };

    enum class AutomatZustand {
        BEREIT,
        ERKENNUNG,
        TRANSPORT,
        SORTIERUNG,
        RECHNEND,
        LAGERUNG,
        RUECKWEISUNG,
        AUSGABE,
        DONE,
        BLOCKIERT,
        WARTUNG
    };

}

#endif // ENUMTYPE_H
