#pragma once
#include <QDialog>
#include <QPixmap>

namespace Ui { class QrDialog; }

class QrDialog : public QDialog
{
    Q_OBJECT
public:
    explicit QrDialog(double total, int count, QWidget *parent = nullptr);
    ~QrDialog();

private:
    QPixmap generateQr(int size);
    Ui::QrDialog *ui;
};