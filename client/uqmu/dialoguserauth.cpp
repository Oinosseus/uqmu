#include "dialoguserauth.h"
#include <QVBoxLayout>
#include <QPushButton>

DialogUserAuth::DialogUserAuth(QWidget *parent) : QDialog(parent)
{
    login = "";
    password = "";

    this->setWindowTitle("Authorization");
    QVBoxLayout *layout = new QVBoxLayout();
    this->setLayout(layout);

    loginEdit = new QLineEdit("login");
    loginEdit->selectAll();
    loginEdit->setFocus();
    passwordEdit = new QLineEdit();
    passwordEdit->setEchoMode(QLineEdit::Password);

    QPushButton *btn = new QPushButton(tr("OK"));
    connect(btn, SIGNAL(clicked(bool)), this, SLOT(slotBtnClicked(bool)));

    layout->addWidget(loginEdit);
    layout->addWidget(passwordEdit);
    layout->addWidget(btn);
}
QString DialogUserAuth::getLogin()
{
    return login;
}
QString DialogUserAuth::getPassword()
{
    return password;
}

void DialogUserAuth::slotBtnClicked(bool)
{
    login = loginEdit->text();
    password = passwordEdit->text();
    this->close();
}



