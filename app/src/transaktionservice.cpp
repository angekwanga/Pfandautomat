#include "transaktionservice.h"
#include <QDateTime>

static const QVector<AblehnungsGrund> s_ablehnungsGruende = {
    {"Barcode nicht lesbar",             "Scannerfehler"},
    {"Pfandlogo fehlt",                  "Keine Pfandpflicht"},
    {"Artikel nicht in der Datenbank",   "Datenbankprüfung"},
    {"Material wird nicht akzeptiert",   "Materialfehler"},
    {"Gewicht passt nicht zum Artikel",  "Gewichtsprüfung"},
    {"Form beschädigt oder zerdrückt",   "Formerkennung"},
    {"Gebinde ist noch gefüllt",         "Gewichtsprüfung"},
    };

// ---------------------------------------------------------------------------
TransaktionController::TransaktionController(QObject *parent)
    : QObject(parent)
{
    m_timer = new QTimer(this);
    m_timer->setInterval(1000);

    // update() ZUERST, onTimerTick() danach – Qt-Reihenfolge garantiert.
    connect(m_timer, &QTimer::timeout, this, [this]() {
        if (m_aktiv && !m_pausiert)
            m_hauptController.update();
    });
    connect(m_timer, &QTimer::timeout, this, &TransaktionController::onTimerTick);

    // Zustandsänderungen intern abfangen (Button-Blocking + Weiterleitung an GUI)
    connect(&m_hauptController, &Pfandautomat::HauptController::zustandGeaendert,
            this, &TransaktionController::onZustandGeaendertIntern);

    // Gutschrift aus RECHNEND
    connect(&m_hauptController, &Pfandautomat::HauptController::flascheErfolgreichAngenommen,
            this, &TransaktionController::onFlascheErfolgreichAngenommen);

    // Rückweisung einmalig aus RUECKWEISUNG → Display sofort aktualisieren
    connect(&m_hauptController, &Pfandautomat::HauptController::gebindeZurueckgewiesen,
            this, &TransaktionController::onGebindeZurueckgewiesen);
}

void TransaktionController::registriereArtikel(const QString &artikelId, const ArtikelInfo &info)
{
    m_artikel.insert(artikelId, info);
}

// ---------------------------------------------------------------------------
// setzeVerarbeitung – zentrale Stelle für das Button-Blocking-Flag.
// Emittiert verarbeitungAktiv(bool) damit MainWindow die Buttons sperren/
// freigeben kann ohne selbst den FSM-Zustand kennen zu müssen.
// ---------------------------------------------------------------------------
void TransaktionController::setzeVerarbeitung(bool aktiv)
{
    if (m_verarbeitung == aktiv) return;
    m_verarbeitung = aktiv;
    emit verarbeitungAktiv(aktiv);
}

// ---------------------------------------------------------------------------
// onZustandGeaendertIntern
// Empfängt alle FSM-Zustandsänderungen, leitet sie an die GUI weiter und
// kümmert sich um das Button-Blocking:
// – Sobald BEREIT erreicht wird → Verarbeitung beendet, Buttons freigeben.
// – Bei jedem anderen Zustand bleibt m_verarbeitung wie es ist (wurde beim
//   Einwurf auf true gesetzt).
// ---------------------------------------------------------------------------
void TransaktionController::onZustandGeaendertIntern(Pfandautomat::AutomatZustand zustand)
{
    // An GUI weiterleiten
    emit zustandGeaendert(zustand);

    // Buttons wieder freigeben sobald die FSM BEREIT erreicht
    if (zustand == Pfandautomat::AutomatZustand::BEREIT) {
        setzeVerarbeitung(false);
    }
}

// ---------------------------------------------------------------------------
void TransaktionController::starteSession()
{
    if (m_aktiv && !m_pausiert) return;

    m_timer->stop();
    m_hauptController.reset();
    resetIntern();

    m_aktiv = true;
    m_timer->start();
    emit sessionGestartet();
    emit zustandGeaendert(Pfandautomat::AutomatZustand::BEREIT);
}

void TransaktionController::stoppeSession()
{
    if (!m_aktiv) { emit fehler("Keine aktive Transaktion."); return; }
    m_timer->stop();
    m_aktiv    = false;
    m_pausiert = true;
    setzeVerarbeitung(false);
    emit sessionGestoppt();
}

void TransaktionController::resetSession()
{
    m_timer->stop();
    m_hauptController.reset();
    resetIntern();
    emit sessionZurueckgesetzt();
    emit zustandGeaendert(Pfandautomat::AutomatZustand::BEREIT);
}

void TransaktionController::resetTimer()
{
    m_sekunden = 0;
    emit timerTick(0, MAX_SEKUNDEN);
}

// ---------------------------------------------------------------------------
// artikelEinwerfen – Drag&Drop-Pfad
// ---------------------------------------------------------------------------
void TransaktionController::artikelEinwerfen(const QString &artikelId)
{
    if (!m_aktiv)             { emit fehler("Bitte zuerst Start drücken."); return; }
    if (m_verarbeitung)       { emit fehler("Bitte warten – Gebinde wird verarbeitet."); return; }
    if (!m_artikel.contains(artikelId)) { emit fehler("Unbekannter Artikel: " + artikelId); return; }
    m_aktuelleSimulierteId = artikelId;
    HardwareEinwurfTriggern(artikelId);
}

// ---------------------------------------------------------------------------
// HardwareEinwurfTriggern – Button-Click-Pfad (nach Animation)
// ---------------------------------------------------------------------------
void TransaktionController::HardwareEinwurfTriggern(const QString &artikelId)
{
    if (!m_aktiv)       return;
    if (m_verarbeitung) return;   // Zweiter Einwurf während Verarbeitung → ignorieren
    if (!m_artikel.contains(artikelId)) return;

    m_aktuelleSimulierteId = artikelId;

    // Verarbeitung starten → Buttons sperren
    setzeVerarbeitung(true);

    const ArtikelInfo &info = m_artikel[artikelId];
    auto &sc = m_hauptController.getSensorController();

    Pfandautomat::Domain::LogoDaten daten{};
    daten.istPfandpflichtig = true;
    daten.material          = info.material;
    daten.pfandwertInCent   = static_cast<int>(info.preisInEuro * 100.0);
    daten.produktnummer     = daten.pfandwertInCent;
    daten.form              = info.name.toStdString();
    sc.setzeSimulierteLogoDaten(daten, true);

    sc.getGewichtSensor().setzeZustand(Pfandautomat::GewichtSensorZustand::ERFOLGREICH);
    sc.getFlaschenLichtschranke().setzeZustand(Pfandautomat::LichtschrankeZustand::UNTERBROCHEN);
}

// ---------------------------------------------------------------------------
void TransaktionController::ungueltigesGebinde()
{
    if (!m_aktiv)       { emit fehler("Bitte zuerst Start drücken."); return; }
    if (m_verarbeitung) { emit fehler("Bitte warten – Gebinde wird verarbeitet."); return; }

    m_aktuelleSimulierteId = "";

    // Verarbeitung starten → Buttons sperren
    setzeVerarbeitung(true);

    auto &sc = m_hauptController.getSensorController();
    Pfandautomat::Domain::LogoDaten ungueltig{};
    ungueltig.istPfandpflichtig = false;
    sc.setzeSimulierteLogoDaten(ungueltig, false);
    sc.getFlaschenLichtschranke().setzeZustand(Pfandautomat::LichtschrankeZustand::UNTERBROCHEN);
}

// ---------------------------------------------------------------------------
// onGebindeZurueckgewiesen
// Kommt sofort beim ersten RUECKWEISUNG-Takt → Display wird sofort
// aktualisiert, nicht erst wenn DONE/BEREIT erreicht wird.
// ---------------------------------------------------------------------------
void TransaktionController::onGebindeZurueckgewiesen()
{
    AblehnungsGrund grund = m_aktuelleSimulierteId.isEmpty()
    ? s_ablehnungsGruende[1]  // "Pfandlogo fehlt"
    : s_ablehnungsGruende[0]; // "Barcode nicht lesbar"

    m_aktuelleSimulierteId = "";
    emit artikelAbgelehnt(grund);
    // m_verarbeitung bleibt true – Buttons bleiben gesperrt bis FSM → BEREIT
}

// ---------------------------------------------------------------------------
// onFlascheErfolgreichAngenommen – kommt aus RECHNEND
// ---------------------------------------------------------------------------
void TransaktionController::onFlascheErfolgreichAngenommen(
    Pfandautomat::Domain::MaterialTyp material, double preis)
{
    Q_UNUSED(material);
    Q_UNUSED(preis);

    if (m_aktuelleSimulierteId.isEmpty() || !m_artikel.contains(m_aktuelleSimulierteId)) {
        emit artikelAbgelehnt(s_ablehnungsGruende[0]);
        return;
    }

    ArtikelInfo &info  = m_artikel[m_aktuelleSimulierteId];
    info.anzahl++;

    double einzelpreis = info.preisInEuro;
    m_total  += einzelpreis;
    m_anzahl += 1;

    m_statistik.addEntry(
        info.name.toStdString(),
        QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss").toStdString(),
        einzelpreis, 1);

    emit artikelAkzeptiert(m_aktuelleSimulierteId, info.name, einzelpreis, m_total, m_anzahl);
    m_aktuelleSimulierteId = "";
    // m_verarbeitung bleibt true bis FSM → BEREIT
}

// ---------------------------------------------------------------------------
void TransaktionController::bonAusgeben(Pfandautomat::Kern::BonTyp typ)
{
    speichereSession(typ);
    emit bonGespeichert(typ, m_total, m_anzahl);
    resetSession();
}

// ---------------------------------------------------------------------------
void TransaktionController::onTimerTick()
{
    if (m_sekunden >= MAX_SEKUNDEN) {
        m_timer->stop();
        emit fehler("Simulationszeit abgelaufen.");
        return;
    }
    m_sekunden++;
    emit timerTick(m_sekunden, MAX_SEKUNDEN);

    auto  z  = m_hauptController.getAktuellerZustand();
    auto &sc = m_hauptController.getSensorController();

    if (z == Pfandautomat::AutomatZustand::ERKENNUNG) {
        sc.getFlaschenLichtschranke().setzeZustand(
            Pfandautomat::LichtschrankeZustand::FREI);
    }

    if (z == Pfandautomat::AutomatZustand::TRANSPORT) {
        sc.getSortierLichtschranke().setzeZustand(
            Pfandautomat::LichtschrankeZustand::UNTERBROCHEN);
    }

    if (z == Pfandautomat::AutomatZustand::SORTIERUNG ||
        z == Pfandautomat::AutomatZustand::RECHNEND   ||
        z == Pfandautomat::AutomatZustand::LAGERUNG) {
        sc.getSortierLichtschranke().setzeZustand(
            Pfandautomat::LichtschrankeZustand::FREI);
    }
}

// ---------------------------------------------------------------------------
void TransaktionController::speichereSession(Pfandautomat::Kern::BonTyp typ)
{
    const auto format = (typ == Pfandautomat::Kern::BonTyp::PAPIER)
    ? Pfandautomat::Kern::ExportFormat::CSV
    : Pfandautomat::Kern::ExportFormat::PDF;
    m_statistik.save(format, "session");
}

void TransaktionController::exportStatistik(Pfandautomat::Kern::ExportFormat format,
                                            const QString &personalId,
                                            const QString &zielPfad) const
{
    m_statistik.save(format, personalId.toStdString(), zielPfad.toStdString());
}

void TransaktionController::resetIntern()
{
    m_aktiv       = false;
    m_pausiert    = false;
    m_verarbeitung = false;
    m_total       = 0.0;
    m_anzahl      = 0;
    m_sekunden    = 0;
    for (auto it = m_artikel.begin(); it != m_artikel.end(); ++it)
        it.value().anzahl = 0;
    m_statistik.clear();
    m_aktuelleSimulierteId = "";
}