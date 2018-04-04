#include "mainwindow.h"
#include "selectcamera.h"
#include "ui_mainwindow.h"
#include <QCameraInfo>
#include <QFileDialog>
#include <QDebug>
#include <QShortcut>

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
    _movieTimeLine->setCurveShape( QTimeLine::LinearCurve );
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
    createShortcuts();
}

void MainWindow::createShortcuts()
{
    connect( new QShortcut( QKeySequence( Qt::Key_Space ), this ),
             &QShortcut::activated,
             [this] () { ui->_captureButton->clicked(); } );
    connect( new QShortcut( QKeySequence( Qt::Key_Delete ), this ),
             &QShortcut::activated,
             [this] () { ui->_delImage->clicked(); } );
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
    return QDir( _workingDir ).entryList( QStringList( "sm*.jpg" ),
                                          QDir::Files,
                                          flags );
}

QString MainWindow::getImageFilePath(int imageNumber) const
{
    QString result =  QDir( _workingDir ).absoluteFilePath( QString( "sm%1.jpg" ).arg( imageNumber,
                                                                                     8,
                                                                                     10,
                                                                                     QChar('0') ) );
    return result;
}

QString MainWindow::getTempImageFilePath(int imageNumber) const
{
    QString result =  QDir( _workingDir ).absoluteFilePath( QString( "%1.tmp.jpg" ).arg( imageNumber,
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
    QString filePath = getTempImageFilePath( pasteIndex() + 2 );
    _capture->capture( filePath );
}

void MainWindow::imageSaved( int /*id*/, const QString &/*fileName*/ )
{
    int pastePlace = pasteIndex();
    auto newScollPosition = pasteModelIndex();

    createGap( pastePlace, 1 );
    insertImages( pastePlace, 1 );
    ui->_imageIconList->setCurrentRow( pastePlace + 1 );

    if ( newScollPosition.isValid() )
        ui->_imageIconList->scrollTo( newScollPosition );
    else
        ui->_imageIconList->scrollToBottom();

    ui->_imageIconList->clearSelection();
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
        deleteImages( selection.first().row(), selection.size() );
}

void MainWindow::deleteImages( int fromIndex, int numImages )
{
    moveBack( fromIndex, numImages );
}

void MainWindow::reArrangeFiles()
{
    auto allImages = getAllImages( QDir::Name );
    for( int i = 0; i < allImages.size(); ++ i )
    {
        auto shouldBe = getImageFilePath( i + 1 );
        auto current = QDir( _workingDir ).absoluteFilePath( allImages[i] );
        if ( shouldBe != current )
            moveFile( current, shouldBe );
    }
    fillImageList();
}

void MainWindow::on__copyImage_clicked()
{
    _copiedList = ui->_imageIconList->selectionModel()->selectedIndexes();
}

int MainWindow::pasteIndex() const
{
    auto index = pasteModelIndex();
    if ( index.isValid() )
        return index.row();
    return getNumFiles() - 1;
}

QModelIndex MainWindow::pasteModelIndex() const
{
    auto selection = ui->_imageIconList->selectionModel()->selectedIndexes();
    if ( selection.size() > 0 )
        return selection.last();
    return QModelIndex();
}

void MainWindow::createGap( int startIndex, int numImages )
{
    int lastIndex = getNumFiles() - 1;
    for ( int i = lastIndex; i > startIndex; -- i )
    {
        QString oldName = getImageFilePath( i + 1 );
        int newIndex = i + numImages;
        QString newName = getImageFilePath( newIndex + 1 );
        moveFile( oldName, newName );
        if ( newIndex > lastIndex )
        {
            ui->_imageIconList->insertItem( lastIndex + 1, new QListWidgetItem( QIcon( newName ), QString() ) );
        }
        else
            ui->_imageIconList->item( newIndex )->setIcon( QIcon( newName ) );
    }
}

void MainWindow::moveBack( int startIndex, int gap )
{
    int lastIndex = getNumFiles() - 1;
    for ( int i = startIndex + gap; i <= lastIndex + gap; ++ i)
    {
        int toIndex = i - gap;
        QString toName = getImageFilePath( toIndex + 1 );
        int fromIndex = i;
        QString fromName = getImageFilePath( fromIndex + 1 );
        if ( QFile::exists( fromName ) )
            moveFile( fromName, toName );
        else
            QFile::remove( toName );
    }
    //remove entries from view
    for ( int i = startIndex; i < startIndex + gap; ++ i )
        delete ui->_imageIconList->takeItem( startIndex );
}

void MainWindow::copyImages( int startIndex )
{
    int currentIndex = startIndex;
    for ( auto index : _copiedList )
    {
        QString fromName = getImageFilePath( index.row() + 1 );
        QString toName = getTempImageFilePath( currentIndex + 2 );
        copyFile( fromName, toName );
        ++ currentIndex;
    }
}

void MainWindow::insertImages( int startIndex, int numImages )
{
    int lastIndex = getNumFiles() - 1;

    for ( int i = 0; i < numImages; ++ i )
    {
        int index = i + startIndex + 1;
        QString fromName = getTempImageFilePath( index + 1 );
        QString toName = getImageFilePath( index + 1 );
        moveFile( fromName, toName );
        if ( index > lastIndex )
            new QListWidgetItem( QIcon( toName ), QString(), ui->_imageIconList );
        else
            ui->_imageIconList->item( index )->setIcon( QIcon( toName ) );
    }
}

void MainWindow::on__pasteImage_clicked()
{
    int where = pasteIndex();
    int numImages = _copiedList.size();
    copyImages( where );
    createGap( where, numImages );
    insertImages( where, numImages );
    ui->_imageIconList->clearSelection();
}

bool MainWindow::moveFile( const QString &from, const QString &to )
{
    if ( QFile::exists( to ) )
        QFile::remove( to );
    return QFile::rename( from, to );
}

bool MainWindow::copyFile( const QString &from, const QString &to )
{
    if ( QFile::exists( to ) )
        QFile::remove( to );
    return QFile::copy( from, to );
}
