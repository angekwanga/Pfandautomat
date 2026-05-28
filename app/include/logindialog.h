#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    QString getPersonalNr() const;   // nach accept() abrufbar
    QString getPassword()   const;

private slots:
    void onKeyPressed(const QString &key);  // Keypad-Taste gedrückt
    void onClear();                         // 'x' löscht letztes Zeichen

private:
    Ui::LoginDialog *ui;

    void connectKeypad();

    // Welches Feld ist aktiv? (PersonalNr. oder Passwort)
    enum ActiveField { FIELD_NR, FIELD_PW } m_activeField = FIELD_NR;

};

#endif // LOGINDIALOG_H
