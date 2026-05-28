#include "mainwindow.h"
#include "ui/ui_mainwindow.h"
#include "displaymonitor.h"
#include "personal/personalbereich.h"

#include <QPixmap>
#include <QIcon>
#include <QMessageBox>
#include <QTime>
#include <QStyle>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QPropertyAnimation>
#include <QAbstractAnimation>
#include <QEasingCurve>
#include <QToolButton>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QPushButton>
#include <QMap>

static QPixmap loadPx(const QString &file, int w, int h) {
    QPixmap p(":/" + file);
    if (p.isNull()) return p;
    return p.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

// ---------------------------------------------------------------------------
// Konstruktor
// ---------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setMinimumSize(1100, 720);
    showMaximized();
    ui->centralwidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAcceptDrops(true);

    if (ui->automat_img) ui->automat_img->setAcceptDrops(false);

    m_controller = new TransaktionController(this);
    connectController();

    m_automatPix.load(":/automat4.png");
    ui->automat_img->setAlignment(Qt::AlignCenter);
    ui->label_5->setText("â™»");
    ui->label_6->setText("Pfandautomat");

    if (ui->invalid_btn)
        connect(ui->invalid_btn, &QPushButton::clicked,
                m_controller, &TransaktionController::ungueltigesGebinde);

    setupIcons();
    setupStatLabels();
    setupButtonMappings();

    m_personal = new PersonalBereich(this, this);
    m_personal->einrichten(ui->personalHostLayout, ui->personalNavList, ui->personalHeaderLabel);
    m_personal->setController(m_controller);
    connect(m_personal, &PersonalBereich::statusNachricht, this,
            [this](const QString &text, int ms) { statusBar()->showMessage(text, ms); });
    connect(m_personal, &PersonalBereich::hauptContainerLeerenAngefordert, this, [this] {
        ui->progressBar->setValue(0);
    });
    connect(m_controller, &TransaktionController::zustandGeaendert,
            m_personal, &PersonalBereich::onZustandGeaendert);

    ui->lcdNumber->setDigitCount(5);
    ui->lcdNumber->display("00:00");
    ui->progressBar->setValue(0);
    ui->progressBar->setRange(0, 300);

    auto *clockTimer = new QTimer(this);
    clockTimer->setInterval(1000);
    connect(clockTimer, &QTimer::timeout, this, [=]{
        if (ui->timeLabel) ui->timeLabel->setText(QTime::currentTime().toString("HH:mm:ss"));
    });
    clockTimer->start();
    if (ui->timeLabel) ui->timeLabel->setText(QTime::currentTime().toString("HH:mm:ss"));


    // Tabelle
    if (auto *table = ui->displayMonitor->lastThrowsTable()) {
        table->setHorizontalHeaderLabels({"#","Gebinde","Wert","Zeit"});
        table->verticalHeader()->setVisible(false);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->horizontalHeader()->setStretchLastSection(true);
        table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    }

    if (ui->toggleStatesBtn && ui->statesPanel) {
        connect(ui->toggleStatesBtn, &QToolButton::toggled, this, [=](bool on){
            ui->statesPanel->setVisible(on);
            ui->toggleStatesBtn->setText(on ? "ZustÃ¤nde verbergen" : "ZustÃ¤nde anzeigen");
        });
        ui->statesPanel->setVisible(false);
        ui->toggleStatesBtn->setText("ZustÃ¤nde anzeigen");
    }

    connect(ui->bon_btn,   &QPushButton::clicked, this, &MainWindow::showBonScreen);
    connect(ui->start_btn, &QPushButton::clicked, m_controller, &TransaktionController::starteSession);
    connect(ui->stop_btn,  &QPushButton::clicked, m_controller, &TransaktionController::stoppeSession);

    if (ui->hilfe_btn)
        connect(ui->hilfe_btn, &QPushButton::clicked, this, [=]{
            statusBar()->showMessage("Personal wurde gerufen.", 4000);
        });

    // Simulationszeit manuell zurÃ¼cksetzen ohne die laufende Session zu beenden.
    // Nur Sekunden und Fortschrittsbalken werden genullt â€“ BetrÃ¤ge bleiben erhalten.
    if (ui->resetTimerBtn)
        connect(ui->resetTimerBtn, &QPushButton::clicked, this, [=]{
            m_controller->resetTimer();
            ui->progressBar->setValue(0);
            ui->lcdNumber->display("00:00");
        });


    setTransactionActive(false);
}

// ---------------------------------------------------------------------------
// Controller verbinden
// ---------------------------------------------------------------------------
void MainWindow::connectController()
{
    connect(m_controller, &TransaktionController::sessionGestartet,
            this,         &MainWindow::onSessionGestartet);
    connect(m_controller, &TransaktionController::sessionGestoppt,
            this,         &MainWindow::onSessionGestoppt);
    connect(m_controller, &TransaktionController::sessionZurueckgesetzt,
            this,         &MainWindow::onSessionZurueckgesetzt);
    connect(m_controller, &TransaktionController::artikelAkzeptiert,
            this,         &MainWindow::onArtikelAkzeptiert);
    connect(m_controller, &TransaktionController::artikelAbgelehnt,
            this,         &MainWindow::onArtikelAbgelehnt);
    connect(m_controller, &TransaktionController::timerTick,
            this,         &MainWindow::onTimerTick);
    connect(m_controller, &TransaktionController::bonGespeichert,
            this,         &MainWindow::onBonGespeichert);
    connect(m_controller, &TransaktionController::zustandGeaendert,
            this,         &MainWindow::onZustandGeaendert);
    connect(m_controller, &TransaktionController::fehler,
            this,         &MainWindow::onFehler);
    // Button-Blocking: Einwurf-Buttons wÃ¤hrend laufender Verarbeitung sperren.
    // Verhindert FSM-Korruption durch Doppeleinwurf.
    connect(m_controller, &TransaktionController::verarbeitungAktiv,
            this, [this](bool aktiv) {
                if (ui->plastic_btn) ui->plastic_btn->setEnabled(!aktiv && m_controller->istAktiv());
                if (ui->glas_btn)    ui->glas_btn->setEnabled(!aktiv && m_controller->istAktiv());
                if (ui->can_btn)     ui->can_btn->setEnabled(!aktiv && m_controller->istAktiv());
                if (ui->crate_btn)   ui->crate_btn->setEnabled(!aktiv && m_controller->istAktiv());
                if (ui->invalid_btn) ui->invalid_btn->setEnabled(!aktiv && m_controller->istAktiv());
            });

}

// ---------------------------------------------------------------------------
// Button-Mapping
// ---------------------------------------------------------------------------
void MainWindow::setupButtonMappings()
{
    auto reg = [&](QPushButton *btn, const QString &id, const ArtikelInfo &info) {
        m_controller->registriereArtikel(id, info);
        m_btnArtikelId[btn] = id;

        connect(btn, &QPushButton::clicked, this, [this, btn, id]{
            if (m_controller && m_controller->istAktiv()) {
                QPixmap iconPixmap = btn->icon().pixmap(46, 46);
                playInsertAnimation(iconPixmap, id);
            }
        });
    };

    reg(ui->plastic_btn, "PET",    {"Plastikflasche", 0.25, Pfandautomat::Domain::MaterialTyp::PET});
    reg(ui->glas_btn,    "GLAS",   {"Glasflasche",    0.08, Pfandautomat::Domain::MaterialTyp::GLAS});
    reg(ui->can_btn,     "ALLU",   {"Pfanddose",      0.25, Pfandautomat::Domain::MaterialTyp::ALLU});
    reg(ui->crate_btn,   "KASTEN", {"GetrÃ¤nkekasten", 1.50, Pfandautomat::Domain::MaterialTyp::HDPE});
}

// ---------------------------------------------------------------------------
// Slots: Controller â†’ GUI
// ---------------------------------------------------------------------------
void MainWindow::onSessionGestartet()
{
    setTransactionActive(true);
    if (ui->statusText) ui->statusText->setText("Bereit");
    statusBar()->showMessage("Neue RÃ¼ckgabe gestartet â€“ Pfandartikel einlegen â€¦", 4000);
    ui->displayMonitor->showReady();
}

void MainWindow::onSessionGestoppt()
{
    setTransactionActive(false);
    if (ui->statusText) ui->statusText->setText("Gestoppt");
    statusBar()->showMessage("RÃ¼ckgabe gestoppt. Start setzt die Session fort.", 4000);
}

void MainWindow::onSessionZurueckgesetzt()
{
    setTransactionActive(false);
    setupStatLabels();
    if (auto *table = ui->displayMonitor->lastThrowsTable()) table->setRowCount(0);
    if (ui->progressBar)     ui->progressBar->setValue(0);
    if (ui->lcdNumber)       ui->lcdNumber->display("00:00");
    if (ui->statusText)      ui->statusText->setText("Bereit");
    ui->displayMonitor->showReady();
}

void MainWindow::onArtikelAkzeptiert(QString artikelId, QString name,
                                     double einzelpreis, double gesamt, int anzahl)
{

    Q_UNUSED(artikelId)

    ui->displayMonitor->showMessage(
        QString("%1 akzeptiert â€¢ +%2 â‚¬ â€¢ Gesamt: %3 â‚¬")
            .arg(name).arg(einzelpreis, 0,'f',2).arg(gesamt, 0,'f',2));

    if (auto *table = ui->displayMonitor->lastThrowsTable()) {
        if (table->rowCount() >= 8)
            table->removeRow(0);
        const int r = table->rowCount();
        table->insertRow(r);
        table->setItem(r, 0, new QTableWidgetItem(QString::number(anzahl)));
        table->setItem(r, 1, new QTableWidgetItem(name));
        table->setItem(r, 2, new QTableWidgetItem(
            QString("%1 â‚¬").arg(einzelpreis, 0,'f',2)));
        table->setItem(r, 3, new QTableWidgetItem(
            QTime::currentTime().toString("HH:mm:ss")));
        table->scrollToBottom();
    }

    // FIX: triggerSensorAnimation() ENTFERNT.
    // Diese Funktion hat die Zustands-LEDs mit eigenen QTimer::singleShot-Ketten
    // unabhÃ¤ngig von der echten FSM Ã¼berschrieben und dabei nach 1500ms st7
    // (RUECKWEISUNG) grÃ¼n gefÃ¤rbt â€“ obwohl die FSM lÃ¤ngst in einem anderen
    // Zustand war. Das fÃ¼hrte zum gleichzeitigen Leuchten von BEREIT + RUECKWEISUNG.
    // Die Zustandsanzeige wird ausschliesslich Ã¼ber onZustandGeaendert() gesteuert,
    // das direkt vom HauptController-Signal kommt.

    if (m_personal)
        m_personal->onArtikelAkzeptiert(anzahl);

    statusBar()->showMessage(
        QString(">  %1  â€¢  +%2 â‚¬  |  Gesamt: %3 â‚¬")
            .arg(name).arg(einzelpreis,0,'f',2).arg(gesamt,0,'f',2), 4000);
}

void MainWindow::onArtikelAbgelehnt(AblehnungsGrund grund)
{
    ui->displayMonitor->showError(grund.problem + "\nGebinde wird zurÃ¼ckgegeben.");
    if (ui->statusText) ui->statusText->setText(grund.statusText);
    statusBar()->showMessage("UngÃ¼ltiges Gebinde: " + grund.problem + " â€“ kein Pfand.", 4000);

    QTimer::singleShot(3000, this, [=]{
        if (!m_controller->istAktiv()) return;
        if (ui->statusText) ui->statusText->setText("Bereit");
        setState(Pfandautomat::AutomatZustand::BEREIT, "#22c55e");
        ui->displayMonitor->showReady();
    });
}

void MainWindow::onTimerTick(int sekunden, int maxSekunden)
{
    auto *anim = new QPropertyAnimation(ui->progressBar, "value", ui->progressBar);
    anim->setDuration(260);
    anim->setStartValue(ui->progressBar->value());
    anim->setEndValue(sekunden);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
    Q_UNUSED(maxSekunden)
    ui->lcdNumber->display(
        QString("%1:%2").arg(sekunden/60,2,10,QChar('0')).arg(sekunden%60,2,10,QChar('0')));
}

void MainWindow::onBonGespeichert(Pfandautomat::Kern::BonTyp typ, double betrag, int anzahl)
{
    Q_UNUSED(typ) Q_UNUSED(betrag) Q_UNUSED(anzahl)
}

void MainWindow::onZustandGeaendert(Pfandautomat::AutomatZustand zustand)
{
    // Einzige Stelle, die die Zustands-LEDs setzt.
    // Farben passend zum Zustand:
    struct { Pfandautomat::AutomatZustand z; const char* farbe; } karte[] = {
                 { Pfandautomat::AutomatZustand::BEREIT,       "#22c55e" },
                 { Pfandautomat::AutomatZustand::ERKENNUNG,    "#ef4444" },
                 { Pfandautomat::AutomatZustand::TRANSPORT,    "#f59e0b" },
                 { Pfandautomat::AutomatZustand::SORTIERUNG,   "#3b82f6" },
                 { Pfandautomat::AutomatZustand::RECHNEND,     "#8b5cf6" },
                 { Pfandautomat::AutomatZustand::LAGERUNG,     "#22c55e" },
                 { Pfandautomat::AutomatZustand::RUECKWEISUNG, "#f97316" },
                 { Pfandautomat::AutomatZustand::AUSGABE,      "#06b6d4" },
                 { Pfandautomat::AutomatZustand::DONE,         "#22c55e" },
                 { Pfandautomat::AutomatZustand::BLOCKIERT,    "#ef4444" },
                 { Pfandautomat::AutomatZustand::WARTUNG,      "#f59e0b" },
                 };
    for (const auto &k : karte) {
        if (k.z == zustand) { setState(zustand, k.farbe); return; }
    }
    setState(zustand, "#22c55e");
}

void MainWindow::onFehler(QString meldung)
{
    statusBar()->showMessage(meldung, 4000);
    if (ui->displayMonitor) ui->displayMonitor->showError(meldung);
}

// ---------------------------------------------------------------------------
// Bon-Screen
// ---------------------------------------------------------------------------
void MainWindow::showBonScreen()
{
    if (!m_controller->istAktiv() || m_controller->gesamtBetrag() <= 0) return;

    ui->displayMonitor->showBon(
        m_controller->gesamtBetrag(),
        m_controller->artikelAnzahl(),
        [=]{
            setEnabled(false);
            QTimer::singleShot(100, this, [=]{
                m_controller->bonAusgeben(Pfandautomat::Kern::BonTyp::PAPIER);
                ui->displayMonitor->showNormal();
                setEnabled(true);
            });
        },
        [=]{
            setEnabled(false);
            QTimer::singleShot(100, this, [=]{
                m_controller->bonAusgeben(Pfandautomat::Kern::BonTyp::DIGITAL);
                ui->displayMonitor->showNormal();
                setEnabled(true);
            });
        }
        );
}

// ---------------------------------------------------------------------------
// Reine UI-Hilfsfunktionen
// ---------------------------------------------------------------------------
void MainWindow::setTransactionActive(bool active)
{
    if (ui->start_btn)   ui->start_btn->setEnabled(!active);
    if (ui->plastic_btn) ui->plastic_btn->setEnabled(active);
    if (ui->glas_btn)    ui->glas_btn->setEnabled(active);
    if (ui->can_btn)     ui->can_btn->setEnabled(active);
    if (ui->crate_btn)   ui->crate_btn->setEnabled(active);
    if (ui->bon_btn)     ui->bon_btn->setEnabled(active);
    if (ui->stop_btn)    ui->stop_btn->setEnabled(active);
}

// FIX: triggerSensorAnimation() vollstÃ¤ndig entfernt.
// Sie hat LEDs mit eigenen Timern gesetzt und damit onZustandGeaendert() sabotiert.

void MainWindow::setState(Pfandautomat::AutomatZustand state, const QString &color)
{
    const QString off = "background-color: #2a3148; border-radius: 3px;";
    for (auto *l : {ui->st1,ui->st2,ui->st3,ui->st4,ui->st5,
                    ui->st6,ui->st7,ui->st8,ui->st9,ui->st10,ui->st11})
        l->setStyleSheet(off);
    const QString on = QString("background-color: %1; border-radius: 3px;").arg(color);
    switch (state) {
    case Pfandautomat::AutomatZustand::BEREIT:       ui->st1->setStyleSheet(on); break;
    case Pfandautomat::AutomatZustand::ERKENNUNG:    ui->st2->setStyleSheet(on); break;
    case Pfandautomat::AutomatZustand::TRANSPORT:    ui->st3->setStyleSheet(on); break;
    case Pfandautomat::AutomatZustand::SORTIERUNG:   ui->st4->setStyleSheet(on); break;
    case Pfandautomat::AutomatZustand::RECHNEND:     ui->st5->setStyleSheet(on); break;
    case Pfandautomat::AutomatZustand::LAGERUNG:     ui->st6->setStyleSheet(on); break;
    case Pfandautomat::AutomatZustand::RUECKWEISUNG: ui->st7->setStyleSheet(on); break;
    case Pfandautomat::AutomatZustand::AUSGABE:      ui->st8->setStyleSheet(on); break;
    case Pfandautomat::AutomatZustand::DONE:         ui->st9->setStyleSheet(on); break;
    case Pfandautomat::AutomatZustand::BLOCKIERT:    ui->st10->setStyleSheet(on); break;
    case Pfandautomat::AutomatZustand::WARTUNG:      ui->st11->setStyleSheet(on); break;
    default:                                         ui->st1->setStyleSheet(on); break;
    }
}

void MainWindow::setupStatLabels()
{
    const QString off = "background-color: #2a3148; border-radius: 3px;";
    for (auto *l : {ui->st1,ui->st2,ui->st3,ui->st4,ui->st5,
                    ui->st6,ui->st7,ui->st8,ui->st9,ui->st10,ui->st11})
        l->setStyleSheet(off);
    ui->st1->setStyleSheet("background-color: #22c55e; border-radius: 3px;");
}

void MainWindow::setupIcons()
{
    const QSize sz(44,44);
    auto setBtn = [&](QPushButton *btn, const QString &file) {
        QPixmap p = loadPx(file, sz.width(), sz.height());
        if (!p.isNull()) { btn->setIcon(QIcon(p)); btn->setIconSize(sz); }
        btn->setMinimumSize(56,56);
        btn->setMaximumWidth(64);
    };
    setBtn(ui->plastic_btn, "bottle_plastik.svg");
    setBtn(ui->glas_btn,    "beer.png");
    setBtn(ui->can_btn,     "can_red.png");
    setBtn(ui->crate_btn,   "kisten.png");
    for (auto *b : {ui->plastic_btn,ui->glas_btn,ui->can_btn,ui->crate_btn,ui->invalid_btn}) {
        if (!b) continue;
        b->setCursor(Qt::OpenHandCursor);
        b->installEventFilter(this);
    }
}

// ---------------------------------------------------------------------------
// Drag & Drop
// ---------------------------------------------------------------------------
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{ if (event->mimeData()->hasText()) event->acceptProposedAction(); }

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{ if (event->mimeData()->hasText()) event->acceptProposedAction(); }

void MainWindow::dropEvent(QDropEvent *event)
{
    const QString item = event->mimeData()->text();
    const QMap<QString,QString> nameToId = {
        {"plastic_btn","PET"},{"glas_btn","GLAS"},
        {"can_btn","ALLU"},  {"crate_btn","KASTEN"}
    };
    if (nameToId.contains(item))
        m_controller->artikelEinwerfen(nameToId[item]);
    else if (item == "invalid_btn")
        m_controller->ungueltigesGebinde();
    else {
        ui->displayMonitor->showError("Unbekannter Artikel");
        statusBar()->showMessage("Unbekannter Artikel â€“ bitte Scanner verwenden.", 2000);
    }
    event->acceptProposedAction();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    auto *btn = qobject_cast<QPushButton*>(obj);
    if (!btn) return QMainWindow::eventFilter(obj, event);
    if (event->type() == QEvent::MouseMove) {
        auto *me = static_cast<QMouseEvent*>(event);
        if (me->buttons() & Qt::LeftButton) {
            QDrag *drag = new QDrag(btn);
            QMimeData *mime = new QMimeData;
            mime->setText(btn->objectName());
            drag->setMimeData(mime);
            if (!btn->icon().isNull()) drag->setPixmap(btn->icon().pixmap(48,48));
            drag->exec(Qt::CopyAction);
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

// ---------------------------------------------------------------------------
// Animation
// ---------------------------------------------------------------------------
void MainWindow::playInsertAnimation(const QPixmap &px, const QString &artikelId)
{
    if (!ui->automat_img) return;

    if (!m_animLabel) {
        m_animLabel = new QLabel(ui->cardMain);
        m_animLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
        m_animLabel->setScaledContents(true);
    }

    m_animLabel->raise();
    m_animLabel->setPixmap(px);
    m_animLabel->setFixedSize(46, 46);

    const QPoint start = QPoint(40, ui->cardMain->height() / 2);
    const QPoint end   = ui->automat_img->mapTo(ui->cardMain, QPoint(20, ui->automat_img->height() / 2));

    m_animLabel->move(start);
    m_animLabel->show();

    if (m_anim) {
        m_anim->stop();
        m_anim->disconnect();
        m_anim->deleteLater();
    }

    m_anim = new QPropertyAnimation(m_animLabel, "pos", this);
    m_anim->setDuration(650);
    m_anim->setStartValue(start);
    m_anim->setEndValue(end);
    m_anim->setEasingCurve(QEasingCurve::InOutCubic);

    connect(m_anim, &QPropertyAnimation::finished, this, [this, artikelId]() {
        if (m_animLabel) m_animLabel->hide();
        if (m_controller && !artikelId.isEmpty())
            m_controller->HardwareEinwurfTriggern(artikelId);
    });

    m_anim->start();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{ QMainWindow::resizeEvent(event); updateAutomatImage(); }

void MainWindow::showEvent(QShowEvent *event)
{ QMainWindow::showEvent(event); updateAutomatImage(); }

void MainWindow::updateAutomatImage()
{
    if (m_automatPix.isNull() || !ui->automat_img) return;
    QSize sz = ui->automat_img->size();
    if (sz.width() < 10 || sz.height() < 10) sz = QSize(260,420);
    ui->automat_img->setPixmap(m_automatPix.scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

MainWindow::~MainWindow() { delete ui; }
