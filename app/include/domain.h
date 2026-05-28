#ifndef DOMAIN_H
#define DOMAIN_H

#pragma once
#include "enumtype.h"
#include <string>

namespace Pfandautomat::Domain {
struct LogoDaten {
        int pfandwertInCent;
        MaterialTyp material;
        std::string form;
        bool istPfandpflichtig;
        int produktnummer;
    };

    class Leergut {
    protected:
        PfandTyp kategorie;
        LogoDaten daten;
        double gewicht;
    public:
        virtual ~Leergut() = default;
        // Getter hier einfügen...
    };

    class Flasche : public Leergut { /* ... */ };
    class Dose : public Leergut { /* ... */ };

    class Preismodell {
    public:
        static int getPfandwertInCent(PfandTyp typ, MaterialTyp mat);
    };
}

#endif // DOMAIN_H
