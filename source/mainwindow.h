#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "camerasettings.h"
#include "selectresolution.h"
#include <QCamera>
#include <QCameraImageCapture>
#include <QCameraInfo>
#include <QDir>
#include <QMainWindow>
#include <QScopedPointer>
#include <QSettings>
#include <QTimeLine>

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
    void on__play_clicked();
    void setMovieImage( int frame );
    void setMovieImage( const QPixmap &image );

    void on_actionExit_triggered();

    void on__delImage_clicked();

private:
    Ui::MainWindow *ui;
    QScopedPointer<QCamera> _camera;
    QCameraInfo _currentCameraInfo;
    QSize _currentResolution;
    QScopedPointer<QCameraImageCapture> _capture;
    QSettings _settings;
    QString _workingDir;
    int _currentFileNumber;

    QTimeLine *_movieTimeLine;

    void setResolution( QSize res );
    void setCamera( QCameraInfo selected, QSize resolution );
    void setWorkingDir( const QString &dir );
    void setTopQuality();

    void setCurrentFileNumber();
    void fillImageList();
    QStringList getAllImages(QDir::SortFlags flags);
    QString getImageFilePath( int imageNumber ) const;

    //overide events
    //void resizeEvent(QResizeEvent *event);

    //manual ui
    QScopedPointer<CameraSettings> _cameraSettings;
    void resizeEvent(QResizeEvent *event);
    void deleteImages(int fromIndex, int toIndex);
    void reArrangeFiles();
};

#endif // MAINWINDOW_H
