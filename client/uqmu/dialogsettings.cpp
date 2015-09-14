#include "dialogsettings.h"
#include <QSettings>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QPushButton>
#include <QFileDialog>
#include <QIcon>
#include "widgetpathselector.h"

DialogSettings::DialogSettings(QWidget *parent) : QWidget(parent)
{
    this->setWindowTitle("Settings");

    // read actual settings
    QSettings settings;
    setting_cachepath = QString(settings.value("Settings/CachePath").toString());

    QVBoxLayout *layout_main = new QVBoxLayout();

    // cache path edit
    layout_main->addWidget(this->createFrameCachePath());

    // buttons
    QPushButton *btnApply = new QPushButton(QIcon::fromTheme("dialog-ok"), tr("apply"));
    connect(btnApply, SIGNAL(clicked(bool)), this, SLOT(actionslotButtonApply(bool)));
    QPushButton *btnReject = new QPushButton(QIcon::fromTheme("dialog-cancel"), tr("reject"));
    connect(btnReject, SIGNAL(clicked(bool)), this, SLOT(actionslotButtonReject(bool)));

    // button layout
    QHBoxLayout *layout_btns = new QHBoxLayout();
    layout_main->addLayout(layout_btns);
    layout_btns->addStretch(1);
    layout_btns->addWidget(btnApply, 0);
    layout_btns->addWidget(btnReject, 0);

    this->setLayout(layout_main);
}

QGroupBox *DialogSettings::createFrameCachePath()
{
    QVBoxLayout *layout_main = new QVBoxLayout();

    // path selector
    WidgetPathSelector *path_select = new WidgetPathSelector(this->setting_cachepath);
    connect(path_select, SIGNAL(signalPathChanged(QString)), this, SLOT(actionslotCachePath(QString)));

    // description text
    layout_main->addWidget(path_select);
    QLabel *description = new QLabel(tr("The cache path is the place where aqmu stores the downloaded files.\nTo avoid downloading the same files multiple times do not remove the files in this directory."));
    description->setWordWrap(true);
    layout_main->addWidget(description);

    // create group box wih layout
    QGroupBox *groupbox = new QGroupBox(tr("Cache Path"));
    groupbox->setLayout(layout_main);
    return groupbox;
}

void DialogSettings::actionslotCachePath(QString path)
{
    this->setting_cachepath = path;
}

void DialogSettings::actionslotButtonApply(bool)
{
    QSettings settings;

    settings.setValue("Settings/CachePath", this->setting_cachepath);

    this->close();
}

void DialogSettings::actionslotButtonReject(bool)
{
    this->close();
}


