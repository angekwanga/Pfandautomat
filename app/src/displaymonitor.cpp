#include "displaymonitor.h"
#include "ui/ui_displaymonitor.h"
#include "qrdialog.h"
#include <QTime>
#include <QRegularExpression>
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>

DisplayMonitor::DisplayMonitor(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::DisplayMonitor)
{
    ui->setupUi(this);

    setStyleSheet(
        "QFrame#DisplayMonitor { background: transparent; border: none; }"
        "QScrollArea#scrollArea { background: transparent; border: none; }"
        "QWidget#scrollContent { background: transparent; }"
        "QLabel { color: #0b1b4f; }"
        "QFrame#screen, QFrame#cardLastThrows, QFrame#cardHelp, QFrame#bonPanel {"
        "  background: #ffffff;"
        "  border: 1px solid #d8dee9;"
        "  border-radius: 10px;"
        "}"
        "QFrame#line { color: #d8dee9; background: #d8dee9; }"
        "QLabel#titleLabel {"
        "  color: #0f3c91; font-size: 13px; font-weight: 800; letter-spacing: 1px; }"
        "QLabel#timeLabel { color: #6b7280; font-size: 13px; }"
        "QLabel#iconLabel { color: #2563eb; font-size: 36px; font-weight: 800; }"
        "QLabel#messageLabel { color: #111827; font-size: 15px; font-weight: 700; }"
        "QLabel#valueLabel { color: #111827; font-size: 18px; font-weight: 800; }"
        "QLabel#totalTitleLabel { color: #4b5563; font-size: 14px; font-weight: 700; }"
        "QLabel#totalLabel { color: #111827; font-size: 20px; font-weight: 800; }"
        "QLabel#lastThrowsTitle, QLabel#helpTitle {"
        "  color: #111827; font-size: 14px; font-weight: 700; }"
        "QLabel#helpTextLabel { color: #4b5563; font-size: 12px; line-height: 1.4; }"
        "QLabel#hintLabel { color: #dc2626; font-size: 12px; font-weight: 600; }"
        "QTableWidget {"
        "  background: #ffffff;"
        "  border: 1px solid #e5e7eb;"
        "  border-radius: 6px;"
        "  font-size: 12px;"
        "  gridline-color: #edf0f5;"
        "}"
        "QHeaderView::section {"
        "  background: #f3f5f8;"
        "  color: #374151;"
        "  border: none;"
        "  border-bottom: 1px solid #e5e7eb;"
        "  padding: 6px 4px;"
        "  font-size: 11px;"
        "  font-weight: 700;"
        "}"
        "QPushButton#bonPapierBtn {"
        "  background: #2563eb; color: #fff; border: none; border-radius: 8px;"
        "  padding: 8px; font-weight: 700; font-size: 13px; }"
        "QPushButton#bonDigitalBtn {"
        "  background: #16a34a; color: #fff; border: none; border-radius: 8px;"
        "  padding: 8px; font-weight: 700; font-size: 13px; }"
        "QPushButton#bonAbbruchBtn {"
        "  background: #f3f4f6; color: #374151; border: 1px solid #d1d5db;"
        "  border-radius: 8px; padding: 6px; font-size: 12px; }"
    );

    if (auto *table = ui->lastThrowsTable) {
        table->verticalHeader()->setDefaultSectionSize(28);
        table->horizontalHeader()->setMinimumSectionSize(36);
        table->setShowGrid(true);
        table->setAlternatingRowColors(true);
    }

    if (auto *layout = ui->contentLayout) {
        layout->setStretch(layout->indexOf(ui->screen), 0);
        layout->setStretch(layout->indexOf(ui->cardLastThrows), 1);
        layout->setStretch(layout->indexOf(ui->cardHelp), 0);
        layout->setStretch(layout->indexOf(ui->hintLabel), 0);
    }

    ui->bonPanel->setVisible(false);
    ui->timeLabel->setText(QTime::currentTime().toString("HH:mm:ss"));
    showReady();
}

DisplayMonitor::~DisplayMonitor()
{
    delete ui;
}

void DisplayMonitor::showMessage(const QString &message)
{
    ui->screen->setVisible(true);
    ui->timeLabel->setText(QTime::currentTime().toString("HH:mm:ss"));
    ui->iconLabel->setText("✓");
    ui->iconLabel->setStyleSheet("QLabel { color: #16a34a; font-size: 36px; font-weight: 800; }");

    QString text  = message;
    QString value = "+0.00 €";
    QString total;

    const QRegularExpression plusRe("(\\+\\d+[\\.,]\\d{2}\\s*€)");
    const auto plusMatch = plusRe.match(message);
    if (plusMatch.hasMatch()) value = plusMatch.captured(1);

    const QRegularExpression totalRe("Gesamt:\\s*(\\d+[\\.,]\\d{2}\\s*€)");
    const auto totalMatch = totalRe.match(message);
    if (totalMatch.hasMatch()) total = totalMatch.captured(1);

    text.remove(plusRe);
    text.remove(totalRe);
    text.remove("•");
    text = text.simplified();
    if (text.isEmpty()) text = message;

    ui->messageLabel->setStyleSheet("QLabel { color: #111827; font-size: 15px; font-weight: 700; }");
    ui->messageLabel->setText(text);
    ui->valueLabel->setText(value);
    if (!total.isEmpty()) ui->totalLabel->setText(total);
}

void DisplayMonitor::showReady()
{
    ui->screen->setVisible(true);
    ui->bonPanel->setVisible(false);
    ui->timeLabel->setText(QTime::currentTime().toString("HH:mm:ss"));
    ui->iconLabel->setText("♻");
    ui->iconLabel->setStyleSheet("QLabel { color: #2563eb; font-size: 36px; font-weight: 800; }");
    ui->messageLabel->setStyleSheet("QLabel { color: #111827; font-size: 15px; font-weight: 700; }");
    ui->messageLabel->setText("Bitte Gebinde einwerfen");
    ui->valueLabel->setText("+0.00 €");
    ui->totalLabel->setText("0,00 €");
}

void DisplayMonitor::showError(const QString &message)
{
    ui->screen->setVisible(true);
    ui->bonPanel->setVisible(false);
    ui->timeLabel->setText(QTime::currentTime().toString("HH:mm:ss"));
    ui->iconLabel->setText("✗");
    ui->iconLabel->setStyleSheet("QLabel { color: #dc2626; font-size: 36px; font-weight: 800; }");
    ui->messageLabel->setStyleSheet("QLabel { color: #dc2626; font-size: 14px; font-weight: 700; }");
    ui->messageLabel->setText(message);
    ui->valueLabel->setText("");
    ui->totalLabel->setText("");
}

void DisplayMonitor::showBon(double total, int count,
                             std::function<void()> onPapier,
                             std::function<void()> onDigital)
{
    ui->screen->setVisible(false);
    ui->bonPanel->setVisible(true);
    ui->timeLabel->setText(QTime::currentTime().toString("HH:mm:ss"));
    ui->bonTotalLabel->setText(QString("Gesamtbetrag: %1 €").arg(total, 0, 'f', 2));
    ui->bonCountLabel->setText(QString("Anzahl Artikel: %1").arg(count));

    disconnect(ui->bonPapierBtn,  nullptr, nullptr, nullptr);
    disconnect(ui->bonDigitalBtn, nullptr, nullptr, nullptr);
    disconnect(ui->bonAbbruchBtn, nullptr, nullptr, nullptr);

    connect(ui->bonPapierBtn, &QPushButton::clicked, this, [=]{
        QrDialog dlg(total, count, this);
        dlg.exec();
        onPapier();
    });

    connect(ui->bonDigitalBtn, &QPushButton::clicked, this, [=]{
        QrDialog dlg(total, count, this);
        dlg.exec();
        onDigital();
    });

    connect(ui->bonAbbruchBtn, &QPushButton::clicked, this, [=]{
        showNormal();
    });
}

void DisplayMonitor::showNormal()
{
    ui->bonPanel->setVisible(false);
    showReady();
}

QTableWidget *DisplayMonitor::lastThrowsTable() const { return ui->lastThrowsTable; }
