#include "mainwindow.h"
#include <QSettings>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QString>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>

#include "dialogabout.h"
#include "dialogsettings.h"
#include "widgettargetprocessor.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    // restore window settings
    QSettings settings;
    if (settings.contains("MainWindow/size"))
        this->resize(settings.value("MainWindow/size").toSize());
    if (settings.contains("Mainwindow/pos"))
        this->move(settings.value("Mainwindow/pos").toPoint());

    // create file menu
    QMenu *menu_file = new QMenu(tr("File"));
    this->menuBar()->addMenu(menu_file);

    QAction *action_opentarget = new QAction(tr("open target"), this);
    connect(action_opentarget, SIGNAL(triggered(bool)), this, SLOT(actionslotOpen(bool)));
    menu_file->addAction(action_opentarget);

    QAction *action_settings = new QAction(tr("settings"), this);
    connect(action_settings, SIGNAL(triggered(bool)), this, SLOT(actionslotSettings(bool)));
    menu_file->addAction(action_settings);

    // create help menu
    QMenu *menu_help = new QMenu(tr("help"));
    this->menuBar()->addMenu(menu_help);

    QAction *action_about = new QAction(tr("about"), this);
    connect(action_about, SIGNAL(triggered(bool)), this, SLOT(actionslotAbout(bool)));
    menu_help->addAction(action_about);
}

MainWindow::~MainWindow()
{
    // save window settings
    QSettings settings;
    settings.setValue("MainWindow/size", this->size());
    settings.setValue("Mainwindow/pos", this->pos());
}

void MainWindow::actionslotAbout(bool)
{
    DialogABout *dialog = new DialogABout();
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setAttribute(Qt::WA_ShowModal);
    dialog->show();
}

void MainWindow::actionslotSettings(bool)
{
    DialogSettings *dialog = new DialogSettings();
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setAttribute(Qt::WA_ShowModal);
    dialog->show();
}

void MainWindow::actionslotOpen(bool)
{
    // do not open a new tdf if busy
    if (this->centralWidget() != NULL && ((WidgetTargetProcessor*) this->centralWidget())->isBusy()) return;

    // get settings
    QSettings settings;

    // open tdf - target definition file
    QString tdf = QFileDialog::getOpenFileName(this,tr("select target definition file"), settings.value("Mainwindow/TdfPath").toString(), "Target Definition *.ini (*.ini);;All files (*.*)");

    // create target processor
    WidgetTargetProcessor *targproc = new WidgetTargetProcessor(this);

    if (tdf.length() > 0) {
        if (targproc->loadTargetDefinitionFile(tdf)) {
            // target processor could load tdf
            if (this->centralWidget() != NULL) {
                // delete old processor
                WidgetTargetProcessor *targprc_old = (WidgetTargetProcessor*) this->centralWidget();
                this->setCentralWidget(NULL);
                targprc_old->deleteLater();
            }
            this->setCentralWidget(targproc);
        } else {
            // target processor could not load tdf
            targproc->deleteLater();
            QMessageBox msgBox;
            msgBox.setText(tr("Target definition file could not be loaded!"));
            msgBox.setWindowTitle(tr("Error"));
            msgBox.exec();
        }

        // save path
        settings.setValue("Mainwindow/TdfPath", tdf);
    }
}
