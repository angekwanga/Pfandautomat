#include "logindialog.h"
#include "ui/ui_logindialog.h"
#include <QPushButton>

// Simulation: jede Eingabe ist gültig (keine echte Auth)

// -------------------------------------------------------------
LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    setWindowTitle("Mitarbeiter-Login");
    setFixedSize(400, 300);   // Größe aus .ui beibehalten

    // -- LineEdits vorbereiten --------------------------------
    // Platzhaltertext statt fester Text
    ui->personr_lineEdit->clear();
    ui->personr_lineEdit->setPlaceholderText("PersonalNr. eingeben");

    ui->lineEdit_->clear();
    ui->lineEdit_->setPlaceholderText("Passwort eingeben");
    ui->lineEdit_->setEchoMode(QLineEdit::Password);  // Passwort verstecken

    // Klick auf LineEdit -> aktives Feld wechseln
    connect(ui->personr_lineEdit, &QLineEdit::selectionChanged, this, [=]{
        m_activeField = FIELD_NR;
    });
    connect(ui->lineEdit_, &QLineEdit::selectionChanged, this, [=]{
        m_activeField = FIELD_PW;
    });

    // -- Keypad verbinden -------------------------------------
    connectKeypad();

    // -- OK-Button: Simulation-Login --------------------------
    // Jede Eingabe ist gültig; wir verhindern nur komplett leere Logins.
    disconnect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [=]{
        const bool hasNr = !ui->personr_lineEdit->text().trimmed().isEmpty();
        const bool hasPw = !ui->lineEdit_->text().trimmed().isEmpty();
        if (hasNr && hasPw) {
            accept();   // Fenster schließen, Rückgabewert = Accepted
            return;
        }

        ui->personr_lineEdit->setStyleSheet("border: 1px solid #e53e3e;");
        ui->lineEdit_->setStyleSheet("border: 1px solid #e53e3e;");
        ui->personr_lineEdit->setPlaceholderText("Bitte PersonalNr. eingeben");
        ui->lineEdit_->setPlaceholderText("Bitte Passwort eingeben");
        m_activeField = hasNr ? FIELD_PW : FIELD_NR;
    });
}

// -------------------------------------------------------------
//  Keypad: alle Zifferntasten + # + x verbinden
// -------------------------------------------------------------
void LoginDialog::connectKeypad()
{
    // Zifferntasten 0–9
    const QList<QPair<QPushButton*, QString>> numKeys = {
                                                          {ui->pushButton0, "0"}, {ui->pushButton1, "1"},
                                                          {ui->pushButton2, "2"}, {ui->pushButton3, "3"},
                                                          {ui->pushButton4, "4"}, {ui->pushButton5, "5"},
                                                          {ui->pushButton6, "6"}, {ui->pushButton7, "7"},
                                                          {ui->pushButton8, "8"}, {ui->pushButton9, "9"},
                                                          {ui->pushButtonHash, "#"},
                                                          };

    for (auto &[btn, key] : numKeys) {
        connect(btn, &QPushButton::clicked, this, [=]{
            onKeyPressed(key);
        });
    }

    // x = Löschen (Backspace)
    connect(ui->pushButtonX, &QPushButton::clicked, this, &LoginDialog::onClear);
}

// -------------------------------------------------------------
//  Taste gedrückt → aktives Feld befüllen
// -------------------------------------------------------------
void LoginDialog::onKeyPressed(const QString &key)
{
    // Roten Rahmen zurücksetzen wenn Benutzer neu tippt
    ui->personr_lineEdit->setStyleSheet("");
    ui->lineEdit_->setStyleSheet("");

    QLineEdit *field = (m_activeField == FIELD_NR)
                           ? ui->personr_lineEdit
                           : ui->lineEdit_;
    field->setFocus();
    field->insert(key);
}

// -------------------------------------------------------------
//  x → letztes Zeichen löschen
// -------------------------------------------------------------
void LoginDialog::onClear()
{
    QLineEdit *field = (m_activeField == FIELD_NR)
    ? ui->personr_lineEdit
    : ui->lineEdit_;
    QString txt = field->text();
    if (!txt.isEmpty())
        field->setText(txt.left(txt.size() - 1));
    field->setFocus();
}

// -------------------------------------------------------------
//  Getter – nach accept() aufrufbar
// -------------------------------------------------------------
QString LoginDialog::getPersonalNr() const { return ui->personr_lineEdit->text(); }
QString LoginDialog::getPassword()   const { return ui->lineEdit_->text(); }

// -------------------------------------------------------------
LoginDialog::~LoginDialog() { delete ui; }