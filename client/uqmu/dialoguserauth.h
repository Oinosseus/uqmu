#ifndef DIALOGUSERAUTH_H
#define DIALOGUSERAUTH_H

#include <QDialog>
#include <QWidget>
#include <QLineEdit>

class DialogUserAuth : public QDialog
{
    Q_OBJECT

public:
    DialogUserAuth(QWidget *parent = 0);
    QString getLogin();
    QString getPassword();

public slots:

private:
    QString login;
    QString password;
    QLineEdit *loginEdit;
    QLineEdit *passwordEdit;

private slots:
    void slotBtnClicked(bool);

};

#endif // DIALOGUSERAUTH_H
