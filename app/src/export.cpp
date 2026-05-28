#include "export.h"
#include <iomanip>
#include <fstream>
#include <sstream>
#include <iostream>


// Definition der statischen Member
Statistics statVar;
const std::string Statistics::statFile = "statistik";
double Statistics::totalPrice = 0.0;
int Statistics::totalCount = 0;

Statistics::Statistics(){}
/******************************************
 *
 *
 *
 * ****************************************/
void Statistics::addEntry(const std::string& article,
                          const std::string& timestamp,
                          double price,
                          int count) {
    entries.push_back({article, timestamp, count, price});
    totalCount += count;
    totalPrice += count * price;

    std::cout << "=== Statistics Debug ===" << std::endl;
    std::cout << "Artikel: " << article << std::endl;
    std::cout << "Zeitstempel: " << timestamp << std::endl;
    std::cout << "Preis: " << std::fixed << std::setprecision(2) << price << " €" << std::endl;
    std::cout << "Anzahl: " << count << std::endl;
    std::cout << "Total Count: " << totalCount << std::endl;
    std::cout << "Total Price: " << std::fixed << std::setprecision(2) << totalPrice << " €" << std::endl;
    std::cout << "Entries Vector Size: " << entries.size() << std::endl;
    std::cout << "=========================" << std::endl;
}

void Statistics::save(Pfandautomat::Kern::ExportFormat format, const std::string& personalId,
                      const std::string& zielPfad) const {
    const std::string filename = zielPfad.empty() ? getFilename(format) : zielPfad;
    std::ofstream os(filename);

    if (!os.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return;
    }

    os << std::fixed << std::setprecision(2);

    if (format == Pfandautomat::Kern::ExportFormat::PDF) {
        os << "Statistik-Report\n";
        os << "==============================\n";
        os << "Erstellt von Personal-Nr.: " << personalId << "\n\n";
        os << "Artikel gesamt: " << totalCount << "\n";
        os << "Gesamtbetrag:   " << totalPrice << " EUR\n";
        os << "Papierbons:     " << m_papierBonCount << "\n";
        os << "Digitalbons:    " << m_digitalBonCount << "\n\n";
        os << "Einzelpositionen\n";
        os << "----------------\n";
        for (const auto& entry : entries) {
            os << entry.timestamp << " | " << entry.article
               << " | Anz: " << entry.count
               << " | " << entry.price << " EUR"
               << " | Summe: " << (entry.price * entry.count) << " EUR\n";
        }
    } else {
        os << "Zeitstempel,Artikelbezeichnung,Anzahl,Einzelpreis,Betrag\n";
        for (const auto& entry : entries) {
            os << entry.timestamp << "," << entry.article << ","
               << entry.count << "," << entry.price << ","
               << (entry.price * entry.count) << "\n";
        }
        os << "\n# Personal-Nr.: " << personalId
           << ", Artikel gesamt: " << totalCount
           << ", Gesamtbetrag: " << totalPrice << " EUR\n";
    }

    os.close();
}

std::string Statistics::getFilename(Pfandautomat::Kern::ExportFormat format) const {
    switch(format) {
    case Pfandautomat::Kern::ExportFormat::CSV: return statFile + ".csv";
    case Pfandautomat::Kern::ExportFormat::PDF: return statFile + ".pdf";
    case Pfandautomat::Kern::ExportFormat::TXT: return statFile + ".txt";
    default: return statFile + ".txt";
    }
}

/*std::string Statistics::formatHeader(const std::string& personalId) const {
    std::ostringstream oss;
    oss << "Statistiken\n"
        << "===========\n"
        << "Erstellt von " << personalId << "\n"
        << std::left << std::setw(20) << "Artikelbezeichnung"
        << std::setw(17) << "Zeitstempel"
        << std::setw(10) << "Anzahl"
        << "Betrag\n"
        << std::string(60, '-') << "\n";
    return oss.str();
}*/

/*std::string Statistics::formatEntry(const Entry& e) const {
    std::ostringstream oss;
    oss << std::left << std::setw(20) << e.article
        << std::setw(17) << e.timestamp
        << std::setw(10) << e.count
        << std::fixed << std::setprecision(2) << e.price << " EUR\n";
    return oss.str();
}*/

void Statistics::incrementDigitalBon() { m_digitalBonCount++; }
void Statistics::incrementPapierBon() { m_papierBonCount++; }
void Statistics::setDigitalBonCount(int count) { m_digitalBonCount = count; }
void Statistics::setPapierBonCount(int count) { m_papierBonCount = count; }

void Statistics::clear() {
    entries.clear();
    // Optional: Auch die statischen totals zurücksetzen
    totalCount = 0;
    totalPrice = 0.0;
}