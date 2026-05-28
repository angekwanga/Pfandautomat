#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#pragma once
#include <stdexcept>
#include <string>

namespace Pfandautomat::Kern {
    class AutomatException : public std::runtime_error {
    public:
        explicit AutomatException(const std::string& msg) : std::runtime_error(msg) {}
    };

    class MechanikException : public AutomatException {
    public:
        explicit MechanikException(const std::string& msg) : AutomatException("Mechanik: " + msg) {}
    };

    class SensorikException : public AutomatException {
    public:
        explicit SensorikException(const std::string& msg) : AutomatException("Sensorik: " + msg) {}
    };

    class BehaelterException : public AutomatException {
    public:
        explicit BehaelterException(const std::string& msg) : AutomatException("Behaelter: " + msg) {}
    };
}

#endif // EXCEPTIONS_H
