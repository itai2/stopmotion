#include "mainwindow.h"
#include "selectcamera.h"
#include "ui_mainwindow.h"
#include <QCameraInfo>
#include <QFileDialog>

void MainWindow::setWorkingDir( const QString &dir )
{
    _workingDir = dir;
    _settings.setValue( "working_dir", _workingDir );
    ui->_workingDirLabel->setText( _workingDir );
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _settings( "Bteam", "StopMotion" )
{
    ui->setupUi(this);

    QString savedCamera = _settings.value( "camera" ).toString();
    _currentResolution = _settings.value( "resolution" ).toSize();
    setWorkingDir( _settings.value( "working_dir" ).toString() );

    _currentCameraInfo = QCameraInfo::defaultCamera();
    const QList<QCameraInfo> availableCameras = QCameraInfo::availableCameras();
    for (const QCameraInfo &cameraInfo : availableCameras)
    {
        if ( cameraInfo.description().contains( savedCamera ) )
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
    _currentResolution = res;
    _settings.setValue( "resolution", _currentResolution );
}

void MainWindow::setCamera( QCameraInfo selected, QSize resolution )
{
    _currentCameraInfo = selected;
    _camera.reset( new QCamera( _currentCameraInfo ) );
    _camera->setViewfinder( ui->_viewfinder );
    _capture.reset( new QCameraImageCapture( _camera.data() ) );
    _camera->start();
    _camera->setCaptureMode( QCamera::CaptureStillImage );
    _settings.setValue( "camera", _currentCameraInfo.description() );
    setResolution( resolution );
}

void MainWindow::on_action_SelectResolution_triggered()
{
    auto dialog = new SelectResolution( *_capture, this );
    if ( dialog->exec() == QDialog::Accepted )
        setResolution( dialog->selectedResolution() );
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

void MainWindow::on_action_Select_Working_Directory_triggered()
{
    QString selectedDir = QFileDialog::getExistingDirectory( this,
                                                             "Please select your working directory",
                                                             _workingDir );
    if ( selectedDir.isEmpty() )
        return;

    setWorkingDir( selectedDir );
}

void MainWindow::on__captureButton_clicked()
{
    if ( !_capture->isReadyForCapture() )
        return;
    _capture->capture( _workingDir + "/image.jpg" );
    while ( !_capture->isReadyForCapture() );
}
