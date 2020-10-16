#include "mainwindow.h"
#include "ui_mainwindow.h"


#pragma execution_character_set("utf-8")
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tab->setEnabled(true);
    ui->tab_2->setEnabled(false);
    ui->tab_3->setEnabled(false);
    ui->tab_4->setEnabled(false);
    ui->tab_5->setEnabled(false);

    InitSerial();


    ui->Led_hard_number->setReadOnly(true);
    ui->Led_soft_number->setReadOnly(true);
    ui->Led_eles_get_point->setReadOnly(true);
    ui->Led_eles_get_point->setReadOnly(true);
    check_cmd=false;
    check_size=0;
    check_error=0;
    requescmdInfo.uUartCmd.ucCmd[0]=0;
    requescmdInfo.uUartCmd.ucCmd[1]=0;
    requescmdInfo.uUartCmd.ucCmd[2]=0;
    requescmdInfo.uUartCmd.ucCmd[3]=0;
    requescmdInfo.uUartCmd.ucCmd[4]=0;
    requescmdInfo.uUartCmd.ucCmd[5]=0;
    requescmdInfo.uUartCmd.ucCmd[6]=0;
    requescmdInfo.uUartCmd.ucCmd[7]=0;

    requescmdInfo.baCmdContent="";


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitSerial()
{

    ui->tabWidget->setCurrentIndex(1);
//    ui->tabWidget-
    ui->tabWidget->setCurrentIndex(0);

    SerialNode=new Q268Driver;
    connect(SerialNode,SIGNAL(SendUsartData2Mcu(SRequestCmdInfo)),this,SLOT(SlotUsartData2Mcu(SRequestCmdInfo)));
    connect(this,SIGNAL(Sign_SendDriver(QByteArray)),SerialNode,SLOT(SlotGetUpDriver(QByteArray)));

    SetObjectEnabled();
    driver_data="";
    send_data.clear();

    send_node_tmp.Head=0;
    send_node_tmp.Len=0;
    send_node_tmp.CRC=0;
    send_node_tmp.Ack=0;
    send_node_tmp.Data.clear();
    check_curve=0;
    curve_point=0;
    check_curve_flou=0;
    curve_point_flou=0;



    memset(static_cast<void *>(&send_node_tmp.CMD),0,sizeof (send_node_tmp.CMD));
    memset(static_cast<void *>(&send_node_tmp.CH),0,sizeof (send_node_tmp.CH));
    memset(static_cast<void *>(&send_node_tmp.ThiCMD),0,sizeof (send_node_tmp.ThiCMD));


     memset(static_cast<void *>(&requescmdInfo.uUartCmd),0,sizeof (requescmdInfo.uUartCmd));
     requescmdInfo.baCmdContent.clear();

    memset(static_cast<void *>(&m_cmd_bit),0,sizeof (m_cmd_bit));
    memset(static_cast<void *>(&m_cH),0,sizeof (m_cmd_bit));
    memset(static_cast<void *>(&m_thiCMD),0,sizeof (m_cmd_bit));




    ui->Texdit_accept->setReadOnly(true);
    QRegExp regExp("[0-9]{0,2}[.][0-9]{0,2}");
    ui->Line_temp_comp_set->setValidator(new QRegExpValidator(regExp,this));
    ui->Line_KP_set->setValidator(new QRegExpValidator(regExp,this));
    ui->Line_KI_set->setValidator(new QRegExpValidator(regExp,this));
    ui->Line_KD_set->setValidator(new QRegExpValidator(regExp,this));
    ui->Line_temp_target_set->setValidator(new QRegExpValidator(regExp,this));
    ui->Line_temp_up_set->setValidator(new QRegExpValidator(regExp,this));
    ui->Line_temp_down_set->setValidator(new QRegExpValidator(regExp,this));
    ui->Line_diss_curve_start_set->setValidator(new QRegExpValidator(regExp,this));
    ui->Line_diss_curve_end_set->setValidator(new QRegExpValidator(regExp,this));
    ui->Line_diss_curve_max_set->setValidator(new QRegExpValidator(regExp,this));

//    ui->Wdg_T_mode->Init();
    ui->Wdg_T_mode->AddLine("temp_1");
    ui->Wdg_T_mode->AddLine("temp_2");
    ui->Wdg_T_mode->AddLine("temp_3");
    ui->Wdg_T_mode->AddLine("temp_4");
    ui->Wdg_F_mode->AddLine("flou_1");
    ui->Wdg_F_mode->AddLine("flou_2");
    ui->Wdg_F_mode->AddLine("flou_3");
    ui->Wdg_F_mode->AddLine("flou_4");
    ui->Wdg_debug->AddLine("flou_1");
    ui->Wdg_debug->AddLine("flou_2");
    ui->Wdg_debug->AddLine("flou_3");
    ui->Wdg_debug->AddLine("flou_4");




}
//串口消息接受函数
void MainWindow::SlotUsartData2Mcu(SRequestCmdInfo cmdInfo)
{
    qDebug()<<"串口消息接受函数"<<cmdInfo.uUartCmd.sUartCmd.unStartTag;
    qDebug()<<"串口消息接受函数 unFunId"<<cmdInfo.uUartCmd.sUartCmd.unFunId;
    qDebug()<<"串口消息接受函数 unRorW"<<cmdInfo.uUartCmd.sUartCmd.unRorW;
    qDebug()<<"串口消息接受函数 unModuleType"<<cmdInfo.uUartCmd.sUartCmd.unModuleType;
    qDebug()<<"串口消息接受函数 unCh"<<cmdInfo.uUartCmd.sUartCmd.unCh;
//    if(cmdInfo.uUartCmd.sUartCmd.unStatusAck == 0)
//    {
//        return;
//    }
    switch (cmdInfo.uUartCmd.sUartCmd.unModuleType)
    {
        //温控
    case 1:
        GetTempControl(cmdInfo);
        break;
        //荧光
    case 2:
        GetFluorControl(cmdInfo);
        break;
        //主控
    case 3:
        GetMainControl(cmdInfo);
        break;
        //系统调试
    case 4:
        GetDebugControl(cmdInfo);
        break;
        //错误
    default:
        break;
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
 //   qDebug()<<"ui->Wdg_T_mode->size()"<<ui->Wdg_T_mode->size()<<event->size();
    ui->Wdg_T_mode->Updatasize();
    ui->Wdg_F_mode->Updatasize();
    ui->Wdg_debug->Updatasize();

}
quint8 MainWindow::Cal_CRC(quint8 *ptr, quint32 len)
{
   // qDebug()<<"ptr"<<*ptr;
    quint8 crc;
    quint8 i;
    crc = 0;
    while (len--)
    {
        crc ^= *ptr++;
        for (i = 0; i < 8; i++)
        {
            if (crc & 0x01)
            {
                crc = (crc >> 1) ^ 0x8C;
            }
            else crc >>= 1;
        }
    }
   // qDebug()<<"crc"<<crc;
    return crc;
}


void MainWindow::on_comboBox_Module_activated(int index)
{
   // qDebug()<<"on_comboBox_Module_activated ui->Wdg_T_mode->size()"<<ui->Wdg_T_mode->size();

    ui->tabWidget->setCurrentIndex(index+1);
    switch (ui->comboBox_Module->currentIndex())
    {
    case 0:
        ui->tab_2->setEnabled(true);
        ui->tab_3->setEnabled(false);
        ui->tab_4->setEnabled(false);
        ui->tab_5->setEnabled(false);
        break;
    case 1:
        ui->tab_2->setEnabled(false);
        ui->tab_3->setEnabled(true);
        ui->tab_4->setEnabled(false);
        ui->tab_5->setEnabled(false);
//        ui->Wdg_T_mode->Updatasize();
        break;
    case 2:
        ui->tab_2->setEnabled(false);
        ui->tab_3->setEnabled(false);
        ui->tab_4->setEnabled(true);
        ui->tab_5->setEnabled(false);
        break;
    case 3:
        ui->tab_2->setEnabled(false);
        ui->tab_3->setEnabled(false);
        ui->tab_4->setEnabled(false);
        ui->tab_5->setEnabled(true);
        break;
    default:
        ui->tab_2->setEnabled(false);
        ui->tab_3->setEnabled(false);
        ui->tab_4->setEnabled(false);
        ui->tab_5->setEnabled(false);
        break;
    }

    SetObjectEnabled();
    GetData();
//    ui->Wdg_T_mode->m_size=ui->Wdg_T_mode->size();
    ui->Wdg_T_mode->Updatasize();
    ui->Wdg_F_mode->Updatasize();
    ui->Wdg_debug->Updatasize();
//    ui->Wdg_T_mode->AddLine("yitiji");
//    for (int i=0;i<50;i++)
//    {
//        ui->Wdg_T_mode->SLot_AddPointToCurve(0,i,i+i%5);
//    }

}

void MainWindow::on_Btn_update_head_clicked()
{
    //填充数据帧头
    switch (ui->tabWidget->currentIndex())
    {
    case 0:
        return;
        //温控
    case 1:
//        requescmdInfo.uUartCmd.sUartCmd.unFunId=m_cmd_bit.FunID;
//        requescmdInfo.uUartCmd.sUartCmd.unRorW=m_cmd_bit.FunRW;
//        requescmdInfo.uUartCmd.sUartCmd.unNodeType=m_cmd_bit.NodeType;

//        requescmdInfo.uUartCmd.sUartCmd.unCh= static_cast<quint32>(m_cH.Channel<<3)+m_cH.TEC_ID;
//        requescmdInfo.uUartCmd.sUartCmd.unModuleType=m_cH.Module;
//        requescmdInfo.uUartCmd.sUartCmd.unSubCmd=static_cast<quint32>(m_thiCMD.TempGap<<4)+m_thiCMD.ThiCmd;
        break;
        //荧光
    case 2:
        switch (m_cmd_bit.FunID)
        {
        case 5:
        case 7:
            requescmdInfo.uUartCmd.sUartCmd.unRorW=1;
            break;
        default:
            requescmdInfo.uUartCmd.sUartCmd.unRorW=0;
            break;
        }
//        requescmdInfo.uUartCmd.sUartCmd.unModuleType=m_cH.Module;
        break;
        //主控制
    case 3:
        break;
        //系统调试
    case 5:
        break;
    default:
        return;
    }
    requescmdInfo.uUartCmd.sUartCmd.unStartTag=0xff;
    requescmdInfo.uUartCmd.sUartCmd.unDataLen_empty=0xff;

    requescmdInfo.uUartCmd.sUartCmd.unDataLen=sizeof(requescmdInfo.uUartCmd) + static_cast<quint32>(requescmdInfo.baCmdContent.size());
    requescmdInfo.uUartCmd.sUartCmd.unCrc=0;

    requescmdInfo.uUartCmd.sUartCmd.unFunId=m_cmd_bit.FunID;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=m_cmd_bit.FunRW;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=m_cmd_bit.NodeType;

    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;

    requescmdInfo.uUartCmd.sUartCmd.unCh= static_cast<quint32>(m_cH.Channel<<3)+m_cH.TEC_ID;
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=m_cH.Module;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=static_cast<quint32>(m_thiCMD.TempGap<<4)+m_thiCMD.ThiCmd;

    send_data.clear();
    for (int i=0; i<8; i++)
    {
        send_data.append(static_cast<char>(requescmdInfo.uUartCmd.ucCmd[i]));
    }
    send_data.append(requescmdInfo.baCmdContent);
    quint8  *pPacketData = (quint8 *)(send_data.data());
//    char crc_temp = static_cast<char>(Cal_CRC(pPacketData+4, static_cast<quint32>(send_data.size()-4)));
     quint8 crc_temp = Cal_CRC(pPacketData+4, static_cast<quint32>(send_data.size()-4));
    send_data.remove(3,1);
    send_data.insert(3,static_cast<char>(crc_temp));
    ui->Line_frame_head_by1->setText(QString("%1").arg(requescmdInfo.uUartCmd.sUartCmd.unDataLen));
    ui->Line_frame_head_by2->setText(QString("%1").arg(static_cast<int>(crc_temp)));

}

void MainWindow::SetObjectEnabled()
{
    ui->Line_temp_comp_set->setEnabled(false);
    ui->Line_KP_set->setEnabled(false);
    ui->Line_KI_set->setEnabled(false);
    ui->Line_KD_set->setEnabled(false);
    ui->Btn_temp_comp->setEnabled(false);
    ui->Btn_KP->setEnabled(false);
    ui->Btn_KI->setEnabled(false);
    ui->Btn_KD->setEnabled(false);

    ui->Line_temp_target_set->setEnabled(false);
    ui->Btn_temp_target->setEnabled(false);

    ui->Line_temp_up_set->setEnabled(false);
    ui->Line_temp_down_set->setEnabled(false);
    ui->Btn_diss_curve->setEnabled(false);
    ui->Btn_temp_up->setEnabled(false);
    ui->Btn_temp_down->setEnabled(false);

    ui->Line_diss_curve_start_set->setEnabled(false);
    ui->Line_diss_curve_end_set->setEnabled(false);
    ui->Line_diss_curve_max_set->setEnabled(false);
    ui->Btn_diss_curve_start->setEnabled(false);
    ui->Btn_diss_curve_end->setEnabled(false);
    ui->Btn_diss_curve_max->setEnabled(false);


//    ui->Line_adc_report_set->setEnabled(false);
//    ui->Btn_adc_report->setEnabled(false);
    ui->comboBox_F_Modeoption->setEnabled(false);

    SetFluorControl();
    SetMainControl();
//    ui->Line_led_intens_set->setEnabled(false);
//    ui->Btn_led_intens->setEnabled(false);
//    ui->Line_adc_rate_set->setEnabled(false);
//    ui->Btn_adc_rate->setEnabled(false);
//    ui->Line_adc_nu_set->setEnabled(false);
//    ui->Btn_adc_nu->setEnabled(false);

//    ui->Line_led_gap_set->setEnabled(false);
//    ui->Btn_led_gap->setEnabled(false);
//    ui->Line_delay_time_set->setEnabled(false);
//    ui->Btn_delay_time->setEnabled(false);
//    ui->Line_channel_set->setEnabled(false);
//    ui->Btn_channel->setEnabled(false);
//    ui->Line_calcul_nu_set->setEnabled(false);
//    ui->Btn_calcul_nu->setEnabled(false);
}

void MainWindow::GetData()
{
    //判断当前页面
    switch (ui->tabWidget->currentIndex())
    {
    case 0:
        break;
        //温控
    case 1:
        //
        m_cmd_bit.FunID=static_cast<quint8>(ui->comboBox_T_FunID->currentIndex()+1);
        m_cmd_bit.FunRW= static_cast<quint8>(1);
        m_cmd_bit.NodeType=static_cast<quint8>(7);

        m_cH.Channel=static_cast<quint8>(ui->comboBox_T_Channel->currentIndex()<<3);
        m_cH.TEC_ID=static_cast<quint8>(ui->comboBox_T_TEC_ID->currentIndex()+1);
        m_cH.Module=static_cast<quint8>(1);

        m_thiCMD.TempGap= static_cast<quint8>(0);
        m_thiCMD.ThiCmd= static_cast<quint8>(0);
        break;
        //荧光
    case 2:
        m_cmd_bit.FunID=static_cast<quint8>(ui->comboBox_F_FunID->currentIndex()+1);
        m_cmd_bit.FunRW= static_cast<quint8>(0);
        m_cmd_bit.NodeType=static_cast<quint8>(7);

        m_cH.Channel=static_cast<quint8>(ui->comboBox_F_Channel->currentIndex()<<3);
        m_cH.TEC_ID=0;
        m_cH.Module=static_cast<quint8>(2);

        m_thiCMD.TempGap= static_cast<quint8>(0);
        m_thiCMD.ThiCmd= static_cast<quint8>(0);
        break;
        //主控
    case 3:
        m_cmd_bit.FunID=static_cast<quint8>(ui->comboBox_F_FunID->currentIndex()+1);
        m_cmd_bit.FunRW= static_cast<quint8>(0);
        m_cmd_bit.NodeType=static_cast<quint8>(7);

        m_cH.Channel=static_cast<quint8>(ui->comboBox_F_Channel->currentIndex()<<3);
        m_cH.TEC_ID=0;
        m_cH.Module=static_cast<quint8>(3);

        m_thiCMD.TempGap= static_cast<quint8>(0);
        m_thiCMD.ThiCmd= static_cast<quint8>(0);
        break;
        //调试
    case 4:
        break;
    default:
        break;

    }

}

void MainWindow::SetTexditData(const QString info)
{
    QDateTime cueernt_date=QDateTime::currentDateTime();
    QString current_time=cueernt_date.toString("yyy-MM-dd hh:mm:ss");
    ui->Texdit_accept->append(current_time+"_"+info);
}

void MainWindow::GetTempControl(SRequestCmdInfo cmdInfo)
{
//    if(cmdInfo.uUartCmd.sUartCmd.unStatusAck == 0)
//    {
//        return;
//    }

//    if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 != ui->comboBox_T_Channel->currentIndex())
//    {
//        return;
//    }
//    SetTexditData(tr("收到数据为：")+QString(cmdInfo.baCmdContent));
    switch (cmdInfo.uUartCmd.sUartCmd.unFunId)
    {
    case 1:
        break;
    case 2:
        break;
        //读取目标设定温度
    case 3:
        ui->Line_temp_targe->setText(QString(cmdInfo.baCmdContent));
        //绘制温度监控曲线
        ui->Wdg_T_mode->SLot_AddPointToCurve(check_curve%4,curve_point/4,QString(cmdInfo.baCmdContent).toDouble());
        check_curve++;
        curve_point++;
       // qDebug()<<"check_curve="<<check_curve<<"curve_point="<<curve_point<<"check_curve%4="<<check_curve%4<<"curve_point/4="<<curve_point/4;

        break;
    case 4:
        break;
        //模块参数配置
    case 5:
        switch (cmdInfo.uUartCmd.sUartCmd.unSubCmd)
        {
        case 1:
            //设置温补
            ui->Line_temp_comp->setText(QString(cmdInfo.baCmdContent));
            break;
        case 2:
            ui->Line_KP->setText(QString(cmdInfo.baCmdContent));
            break;
        case 3:
            ui->Line_KI->setText(QString(cmdInfo.baCmdContent));
            break;
        case 4:
            ui->Line_KD->setText(QString(cmdInfo.baCmdContent));
            break;
        default:
            break;
        }
        break;
        //实验参数设置
    case 6:
        switch (cmdInfo.uUartCmd.sUartCmd.unSubCmd)
        {
        case 1:
            //溶解曲线类型

            break;
        case 2:
            ui->Line_temp_up->setText(QString(cmdInfo.baCmdContent));
            break;
        case 3:
            ui->Line_temp_down->setText(QString(cmdInfo.baCmdContent));
            break;
        case 4:
            ui->Line_diss_curve_start->setText(QString(cmdInfo.baCmdContent));
            break;
        case 5:
            ui->Line_diss_curve_end->setText(QString(cmdInfo.baCmdContent));
            break;
        case 6:
            ui->Line_diss_curve_max->setText(QString(cmdInfo.baCmdContent));
            break;
        default:
            break;
        }
        break;
        //上报的温度
    case 7:
//        显示当前上报的温度值
        SetTexditData(tr("设备当前温度：")+QString(cmdInfo.baCmdContent));
        break;
        //错误状态
    case 8:
        SetTexditData(tr("设备出错，错误信息：")+QString(cmdInfo.baCmdContent));
        break;
    default:
        break;
    }
}

void MainWindow::GetFluorControl(SRequestCmdInfo cmdInfo)
{
    //    if(cmdInfo.uUartCmd.sUartCmd.unStatusAck == 0)
    //    {
    //        return;
    //    }
    //判断传送上来的A、B模块数据是否和当前选择的模块一致
//        if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 != ui->comboBox_T_Channel->currentIndex())
//        {
//            return;
//        }
//    SetTexditData(tr("收到数据为：")+QString(cmdInfo.baCmdContent));
    switch (cmdInfo.uUartCmd.sUartCmd.unFunId)
    {
    //开始采集荧光
    case 1:
        break;
        //荧光数据报告
    case 2:
        //绘制荧光曲线
        ui->Wdg_F_mode->SLot_AddPointToCurve(check_curve_flou%4,curve_point_flou/4,QString(cmdInfo.baCmdContent).toDouble());
        check_curve_flou++;
        curve_point_flou++;



        break;
        //ADC数据上报
    case 3:
        break;
        //光强数据上报
    case 4:
        break;
        //模块参数设置
    case 5:
        switch (cmdInfo.uUartCmd.sUartCmd.unSubCmd)
        {
        case 1:
            ui->Line_led_intens->setText(QString(cmdInfo.baCmdContent));
            break;
        case 2:
            ui->Line_adc_rate->setText(QString(cmdInfo.baCmdContent));
            break;
        case 3:
            ui->Line_adc_nu->setText(QString(cmdInfo.baCmdContent));
            break;
        case 4:
            ui->Line_led_gap->setText(QString(cmdInfo.baCmdContent));
            break;
        case 5:
            ui->Line_delay_time->setText(QString(cmdInfo.baCmdContent));
            break;
        case 6:
            ui->Line_channel->setText(QString(cmdInfo.baCmdContent));
            break;
        case 7:
            ui->Line_calcul_nu->setText(QString(cmdInfo.baCmdContent));
            break;
        }
        break;
        //错误状态上报
    case 6:
        break;
        //连续采样次数设置
    case 7:
        ui->Line_adc_report->setText(QString(cmdInfo.baCmdContent));
        break;
        //荧光模块测试
    case 8:
        break;
    default:
        break;

    }
}

void MainWindow::GetMainControl(SRequestCmdInfo cmdInfo)
{
    //    if(cmdInfo.uUartCmd.sUartCmd.unStatusAck == 0)
    //    {
    //        return;
    //    }
    //判断传送上来的A、B模块数据是否和当前选择的模块一致
//        if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 != ui->comboBox_T_Channel->currentIndex())
//        {
//            return;
//        }
//    SetTexditData(tr("收到数据为：")+QString(cmdInfo.baCmdContent));
    switch (cmdInfo.uUartCmd.sUartCmd.unFunId)
    {
    //串口握手
    case 1:
        //显示硬件号和软件版本号
    {
        QList<DataContent> cmddata;
        for (int i=0; i<cmdInfo.baCmdContent.size()/4; i++) {
            DataContent tempdata;
            tempdata.m_uint8data[0] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+0));
            tempdata.m_uint8data[1] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+1));
            tempdata.m_uint8data[2] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+2));
            tempdata.m_uint8data[3] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+3));
            cmddata.append(tempdata);
        }
        ui->Led_soft_number->setText(QString::number(cmddata[0].m_uint32data));
        ui->Led_hard_number->setText(QString::number(cmddata[1].m_uint32data));
    }


        break;
         //风扇控制
    case 2:

        ui->Line_fan_ctrl->setText(QString(cmdInfo.baCmdContent));
        break;
         //报告风扇状态
    case 3:
        break;
         //热盖控制
    case 4:

        ui->Line_lid_ctrl->setText(QString(cmdInfo.baCmdContent));
        break;
         //报告热改状态
    case 5:
        break;
         //实验过程通知
    case 6:
        break;
         //仪器参数读写
    case 7:
        break;
         //热盖设定温度的读写
    case 8:

        ui->Line_hot_lid->setText(QString(cmdInfo.baCmdContent));
        break;
         //温度补偿参数读写
    case 9:

        ui->Line_temp_compen->setText(QString(cmdInfo.baCmdContent));
        break;
         //仪器错误报告
    case 10:
        break;
         //读取铜座温度
    case 11:
        break;
    }
}

void MainWindow::GetDebugControl(SRequestCmdInfo cmdInfo)
{
    QList<DataContent> cmddata;
    cmddata.clear();
    DataContent tempdata;
    switch (cmdInfo.uUartCmd.sUartCmd.unFunId)
    {
    //修改仪器运行模式
    case 1:
        for (int i=0; i<cmdInfo.baCmdContent.size()/4; i++) {
            tempdata.m_uint8data[0] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+0));
            tempdata.m_uint8data[1] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+1));
            tempdata.m_uint8data[2] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+2));
            tempdata.m_uint8data[3] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+3));
            cmddata.append(tempdata);
        }
//        SetTexditData(tr("仪器当前运行模式为：")+QString(cmddata[0].m_uint32data));
        if(cmddata[0].m_uint32data == 1)
        {
           SetTexditData(tr("仪器当前运行模式为：正常模式"));
        }
        else
        {
           SetTexditData(tr("仪器当前运行模式为：调试模式"));
        }
        break;
        //采集荧光
    case 2:
        check_cmd=true;
        for (int i=0; i<cmdInfo.baCmdContent.size()/4; i++) {
            tempdata.m_uint8data[0] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+0));
            tempdata.m_uint8data[1] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+1));
            tempdata.m_uint8data[2] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+2));
            tempdata.m_uint8data[3] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+3));
            cmddata.append(tempdata);
        }
        SetTexditData(tr("获取到的荧光值为：")+QString::number(cmddata[0].m_uint32data));
        break;
        //荧光采集位置的读取和保存
    case 3:

        for (int i=0; i<cmdInfo.baCmdContent.size()/4; i++) {
            tempdata.m_uint8data[0] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+0));
            tempdata.m_uint8data[1] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+1));
            tempdata.m_uint8data[2] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+2));
            tempdata.m_uint8data[3] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+3));
            cmddata.append(tempdata);
        }
        if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 0)
        {
            SetTexditData(tr("当前电机运转方向为：正向"));
        }
        else  if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
        {
            SetTexditData(tr("当前电机运转方向为：反向"));
        }

//        SetTexditData(tr("获取到的荧光采集位置：")+QString("%1").arg( cmddata[0].m_uint32data));
        SetTexditData(tr("获取到的荧光采集位置：")+QString::number(cmddata[0].m_uint32data));
        break;
        //电机运动
    case 4:
    {
       // QList<DataContent> cmddata;
        for (int i=0; i<cmdInfo.baCmdContent.size()/4; i++) {
          //  DataContent tempdata;
//            tempdata.m_uint8data[0] = cmdInfo.baCmdContent.at(i*4+0);
//            tempdata.m_uint8data[1] = cmdInfo.baCmdContent.at(i*4+1);
//            tempdata.m_uint8data[2] = cmdInfo.baCmdContent.at(i*4+2);
//            tempdata.m_uint8data[3] = cmdInfo.baCmdContent.at(i*4+3);
            tempdata.m_uint8data[0] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+0));
            tempdata.m_uint8data[1] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+1));
            tempdata.m_uint8data[2] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+2));
            tempdata.m_uint8data[3] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+3));
            cmddata.append(tempdata);
        }
        SetTexditData(tr("电机当前位置：")+QString::number(cmddata[0].m_uint32data));
        ui->Led_eles_get_point->setText(QString::number(cmddata[0].m_uint32data));
        break;
    }
    default:
        break;

    }
}


void MainWindow::on_comboBox_T_FunID_activated(int index)
{

    SetObjectEnabled();
    m_cmd_bit.FunID=static_cast<quint8>(index+1);
     m_cmd_bit.FunRW=0;
    switch (index)
    {
    //打开/关闭通道
    case 0:
        break;
    case 1:
        break;
        //目标温度设置
    case 2:
        ui->Line_temp_target_set->setEnabled(true);
        ui->Btn_temp_target->setEnabled(true);
        break;
        //温度上报
    case 3:
        break;
        //模块参数配置
    case 4:
        ui->Line_temp_comp_set->setEnabled(true);
        ui->Line_KP_set->setEnabled(true);
        ui->Line_KI_set->setEnabled(true);
        ui->Line_KD_set->setEnabled(true);
        ui->Btn_temp_comp->setEnabled(true);
        ui->Btn_KP->setEnabled(true);
        ui->Btn_KI->setEnabled(true);
        ui->Btn_KD->setEnabled(true);
        break;
        //实验参数配置
    case 5:
        ui->Line_temp_up_set->setEnabled(true);
        ui->Line_temp_down_set->setEnabled(true);
        ui->Btn_diss_curve->setEnabled(true);
        ui->Btn_temp_up->setEnabled(true);
        ui->Btn_temp_down->setEnabled(true);

        ui->Line_diss_curve_start_set->setEnabled(true);
        ui->Line_diss_curve_end_set->setEnabled(true);
        ui->Line_diss_curve_max_set->setEnabled(true);
        ui->Btn_diss_curve_start->setEnabled(true);
        ui->Btn_diss_curve_end->setEnabled(true);
        ui->Btn_diss_curve_max->setEnabled(true);
        break;
        //温度上报 测试时使用
    case 6:
        break;
        //错误状态
    case 7:
        break;
    }

}

//选择温度传感器编号
void MainWindow::on_comboBox_T_TEC_ID_activated(int index)
{
     m_cH.TEC_ID=static_cast<quint8>(index+1);
}

//设置温度补偿值
void MainWindow::on_Btn_temp_comp_clicked()
{
    if(ui->Line_temp_comp_set->text().isEmpty())
    {
        return;
    }

    m_thiCMD.ThiCmd= static_cast<quint8>(1);
     m_cmd_bit.FunRW=1;

    requescmdInfo.baCmdContent=ui->Line_temp_comp_set->text().toLocal8Bit();
    SetTexditData(tr("设置温度补偿值：")+ui->Line_temp_comp_set->text());
    ui->Line_temp_comp->setText(ui->Line_temp_comp_set->text());
}

void MainWindow::on_Btn_KP_clicked()
{
    if(ui->Line_KP_set->text().isEmpty())
    {
        return;
    }

    m_thiCMD.ThiCmd= static_cast<quint8>(2);
     m_cmd_bit.FunRW=1;

     requescmdInfo.baCmdContent=ui->Line_KP_set->text().toLocal8Bit();
    SetTexditData(tr("设置KP值：")+ui->Line_KP_set->text());
    ui->Line_KP->setText(ui->Line_KP_set->text());
}

void MainWindow::on_Btn_KI_clicked()
{
    if(ui->Line_KI_set->text().isEmpty())
    {
        return;
    }

    m_thiCMD.ThiCmd= static_cast<quint8>(3);
     m_cmd_bit.FunRW=1;

    requescmdInfo.baCmdContent=ui->Line_KI_set->text().toLocal8Bit();

    SetTexditData(tr("设置KI值：")+ui->Line_KI_set->text());
    ui->Line_KI->setText(ui->Line_KI_set->text());
}

void MainWindow::on_Btn_KD_clicked()
{
    if(ui->Line_KD_set->text().isEmpty())
    {
        return;
    }

    m_thiCMD.ThiCmd= static_cast<quint8>(4);
     m_cmd_bit.FunRW=1;

     requescmdInfo.baCmdContent=ui->Line_KD_set->text().toLocal8Bit();
    SetTexditData(tr("设置KD值：")+ui->Line_KD_set->text());
    ui->Line_KD->setText(ui->Line_KD_set->text());
}
//设置目标温度 Line_temp_target_set
void MainWindow::on_Btn_temp_target_clicked()
{
    if(ui->Line_temp_target_set->text().isEmpty())
    {
        return;
    }

    m_cmd_bit.FunRW=1;
    requescmdInfo.baCmdContent=ui->Line_temp_target_set->text().toLocal8Bit();
    SetTexditData(tr("设置目标温度值：")+ui->Line_temp_target_set->text());
    ui->Line_temp_targe->setText(ui->Line_temp_target_set->text());
}
//设置溶解曲线类型(溶解曲线只下发命令，让下位机切换溶解PID;扩增命令也是只下发，让下位机切换扩增PID，都不带参数)
void MainWindow::on_Btn_diss_curve_clicked()
{

    m_thiCMD.ThiCmd= static_cast<quint8>(1);
     m_cmd_bit.FunRW=1;
    SetTexditData(tr("设置溶解曲线类型"));

}

void MainWindow::on_Btn_temp_up_clicked()
{
    if(ui->Line_temp_up_set->text().isEmpty())
    {
        return;
    }

    m_thiCMD.ThiCmd= static_cast<quint8>(2);
     m_cmd_bit.FunRW=1;

    requescmdInfo.baCmdContent=ui->Line_temp_up_set->text().toLocal8Bit();
    SetTexditData(tr("设置升温速率：")+ui->Line_temp_up_set->text());
    ui->Line_temp_up->setText(ui->Line_temp_up_set->text());
}

void MainWindow::on_Btn_temp_down_clicked()
{
    if(ui->Line_temp_down_set->text().isEmpty())
    {
        return;
    }

    m_thiCMD.ThiCmd= static_cast<quint8>(3);
     m_cmd_bit.FunRW=1;

    requescmdInfo.baCmdContent=ui->Line_temp_down_set->text().toLocal8Bit();
    SetTexditData(tr("设置降温速率：")+ui->Line_temp_down_set->text());
    ui->Line_temp_down->setText(ui->Line_temp_down_set->text());
}
//溶解曲线起始值
void MainWindow::on_Btn_diss_curve_start_clicked()
{
    if(ui->Line_diss_curve_start_set->text().isEmpty())
    {
        return;
    }
        m_thiCMD.ThiCmd= static_cast<quint8>(4);
         m_cmd_bit.FunRW=1;

   requescmdInfo.baCmdContent=ui->Line_diss_curve_start_set->text().toLocal8Bit();
    SetTexditData(tr("设置曲线起始值：")+ui->Line_diss_curve_start_set->text());
    ui->Line_diss_curve_start->setText(ui->Line_diss_curve_start_set->text());
}

void MainWindow::on_Btn_diss_curve_end_clicked()
{
    if(ui->Line_diss_curve_end_set->text().isEmpty())
    {
        return;
    }
      m_thiCMD.ThiCmd= static_cast<quint8>(5);
       m_cmd_bit.FunRW=1;

    requescmdInfo.baCmdContent=ui->Line_diss_curve_end_set->text().toLocal8Bit();
    SetTexditData(tr("设置曲线终止值：")+ui->Line_diss_curve_end_set->text());
    ui->Line_diss_curve_end->setText(ui->Line_diss_curve_end_set->text());
}

void MainWindow::on_Btn_diss_curve_max_clicked()
{
    if(ui->Line_diss_curve_max_set->text().isEmpty())
    {
        return;
    }

    m_thiCMD.ThiCmd= static_cast<quint8>(6);
    m_cmd_bit.FunRW=1;

    requescmdInfo.baCmdContent=ui->Line_diss_curve_max_set->text().toLocal8Bit();
    SetTexditData(tr("设置溶解曲线最大值：")+ui->Line_diss_curve_max_set->text());
    ui->Line_diss_curve_max->setText(ui->Line_diss_curve_max_set->text());
}
//发送串口数据
void MainWindow::on_Btn_send_data_clicked()
{
    on_Btn_update_head_clicked();
   emit Sign_SendDriver(send_data);

}

void MainWindow::GetInitDeviceState()
{
    requescmdInfo.baCmdContent="";
    requescmdInfo.uUartCmd.sUartCmd.unStartTag=0xff;
    requescmdInfo.uUartCmd.sUartCmd.unDataLen_empty=0xff;

    requescmdInfo.uUartCmd.sUartCmd.unDataLen=sizeof(requescmdInfo.uUartCmd) + static_cast<quint32>(requescmdInfo.baCmdContent.size());
    requescmdInfo.uUartCmd.sUartCmd.unCrc=0;

//    requescmdInfo.uUartCmd.sUartCmd.unFunId=m_cmd_bit.FunID;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=0;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=m_cmd_bit.NodeType;

    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;

    requescmdInfo.uUartCmd.sUartCmd.unCh= static_cast<quint32>(m_cH.Channel<<3)+m_cH.TEC_ID;
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=m_cH.Module;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=static_cast<quint32>(m_thiCMD.TempGap<<4)+m_thiCMD.ThiCmd;

    for (int i=1;i<9;i++)
    {
        switch (i)
        {
        case 3:
             requescmdInfo.uUartCmd.sUartCmd.unFunId=3;
            break;
        case 5:
            requescmdInfo.uUartCmd.sUartCmd.unFunId=5;
            break;
        case 6:
            requescmdInfo.uUartCmd.sUartCmd.unFunId=6;
            break;
        default:
            break;

        }
        //读取模块参数配置
        if(i == 5)
        {
            for (int j=1;j<5;j++)
            {
                m_thiCMD.ThiCmd=static_cast<quint8>(j);
                requescmdInfo.uUartCmd.sUartCmd.unSubCmd=static_cast<quint32>(m_thiCMD.TempGap<<4)+m_thiCMD.ThiCmd;
                send_data.clear();
                for (int i=0; i<8; i++)
                {
                    send_data.append(static_cast<char>(requescmdInfo.uUartCmd.ucCmd[i]));
                }
                send_data.append(requescmdInfo.baCmdContent);
                quint8  *pPacketData = (quint8 *)(send_data.data());
                 quint8 crc_temp = Cal_CRC(pPacketData+4, static_cast<quint32>(send_data.size()-4));
                send_data.remove(3,1);
                send_data.insert(3,static_cast<char>(crc_temp));
                Sleep(100);
            }
        }
        //读取实验参数设置
        if(i == 6)
        {
            for (int j=2;j<7;j++)
            {
                m_thiCMD.ThiCmd=static_cast<quint8>(j);
                requescmdInfo.uUartCmd.sUartCmd.unSubCmd=static_cast<quint32>(m_thiCMD.TempGap<<4)+m_thiCMD.ThiCmd;
                send_data.clear();
                for (int i=0; i<8; i++)
                {
                    send_data.append(static_cast<char>(requescmdInfo.uUartCmd.ucCmd[i]));
                }
                send_data.append(requescmdInfo.baCmdContent);
                quint8  *pPacketData = (quint8 *)(send_data.data());
                 quint8 crc_temp = Cal_CRC(pPacketData+4, static_cast<quint32>(send_data.size()-4));
                send_data.remove(3,1);
                send_data.insert(3,static_cast<char>(crc_temp));
                Sleep(100);
            }
        }

    }
}

void MainWindow::on_comboBox_F_FunID_activated(int index)
{
    m_cmd_bit.FunID=static_cast<quint8>(index+1);
    ui->comboBox_F_Modeoption->setEnabled(false);
    m_cmd_bit.FunRW=0;
//    ui->Line_adc_report_set->setEnabled(false);
//    ui->Btn_adc_report->setEnabled(false);
       SetFluorControl();
    int inde=ui->comboBox_F_Modeoption->currentIndex();
    switch (index)
    {
    //模块参数设置
    case 4:
        ui->comboBox_F_Modeoption->setEnabled(true);
        on_comboBox_F_Modeoption_activated(inde);
        m_cmd_bit.FunRW=1;
        break;
//     连续采样次数设置
    case 6:
        ui->Line_adc_report_set->setEnabled(true);
        ui->Btn_adc_report->setEnabled(true);
        m_cmd_bit.FunRW=1;
        break;
    }
}

//void MainWindow::on_comboBox_F_Channel_activated(int index)
//{
//     m_cH.Channel=static_cast<quint8>(index<<3);
//}

void MainWindow::on_comboBox_F_Dyestuff_activated(int index)
{
    m_thiCMD.ThiCmd=static_cast<quint8>(index) ;
}

void MainWindow::on_comboBox_F_Modeoption_activated(int index)
{

    SetFluorControl();

    switch (index+1)
    {
    case 1:
        ui->Line_led_intens_set->setEnabled(true);
        ui->Btn_led_intens->setEnabled(true);
        break;
    case 2:
        ui->Line_adc_rate_set->setEnabled(true);
        ui->Btn_adc_rate->setEnabled(true);
        break;
    case 3:
        ui->Line_adc_nu_set->setEnabled(true);
        ui->Btn_adc_nu->setEnabled(true);

        break;
    case 4:
        ui->Line_led_gap_set->setEnabled(true);
        ui->Btn_led_gap->setEnabled(true);
        break;
    case 5:
        ui->Line_delay_time_set->setEnabled(true);
        ui->Btn_delay_time->setEnabled(true);
        break;
    case 6:
        ui->Line_channel_set->setEnabled(true);
        ui->Btn_channel->setEnabled(true);
        break;
    case 7:
        ui->Line_calcul_nu_set->setEnabled(true);
        ui->Btn_calcul_nu->setEnabled(true);
        break;
    }
}

void MainWindow::SetFluorControl()
{

    ui->Line_adc_report_set->setEnabled(false);
    ui->Btn_adc_report->setEnabled(false);

    ui->Line_led_intens_set->setEnabled(false);
    ui->Btn_led_intens->setEnabled(false);

    ui->Line_adc_rate_set->setEnabled(false);
    ui->Btn_adc_rate->setEnabled(false);

    ui->Line_adc_nu_set->setEnabled(false);
    ui->Btn_adc_nu->setEnabled(false);

    ui->Line_led_gap_set->setEnabled(false);
    ui->Btn_led_gap->setEnabled(false);

    ui->Line_delay_time_set->setEnabled(false);
    ui->Btn_delay_time->setEnabled(false);

    ui->Line_channel_set->setEnabled(false);
    ui->Btn_channel->setEnabled(false);

    ui->Line_calcul_nu_set->setEnabled(false);
    ui->Btn_calcul_nu->setEnabled(false);
}

void MainWindow::SetMainControl()
{
    ui->lineEditLine_fan_ctrl_set->setEnabled(false);
    ui->Btn_fan_ctrl->setEnabled(false);
    ui->Line_lid_ctrl_set->setEnabled(false);
    ui->Btn_lid_ctrl->setEnabled(false);
    ui->Line_hot_lid_set->setEnabled(false);
    ui->Btn_hot_lid->setEnabled(false);
    ui->Line_temp_compen_set->setEnabled(false);
    ui->Btn_temp_compen->setEnabled(false);
}
//采样频率设置
void MainWindow::on_Btn_adc_report_clicked()
{
    m_cmd_bit.FunID=7;
    //测试
    m_cmd_bit.FunID=2;


    requescmdInfo.baCmdContent=ui->Line_adc_report_set->text().toLocal8Bit();
}
//Line_led_intens_set
void MainWindow::on_Btn_led_intens_clicked()
{
    m_cmd_bit.FunID=5;
    m_thiCMD.ThiCmd=1;
    requescmdInfo.baCmdContent=ui->Line_led_intens_set->text().toLocal8Bit();
}
//Line_adc_rate_set
void MainWindow::on_Btn_adc_rate_clicked()
{
    m_cmd_bit.FunID=5;
    m_thiCMD.ThiCmd=2;
    requescmdInfo.baCmdContent=ui->Line_adc_rate_set->text().toLocal8Bit();
}
//Line_adc_nu_set
void MainWindow::on_Btn_adc_nu_clicked()
{
    m_cmd_bit.FunID=5;
    m_thiCMD.ThiCmd=3;
    requescmdInfo.baCmdContent=ui->Line_adc_nu_set->text().toLocal8Bit();
}
//Line_led_gap_set
void MainWindow::on_Btn_led_gap_clicked()
{
    m_cmd_bit.FunID=5;
    m_thiCMD.ThiCmd=4;
    requescmdInfo.baCmdContent=ui->Line_led_gap_set->text().toLocal8Bit();
}
//Line_delay_time_set
void MainWindow::on_Btn_delay_time_clicked()
{
    m_cmd_bit.FunID=5;
    m_thiCMD.ThiCmd=5;
    requescmdInfo.baCmdContent=ui->Line_delay_time_set->text().toLocal8Bit();
}
//Line_channel_set
void MainWindow::on_Btn_channel_clicked()
{
    m_cmd_bit.FunID=5;
    m_thiCMD.ThiCmd=6;
    requescmdInfo.baCmdContent=ui->Line_channel_set->text().toLocal8Bit();
}
//Line_calcul_nu_set
void MainWindow::on_Btn_calcul_nu_clicked()
{
    m_cmd_bit.FunID=5;
    m_thiCMD.ThiCmd=7;
    requescmdInfo.baCmdContent=ui->Line_calcul_nu_set->text().toLocal8Bit();
}

void MainWindow::on_comboBox_M_FunID_activated(int index)
{
    SetMainControl();
     m_cmd_bit.FunRW=0;
    m_cmd_bit.FunID = static_cast<quint8>(index+1);
    switch (index+1)
    {
    //串口握手
    case 1:

        break;
         //风扇控制
    case 2:
        ui->lineEditLine_fan_ctrl_set->setEnabled(true);
        ui->Btn_fan_ctrl->setEnabled(true);
        break;
         //报告风扇状态
    case 3:
        break;
         //热盖控制
    case 4:
        ui->Line_lid_ctrl_set->setEnabled(true);
        ui->Btn_lid_ctrl->setEnabled(true);
        break;
         //报告热改状态
    case 5:
        break;
         //实验过程通知
    case 6:
        break;
         //仪器参数读写
    case 7:
        break;
         //热盖设定温度的读写
    case 8:
        ui->Line_hot_lid_set->setEnabled(true);
        ui->Btn_hot_lid->setEnabled(true);
        break;
         //温度补偿参数读写
    case 9:
        ui->Line_temp_compen_set->setEnabled(true);
        ui->Btn_temp_compen->setEnabled(true);
        break;
         //仪器错误报告
    case 10:
        break;
         //读取铜座温度
    case 11:
        break;
    }
}
//风扇控制
void MainWindow::on_Btn_fan_ctrl_clicked()
{
    m_cmd_bit.FunRW=1;
    requescmdInfo.baCmdContent=ui->lineEditLine_fan_ctrl_set->text().toLocal8Bit();
}
//热盖控制
void MainWindow::on_Btn_lid_ctrl_clicked()
{
    m_cmd_bit.FunRW=1;
    requescmdInfo.baCmdContent=ui->Line_lid_ctrl_set->text().toLocal8Bit();
}
//设置热盖温度
void MainWindow::on_Btn_hot_lid_clicked()
{
    m_cmd_bit.FunRW=1;
    requescmdInfo.baCmdContent=ui->Line_hot_lid_set->text().toLocal8Bit();
}
//设置温度补偿参数
void MainWindow::on_Btn_temp_compen_clicked()
{
    m_cmd_bit.FunRW=1;
    requescmdInfo.baCmdContent=ui->Line_temp_compen_set->text().toLocal8Bit();
}
//通道选择
void MainWindow::on_comboBox_M_Channel_activated(int index)
{
    m_cmd_bit.FunRW=1;
    m_cH.Channel=static_cast<quint8>(index<<3);
}
//选择A、B通道
void MainWindow::on_comboBox_T_Channel_currentIndexChanged(int index)
{
    m_cH.Channel=static_cast<quint8>(index<<3);
    qDebug()<<"index A_B="<<index;
    //清空之前的数据
    ui->Wdg_T_mode->Cleardata();
    check_curve=0;
    curve_point=0;

    //添加曲线为下次绘图做准备
//    ui->Wdg_T_mode->AddLine("Temp_1");
//    ui->Wdg_T_mode->AddLine("Temp_2");
//    ui->Wdg_T_mode->AddLine("Temp_3");
//    ui->Wdg_T_mode->AddLine("Temp_4");

}

void MainWindow::on_comboBox_F_Channel_currentIndexChanged(int index)
{
      m_cH.Channel=static_cast<quint8>(index<<3);
      ui->Wdg_F_mode->Cleardata();
      check_curve_flou=0;
      curve_point_flou=0;

}

void MainWindow::ReceiveCmd(SRequestCmdInfo cmdInfo)
{
    cmdInfo.uUartCmd.sUartCmd.unStartTag=0xff;
    cmdInfo.uUartCmd.sUartCmd.unDataLen_empty=0;

    cmdInfo.uUartCmd.sUartCmd.unDataLen=sizeof(cmdInfo.uUartCmd) + static_cast<quint32>(cmdInfo.baCmdContent.size());
    cmdInfo.uUartCmd.sUartCmd.unCrc=0;

    send_data.clear();
    for (int i=0; i<8; i++)
    {
        send_data.append(static_cast<char>(cmdInfo.uUartCmd.ucCmd[i]));
    }
    send_data.append(cmdInfo.baCmdContent);
    quint8  *pPacketData = (quint8 *)(send_data.data());
//    char crc_temp = static_cast<char>(Cal_CRC(pPacketData+4, static_cast<quint32>(send_data.size()-4)));
     quint8 crc_temp = Cal_CRC(pPacketData+4, static_cast<quint32>(send_data.size()-4));
    send_data.remove(3,1);
    send_data.insert(3,static_cast<char>(crc_temp));

    emit Sign_SendDriver(send_data);

}

void MainWindow::ReceiveCmd()
{
    requescmdInfo.uUartCmd.sUartCmd.unStartTag=0xff;
    requescmdInfo.uUartCmd.sUartCmd.unDataLen_empty=0;

    requescmdInfo.uUartCmd.sUartCmd.unDataLen=sizeof(requescmdInfo.uUartCmd) + static_cast<quint32>(requescmdInfo.baCmdContent.size());
    requescmdInfo.uUartCmd.sUartCmd.unCrc=0;




    qDebug()<<"0"<<QString("%1").number(requescmdInfo.uUartCmd.ucCmd[0],16);
    qDebug()<<"1"<<QString("%1").number(requescmdInfo.uUartCmd.ucCmd[1],16);
    qDebug()<<"2"<<QString("%1").number(requescmdInfo.uUartCmd.ucCmd[2],16);
    qDebug()<<"3"<<QString("%1").number(requescmdInfo.uUartCmd.ucCmd[3],16);
    qDebug()<<"4"<<QString("%1").number(requescmdInfo.uUartCmd.ucCmd[4],16);
    qDebug()<<"5"<<QString("%1").number(requescmdInfo.uUartCmd.ucCmd[5],16);
    qDebug()<<"6"<<QString("%1").number(requescmdInfo.uUartCmd.ucCmd[6],16);
    qDebug()<<"7"<<QString("%1").number(requescmdInfo.uUartCmd.ucCmd[7],16);


    send_data.clear();
    for (int i=0; i<8; i++)
    {
        send_data.append(static_cast<char>(requescmdInfo.uUartCmd.ucCmd[i]));
    }
    send_data.append(requescmdInfo.baCmdContent);
    quint8  *pPacketData = (quint8 *)(send_data.data());
   //    char crc_temp = static_cast<char>(Cal_CRC(pPacketData+4, static_cast<quint32>(send_data.size()-4)));
     quint8 crc_temp = Cal_CRC(pPacketData+4, static_cast<quint32>(send_data.size()-4));
    send_data.remove(3,1);
    send_data.insert(3,static_cast<char>(crc_temp));
       emit Sign_SendDriver(send_data);


    requescmdInfo.uUartCmd.ucCmd[0]=0;
    requescmdInfo.uUartCmd.ucCmd[1]=0;
    requescmdInfo.uUartCmd.ucCmd[2]=0;
    requescmdInfo.uUartCmd.ucCmd[3]=0;
    requescmdInfo.uUartCmd.ucCmd[4]=0;
    requescmdInfo.uUartCmd.ucCmd[5]=0;
    requescmdInfo.uUartCmd.ucCmd[6]=0;
    requescmdInfo.uUartCmd.ucCmd[7]=0;

    requescmdInfo.baCmdContent="";

}
//设置设备运行模式
void MainWindow::on_Btn_set_mode_clicked()
{
//    SRequestCmdInfo cmdInfo;

//    cmdInfo.uUartCmd.sUartCmd.unFunId         = 1;
//    cmdInfo.uUartCmd.sUartCmd.unRorW          = M_WRITE;
//    cmdInfo.uUartCmd.sUartCmd.unNodeType      = 7;
//    cmdInfo.uUartCmd.sUartCmd.unStatusAck     = 0;
//    cmdInfo.uUartCmd.sUartCmd.unCh            = static_cast<quint32>(ui->Cbx_modu->currentIndex()<<3);
//    cmdInfo.uUartCmd.sUartCmd.unModuleType    = 4;
//    cmdInfo.uUartCmd.sUartCmd.unSubCmd        = 0;

//    DataContent tempdata;
//    tempdata.m_uint32data =static_cast<quint32>(ui->Cbx_mode->currentIndex()) ;

//    cmdInfo.baCmdContent.append(static_cast<char>(tempdata.m_uint8data[0]));
//    cmdInfo.baCmdContent.append(static_cast<char>(tempdata.m_uint8data[1]));
//    cmdInfo.baCmdContent.append(static_cast<char>(tempdata.m_uint8data[2]));
//    cmdInfo.baCmdContent.append(static_cast<char>(tempdata.m_uint8data[3]));

//    ReceiveCmd(cmdInfo);

    requescmdInfo.uUartCmd.sUartCmd.unFunId=1;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->Cbx_modu->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=4;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    DataContent datacon;
    datacon.m_uint32data=static_cast<quint32>(ui->Cbx_mode->currentIndex()) ;
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();


}
//读取设备运行模式
void MainWindow::on_Btn_get_mode_clicked()
{
//    SRequestCmdInfo cmdInfo;

//    cmdInfo.uUartCmd.sUartCmd.unFunId         = 1;
//    cmdInfo.uUartCmd.sUartCmd.unRorW          = M_READ;
//    cmdInfo.uUartCmd.sUartCmd.unNodeType      = 7;
//    cmdInfo.uUartCmd.sUartCmd.unStatusAck     = 0;
//    cmdInfo.uUartCmd.sUartCmd.unCh            = static_cast<quint32>(ui->Cbx_modu->currentIndex()<<3);
//    cmdInfo.uUartCmd.sUartCmd.unModuleType    = 4;
//    cmdInfo.uUartCmd.sUartCmd.unSubCmd        = 0;

//     ReceiveCmd(cmdInfo);

    requescmdInfo.uUartCmd.sUartCmd.unFunId=1;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->Cbx_modu->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=4;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;
    requescmdInfo.baCmdContent="";

    ReceiveCmd();
}
//荧光采集位置读取和保存 下发设置
void MainWindow::on_Btn_set_fluo_clicked()
{
    /*
    SRequestCmdInfo cmdInfo;

    cmdInfo.uUartCmd.sUartCmd.unFunId         = 3;
    cmdInfo.uUartCmd.sUartCmd.unRorW          = M_WRITE;
    cmdInfo.uUartCmd.sUartCmd.unNodeType      = 7;
    cmdInfo.uUartCmd.sUartCmd.unStatusAck     = 0;
    cmdInfo.uUartCmd.sUartCmd.unCh            = static_cast<quint32>(ui->Cbx_rotate_dire->currentIndex()<<3);
    cmdInfo.uUartCmd.sUartCmd.unModuleType    = 4;
    cmdInfo.uUartCmd.sUartCmd.unSubCmd        = 0;

    DataContent tempdata;
    tempdata.m_uint32data = ui->Led_fluo_set_point->text().toUInt();

    cmdInfo.baCmdContent.append(static_cast<char>(tempdata.m_uint8data[0]));
    cmdInfo.baCmdContent.append(static_cast<char>(tempdata.m_uint8data[1]));
    cmdInfo.baCmdContent.append(static_cast<char>(tempdata.m_uint8data[2]));
    cmdInfo.baCmdContent.append(static_cast<char>(tempdata.m_uint8data[3]));

    ReceiveCmd(cmdInfo);
    */
    requescmdInfo.uUartCmd.sUartCmd.unFunId=3;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->Cbx_rotate_dire->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=4;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    DataContent datacon;
    datacon.m_uint32data=ui->Led_fluo_set_point->text().toInt();
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();
}
//荧光采集位置读取和保存 读取数据
void MainWindow::on_Btn_get_fluo_clicked()
{
    /*
    SRequestCmdInfo cmdInfo;

    cmdInfo.uUartCmd.sUartCmd.unFunId         = 3;
    cmdInfo.uUartCmd.sUartCmd.unRorW          = M_READ;
    cmdInfo.uUartCmd.sUartCmd.unNodeType      = 7;
    cmdInfo.uUartCmd.sUartCmd.unStatusAck     = 0;
    cmdInfo.uUartCmd.sUartCmd.unCh            =  static_cast<quint32>(ui->Cbx_rotate_dire->currentIndex()<<3);
    cmdInfo.uUartCmd.sUartCmd.unModuleType    = 4;
    cmdInfo.uUartCmd.sUartCmd.unSubCmd        = 0;

    ReceiveCmd(cmdInfo);
    */
    requescmdInfo.uUartCmd.sUartCmd.unFunId=3;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->Cbx_rotate_dire->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=4;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;
    requescmdInfo.baCmdContent="";

    ReceiveCmd();
}
//采集荧光
void MainWindow::on_Btn_coll_fluo_clicked()
{
//    SRequestCmdInfo cmdInfo;

//     cmdInfo.uUartCmd.sUartCmd.unFunId         = 2;
//    cmdInfo.uUartCmd.sUartCmd.unRorW          = M_READ;
//    cmdInfo.uUartCmd.sUartCmd.unNodeType      = 7;
//    cmdInfo.uUartCmd.sUartCmd.unStatusAck     = 0;
//    cmdInfo.uUartCmd.sUartCmd.unCh            = 0;
//     cmdInfo.uUartCmd.sUartCmd.unModuleType    = 4;
//    cmdInfo.uUartCmd.sUartCmd.unSubCmd        = static_cast<quint8>(ui->Cbx_fluo_type->currentIndex());

//    ReceiveCmd(cmdInfo);

    requescmdInfo.uUartCmd.sUartCmd.unFunId=2;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->Cbx_modu->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=4;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=static_cast<quint8>(ui->Cbx_fluo_type->currentIndex());

    requescmdInfo.baCmdContent="";

    ReceiveCmd();



}
//设置电机位置
void MainWindow::on_Btn_elec_set_point_clicked()
{

    requescmdInfo.uUartCmd.sUartCmd.unFunId=4;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->Cbx_modu->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=4;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    DataContent datacon;
    datacon.m_uint32data=ui->Led_eles_set_point->text().toInt();
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();

/*
 requescmdInfo.uUartCmd.sUartCmd.unStartTag=0xff;
 requescmdInfo.uUartCmd.sUartCmd.unDataLen_empty=0;

 requescmdInfo.uUartCmd.sUartCmd.unDataLen=sizeof(requescmdInfo.uUartCmd) + static_cast<quint32>(requescmdInfo.baCmdContent.size());
 requescmdInfo.uUartCmd.sUartCmd.unCrc=0;

 send_data.clear();
 for (int i=0; i<8; i++)
 {
     send_data.append(static_cast<char>(requescmdInfo.uUartCmd.ucCmd[i]));
 }
 send_data.append(requescmdInfo.baCmdContent);
 quint8  *pPacketData = (quint8 *)(send_data.data());
//    char crc_temp = static_cast<char>(Cal_CRC(pPacketData+4, static_cast<quint32>(send_data.size()-4)));
  quint8 crc_temp = Cal_CRC(pPacketData+4, static_cast<quint32>(send_data.size()-4));
 send_data.remove(3,1);
 send_data.insert(3,static_cast<char>(crc_temp));
    emit Sign_SendDriver(send_data);
 */


}
//读取电机位置
void MainWindow::on_Btn_elec_get_point_clicked()
{
//    SRequestCmdInfo cmdInfo;

//    cmdInfo.uUartCmd.sUartCmd.unFunId         = 4;
//    cmdInfo.uUartCmd.sUartCmd.unRorW          = M_READ;
//    cmdInfo.uUartCmd.sUartCmd.unNodeType      = 7;
//    cmdInfo.uUartCmd.sUartCmd.unStatusAck     = 0;
//    cmdInfo.uUartCmd.sUartCmd.unCh            =  static_cast<quint32>(ui->Cbx_modu->currentIndex()<<3);
//    cmdInfo.uUartCmd.sUartCmd.unModuleType    = 4;
//    cmdInfo.uUartCmd.sUartCmd.unSubCmd        = 0;

//    ReceiveCmd(cmdInfo);

    requescmdInfo.uUartCmd.sUartCmd.unFunId=4;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->Cbx_modu->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=4;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;
    requescmdInfo.baCmdContent="";



    ReceiveCmd();



}

void MainWindow::on_pushButton_clicked()
{
    while(1)
    {
        if(check_cmd)
        {
            if(((ui->Spx_elec_step->value()+ui->Led_eles_set_point->text().toInt()) > 10500) || (check_size >50 ))
            {
                return;
            }
            //下发电机位置
           // on_Btn_elec_set_point_clicked();
            QThread::msleep(100);
            //下发采集荧光
          //  on_Btn_coll_fluo_clicked();
            check_size++;
            check_cmd=false;
            check_error=0;
            ui->Led_eles_set_point->setText(QString("%1").arg(ui->Spx_elec_step->value()+ui->Led_eles_set_point->text().toInt()));

        }
        else
        {
            if(check_error >10 )
            {
                return;
            }
            check_error++;
            qDebug()<<"check_error="<<check_error;
        }
        QThread::msleep(100);


    }
}
