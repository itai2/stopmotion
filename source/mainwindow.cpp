#include "mainwindow.h"
#include "selectcamera.h"
#include "ui_mainwindow.h"
#include <QCameraInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _currentCameraInfo = QCameraInfo::defaultCamera();
    const QList<QCameraInfo> availableCameras = QCameraInfo::availableCameras();
    for (const QCameraInfo &cameraInfo : availableCameras)
    {
        if ( cameraInfo.description().contains( "930" ) )
             _currentCameraInfo = cameraInfo;
    }

    setCamera( _currentCameraInfo, _currentResolution );
    ui->_viewfinder->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setResolution( QSize res )
{
    QImageEncoderSettings imageSettings;
    imageSettings.setResolution( res );
    _capture->setEncodingSettings( imageSettings );
}

void MainWindow::setCamera( QCameraInfo selected, QSize resolution )
{
    _currentCameraInfo = selected;
    _camera.reset( new QCamera( _currentCameraInfo ) );
    _camera->setViewfinder( ui->_viewfinder );
    _capture.reset( new QCameraImageCapture( _camera.data() ) );
    _camera->start();
    setResolution( resolution );
}

void MainWindow::on_action_SelectResolution_triggered()
{
    auto dialog = new SelectResolution( *_capture, this );
    if ( dialog->exec() == QDialog::Accepted )
    {
        _currentResolution = dialog->selectedResolution();
        setResolution( _currentResolution );
    }
    delete dialog;
}

void MainWindow::on_actionSelect_Ca_mera_triggered()
{
    _camera.reset();

    auto dialog = new SelectCamera( _currentCameraInfo, this );
    if ( dialog->exec() == QDialog::Accepted )
    {
        if ( dialog->selectedCamera() != _currentCameraInfo )
            _currentResolution = QSize();
        setCamera( dialog->selectedCamera(), _currentResolution );
    }
    else
        setCamera( _currentCameraInfo, _currentResolution );
    delete dialog;
}
