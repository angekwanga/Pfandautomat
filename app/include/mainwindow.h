#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QTimer>
#include <QResizeEvent>
#include <QString>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <map>
#include "enumtype.h"
#include "transaktionservice.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QLabel;
class QPushButton;
class QPropertyAnimation;
class PersonalBereich;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onSessionGestartet();
    void onSessionGestoppt();
    void onSessionZurueckgesetzt();
    void onArtikelAkzeptiert(QString artikelId, QString name,
                             double einzelpreis, double gesamt, int anzahl);
    void onArtikelAbgelehnt(AblehnungsGrund grund);
    void onTimerTick(int sekunden, int maxSekunden);
    void onBonGespeichert(Pfandautomat::Kern::BonTyp typ, double betrag, int anzahl);
    void onZustandGeaendert(Pfandautomat::AutomatZustand zustand);
    void onFehler(QString meldung);

private:
    void setupIcons();
    void setupStatLabels();
    void setupButtonMappings();
    void connectController();

    void updateAutomatImage();
    void setState(Pfandautomat::AutomatZustand state, const QString &color);
    void setTransactionActive(bool active);
    void playInsertAnimation(const QPixmap &px, const QString &artikelId = "");
    void triggerSensorAnimation();
    void showBonScreen();

    Ui::MainWindow *ui;

    TransaktionController *m_controller = nullptr;
    PersonalBereich      *m_personal     = nullptr;

    QPixmap             m_automatPix;
    QLabel             *m_animLabel = nullptr;
    QPropertyAnimation *m_anim      = nullptr;

    QMap<QPushButton*, QString> m_btnArtikelId;

    std::map<Pfandautomat::Domain::MaterialTyp, QPushButton*> btnTyp;
};

#endif // MAINWINDOW_H
