#ifndef SLOG_H
#define SLOG_H

#include <QObject>
#include <QXmlStreamWriter>
#include <QFile>

// MAJOR.MINOR [STATE]
// MAJOR - integer (increases with XML format changing)
// .     - dot char
// MINOR - integer
// .     - dot char
// STATE - a (alpha, development state) | <r> (released)
#define RUNLOG_VERSION "2.0.a"

#define __RUNLOG__ __FILE__,__LINE__,__PRETTY_FUNCTION__

class runLog : public QObject
{
    Q_OBJECT

public:
    explicit runLog(QObject *parent = 0);
    ~runLog();
    enum Severity {
        sevPanic,       // an error that is supposed to happen -> hesitate from regular program execution
        sevError,       //  intended program execution must be canceled -> go back to base/save point
        sevWarning,     // parameters are not processable and are changed to proceed
        sevNotice,      // various actions that are processed
        sevUserInput,   // user triggered actions
        sevPiffle       // like sevNoitce bot more verbose
    };

signals:

public slots:
    // if flush is not set, it is assumed as true
    bool log(const QString SrcFile, const int SrcLineNr, const QString SrcFunction, const Severity severity, const QString message);
    bool log(const QString SrcFile, const int SrcLineNr, const QString SrcFunction, const QString severity, const QString message);
    bool finalize(void);

private:
    bool _log(const bool flush, const QString SrcFile, const int SrcLineNr, const QString SrcFunction, const QString severity, const QString message);
    QXmlStreamWriter outStream;
    QFile *outFile;

};

#endif // SLOG_H
