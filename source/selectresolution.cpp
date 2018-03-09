#include "selectresolution.h"
#include "ui_selectresolution.h"

#include <QCameraImageCapture>

SelectResolution::SelectResolution( const QCameraImageCapture &capture, QWidget *parent ) :
    QDialog(parent),
    ui(new Ui::SelectResolution),
    _capture( capture )
{
    ui->setupUi(this);

    auto currentResolution = capture.encodingSettings().resolution();
    auto resolutions = _capture.supportedResolutions();
    for ( const QSize &res : resolutions )
    {
        QString itemDescription = QString( "%1 X %2").arg( res.width() ).arg( res.height() );
        auto item = new QListWidgetItem( itemDescription, ui->_listOfResolutions );
        _itemToRes[item] = res;

        if ( res == currentResolution )
            ui->_listOfResolutions->setCurrentItem( item );
    }
    if ( ui->_listOfResolutions->currentItem() == 0 )
        ui->_listOfResolutions->setCurrentRow( 0 );
}

SelectResolution::~SelectResolution()
{
    delete ui;
}

QSize SelectResolution::selectedResolution() const
{
   return _itemToRes.at( ui->_listOfResolutions->currentItem() );
}
