#ifndef WIDGETTARGETPROCESSOR_H
#define WIDGETTARGETPROCESSOR_H

#include <QWidget>
#include <QStringList>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QList>
#include <QTimer>
#include "runlog.h"
#include "widgetpathselector.h"
#include "servermanager.h"
#include "widgettextedit.h"

class WidgetTargetProcessor : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetTargetProcessor(QWidget *parent = 0);
    bool loadTargetDefinitionFile(const QString &file);
    bool isBusy();

signals:

public slots:

private:
    void setBusy(bool busy);
    QString humanReadableSize(qint64 size);

    QString tdf;
    runLog *myRunLog;
    QList<ServerManager*> myServerList;
    bool busy;
    bool stopRequested;
    QString tdfName;
    QString tdfFileScan;

    int SrvIndex;
    int SrvIndexCounter;
    int SrvFileDownloadCounter;
    int SrvFileInstallCounter;
    int SrvFileOverallCount;
    qint64 SrvFileOverallSize;

    QLabel *uiName;
    WidgetPathSelector *uiTargetPath;
    WidgetTextEdit *uiTxtEdit;
    QProgressBar *uiProgressOverall;
    QProgressBar *uiProgressDetail;
    QPushButton *uiBtnStart;
    QPushButton *uiBtnStop;

private slots:
    void slotTargetPathChanged(QString);
    void slotButtonStartClicked(bool);
    void slotButtonStopClicked(bool);
    void slotNextServerFetchFileList(bool success);
    void slotStartNextFileDownload(bool success);
    void slotFileDownloadProgress(int percent, qint64 receivedbytes);
    void slotStartNextFileInstall(bool success);
};

#endif // WIDGETTARGETPROCESSOR_H
