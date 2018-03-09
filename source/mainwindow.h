#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "selectresolution.h"
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
    void on_action_SelectResolution_triggered()
    {
        auto dialog = new SelectResolution( *_capture, this );
        if ( dialog->exec() == QDialog::Accepted )
            setResolution( dialog->selectedResolution() );
        delete dialog;
    }

private:
    Ui::MainWindow *ui;
    QCamera *_camera;
    QCameraImageCapture *_capture;
    void setResolution( QSize res );
};

#endif // MAINWINDOW_H
