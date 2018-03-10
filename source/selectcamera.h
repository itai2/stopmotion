#ifndef SELECTCAMERA_H
#define SELECTCAMERA_H

#include <QCameraInfo>
#include <QDialog>
#include <QListWidgetItem>
#include <map>

namespace Ui {
class SelectCamera;
}

class SelectCamera : public QDialog
{
    Q_OBJECT

public:
    explicit SelectCamera( QCameraInfo current, QWidget *parent = 0 );
    QCameraInfo selectedCamera();
    ~SelectCamera();

private:
    Ui::SelectCamera *ui;
    std::map<QListWidgetItem *, QCameraInfo> _itemToCamera;
};

#endif // SELECTCAMERA_H
