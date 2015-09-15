#ifndef DIALOGLICENSE_H
#define DIALOGLICENSE_H

#include <QDialog>
#include <QWidget>

class DialogLicense : public QDialog
{
    Q_OBJECT

public:
    DialogLicense(QWidget *parent = 0);

private slots:
    void slotButtonOk(bool);
};

#endif // DIALOGLICENSE_H
