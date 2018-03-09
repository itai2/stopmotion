#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCameraInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QCameraInfo selected = QCameraInfo::defaultCamera();
    const QList<QCameraInfo> availableCameras = QCameraInfo::availableCameras();
    for (const QCameraInfo &cameraInfo : availableCameras)
    {
        if ( cameraInfo.description().contains( "930" ) )
             selected = cameraInfo;
    }

    _camera = new QCamera( selected );
    _camera->setViewfinder( ui->_viewfinder );
    _camera->start();
    ui->_viewfinder->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
