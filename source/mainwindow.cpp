#include "mainwindow.h"
#include "selectcamera.h"
#include "ui_mainwindow.h"
#include <QCameraInfo>
#include <QFileDialog>
#include <QDebug>

void MainWindow::setWorkingDir( const QString &dir )
{
    _workingDir = dir;
    _settings.setValue( "working_dir", _workingDir );
    ui->_workingDirLabel->setText( _workingDir );
    reArrangeFiles();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _settings( "Bteam", "StopMotion" ),
    _cameraSettings( 0 )
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
    ui->_captureButton->setFocus();

    _movieTimeLine = new QTimeLine( 0 ,this );
    connect( _movieTimeLine,
             &QTimeLine::frameChanged,
             this,
             QOverload<int>::of(&MainWindow::setMovieImage) );

    int frameTimeMs = _settings.value( "frame_time_ms" ).toInt();
    ui->_frameTimeMs->setRange( 1, 10000 );
    ui->_frameTimeMs->setValue( frameTimeMs );
    connect( ui->_frameTimeMs,
             QOverload<int>::of(&QSpinBox::valueChanged),
             [this] (int newValue ) {_settings.setValue( "frame_time_ms", newValue ); } );

    setMovieImage( 1 );
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

QStringList MainWindow::getAllImages( QDir::SortFlags flags) const
{
    return QDir( _workingDir ).entryList( QStringList( "*.jpg" ),
                                          QDir::Files,
                                          flags );
}

QString MainWindow::getImageFilePath(int imageNumber) const
{
    QString result =  QDir( _workingDir ).absoluteFilePath( QString( "%1.jpg" ).arg( imageNumber,
                                                                                 8,
                                                                                 10,
                                                                                 QChar('0') ) );
    return result;
}

int MainWindow::getNumFiles() const
{
    return getAllImages( QDir::Unsorted ).count();
}

void MainWindow::fillImageList()
{
    ui->_imageIconList->clear();
    ui->_imageIconList->setIconSize( QSize( 160, 90 ) );
    ui->_imageIconList->setFixedWidth( ui->_imageIconList->iconSize().width() + 30 );
    qDebug() << "Icon size is " << ui->_imageIconList->iconSize();
    auto allImages = getAllImages( QDir::Name );
    for ( auto &imageFileName : allImages )
    {
        auto filePath = QDir( _workingDir ).absoluteFilePath( imageFileName );
        new QListWidgetItem( QIcon( filePath ), QString(), ui->_imageIconList );
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

    _cameraSettings.reset( new CameraSettings( ui->centralWidget ) );
    ui->_cameraSettingsLayout->addWidget( _cameraSettings.data() );
    _cameraSettings->setDevice( selected.deviceName(), false );
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
    QString filePath = getImageFilePath( getNumFiles() + 1 );
    _capture->capture( filePath );
}

void MainWindow::imageSaved( int /*id*/, const QString &fileName )
{
    ui->_imageIconList->addItem( new QListWidgetItem( QIcon( fileName ), QString() ) );
    ui->_imageIconList->scrollToBottom();
    ui->_captureButton->setEnabled( true );
    ui->_captureButton->setFocus();
}

void MainWindow::on__play_clicked()
{
    int numFiles = getNumFiles();

    _movieTimeLine->setDuration( ( numFiles ) * ui->_frameTimeMs->value() );
    _movieTimeLine->setFrameRange( 1, numFiles );
    _movieTimeLine->start();
}

void MainWindow::setMovieImage(int frame)
{
    QPixmap nextImage( getImageFilePath( frame ) );
    setMovieImage( nextImage );
}

void MainWindow::setMovieImage(const QPixmap &image)
{
    ui->_movie->setPixmap( image.scaled( ui->_movie->width(),
                                         ui->_movie->height(),
                                         Qt::KeepAspectRatio ) );

}

void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::quit();
}

void MainWindow::resizeEvent(QResizeEvent */*event*/)
{
    setMovieImage( *ui->_movie->pixmap() );
}

void MainWindow::on__delImage_clicked()
{
    auto selection = ui->_imageIconList->selectionModel()->selectedIndexes();
    if ( selection.size() > 0 )
        deleteImages( selection.first().row(), selection.last().row() );
}

void MainWindow::deleteImages( int fromIndex, int toIndex )
{
    for ( int i = fromIndex; i <= toIndex; ++ i )
    {
        QString toRemove = getImageFilePath( i + 1 );
        qDebug() << "Removing: " << toRemove;
        QFile::remove( toRemove );
    }
    reArrangeFiles();
}

void MainWindow::reArrangeFiles()
{
    auto allImages = getAllImages( QDir::Name );
    for( int i = 0; i < allImages.size(); ++ i )
    {
        auto shouldBe = getImageFilePath( i + 1 );
        auto current = QDir( _workingDir ).absoluteFilePath( allImages[i] );
        if ( shouldBe != current )
            QFile::rename( current, shouldBe );
    }
    fillImageList();
}
