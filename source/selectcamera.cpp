#include "selectcamera.h"
#include "ui_selectcamera.h"

SelectCamera::SelectCamera( QCameraInfo current, QWidget *parent ) :
    QDialog(parent),
    ui(new Ui::SelectCamera)
{
    ui->setupUi(this);

    auto availableCameras = QCameraInfo::availableCameras();
    auto selected = QCameraInfo::defaultCamera();
    for ( auto &cameraInfo : availableCameras)
    {
        auto item = new QListWidgetItem( cameraInfo.description(), ui->_listOfCameras );
        _itemToCamera[item] = cameraInfo;

        if ( cameraInfo.description() == current.description() )
            ui->_listOfCameras->setCurrentItem( item );
    }
    if ( ui->_listOfCameras->currentItem() == 0 )
        ui->_listOfCameras->setCurrentRow( 0 );
}

QCameraInfo SelectCamera::selectedCamera()
{
    return _itemToCamera.at( ui->_listOfCameras->currentItem() );
}

SelectCamera::~SelectCamera()
{
    delete ui;
}
