#ifndef WIDGETPATHSELECTOR_H
#define WIDGETPATHSELECTOR_H

#include <QWidget>
#include <QString>
#include <QLineEdit>
#include <QToolButton>

class WidgetPathSelector : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetPathSelector(const QString &path, QWidget *parent = 0);

    QString getPath() const;
    void setPath(const QString &path);
    void setEnabled(bool);
    void setDisabled(bool);

signals:
    void signalPathChanged(QString);

public slots:

private:
    QLineEdit *lineedit;
    QToolButton *toolbtn;

private slots:
    void slotPathButton(bool);
    void slotPathEdit(QString);
};

#endif // WIDGETPATHSELECTOR_H
