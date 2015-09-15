#include "dialoglicense.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QFile>
#include <QCoreApplication>
#include <QPushButton>

DialogLicense::DialogLicense(QWidget *parent) : QDialog(parent)
{
    this->setWindowTitle(tr("License"));

    QVBoxLayout *layout = new QVBoxLayout();
    this->setLayout(layout);

    QTextEdit *txt_edit = new QTextEdit();
    layout->addWidget(txt_edit);
    txt_edit->setReadOnly(true);
    QFile file(QCoreApplication::applicationDirPath() + "/LICENSE");
    file.open(QIODevice::ReadOnly);
    txt_edit->setPlainText(file.readAll());
    file.close();


    QPushButton *btn_ok = new QPushButton(QIcon::fromTheme("dialog-ok"), tr("ok"));
    connect(btn_ok, SIGNAL(clicked(bool)), this, SLOT(slotButtonOk(bool)));

    QHBoxLayout *layout_btn = new QHBoxLayout();
    layout->addLayout(layout_btn);
    layout_btn->addStretch(1);
    layout_btn->addWidget(btn_ok, 0);
    layout_btn->addStretch(1);
}

void DialogLicense::slotButtonOk(bool)
{
    this->close();
}

