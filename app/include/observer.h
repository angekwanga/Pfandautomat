#pragma once

#pragma once
#include <unordered_set>

namespace Pfandautomat {

class Observer {
public:
    virtual ~Observer() = default;
    virtual void update() = 0;
};

class Observable {
private:
    std::unordered_set<Observer*> m_observers;
public:
    virtual ~Observable() = default;

    void subscribe(Observer* o)   { if(o) m_observers.insert(o); }
    void unsubscribe(Observer* o) { m_observers.erase(o); }

    void notify() {
        for (auto o : m_observers) {
            o->update();
        }
    }
};

} // namespace Pfandautomat