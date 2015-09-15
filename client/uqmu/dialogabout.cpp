#include "dialogabout.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

DialogABout::DialogABout(QWidget *parent) : QDialog(parent)
{
    this->setWindowTitle("About");

    QVBoxLayout *layout = new QVBoxLayout();
    this->setLayout(layout);

    QLabel *lblApp = new QLabel("uqmu - universal quick mod updater");
    layout->addWidget(lblApp);

    QLabel *lblVer = new QLabel(QString("Version: ") + QString(UQMU_VERSION));
    layout->addWidget(lblVer);

    QLabel *lblLnk = new QLabel("<a href=\"https://github.com/Oinosseus/uqmu\">github.com/Oinosseus/uqmu</a>");
    lblLnk->setTextInteractionFlags(Qt::TextBrowserInteraction);
    lblLnk->setOpenExternalLinks(true);
    layout->addWidget(lblLnk);

    QPushButton *btn_ok = new QPushButton(QIcon::fromTheme("dialog-ok"), tr("ok"));
    connect(btn_ok, SIGNAL(clicked(bool)), this, SLOT(slotButtonOk(bool)));

    QHBoxLayout *layout_btn = new QHBoxLayout();
    layout->addLayout(layout_btn);
    layout_btn->addStretch(1);
    layout_btn->addWidget(btn_ok, 0);
    layout_btn->addStretch(1);
}

void DialogABout::slotButtonOk(bool)
{
    this->close();
}

