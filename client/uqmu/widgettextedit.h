#ifndef WIDGETTEXTEDIT_H
#define WIDGETTEXTEDIT_H

#include <QTextEdit>

class WidgetTextEdit : public QTextEdit
{
public:
    WidgetTextEdit(QWidget *parent = 0);
    void appendFileInfo(const QString &msg);
    void appendDownload(const QString &msg);
    void appendInstall(const QString &msg);
    void appendSuccess(const QString &msg);
    void appendError(const QString &msg);
};

#endif // WIDGETTEXTEDIT_H
