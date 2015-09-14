#include "runlog.h"
#include <QDateTime>
#include <QDir>
#include <QTextStream>
#include <QCoreApplication>
#include <unwind.h>

runLog::runLog(QObject *parent) : QObject(parent)
{
    outFile   = (QFile *) NULL;
}

runLog::~runLog()
{
    this->finalize();
}

bool runLog::log(const QString SrcFile, const int SrcLineNr, const QString SrcFunction, const QString severity, const QString message)
{
    return this->_log(false, SrcFile, SrcLineNr, SrcFunction, severity, message);
}

bool runLog::log(const QString SrcFile, const int SrcLineNr, const QString SrcFunction, const runLog::Severity severity, const QString message)
{
    QString sev = "";
    bool flush = false;

    switch (severity) {
    case runLog::sevPiffle:
        sev = "Piffle";
        break;
    case runLog::sevUserInput:
        sev = "User Input";
        break;
    case runLog::sevNotice:
        sev = "Notice";
        break;
    case runLog::sevWarning:
        sev = "Warning";
        break;
    case runLog::sevError:
        sev = "Error";
        flush = true;
        break;
    case runLog::sevPanic:
        sev = "Panic";
        flush = true;
        break;
    }

    return this->_log(flush, SrcFile, SrcLineNr, SrcFunction, sev, message);
}

bool runLog::_log(const bool flush, const QString SrcFile, const int SrcLineNr, const QString SrcFunction, const QString severity, const QString message)
{
    QDateTime actDate = QDateTime::currentDateTime();

    // open file
    if (outFile == (QFile *) NULL) {
        // create logfile name
        QString outFileName = actDate.toString("yyyy-MM-dd_hh-mm-ss_zzz");
        outFileName +=  ".runlog.xml";

        // create output directory
        QString outDirPath = QDir::tempPath() + "/runLog_" + QCoreApplication::applicationName() + "/";
        QDir outDir = QDir(outDirPath);
        if (!outDir.mkpath(outDirPath)) {
            QTextStream(stderr) << "Could not create logdirectory '" << outDirPath << "'!";
            return false;
        }

        // open output file
        outFile = new QFile(outDirPath + outFileName);
        if (!outFile->open(QFile::WriteOnly)) {
            QTextStream(stderr) << "Could not create logfile '" << outDirPath << outFileName << "'!";
            return false;
        }

        outStream.setDevice(outFile);

        outStream.setAutoFormatting(true);
        outStream.writeStartDocument();
        outStream.writeStartElement("runLog");
        outStream.writeAttribute("version", RUNLOG_VERSION);
    }

    // entry
    outStream.writeStartElement("LogEntry");
    outStream.writeTextElement("Time", actDate.toString("yyyy-MM-dd hh:mm:ss.zzz"));
    outStream.writeTextElement("Severity", severity);
    outStream.writeTextElement("File", SrcFile);
    outStream.writeTextElement("Line", QString::number(SrcLineNr));
    outStream.writeTextElement("Function", SrcFunction);
    outStream.writeTextElement("Message", message);
    outStream.writeEndElement();

    if (flush) outFile->flush();

    return true;
}

bool runLog::finalize()
{
    if (outFile == (QFile *) NULL) {
        return true;
    } else {
        outStream.writeEndDocument();
        outFile->close();
        outFile = (QFile *) NULL;
        return true;
    }
}

