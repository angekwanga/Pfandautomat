#pragma once
#include <string>
#include <vector>
#include "enumtype.h"

class DisplayMonitor;  // Forward declaration

class Statistics {
public:
    Statistics();
    void addEntry(const std::string& article,
                  const std::string& timestamp,
                  double price,
                  int count);
    void save(Pfandautomat::Kern::ExportFormat format, const std::string& personalId,
              const std::string& zielPfad = "") const;
    void clear();

    // Getter für DisplayMonitor
    int getDigitalBonCount() const { return m_digitalBonCount; }
    int getPapierBonCount() const { return m_papierBonCount; }

private:
    // Nur DisplayMonitor darf diese Methoden verwenden
    friend class DisplayMonitor;

    void incrementDigitalBon();
    void incrementPapierBon();
    void setDigitalBonCount(int count);
    void setPapierBonCount(int count);

    struct Entry {
        std::string article;
        std::string timestamp;
        int count;
        double price;
    };

    std::vector<Entry> entries;
    static const std::string statFile;

    static double totalPrice;
    static int totalCount;

    int m_digitalBonCount = 0;
    int m_papierBonCount = 0;

    std::string formatHeader(const std::string& personalId) const;
    std::string formatEntry(const Entry& e) const;
    std::string getFilename(Pfandautomat::Kern::ExportFormat format) const;
};

extern Statistics statVar;