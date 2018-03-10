#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "selectresolution.h"
#include <QCamera>
#include <QCameraImageCapture>
#include <QCameraInfo>
#include <QMainWindow>
#include <QScopedPointer>

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

    void on_actionSelect_Ca_mera_triggered();

private:
    Ui::MainWindow *ui;
    QScopedPointer<QCamera> _camera;
    QCameraInfo _currentCameraInfo;
    QSize _currentResolution;
    QScopedPointer<QCameraImageCapture> _capture;
    void setResolution( QSize res );
    void setCamera( QCameraInfo selected, QSize resolution );
};

#endif // MAINWINDOW_H
