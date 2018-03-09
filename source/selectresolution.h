#ifndef SELECTRESOLUTION_H
#define SELECTRESOLUTION_H

#include <QCameraImageCapture>
#include <QDialog>
#include <QListWidgetItem>
#include <map>

namespace Ui {
class SelectResolution;
}

class SelectResolution : public QDialog
{
    Q_OBJECT

public:
    SelectResolution( const QCameraImageCapture &capture, QWidget *parent = 0 );
    ~SelectResolution();
    QSize selectedResolution() const;

private:
    Ui::SelectResolution *ui;
    const QCameraImageCapture &_capture;
    std::map<QListWidgetItem *, QSize> _itemToRes;
};

#endif // SELECTRESOLUTION_H
