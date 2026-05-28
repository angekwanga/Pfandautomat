#pragma once
#include <QFrame>
#include <functional>

class QTableWidget;

// ---------------------------------------------------------------------------
// DisplayMonitor
// Reine Anzeigekomponente – kennt KEINE anderen Fensterklassen.
//
// Fix 1: setMainWindow(this) entfernt.
//        Statt dessen emittiert DisplayMonitor Signals wenn Statistikdaten
//        gebraucht werden, und empfängt sie über einen Slot.
//
// Kopplung vorher: DisplayMonitor → MainWindow (zirkulär)
// Kopplung nachher: DisplayMonitor ↔ TransaktionController (über Signals)
// ---------------------------------------------------------------------------

namespace Ui { class DisplayMonitor; }

class DisplayMonitor : public QFrame
{
    Q_OBJECT
public:
    explicit DisplayMonitor(QWidget *parent = nullptr);
    ~DisplayMonitor();

    // Anzeige-Methoden (nur UI – keine Logik)
    void showMessage(const QString &message);
    void showReady();
    void showError(const QString &message);
    void showNormal();

    // Bon-Screen: Callbacks kommen vom TransaktionController über MainWindow
    void showBon(double total, int count,
                 std::function<void()> onPapier,
                 std::function<void()> onDigital);

    QTableWidget *lastThrowsTable() const;

signals:
    // Wird emittiert wenn Bon-Button gedrückt – MainWindow/Controller reagiert
    void bonPapierAngefordert();
    void bonDigitalAngefordert();

private:
    Ui::DisplayMonitor *ui;
};
