#pragma once
#include <QObject>
#include <QString>
#include <QMap>
#include <QTimer>
#include "enumtype.h"
#include "hauptkontroller.h"
#include "export.h"

struct ArtikelInfo {
    QString                           name;
    double                            preisInEuro;
    Pfandautomat::Domain::MaterialTyp material;
    unsigned int                      anzahl = 0;
};

struct AblehnungsGrund {
    QString problem;
    QString statusText;
};

class TransaktionController : public QObject {
    Q_OBJECT

public:
    explicit TransaktionController(QObject *parent = nullptr);
    ~TransaktionController() = default;

    void registriereArtikel(const QString &artikelId, const ArtikelInfo &info);

    void starteSession();
    void stoppeSession();
    void resetSession();

    void artikelEinwerfen(const QString &artikelId);
    void ungueltigesGebinde();
    void bonAusgeben(Pfandautomat::Kern::BonTyp typ);
    void HardwareEinwurfTriggern(const QString &artikelId);

    bool   istAktiv()         const { return m_aktiv; }
    double gesamtBetrag()     const { return m_total; }
    int    artikelAnzahl()    const { return m_anzahl; }
    int    gesamtAnzahl()     const { return m_anzahl; }
    int    sekunden()         const { return m_sekunden; }
    int    aktuelleSekunden() const { return m_sekunden; }

    // Gibt an ob gerade ein Gebinde verarbeitet wird.
    // MainWindow sperrt damit die Einwurf-Buttons bis die FSM wieder BEREIT ist.
    bool istInVerarbeitung()  const { return m_verarbeitung; }

    void resetTimer(); // Nur Sekundenzähler nullen, Session läuft weiter

    void exportStatistik(Pfandautomat::Kern::ExportFormat format,
                         const QString &personalId,
                         const QString &zielPfad = QString()) const;

signals:
    void sessionGestartet();
    void sessionGestoppt();
    void sessionZurueckgesetzt();
    void artikelAkzeptiert(QString artikelId, QString name,
                           double einzelpreis, double gesamt, int anzahl);
    void artikelAbgelehnt(AblehnungsGrund grund);
    void timerTick(int sekunden, int maxSekunden);
    void bonGespeichert(Pfandautomat::Kern::BonTyp typ, double betrag, int anzahl);
    void zustandGeaendert(Pfandautomat::AutomatZustand zustand);
    void fehler(QString meldung);

    // Wird emittiert wenn sich der Verarbeitungs-Status ändert.
    // MainWindow verbindet dies mit dem En-/Disablen der Einwurf-Buttons.
    void verarbeitungAktiv(bool aktiv);

private slots:
    void onTimerTick();
    void onFlascheErfolgreichAngenommen(Pfandautomat::Domain::MaterialTyp material, double preis);
    void onGebindeZurueckgewiesen();
    void onZustandGeaendertIntern(Pfandautomat::AutomatZustand zustand);

private:
    bool   m_aktiv       = false;
    bool   m_pausiert    = false;
    bool   m_verarbeitung = false; // true = Gebinde unterwegs, Buttons gesperrt
    double m_total       = 0.0;
    int    m_anzahl      = 0;
    int    m_sekunden    = 0;

    static constexpr int MAX_SEKUNDEN = 300;

    Pfandautomat::HauptController m_hauptController;
    QTimer                       *m_timer = nullptr;
    QMap<QString, ArtikelInfo>    m_artikel;
    Statistics                    m_statistik;

    void setzeVerarbeitung(bool aktiv);
    void speichereSession(Pfandautomat::Kern::BonTyp typ);
    void resetIntern();

    QString m_aktuelleSimulierteId;
};