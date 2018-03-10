#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "selectresolution.h"
#include <QCamera>
#include <QCameraImageCapture>
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
    void on_action_SelectResolution_triggered()
    {
        auto dialog = new SelectResolution( *_capture, this );
        if ( dialog->exec() == QDialog::Accepted )
            setResolution( dialog->selectedResolution() );
        delete dialog;
    }

private:
    Ui::MainWindow *ui;
    QScopedPointer<QCamera> _camera;
    QScopedPointer<QCameraImageCapture> _capture;
    void setResolution( QSize res );

    void setCamera( QCameraInfo selected );
};

#endif // MAINWINDOW_H
