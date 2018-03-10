#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "selectresolution.h"
#include <QCamera>
#include <QCameraImageCapture>
#include <QCameraInfo>
#include <QDir>
#include <QMainWindow>
#include <QScopedPointer>
#include <QSettings>

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
    void on_action_Select_Working_Directory_triggered();
    void on__captureButton_clicked();
    void imageSaved( int id, const QString &fileName );

private:
    Ui::MainWindow *ui;
    QScopedPointer<QCamera> _camera;
    QCameraInfo _currentCameraInfo;
    QSize _currentResolution;
    QScopedPointer<QCameraImageCapture> _capture;
    QSettings _settings;
    QString _workingDir;
    int _currentFileNumber;
    void setResolution( QSize res );
    void setCamera( QCameraInfo selected, QSize resolution );
    void setWorkingDir( const QString &dir );
    void setTopQuality();

    void setCurrentFileNumber();
    void setupImageList();
    QStringList getAllImages(QDir::SortFlags flags);
};

#endif // MAINWINDOW_H
