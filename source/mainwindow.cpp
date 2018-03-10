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
    setCurrentFileNumber();
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

    setupImageList();
    ui->_captureButton->setFocus();
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

void MainWindow::setTopQuality()
{
    auto encodingSettings = _capture->encodingSettings();
    encodingSettings.setQuality( QMultimedia::VeryHighQuality );
    _capture->setEncodingSettings( encodingSettings );
}

QStringList MainWindow::getAllImages( QDir::SortFlags flags)
{
    return QDir( _workingDir ).entryList( QStringList( "*.jpg" ),
                                          QDir::Files,
                                          flags );
}

void MainWindow::setCurrentFileNumber()
{
    auto allImages = getAllImages( QDir::Name | QDir::Reversed );
    if ( allImages.isEmpty() )
        _currentFileNumber = 1;
    else
        _currentFileNumber = QFileInfo( allImages[0] ).baseName().toInt() + 1;
}

void MainWindow::setupImageList()
{
    ui->_imageIconList->setIconSize( QSize( 160, 90 ) );
    ui->_imageIconList->setFixedWidth( ui->_imageIconList->iconSize().width() + 30 );
    qDebug() << "Icon size is " << ui->_imageIconList->iconSize();
    auto allImages = getAllImages( QDir::Name );
    for ( auto imageFileName : allImages )
    {
        new QListWidgetItem( QIcon( QDir( _workingDir ).absoluteFilePath( imageFileName ) ), QString(), ui->_imageIconList );
    }
}

void MainWindow::setCamera( QCameraInfo selected, QSize resolution )
{
    _currentCameraInfo = selected;
    _camera.reset( new QCamera( _currentCameraInfo ) );
    _camera->setViewfinder( ui->_viewfinder );
    _capture.reset( new QCameraImageCapture( _camera.data() ) );
    QObject::connect( _capture.data(), &QCameraImageCapture::imageSaved, this, &MainWindow::imageSaved );
    _camera->start();
    _camera->setCaptureMode( QCamera::CaptureStillImage );

    setTopQuality();

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

    ui->_captureButton->setEnabled( false );
    QString filePath = QDir( _workingDir ).absoluteFilePath( QString( "%1" ).arg( _currentFileNumber,
                                                                                  8,
                                                                                  10,
                                                                                  QChar('0') ) );
    _capture->capture( filePath );
}

void MainWindow::imageSaved( int /*id*/, const QString &fileName )
{
    ++ _currentFileNumber;
    ui->_imageIconList->addItem( new QListWidgetItem( QIcon( fileName ), QString() ) );
    ui->_imageIconList->scrollToBottom();
    ui->_captureButton->setEnabled( true );
    ui->_captureButton->setFocus();
}
