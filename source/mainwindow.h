#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCamera>
#include <QCameraImageCapture>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_action_SelectResolution_triggered();

private:
    Ui::MainWindow *ui;
    QCamera *_camera;
    QCameraImageCapture *_capture;
};

#endif // MAINWINDOW_H
