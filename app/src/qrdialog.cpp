#include "qrdialog.h"
#include "ui/ui_qrdialog.h"

#include <QPainter>
#include <QPixmap>
#include <QRandomGenerator>

QrDialog::QrDialog(double total, int count, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::QrDialog)
{
    ui->setupUi(this);
    setFixedSize(320, 480);

    setStyleSheet(
        "QDialog { background: #ffffff; }"
        "QLabel#titleLabel { font-size: 18px; font-weight: 800; color: #0b1b4f; }"
        "QLabel#totalLabel { font-size: 16px; font-weight: 800; color: #2563eb; }"
        "QLabel#countLabel { font-size: 14px; color: #374151; }"
        "QLabel#hintLabel  { font-size: 12px; color: #6b7280; }"
        "QPushButton#closeBtn {"
        "  background: #2563eb; color: #ffffff;"
        "  border-radius: 10px; padding: 10px;"
        "  font-weight: 700; font-size: 14px;"
        "}"
        "QPushButton#closeBtn:hover { background: #1d4ed8; }"
    );

    ui->totalLabel->setText(QString("Gesamtbetrag: %1 €").arg(total, 0, 'f', 2));
    ui->countLabel->setText(QString("Anzahl Artikel: %1").arg(count));
    ui->qrLabel->setPixmap(generateQr(240));

    connect(ui->closeBtn, &QPushButton::clicked, this, &QDialog::accept);
}

QrDialog::~QrDialog()
{
    delete ui;
}

QPixmap QrDialog::generateQr(int size)
{
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, false);

    const int modules = 21; // simulate 21x21 QR grid
    const int cellSize = size / modules;
    const int offset = (size - modules * cellSize) / 2;

    // Seed with total for consistent pattern per transaction
    QRandomGenerator rng(42);

    // Draw random black/white cells (simulated QR)
    for (int row = 0; row < modules; ++row) {
        for (int col = 0; col < modules; ++col) {
            bool black = rng.bounded(2) == 0;

            // Force finder patterns (3 corners)
            bool inTopLeft     = row < 7 && col < 7;
            bool inTopRight    = row < 7 && col >= modules - 7;
            bool inBottomLeft  = row >= modules - 7 && col < 7;

            if (inTopLeft || inTopRight || inBottomLeft) {
                // Draw finder pattern border
                bool onBorder = (row == 0 || row == 6 || col == 0 || col == 6) ||
                                (row >= 2 && row <= 4 && col >= 2 && col <= 4);
                if (inTopRight)    onBorder = (row == 0 || row == 6 || col == modules-7 || col == modules-1) ||
                                              (row >= 2 && row <= 4 && col >= modules-5 && col <= modules-3);
                if (inBottomLeft)  onBorder = (row == modules-7 || row == modules-1 || col == 0 || col == 6) ||
                                              (row >= modules-5 && row <= modules-3 && col >= 2 && col <= 4);
                black = onBorder;
            }

            if (black) {
                painter.fillRect(
                    offset + col * cellSize,
                    offset + row * cellSize,
                    cellSize, cellSize,
                    Qt::black);
            }
        }
    }

    painter.end();
    return pixmap;
}
