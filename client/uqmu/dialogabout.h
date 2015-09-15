#ifndef DIALOGABOUT_H
#define DIALOGABOUT_H

#include <QDialog>
#include <QWidget>

class DialogABout : public QDialog
{
    Q_OBJECT
public:
    explicit DialogABout(QWidget *parent = 0);

signals:

public slots:

private slots:
    void slotButtonOk(bool);
};

#endif // DIALOGABOUT_H
