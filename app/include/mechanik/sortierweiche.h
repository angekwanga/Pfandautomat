#pragma once
#include "mechanik/mechanik.h"

namespace Pfandautomat {

class Sortierweiche {
private:
    KlappeZustand m_zustand;
public:
    Sortierweiche();
    void aufklappen();
    void zuklappen();
};

} // namespace Pfandautomat