#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QDebug>
#include<QDateTime>
#include<QLatin1String>
#include <Windows.h>
#include<QThread>
#include<QClipboard>
#include<QApplication>

#include"q268driver.h"

#include"mythrtead_t.h"

#define HEAD_LENGTH 8
#define INIT_DEVICE_SIZE 8

namespace Ui {
class MainWindow;
}
union M_BIT32
{
    float   m_float;
    quint32 m_int;
    char    m_char[4];
};
struct MessageNode
{
    int Channel;
    int TEC_ID;
    int Module;
    int Fun_ID;
    int RW;
    int NodeType;
    int TempGap;
    int ThiCmd;
    QList<M_BIT32>  Data;
};

struct CMD_bit
{
    quint8  FunID : 4;
    quint8 	FunRW : 1;
    quint8  NodeType : 3;
};
struct CH_bit
{
    quint8  Channel : 1;
    quint8 	TEC_ID : 3;
    quint8  Module : 4;
};
struct ThiCMD_bit
{
    quint8  ThiCmd  : 4;
    quint8 	TempGap : 4;
};

struct SendNode
{
    quint8          Head;
    quint16         Len;
    quint8          CRC;
    CMD_bit         CMD;
    quint8          Ack;
    CH_bit          CH;
    ThiCMD_bit      ThiCMD;
    QList<M_BIT32>  Data;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void InitSerial();

//    MessageNode  MesNode;
    Q268Driver  *SerialNode;

    SendNode send_node_tmp;

    //下发串口消息数据
    QByteArray send_data;
    //串口数据帧结构
    SRequestCmdInfo requescmdInfo;

  //  SendNode    MesNode2SendNode(MessageNode);

public slots:
    //
    void Slot_driver(void);
    void SlotUsartData2Mcu(SRequestCmdInfo cmdInfo);
    void Slot_gat_max(void);
    //清空曲线计数器
    void Slot_Clear_A(void);
    void Slot_Clear_B(void);
    void Slot_Clear_fluor(void);

signals:
    void Sign_SendDriver(QByteArray bdata);
    void Sign_check_state(bool check_state);

protected:
    void resizeEvent(QResizeEvent *event);


private:
    CMD_bit     m_cmd_bit;
    CH_bit      m_cH;
    ThiCMD_bit  m_thiCMD;
    //串口下发的数据
    QString     driver_data;

    //当前添加到那个曲线(温控)
    int check_curve;
    int check_curve_B;
    //添加到曲线的点位(温控)
    int curve_point;
    int curve_point_B;

    //当前添加到那个曲线(荧光)
    int check_curve_flou;
    //添加到曲线的点位(荧光)
    int curve_point_flou;

    bool check_cmd;
    int  check_size;
    int  check_error;
    int  elec_start_point;

    //调试模式下自动采集， 计数值
    int  debug_flou_size;
    //调试模式下，当前进行的是测试最佳位置还是单孔采集荧光
    int point_and_flou;//0 为最佳位置 1 为单孔采集荧光
    //电机运动位置
    int electric_machinery_point;
    int fluo_auto_acq_number;







private slots:
    //单点采集荧光函数
    void Get_Point_Fluo(void);
    //连续采集荧光函数
    void Get_cont_Fluo(void);
    //荧光最佳位置测试荧光采集函数
    void Get_Fluo_Best_Location(void);
    //荧光最佳位置测试电机运动控制函数
    void Sen_Fluo_Motion_control(void);


    void on_comboBox_Module_activated(int index);

//    void on_Btn_update_head_clicked();

    void on_Btn_temp_comp_clicked();

    void on_Btn_KP_clicked();

    void on_Btn_KI_clicked();

    void on_Btn_KD_clicked();

    void on_Btn_temp_target_clicked();

    void on_Btn_temp_up_clicked();

    void on_Btn_temp_down_clicked();

    void on_Btn_diss_curve_start_clicked();

    void on_Btn_diss_curve_end_clicked();

    void on_Btn_diss_curve_max_clicked();

    //void on_Btn_send_data_clicked();

    void on_comboBox_F_Dyestuff_activated(int index);

    void on_Btn_adc_report_clicked();

    void on_Btn_led_intens_clicked();

    void on_Btn_adc_rate_clicked();

    void on_Btn_adc_nu_clicked();

    void on_Btn_led_gap_clicked();

    void on_Btn_delay_time_clicked();

    void on_Btn_channel_clicked();

    void on_Btn_calcul_nu_clicked();

//    void on_comboBox_M_FunID_activated(int index);

    void on_Btn_hot_lid_clicked();

//    void on_Btn_temp_compen_clicked();

    void on_comboBox_M_Channel_activated(int index);

    void on_comboBox_T_Channel_currentIndexChanged(int index);

    void on_comboBox_F_Channel_currentIndexChanged(int index);

    void on_Btn_set_mode_clicked();

    void on_Btn_get_mode_clicked();

    void on_Btn_set_fluo_clicked();

    void on_Btn_get_fluo_clicked();

    void on_Btn_coll_fluo_clicked();

    void on_Btn_elec_set_point_clicked();

//    void on_Btn_elec_get_point_clicked();

//    void on_pushButton_clicked();

//    void on_Chb_passageway_open_clicked(bool checked);

//    void on_Chb_passageway_close_clicked(bool checked);

    void on_Btn_temp_comp_get_clicked();

    void on_Btn_temp_target_get_clicked();

    void on_Btn_temp_up_get_clicked();

    void on_Btn_temp_down_get_clicked();

    void on_Btn_diss_curve_start_get_clicked();

    void on_Btn_diss_curve_endget_clicked();

    void on_Btn_diss_curve_max_get_clicked();

    void on_Btn_adc_report_get_clicked();

    void on_Btn_calcul_nu_get_clicked();

    void on_Btn_software_ver_clicked();

    void on_Btn_hardware_ver_clicked();

    void on_Btn_production_date_clicked();

    void on_Btn_led_intens_get_clicked();

    void on_Btn_adc_rate_get_clicked();

    void on_Btn_adc_nu_get_clicked();

    void on_Btn_led_gap_get_clicked();

    void on_Btn_delay_time_get_clicked();

    void on_Btn_channel_get_clicked();

    void on_Btn_software_ver_get_clicked();

    void on_Btn_hardware_ver_get_clicked();

    void on_Btn_production_date_get_clicked();

//    void on_Btn_fan_ctrl_open_clicked();

//    void on_Btn_fan_ctrl_close_clicked();

    void on_Btn_hot_lid_temp_wr_clicked();

    void on_Btn_hot_lid_temp_wr_get_clicked();

    void on_Btn_hot_lid_get_clicked();

//    void on_Btn_lid_ctrl_open_clicked();

//    void on_Btn_lid_ctrl_close_clicked();

    void on_Btn_base_temp_wr_clicked();

    void on_Btn_base_temp_wr_get_clicked();

//    void on_Chb_passageway_open_clicked();

//    void on_Chb_passageway_close_clicked();

    void on_Btn_passageway_get_clicked();

    void on_Btn_KP_get_clicked();

    void on_Btn_KI_get_clicked();

    void on_Btn_KD_get_clicked();

    void on_Cbx_fan_ctrl_open_clicked();

    void on_Cbx_fan_ctrl_close_clicked();

    void on_Chb_lid_ctrl_open_clicked();

    void on_Chb__lid_ctrl_close_clicked();

    void on_comboBox_T_Channel_currentTextChanged(const QString &arg1);

    void on_Chb_passageway_open_B_clicked();

    void on_Chb_passageway_close_B_clicked();

    void on_Chb_passageway_open_A_clicked();

    void on_Chb_passageway_close_A_clicked();

    void on_Btn_clear_get_clicked();

    void on_Btn_fluo_Best_location_clicked();

    void on_Btn_LED_stren_clicked();

    void on_Chb_fluo_auto_acqu_clicked(bool checked);

    void on_Btn_get_curve_data_clicked();


private:
    //串口消息封装
    void    ReceiveCmd(SRequestCmdInfo cmdInfo);
    void    ReceiveCmd(void);
    //设置荧光界面控件状态
    void SetFluorControl(void);
    //设置主控模块界面控件状态
    void SetMainControl(void);
    //初始化时获取设备状态
    void GetInitDeviceState(void);
    void SetObjectEnabled(void);
    void GetData(void);
    quint8   Cal_CRC(quint8 *ptr, quint32 len);
    //设置显示串口接受到的信息
    void SetTexditData(const QString info);
    //温控串口接受函数
    void GetTempControl(SRequestCmdInfo cmdInfo);
    //荧光串口接受函数
    void GetFluorControl(SRequestCmdInfo cmdInfo);
    //主控界面串口接受函数
    void GetMainControl(SRequestCmdInfo cmdInfo);
    //调试界面串口接受函数
    void GetDebugControl(SRequestCmdInfo cmdInfo);
private:
    Ui::MainWindow *ui;


    MyThrtead_T *mythread_t;
    QList<quint32> fluo_size;
    int         fluo_check;
};

#endif // MAINWINDOW_H
