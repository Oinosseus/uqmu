#include "widgettargetprocessor.h"
#include <QSettings>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include "servermanager_file.h"
#include "servermanager_uqmu_http.h"

WidgetTargetProcessor::WidgetTargetProcessor(QWidget *parent) : QWidget(parent)
{
    // initialize properties
    tdf                = "";
    myRunLog           = new runLog(this);
    myServerList.clear();
    busy               = false;
    uiName             = new QLabel();
    uiTargetPath       = new WidgetPathSelector("");
    connect(uiTargetPath, SIGNAL(signalPathChanged(QString)), this, SLOT(slotTargetPathChanged(QString)));
    uiTxtEdit          = new WidgetTextEdit();
    uiProgressOverall  = new QProgressBar();
    uiProgressDetail   = new QProgressBar();
    uiBtnStart         = new QPushButton(QIcon::fromTheme("media-playback-start"), tr("start"));
    connect(uiBtnStart, SIGNAL(clicked(bool)), this, SLOT(slotButtonStartClicked(bool)));
    uiBtnStop          = new QPushButton(QIcon::fromTheme("media-playback-stop"), tr("stop"));
    connect(uiBtnStop, SIGNAL(clicked(bool)), this, SLOT(slotButtonStopClicked(bool)));
    SrvIndex           = -1;

    // enable controls
    stopRequested = false;
    this->setBusy(false);

    // layout
    QVBoxLayout *layout = new QVBoxLayout();
    this->setLayout(layout);

    // ui target name
    QHBoxLayout *layout_name = new QHBoxLayout();
    layout->addLayout(layout_name);
    layout_name->addStretch(1);
    layout_name->addWidget(this->uiName, 0);
    layout_name->addStretch(1);

    // other ui items
    layout->addWidget(this->uiTargetPath, 0);
    layout->addWidget(this->uiTxtEdit, 1);
    layout->addWidget(this->uiProgressDetail, 0);
    layout->addWidget(this->uiProgressOverall, 0);

    // ui buttons
    QHBoxLayout *layout_btns = new QHBoxLayout();
    layout->addLayout(layout_btns);
    layout_btns->addStretch(1);
    layout_btns->addWidget(uiBtnStart);
    layout_btns->addStretch(1);
    layout_btns->addWidget(uiBtnStop);
    layout_btns->addStretch(1);
}

bool WidgetTargetProcessor::loadTargetDefinitionFile(const QString &file)
{
    bool success = true;
    this->tdf = file;
    QSettings ini_reader(file, QSettings::IniFormat);

    // check necessary settings
    if (!ini_reader.contains("Server0/Url")) {
        myRunLog->log(__FILE__, __LINE__, "loadTargetDefinitionFile()", runLog::sevError, QString("Entry 'Server0/Url' not found in tdf '") + file + QString("'!"));
        success = false;
    }

    // read values
    this->tdfName     = ini_reader.value("Local/Name").toString();
    this->tdfFileScan = ini_reader.value("Local/FileScan").toString();
    myServerList.clear();

    // log output
    QString log_msg;

    log_msg += "Local/Name = '";
    log_msg += this->tdfName;
    log_msg += "'\n";

    log_msg += "Local/FileScan = '";
    log_msg += this->tdfFileScan;
    log_msg += "'\n";

    myRunLog->log(__FILE__, __LINE__, "loadTargetDefinitionFile()", runLog::sevPiffle, log_msg);

    // fill ui elements
    QSettings settings;
    settings.beginGroup(QString("TDF ") + this->tdfName);
    this->uiName->setText(this->tdfName);
    this->uiTargetPath->setPath(settings.value("TargetPath").toString());
    this->uiTxtEdit->clear();
    this->uiProgressDetail->reset();
    this->uiProgressOverall->reset();
    this->uiBtnStart->setEnabled(success);
    this->uiBtnStop->setEnabled(false);

    return success;
}

bool WidgetTargetProcessor::isBusy()
{
    return this->busy;
}

void WidgetTargetProcessor::setBusy(bool busy)
{
    uiBtnStart->setDisabled(busy);
    uiBtnStop->setEnabled(busy);
    uiTargetPath->setDisabled(busy);
    this->busy = busy;
}

QString WidgetTargetProcessor::humanReadableSize(qint64 size)
{
    QStringList units;
    units << " B" << " kiB" << " MiB" << " GiB" << " TiB";

    int i;
    for (i = 0; i < units.length() && size >= 1024; i++) {
        size = size >> 10;
    }

    return QString::number(size) + units[i];
}

void WidgetTargetProcessor::slotTargetPathChanged(QString path)
{
    QSettings settings;
    settings.beginGroup(QString("TDF ") + this->tdfName);
    settings.setValue("TargetPath", path);
}

void WidgetTargetProcessor::slotButtonStartClicked(bool)
{
    QSettings settings;

    SrvIndex = -1;
    SrvFileDownloadCounter = -1;
    SrvFileInstallCounter = -1;
    stopRequested = false;

    myRunLog->log(__FILE__, __LINE__, "slotButtonStartClicked()", runLog::sevUserInput, "Start button clicked.");
    this->setBusy(true);
    uiTxtEdit->clear();
    uiProgressDetail->reset();
    uiProgressOverall->reset();

    // reset server list
    for (int i = 0; i < myServerList.length(); i++) {
        myServerList[i]->deleteLater();
    }
    myServerList.clear();

    // check target directory
    QDir target_dir(uiTargetPath->getPath());
    if (uiTargetPath->getPath().length() == 0 || !target_dir.exists()) {
        QString msg;
        msg += "Target directory '";
        msg += uiTargetPath->getPath();
        msg += "' must be an existing directory!";
        myRunLog->log(__FILE__, __LINE__, "slotButtonStartClicked()", runLog::sevError, msg);
        uiTxtEdit->appendError(msg);
        this->setBusy(false);
        return;
    }

    // check ScanFile
    if (tdfFileScan.length() > 0) {
        QDir target_path(uiTargetPath->getPath());
        QFileInfo checkFile(target_path.filePath(tdfFileScan));
        if (!checkFile.exists() || !checkFile.isFile()) {
            QString msg;
            msg += "Scan for file failed with '";
            msg += checkFile.absoluteFilePath();
            msg += "'!";
            myRunLog->log(__FILE__, __LINE__, "slotButtonStartClicked()", runLog::sevError, msg);
            uiTxtEdit->appendError(msg);
            this->setBusy(false);
            return;
        }
    }

    // check cache directory
    QString cache_path = settings.value("Settings/CachePath").toString();
    QDir cache_dir(cache_path);
    if (cache_path.length() == 0 || !cache_dir.exists()) {
        QString msg;
        msg += "Cache directory is not set (check settings)!'";
        myRunLog->log(__FILE__, __LINE__, "slotButtonStartClicked()", runLog::sevError, msg);
        uiTxtEdit->appendError(msg);
        this->setBusy(false);
        return;
    }

    // load servers from tdf
    QSettings ini_reader(this->tdf, QSettings::IniFormat);
    for (int i=0; ini_reader.childGroups().contains(QString("Server") + QString::number(i)); i++) {
        QUrl url(ini_reader.value(QString("Server") + QString::number(i) + QString("/Url")).toString());

        ServerManager *srvmng = NULL;

        if (url.scheme() == "file") {
            QDir targetPath(uiTargetPath->getPath());
            QDir cachePath(settings.value("Settings/CachePath").toString() + QString("/") + uiName->text());
            srvmng = new ServerManager_file(this->myRunLog, cachePath, targetPath, this);

        } else if (url.scheme() == "uqmu-http" || url.scheme() == "uqmu-https") {
            QDir targetPath(uiTargetPath->getPath());
            QDir cachePath(settings.value("Settings/CachePath").toString() + QString("/") + uiName->text());
            srvmng = new ServerManager_uqmu_http(this->myRunLog, cachePath, targetPath, this);

        } else {
            QString msg = "Url scheme not supported for 'Server";
            msg += QString::number(i);
            msg += "' in target definition file! (Url =";
            msg += url.toString();
            msg += ")";
            myRunLog->log(__FILE__, __LINE__, "slotButtonStartClicked()", runLog::sevPanic, msg);
            return;
        }

        myServerList.append(srvmng);
        int timeout = ini_reader.value(QString("Server") + QString::number(i) + QString("/Timeout")).toInt();
        srvmng->setUrl(url, timeout);
    }

    // command first server to fetch file list
    SrvIndex = 0;
    uiTxtEdit->append(tr("connecting server ") + myServerList[SrvIndex]->getUrl().toString());
    connect(myServerList[SrvIndex], SIGNAL(finishFetchFileList(bool)), this, SLOT(slotNextServerFetchFileList(bool)));
    myServerList[SrvIndex]->startFetchFileList();
}

void WidgetTargetProcessor::slotButtonStopClicked(bool)
{
    this->myRunLog->log(__FILE__, __LINE__, __FUNCTION__, runLog::sevUserInput, "stop button clikced");
    stopRequested = false;
//    if (SrvIndex >= 0) {
//        myServerList[SrvIndex]->stopAllProcesses();
//    }
}

void WidgetTargetProcessor::slotNextServerFetchFileList(bool success)
{
    disconnect(myServerList[SrvIndex], SIGNAL(finishFetchFileList(bool)), this, SLOT(slotNextServerFetchFileList(bool)));

    if (stopRequested) {
        uiTxtEdit->appendError("user abort");
        this->setBusy(false);
        return;
    }

    // progress bar update
    uiProgressDetail->reset();
    uiProgressOverall->setValue( (SrvIndex + 1) * 100 / myServerList.length());
    uiProgressOverall->setFormat("%p% connect");

    // server has successfully fetched file list
    if (success) {
        // set 100% if finished
        uiProgressOverall->setValue(100);
        this->repaint();

        // count file statistics
        SrvFileOverallCount = 0;
        SrvFileOverallSize  = 0;
        int    file_download_count = 0;
        qint64 download_file_size  = 0;
        foreach (ServerFile *srvfile, myServerList[SrvIndex]->getServerFileList()) {
            SrvFileOverallCount += 1;
            SrvFileOverallSize  += srvfile->getServerSize();
            if (srvfile->getNeedsDownload()) {
                file_download_count += 1;
                download_file_size  += srvfile->getServerSize();
            }
        }

        // file statistics info
        uiTxtEdit->appendSuccess("Server Found\n");
        uiTxtEdit->appendFileInfo("files on server: " + QString::number(SrvFileOverallCount) + " (" + this->humanReadableSize(SrvFileOverallSize) + ")");
        uiTxtEdit->appendFileInfo("need to download: " + QString::number(file_download_count) + " (" + this->humanReadableSize(download_file_size) + ")");
        uiTxtEdit->append("");
        myRunLog->log(__FILE__, __LINE__, "slotTimerSrvConnection()", runLog::sevPiffle, "server found: " + myServerList[SrvIndex]->getUrl().toString());

        // start file download
        SrvFileDownloadCounter = -1;
        this->slotStartNextFileDownload(true);

    // server failed but other servers are available
    } else if (SrvIndex < (myServerList.length() - 1)) {
        // request next server
        SrvIndex += 1;
        uiTxtEdit->append(tr("connecting server ") + myServerList[SrvIndex]->getUrl().toString());
        connect(myServerList[SrvIndex], SIGNAL(finishFetchFileList(bool)), this, SLOT(slotNextServerFetchFileList(bool)));
        myServerList[SrvIndex]->startFetchFileList();

    // no server is available
    } else {
        uiTxtEdit->appendError("no server available");
        myRunLog->log(__FILE__, __LINE__, "slotTimerSrvConnection()", runLog::sevError, "No server found!");
        setBusy(false);
    }
}

void WidgetTargetProcessor::slotStartNextFileDownload(bool success)
{
    disconnect(myServerList[SrvIndex], SIGNAL(finishedFileDownload(bool)), this, SLOT(slotStartNextFileDownload(bool)));
    disconnect(myServerList[SrvIndex], SIGNAL(fileDownloadProgress(int,qint64)), this, SLOT(slotFileDownloadProgress(int,qint64)));

    if (stopRequested) {
        uiTxtEdit->appendError("user abort");
        this->setBusy(false);
        return;
    }

    // check success
    if (success == false) {
        QString msg = "File download failed at file '";
        msg += myServerList[SrvIndex]->getServerFileList()[SrvFileDownloadCounter]->getRelativeCachePath();
        msg += "'";
        myRunLog->log(__FILE__, __LINE__, __FUNCTION__, runLog::sevPiffle, msg);
        uiTxtEdit->appendError(msg);
        this->setBusy(false);
        return;
    }

    // search next file to download
    bool next_download_available = false;
    for (int i = (SrvFileDownloadCounter + 1); i < myServerList[SrvIndex]->getServerFileList().length(); i++) {
        if (myServerList[SrvIndex]->getServerFileList()[i]->getNeedsDownload()) {
            SrvFileDownloadCounter = i;
            next_download_available = true;
            break;
        }
    }

    // download finished
    if (!next_download_available) {
        // download finished
        uiProgressDetail->reset();
        uiTxtEdit->appendSuccess("Finished Download\n");
        SrvFileInstallCounter = -1;
        this->slotStartNextFileInstall(true);
        return;
    }

    // count already recevied data
    qint64 rxbytes = 0;
    for (int i=0; i<=SrvFileDownloadCounter; i++) {
        if (myServerList[SrvIndex]->getServerFileList()[i]->getNeedsDownload())
            rxbytes += myServerList[SrvIndex]->getServerFileList()[i]->getServerSize();
    }

    // actualize user info
    uiProgressDetail->reset();
    int percent = rxbytes * 100 / SrvFileOverallSize;
    uiProgressOverall->setValue(percent);
    uiProgressOverall->setFormat("%p% download");
    QString msg = "downloading " + myServerList[SrvIndex]->getServerFileList()[SrvFileDownloadCounter]->getRelativeCachePath();
    uiTxtEdit->appendDownload(msg);
    this->repaint();

    // start new download
    connect(myServerList[SrvIndex], SIGNAL(finishedFileDownload(bool)), this, SLOT(slotStartNextFileDownload(bool)));
    connect(myServerList[SrvIndex], SIGNAL(fileDownloadProgress(int,qint64)), this, SLOT(slotFileDownloadProgress(int,qint64)));
    myServerList[SrvIndex]->startFileDownload(SrvFileDownloadCounter);
}

void WidgetTargetProcessor::slotFileDownloadProgress(int percent, qint64 receivedbytes)
{
    uiProgressDetail->setValue(percent);
}

void WidgetTargetProcessor::slotStartNextFileInstall(bool success)
{
    disconnect(myServerList[SrvIndex], SIGNAL(finishedFileInstall(bool)), this, SLOT(slotStartNextFileInstall(bool)));

    if (stopRequested) {
        uiTxtEdit->appendError("user abort");
        this->setBusy(false);
        this->repaint();
        return;
    }

    // check success
    if (success == false) {
        QString msg = "File install failed at file '";
        msg += myServerList[SrvIndex]->getServerFileList()[SrvFileInstallCounter]->getRelativeCachePath();
        msg += "'";
        myRunLog->log(__FILE__, __LINE__, __FUNCTION__, runLog::sevPiffle, msg);
        uiTxtEdit->appendError(msg);
        this->setBusy(false);
        return;
    }

    // next file to download
    SrvFileInstallCounter += 1;

    // download finished
    if (SrvFileInstallCounter >= myServerList[SrvIndex]->getServerFileList().length()) {
        // download finished
        SrvIndex = -1;
        uiProgressOverall->setValue(100);
        uiTxtEdit->appendSuccess("Finished Installation");
        this->setBusy(false);
        return;
    }

    // actualize user info
    int percent = SrvFileInstallCounter * 100 / (myServerList[SrvIndex]->getServerFileList().length() - 1);
    uiProgressDetail->reset();
    uiProgressOverall->setValue(percent);
    uiProgressOverall->setFormat("%p% install");
    QString msg = "installing " + myServerList[SrvIndex]->getServerFileList()[SrvFileInstallCounter]->getRelativeCachePath();
    uiTxtEdit->appendInstall(msg);
    this->repaint();

    // start new download
    connect(myServerList[SrvIndex], SIGNAL(finishedFileInstall(bool)), this, SLOT(slotStartNextFileInstall(bool)));
    myServerList[SrvIndex]->startFileInstall(SrvFileInstallCounter);
}
