#ifndef QWTDRIVER_H
#define QWTDRIVER_H

#include <QObject>
#include<QWidget>
#include<QDebug>
#include<QMenu>
#include<QMouseEvent>
#include<QBackingStore>


#include"QWT/qwt_plot.h"
#include"QWT/qwt_plot.h"
//#include"QWT/qwt_graphic.h"
#include"QWT/qwt_plot_zoomer.h"
#include"QWT/qwt_plot_curve.h"
#include"QWT/qwt_plot_grid.h"
#include"QWT/qwt_plot_magnifier.h"
#include"QWT/qwt_plot_panner.h"
#include"QWT/qwt_legend_data.h"
#include"QWT/qwt_legend.h"
#include"QWT/qwt_legend_label.h"


class QwtDriver : public QWidget
{
    Q_OBJECT
public:
    explicit QwtDriver(QWidget *parent = nullptr);
    ~QwtDriver();

    void Init(void);
    void AddLine(QString curvename);
    //���ñ�������·��
    void SaveImagePath(QString path);
    //���½�����ʾ��С
    void Updatasize();
    //ÿ�����г���������ϴ�����
    void Cleardata(void);
    //��������
    QString curve_data;
    //��ȡ��������
    void GetCurveData(void);


public:
    QSize  m_size;



signals:
    void Sign_clear(void);

public slots:
    void SLot_AddPointToCurve(int index,int x,double y);
    void SLot_AddPointToCurve(int index,int x,quint32 y);
    void Slot_ACtion_Resetcurve(void);
    void Slot_ACtion_Clearcurve(void);
    void Slot_ACtion_CopyDatacurve(void);
    void Slot_ACtion_Copycurve(void);
    void Slot_ACtion_PanelAct(void);

protected:
    void mousePressEvent(QMouseEvent *event);

private:
    void GetCurveItem(void);

private slots:
    //���ͼ������ʾ��Ӧ������
    void showcurveiten(const QVariant &iteminfo,bool on);
private:

    QString imagepath;
    QwtPlot *plotView;
    QwtPlotGrid *grid;
    //�Ŵ�ѡ�е�����
    QwtPlotZoomer *zoomer;
//    //ʹ�ù��ַŴ�
//    QwtPlotMagnifier *magnifier;
//    //ʹ���������ƶ�����
//    QwtPlotPanner *panner;

    //����������ɫ
    QVector<QColor>             m_Color;
    //�洢���ߵ�����
    QVector<QVector<double>>    X_Data;
    QVector<QVector<double>>    Y_Data;
    //�洢���߶����б�
    QList<QwtPlotCurve*>        AllLinesList;
    //X���������ֵ
    double X_axis_max;
    //y���������ֵ
    double Y_axis_max;



    QMenu                       *MenuCtrl;//�˵�
    QAction                     *m_ResetCurve;//�ָ���ʼ״̬
    QAction                     *m_ClearCurve;//���ͼ��
    QAction                     *m_cpDataAct;//��������
    QAction                     *m_cpImagAct;//����ͼƬ
    QAction                     *m_ctrlPanelAct;//�������߿ɼ� �������
};

#endif // QWTDRIVER_H
