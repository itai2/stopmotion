#ifndef CAMERASETTINGS_H
#define CAMERASETTINGS_H

#include <libv4l2.h>
#include "cv4l-helpers.h"
#include <map>
#include <vector>
#include <QTabWidget>
#include <QGridLayout>
#include <QLabel>
#include <QSignalMapper>

class CameraSettings :
        public QTabWidget,
        public cv4l_fd
{
public:
    CameraSettings( QWidget *parent );
    void setUpCameraControlTabs( int m_winWidth );
    ~CameraSettings();
private slots:
    void ctrlAction(int id);
private:

    typedef std::vector<unsigned> ClassIDVec;
    typedef std::map<unsigned, struct v4l2_query_ext_ctrl> CtrlMap;
    typedef std::map<unsigned, ClassIDVec> ClassMap;
    typedef std::map<unsigned, QWidget *> WidgetMap;
    int m_row, m_col, m_cols;
    int m_maxw[4];
    const int m_minWidth;
    const int m_vMargin;
    const int m_hMargin;
    int m_increment;
    CtrlMap m_ctrlMap;
    ClassMap m_classMap;
    const double m_pxw;
    WidgetMap m_widgetMap;
    WidgetMap m_sliderMap;
    QSignalMapper *m_sigMapper;

    static bool is_valid_type(quint32 type);
    void addCtrl(QGridLayout *grid, const v4l2_query_ext_ctrl &qec);
    void fixWidth(QGridLayout *grid);
    void finishGrid(QGridLayout *grid, unsigned which);
    void addWidget(QGridLayout *grid, QWidget *w, Qt::Alignment align = Qt::AlignLeft);
    void addLabel(QGridLayout *grid, const QString &text, Qt::Alignment align = Qt::AlignLeft)
    {
        addWidget(grid, new QLabel(text, parentWidget()), align);
    }
    void refresh(unsigned which);
    void error(const QString &error);
    void error(int err);
    void errorCtrl(unsigned id, int err);
    void errorCtrl(unsigned id, int err, const QString &v);
    void errorCtrl(unsigned id, int err, long long v);
    void setVal64(unsigned id, long long v);
    void setString(unsigned id, const QString &v);
    void setVal(unsigned id, int v);
    void setWhat(QWidget *w, unsigned id, const QString &v);
    void setWhat(QWidget *w, unsigned id, long long v);
    QString getCtrlFlags(unsigned flags);
    void setDevice(const QString &device, bool rawOpen);
    void closeDevice();
    void setDefaults(unsigned which);
    void updateCtrl(unsigned id);
    long long getVal64(unsigned id);
    QString getString(unsigned id);
    int getVal(unsigned id);
};

#endif // CAMERASETTINGS_H
