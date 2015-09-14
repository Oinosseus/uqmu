#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:

private slots:
    void actionslotAbout(bool);
    void actionslotSettings(bool);
    void actionslotOpen(bool);
};

#endif // MAINWINDOW_H
