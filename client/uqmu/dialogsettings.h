#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

#include <QWidget>
#include <QGroupBox>

class DialogSettings : public QWidget
{
    Q_OBJECT
public:
    explicit DialogSettings(QWidget *parent = 0);

private:
    QString setting_cachepath;
    QGroupBox *createFrameCachePath(void);

private slots:
    void actionslotCachePath(QString path);
    void actionslotButtonApply(bool);
    void actionslotButtonReject(bool);
};

#endif // DIALOGSETTINGS_H
