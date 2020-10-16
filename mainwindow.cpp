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
//    ui->Led_eles_get_point->setReadOnly(true);
//    ui->Led_eles_get_point->setReadOnly(true);
    check_cmd=true;
    check_size=0;
    debug_flou_size=0;
    point_and_flou=-1;
    check_error=0;
    elec_start_point=10000;
    fluo_check=0;
    electric_machinery_point=0;
    fluo_auto_acq_number=-1;
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

    ui->tabWidget->setCurrentIndex(0);

    SerialNode=new Q268Driver;
    connect(SerialNode,SIGNAL(SendUsartData2Mcu(SRequestCmdInfo)),this,SLOT(SlotUsartData2Mcu(SRequestCmdInfo)));
    connect(SerialNode,&Q268Driver::Sign_close,this,&MainWindow::Slot_driver);
    connect(this,SIGNAL(Sign_SendDriver(QByteArray)),SerialNode,SLOT(SlotGetUpDriver(QByteArray)));


    driver_data="";
    send_data.clear();
    fluo_size.clear();

    send_node_tmp.Head=0;
    send_node_tmp.Len=0;
    send_node_tmp.CRC=0;
    send_node_tmp.Ack=0;
    send_node_tmp.Data.clear();
    check_curve=0;
    curve_point=0;
    check_curve_B=0;
    curve_point_B=0;
    check_curve_flou=0;
    curve_point_flou=0;
//    ui->Chb_passageway_open_B->setEnabled(false);
//    ui->Chb_passageway_close_B->setEnabled(false);

    memset(static_cast<void *>(&send_node_tmp.CMD),0,sizeof (send_node_tmp.CMD));
    memset(static_cast<void *>(&send_node_tmp.CH),0,sizeof (send_node_tmp.CH));
    memset(static_cast<void *>(&send_node_tmp.ThiCMD),0,sizeof (send_node_tmp.ThiCMD));


     memset(static_cast<void *>(&requescmdInfo.uUartCmd),0,sizeof (requescmdInfo.uUartCmd));
     requescmdInfo.baCmdContent.clear();

    memset(static_cast<void *>(&m_cmd_bit),0,sizeof (m_cmd_bit));
    memset(static_cast<void *>(&m_cH),0,sizeof (m_cmd_bit));
    memset(static_cast<void *>(&m_thiCMD),0,sizeof (m_cmd_bit));




    ui->Texdit_accept->setReadOnly(true);

    ui->Wdg_T_mode->AddLine("temp_A1");
    ui->Wdg_T_mode->AddLine("temp_A2");
    ui->Wdg_T_mode->AddLine("temp_A3");
    ui->Wdg_T_mode->AddLine("temp_A4");
    connect(ui->Wdg_T_mode,SIGNAL(Sign_clear()),this,SLOT(Slot_Clear_A()));

    ui->Wdg_T_mode_B->AddLine("temp_B1");
    ui->Wdg_T_mode_B->AddLine("temp_B2");
    ui->Wdg_T_mode_B->AddLine("temp_B3");
    ui->Wdg_T_mode_B->AddLine("temp_B4");
    connect(ui->Wdg_T_mode_B,SIGNAL(Sign_clear()),this,SLOT(Slot_Clear_B()));



    ui->Wdg_F_mode->AddLine("flou_1");
    ui->Wdg_F_mode->AddLine("flou_2");
    ui->Wdg_F_mode->AddLine("flou_3");
    ui->Wdg_F_mode->AddLine("flou_4");

    ui->Wdg_debug->AddLine("flou_1");
    ui->Wdg_debug->AddLine("flou_2");
    ui->Wdg_debug->AddLine("flou_3");
    ui->Wdg_debug->AddLine("flou_4");
    connect(ui->Wdg_debug,SIGNAL(Sign_clear()),this,SLOT(Slot_Clear_fluor()));


    ui->Spx_fluo_number->setValue(50);

    //connect(&fluo_time,SIGNAL(timeout()),this,SLOT())



}

void MainWindow::Slot_driver()
{
    qDebug()<<"close this and driber";
}
//串口消息接受函数
void MainWindow::SlotUsartData2Mcu(SRequestCmdInfo cmdInfo)
{

//    qDebug()<<"串口消息接受函数 unFunId"<<cmdInfo.uUartCmd.sUartCmd.unFunId;
//    qDebug()<<"串口消息接受函数 unRorW"<<cmdInfo.uUartCmd.sUartCmd.unRorW;
//    qDebug()<<"串口消息接受函数 unModuleType"<<cmdInfo.uUartCmd.sUartCmd.unModuleType;
//    qDebug()<<"串口消息接受函数 unCh"<<cmdInfo.uUartCmd.sUartCmd.unCh;
//    qDebug()<<"串口消息接受函数 unDataLen_empty"<<cmdInfo.uUartCmd.sUartCmd.unDataLen_empty;
//    qDebug()<<"串口消息接受函数 unDataLen"<<cmdInfo.uUartCmd.sUartCmd.unDataLen;

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

void MainWindow::Slot_gat_max()
{
//    fluo_size
    QList<quint32> list;
    list.clear();
    int siz=0;
    list=fluo_size;
    //计算最大值
    qSort(fluo_size.begin(),fluo_size.end());

    qDebug()<<fluo_size;
    qDebug()<<"max="<< fluo_size[fluo_size.size()-1]<<fluo_size.size()<<"list"<<list.size();
    ui->Texdit_accept->append("最大值为："+QString("%1").arg(fluo_size[fluo_size.size()-1]));

    for(auto mlist:list)
    {
        if(mlist == fluo_size[fluo_size.size()-1])
        {
            qDebug()<<"位置"<<siz;
            int max_point=(siz*ui->Spx_elec_step->value())+(ui->Led_fluo_Best_location->text().toInt()-ui->Spx_fluo_hole_width->value()/2);
            ui->Led_fluo_Best_location->setText(QString("%1").arg(max_point));
            ui->Texdit_accept->append("电机位置为"+QString("%1").arg(max_point));
           // on_Btn_set_fluo_clicked();
        }
        siz++;
    }
}

void MainWindow::Slot_Clear_A()
{
    check_curve=0;
    curve_point=0;
}

void MainWindow::Slot_Clear_B()
{
    check_curve_B=0;
    curve_point_B=0;
}

void MainWindow::Slot_Clear_fluor()
{
    check_curve_flou=0;
    curve_point_flou=0;

    debug_flou_size=0;
    fluo_check=0;

    check_size=0;

    ui->Wdg_debug->AddLine("flou_1");
    ui->Wdg_debug->AddLine("flou_2");
    ui->Wdg_debug->AddLine("flou_3");
    ui->Wdg_debug->AddLine("flou_4");
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
 //   qDebug()<<"ui->Wdg_T_mode->size()"<<ui->Wdg_T_mode->size()<<event->size();
    ui->Wdg_T_mode->Updatasize();
    ui->Wdg_T_mode_B->Updatasize();
    ui->Wdg_F_mode->Updatasize();
    ui->Wdg_debug->Updatasize();

}

void MainWindow::Get_Point_Fluo()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=9;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=0;
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    ReceiveCmd();
 //   on_Btn_coll_fluo_clicked();
}

void MainWindow::Get_cont_Fluo()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=9;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    //    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->Cbx_modu->currentIndex()<<3);
          requescmdInfo.uUartCmd.sUartCmd.unCh=0;
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
    //    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=static_cast<quint8>(ui->Cbx_fluo_type->currentIndex());
        requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    requescmdInfo.baCmdContent="";

    ReceiveCmd();
}

void MainWindow::Get_Fluo_Best_Location()
{
    if(electric_machinery_point == 0)
    {
        return;
    }
    else
    {
        //采集荧光
        requescmdInfo.uUartCmd.sUartCmd.unFunId=9;
        requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
        requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
        requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
        requescmdInfo.uUartCmd.sUartCmd.unCh=0;
        requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
        requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

        requescmdInfo.baCmdContent="";

        ReceiveCmd();

    }
}

void MainWindow::Sen_Fluo_Motion_control()
{

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

//    SetObjectEnabled();
    GetData();

    ui->Wdg_T_mode->Updatasize();
    ui->Wdg_T_mode_B->Updatasize();
    ui->Wdg_F_mode->Updatasize();
    ui->Wdg_debug->Updatasize();

}

/*
void MainWindow::on_Btn_update_head_clicked()
{
    //填充数据帧头
    switch (ui->tabWidget->currentIndex())
    {
    case 0:
        return;
        //温控
    case 1:
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
*/
void MainWindow::SetObjectEnabled()
{
//    ui->Line_temp_comp_set->setEnabled(false);
//    ui->Line_KP_set->setEnabled(false);
//    ui->Line_KI_set->setEnabled(false);
//    ui->Line_KD_set->setEnabled(false);
//    ui->Btn_temp_comp->setEnabled(false);
    ui->Btn_KP->setEnabled(false);
    ui->Btn_KI->setEnabled(false);
    ui->Btn_KD->setEnabled(false);

    ui->Line_temp_target_set->setEnabled(false);
    ui->Btn_temp_target->setEnabled(false);

    ui->Line_temp_up_set->setEnabled(false);
    ui->Line_temp_down_set->setEnabled(false);
//    ui->Btn_diss_curve->setEnabled(false);
    ui->Btn_temp_up->setEnabled(false);
    ui->Btn_temp_down->setEnabled(false);

    ui->Line_diss_curve_start_set->setEnabled(false);
    ui->Line_diss_curve_end_set->setEnabled(false);
    ui->Line_diss_curve_max_set->setEnabled(false);
    ui->Btn_diss_curve_start->setEnabled(false);
    ui->Btn_diss_curve_end->setEnabled(false);
    ui->Btn_diss_curve_max->setEnabled(false);

    SetFluorControl();
    SetMainControl();

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
//        m_cmd_bit.FunID=static_cast<quint8>(ui->comboBox_T_FunID->currentIndex()+1);
        m_cmd_bit.FunRW= static_cast<quint8>(1);
        m_cmd_bit.NodeType=static_cast<quint8>(7);

        m_cH.Channel=static_cast<quint8>(ui->comboBox_T_Channel->currentIndex()<<3);
//        m_cH.TEC_ID=static_cast<quint8>(ui->comboBox_T_TEC_ID->currentIndex()+1);
        m_cH.Module=static_cast<quint8>(1);

        m_thiCMD.TempGap= static_cast<quint8>(0);
        m_thiCMD.ThiCmd= static_cast<quint8>(0);
        break;
        //荧光
    case 2:
//        m_cmd_bit.FunID=static_cast<quint8>(ui->comboBox_F_FunID->currentIndex()+1);
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
//        m_cmd_bit.FunID=static_cast<quint8>(ui->comboBox_F_FunID->currentIndex()+1);
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
//温控串口接受函数
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
    QList<DataContent> cmddata;
    cmddata.clear();
    DataContent tempdata;
    for (int i=0; i<cmdInfo.baCmdContent.size()/4; i++)
    {
        tempdata.m_uint8data[0] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+0));
        tempdata.m_uint8data[1] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+1));
        tempdata.m_uint8data[2] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+2));
        tempdata.m_uint8data[3] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+3));
        cmddata.append(tempdata);
    }
    switch (cmdInfo.uUartCmd.sUartCmd.unFunId)
    {
    //
    case 1:
        switch (tempdata.m_uint32data)
        {
        case 1:
            if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 0)
            {
                SetTexditData(tr("A模块温控通道开启"));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("B模块温控通道开启"));
            }

            break;
        case 2:
            if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 0)
            {
                SetTexditData(tr("A模块温控通道关闭"));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("B模块温控通道关闭"));
            }

            break;
        default:
            if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 0)
            {
                SetTexditData(tr("A模块温控通道数据返回错误！"));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("B模块温控通道数据返回错误！"));
            }

            break;

        }


        break;
    case 2:
        break;
        //读取目标设定温度
    case 3:
//        ui->Line_temp_targe->setText(QString(cmdInfo.baCmdContent));
//        SetTexditData(tr("目标温度设置值为：")+QString(cmdInfo.baCmdContent));



       // qDebug()<<"check_curve="<<check_curve<<"curve_point="<<curve_point<<"check_curve%4="<<check_curve%4<<"curve_point/4="<<curve_point/4;
        if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
        {
            SetTexditData(tr("A模块目标温度设置值为：")+QString("%1").arg(tempdata.m_floatdata));
            SetTexditData(tr("A模块目标温度设置值为：")+QString("%1").arg(tempdata.m_uint32data));
            //绘制温度监控曲线 tempdata.m_floatdata
//             ui->Wdg_T_mode->SLot_AddPointToCurve(check_curve%4,curve_point/4,QString(cmdInfo.baCmdContent).toDouble());
            ui->Wdg_T_mode->SLot_AddPointToCurve(check_curve%4,curve_point/4,static_cast<double>(tempdata.m_floatdata));
            check_curve++;
            curve_point++;
        }
        else if((cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 1)
        {
            SetTexditData(tr("B模块目标温度设置值为：")+QString("%1").arg(tempdata.m_floatdata));
            SetTexditData(tr("B模块目标温度设置值为：")+QString("%1").arg(tempdata.m_uint32data));
            //绘制温度监控曲线
            ui->Wdg_T_mode_B->SLot_AddPointToCurve(check_curve_B%4,curve_point_B/4,static_cast<double>(tempdata.m_floatdata));
            check_curve_B++;
            curve_point_B++;
        }
        break;
    case 4:
//         SetTexditData(tr("接收到的温度值为：")+QString(cmdInfo.baCmdContent));

         if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 0)
         {
             SetTexditData(tr("A模块接收到的温度值为：")+QString("%1").arg(tempdata.m_floatdata));
             SetTexditData(tr("A模块接收到的温度值为：")+QString("%1").arg(tempdata.m_uint32data));
             //绘制温度监控曲线 tempdata.m_floatdata
//             ui->Wdg_T_mode->SLot_AddPointToCurve(check_curve%4,curve_point/4,QString(cmdInfo.baCmdContent).toDouble());
             ui->Wdg_T_mode->SLot_AddPointToCurve(check_curve%4,curve_point/4,static_cast<double>(tempdata.m_floatdata));
             check_curve++;
             curve_point++;
         }
         else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
         {
             SetTexditData(tr("B模块接收到的温度值为：")+QString("%1").arg(tempdata.m_floatdata));
             SetTexditData(tr("B模块接收到的温度值为：")+QString("%1").arg(tempdata.m_uint32data));
             //绘制温度监控曲线
             ui->Wdg_T_mode_B->SLot_AddPointToCurve(check_curve_B%4,curve_point_B/4,static_cast<double>(tempdata.m_floatdata));
             check_curve_B++;
             curve_point_B++;
         }

        break;
        //模块参数配置
    case 5:
        switch (cmdInfo.uUartCmd.sUartCmd.unSubCmd)
        {
        case 1:
            //设置温补
//            ui->Line_temp_comp->setText(QString(cmdInfo.baCmdContent));
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("接收到的A模块温度补偿值为：")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("接收到的A模块温度补偿值为：")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("接收到的B模块温度补偿值为：")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("接收到的B模块温度补偿值为：")+QString("%1").arg(tempdata.m_uint32data));
            }


            break;
        case 2:
//            ui->Line_KP->setText(QString(cmdInfo.baCmdContent));
//            SetTexditData(tr("接收到的PID参数中的P值为：")+QString(cmdInfo.baCmdContent));
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("接收到的A模块PID参数中的P值为：")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("接收到的A模块PID参数中的P值为：")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("接收到的B模块PID参数中的P值为：")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("接收到的B模块PID参数中的P值为：")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        case 3:
//            ui->Line_KI->setText(QString(cmdInfo.baCmdContent));
//            SetTexditData(tr("接收到的PID参数中的I值为：")+QString(cmdInfo.baCmdContent));
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("接收到的A模块PID参数中的I值为：")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("接收到的A模块PID参数中的I值为：")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("接收到的B模块PID参数中的I值为：")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("接收到的B模块PID参数中的I值为：")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        case 4:
//            ui->Line_KD->setText(QString(cmdInfo.baCmdContent));
//            SetTexditData(tr("接收到的PID参数中的D值为：")+QString(cmdInfo.baCmdContent));
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("接收到的A模块PID参数中的D值为：")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("接收到的A模块PID参数中的D值为：")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("接收到的B模块PID参数中的D值为：")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("接收到的B模块PID参数中的D值为：")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        default:
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("A模块参数数据错误！：")+QString(cmdInfo.baCmdContent));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("B模块参数数据错误！：")+QString(cmdInfo.baCmdContent));
            }

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
            //升温速率
        case 2:
//            ui->Line_temp_up->setText(QString(cmdInfo.baCmdContent));
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("接收到的A模块升温速率值为：")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("接收到的A模块升温速率值为：")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("接收到的B模块升温速率值为：")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("接收到的B模块升温速率值为：")+QString("%1").arg(tempdata.m_uint32data));

            }

            break;
            //降温速率
        case 3:
//            ui->Line_temp_down->setText(QString(cmdInfo.baCmdContent));
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("接收到的A模块降温速率值为：")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("接收到的A模块降温速率值为：")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("接收到的B模块降温速率值为：")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("接收到的B模块降温速率值为：")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
            //溶解曲线起始值
        case 4:
//            ui->Line_diss_curve_start->setText(QString(cmdInfo.baCmdContent));
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("接收到A模块溶解曲线起始值值为：")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("接收到A模块溶解曲线起始值值为：")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("接收到B模块溶解曲线起始值值为：")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("接收到B模块溶解曲线起始值值为：")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
            //溶解曲线终止者
        case 5:
//            ui->Line_diss_curve_end->setText(QString(cmdInfo.baCmdContent));
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("接收到的A模块溶解曲线终止值为：")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("接收到的A模块溶解曲线终止值为：")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("接收到的B模块溶解曲线终止值为：")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("接收到的B模块溶解曲线终止值为：")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
            //溶解曲线最大值
        case 6:
//            ui->Line_diss_curve_max->setText(QString(cmdInfo.baCmdContent));
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("接收到的A模块溶解曲线最大值为：")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("接收到的A模块溶解曲线最大值为：")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("接收到的B模块溶解曲线最大值为：")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("接收到的B模块溶解曲线最大值为：")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        default:
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("A模块实验参数数据错误！：")+QString(cmdInfo.baCmdContent));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("B模块实验参数数据错误！：")+QString(cmdInfo.baCmdContent));
            }

            break;
        }
        break;
        //上报的温度
    case 7:
//        显示当前上报的温度值
//        SetTexditData(tr("设备当前温度：")+QString(cmdInfo.baCmdContent));
        if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 0)
        {
            SetTexditData(tr("接收到的A模块设备当前温度为：")+QString("%1").arg(tempdata.m_floatdata));
             SetTexditData(tr("接收到的A模块设备当前温度为：")+QString("%1").arg(tempdata.m_uint32data));
        }
        else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
        {
            SetTexditData(tr("接收到的B模块设备当前温度为：")+QString("%1").arg(tempdata.m_floatdata));
             SetTexditData(tr("接收到的B模块设备当前温度为：")+QString("%1").arg(tempdata.m_uint32data));
        }

        break;
        //错误状态
    case 8:
        if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 0)
        {
//             SetTexditData(tr("A模块设备出错，错误信息：")+QString(cmdInfo.baCmdContent));
        }
        else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
        {
             SetTexditData(tr("B模块设备出错，错误信息：")+QString(cmdInfo.baCmdContent));
        }

        break;
    default:
        break;
    }
}
//荧光串口接受函数
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

    QList<DataContent> cmddata;
    cmddata.clear();
    DataContent tempdata;
    check_cmd=true;
    for (int i=0; i<cmdInfo.baCmdContent.size()/4; i++) {
        tempdata.m_uint8data[0] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+0));
        tempdata.m_uint8data[1] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+1));
        tempdata.m_uint8data[2] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+2));
        tempdata.m_uint8data[3] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+3));
        cmddata.append(tempdata);
    }
    switch (cmdInfo.uUartCmd.sUartCmd.unFunId)
    {
    //开始采集荧光
    case 1:
        break;
        //荧光数据报告
    case 2:
        if(ui->Chb_fluo_auto_acqu->isChecked())
        {
            ui->Wdg_debug->SLot_AddPointToCurve(0,check_size,cmddata[0].m_uint32data);

            check_size++;
            SetTexditData(tr("接受到的荧光数据为：")+QString("%1").arg(cmddata[0].m_floatdata));
            SetTexditData(tr("接受到的荧光数据为：")+QString("%1").arg(cmddata[0].m_uint32data));
            if(check_size >= ui->Spx_fluo_number->value())
            {
                ui->Chb_fluo_auto_acqu->setCheckState(Qt::Unchecked);
                check_size=0;
            }
            else
            {
                on_Chb_fluo_auto_acqu_clicked(true);
            }

        }
        qDebug()<<ui->Chb_fluo_auto_acqu->isChecked();
/*

        //绘制荧光曲线
//        ui->Wdg_F_mode->SLot_AddPointToCurve(check_curve_flou%4,curve_point_flou/4,QString(cmdInfo.baCmdContent).toDouble());
          ui->Wdg_F_mode->SLot_AddPointToCurve(check_curve_flou%4,curve_point_flou/4,static_cast<double>(tempdata.m_floatdata));
        check_curve_flou++;
        curve_point_flou++;
//        SetTexditData(tr("接受到的荧光数据为：")+QString(cmdInfo.baCmdContent));

        if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
        {
            SetTexditData(tr("接受到的A模块荧光数据为：")+QString("%1").arg(cmddata[0].m_floatdata));
             SetTexditData(tr("接受到的A模块荧光数据为：")+QString("%1").arg(cmddata[0].m_uint32data));
        }
        else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
        {
            SetTexditData(tr("接受到的B模块荧光数据为：")+QString("%1").arg(cmddata[0].m_floatdata));
             SetTexditData(tr("接受到的B模块荧光数据为：")+QString("%1").arg(cmddata[0].m_uint32data));
        }
*/

        break;
        //ADC数据上报
    case 3:
//        SetTexditData(tr("接受到的ADC数据为：")+QString(cmdInfo.baCmdContent));
        if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
        {
            SetTexditData(tr("接受到的A模块ADC数据为：")+QString("%1").arg(tempdata.m_floatdata));
             SetTexditData(tr("接受到的AmokADC数据为：")+QString("%1").arg(tempdata.m_uint32data));
        }
        else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
        {
            SetTexditData(tr("接受到的B模块ADC数据为：")+QString("%1").arg(tempdata.m_floatdata));
             SetTexditData(tr("接受到的B模块ADC数据为：")+QString("%1").arg(tempdata.m_uint32data));
        }

        break;
        //光强数据上报
    case 4:
//        SetTexditData(tr("接受到的光强数据为：")+QString(cmdInfo.baCmdContent));
        if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
        {
            SetTexditData(tr("接受到的A模块光强数据为：")+QString("%1").arg(tempdata.m_floatdata));
             SetTexditData(tr("接受到的A模块光强数据为：")+QString("%1").arg(tempdata.m_uint32data));
        }
        else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
        {
            SetTexditData(tr("接受到的B模块光强数据为：")+QString("%1").arg(tempdata.m_floatdata));
             SetTexditData(tr("接受到的B模块光强数据为：")+QString("%1").arg(tempdata.m_uint32data));
        }

        break;
        //模块参数设置
    case 5:
        switch (cmdInfo.uUartCmd.sUartCmd.unSubCmd)
        {
        case 1:
//             SetTexditData(tr("接受到的LED光强数据为：")+QString(cmdInfo.baCmdContent));
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("接受到的A模块LED光强数据为：")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("接受到的A模块LED光强数据为：")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("接受到的B模块LED光强数据为：")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("接受到的B模块LED光强数据为：")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        case 2:
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("接受到的A模块ADC速率为：")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("接受到的A模块ADC速率为：")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("接受到的B模块ADC速率为：")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("接受到的B模块ADC速率为：")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        case 3:
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("接受到的A模块ADC单位为：")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("接受到的A模块ADC单位为：")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("接受到的B模块ADC单位为：")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("接受到的B模块ADC单位为：")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        case 4:
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("接受到的A模块LED打开间隔为：")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("接受到的A模块LED打开间隔为：")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("接受到的B模块LED打开间隔为：")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("接受到的B模块LED打开间隔为：")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        case 5:
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("接受到的A模块延时时间为：")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("接受到的A模块延时时间为：")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("接受到的B模块延时时间为：")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("接受到的B模块延时时间为：")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        case 6:
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("接受到的A模块打开通道为：")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("接受到的A模块打开通道为：")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("接受到的B模块打开通道为：")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("接受到的B模块打开通道为：")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        case 7:
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("接受到的计A模块算量为：")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("接受到的A模块计算量为：")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("接受到的B模块计算量为：")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("接受到的B模块计算量为：")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        case 8:
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("接受到的A模块软件版本为：")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("接受到的A模块软件版本为：")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("接受到的B模块软件版本为：")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("接受到的B模块软件版本为：")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        case 9:
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("接受到的A模块硬件版本为：")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("接受到的A模块硬件版本为：")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("接受到的B模块硬件版本为：")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("接受到的B模块硬件版本为：")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        case 10:
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("接受到的A模块生产日期为：")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("接受到的A模块生产日期为：")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("接受到的B模块生产日期为：")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("接受到的B模块生产日期为：")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        default:
            SetTexditData(tr("接受到的模块配置数据错误！")+QString(cmdInfo.baCmdContent));
            break;

        }
        break;
        //错误状态上报
    case 6:
        if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
        {
            SetTexditData(tr("接受到的A模块荧光状态错误消息：")+QString::number(cmddata[0].m_uint32data));
        }
        else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
        {
            SetTexditData(tr("接受到的B模块荧光状态错误消息：")+QString::number(cmddata[0].m_uint32data));
        }

        break;
        //连续采样次数设置
    case 7:
//        ui->Line_adc_report->setText(QString(cmdInfo.baCmdContent));
        if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
        {
        SetTexditData(tr("接受到的A模块连续采样次数为：")+QString::number(cmddata[0].m_uint32data));
        }
        else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
        {
        SetTexditData(tr("接受到的B模块连续采样次数为：")+QString::number(cmddata[0].m_uint32data));
        }

        break;
        //模块参数获取
    case 8:
        break;
        //采集单孔荧光数据
    case 9:
    {

        SetTexditData(tr("获取到的荧光值为：")+QString::number(cmddata[0].m_uint32data));
//        double fluordata;
//        QString("%1").arg(cmddata[0].m_uint32data);
//        qDebug()<<"m_uint32data"<<cmddata[0].m_uint32data
//               <<"m_floatdata"<<cmddata[0].m_floatdata
//              <<"mdoubledata"<<cmddata[0].mdoubledata
//              <<"mdoubledata"<<static_cast<double>(cmddata[0].m_uint32data);
        ui->Wdg_debug->SLot_AddPointToCurve(0,check_size,cmddata[0].m_uint32data);
        fluo_size.append(cmddata[0].m_uint32data);
        check_size++;



        if( ui->Chb_fluo_auto_acqu->checkState() == Qt::Checked)
        {
            qDebug()<<"连续测试";
            debug_flou_size++;
            SetTexditData(tr("自动采集荧光,当前采集第")+QString::number(debug_flou_size)+"/"+QString::number(ui->Spx_fluo_number->value()) +tr("次"));
            //判断采集了多少次荧光
            if(debug_flou_size >= ui->Spx_fluo_number->value())
            {
                ui->Chb_fluo_auto_acqu->setCheckState(Qt::Unchecked);
                debug_flou_size=0;
                return;
            }
            else
            {

                //下发让电机运动命令
                on_Btn_elec_set_point_clicked();

            }

        }
        else
        {
            qDebug()<<"非连续测试";
            if(point_and_flou == 0)
            {
                //最佳位置测试
                //下发让电机运动命令
//                on_Btn_elec_set_point_clicked();
            }
            else if(point_and_flou == 1)
            {
                //单孔采集荧光
//                QThread::msleep(500);
                on_Btn_coll_fluo_clicked();
            }
        }

        if(electric_machinery_point == 0)
        {
//           emit Sign_check_state(true);
        }
        else
        {
            qDebug()<<"最佳荧光 运动电机";

            //判断当前荧光位置是否已运动到孔外部
            if(electric_machinery_point > (ui->Led_fluo_Best_location->text().toInt()+ui->Spx_fluo_hole_width->value()/2+10))
            {
                electric_machinery_point=0;
                check_size=0;
                //获取最佳数据
                Slot_gat_max();
            }
            else
            {
                on_Btn_fluo_Best_location_clicked();
            }

        }


    }
        break;
    default:
        break;

    }
}
//主控界面串口接受函数
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
    QList<DataContent> cmddata;
    cmddata.clear();
    DataContent tempdata;
    check_cmd=true;
    for (int i=0; i<cmdInfo.baCmdContent.size()/4; i++) {
        tempdata.m_uint8data[0] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+0));
        tempdata.m_uint8data[1] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+1));
        tempdata.m_uint8data[2] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+2));
        tempdata.m_uint8data[3] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+3));
        cmddata.append(tempdata);
    }
    switch (cmdInfo.uUartCmd.sUartCmd.unFunId)
    {
    //串口握手
    case 1:
        //显示硬件号和软件版本号
    {
//        QList<DataContent> cmddata;
//        for (int i=0; i<cmdInfo.baCmdContent.size()/4; i++) {
//            DataContent tempdata;
//            tempdata.m_uint8data[0] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+0));
//            tempdata.m_uint8data[1] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+1));
//            tempdata.m_uint8data[2] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+2));
//            tempdata.m_uint8data[3] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+3));
//            cmddata.append(tempdata);
//        }
        ui->Led_soft_number->setText(QString::number(cmddata[0].m_uint32data));
        ui->Led_hard_number->setText(QString::number(cmddata[1].m_uint32data));
    }


        break;
         //风扇控制
    case 2:
//            SetTexditData(tr("获取到的荧光值为：")+QString::number(cmddata[0].m_uint32data));
        if((cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
        {
            if(tempdata.m_uint32data == 0)
            {
                SetTexditData(tr("获取到A模块风扇为关闭状态"));
            }
            else if(tempdata.m_uint32data == 1)
            {
                SetTexditData(tr("获取到A模块风扇为开启状态"));
            }

        }
        else if((cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 1)
        {
            if(tempdata.m_uint32data == 0)
            {
                SetTexditData(tr("获取到B模块风扇为关闭状态"));
            }
            else if(tempdata.m_uint32data == 1)
            {
                SetTexditData(tr("获取到B模块风扇为开启状态"));
            }
        }

        break;
         //报告风扇状态
    case 3:
        if((cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
        {
            if(tempdata.m_uint32data == 0)
            {
                SetTexditData(tr("获取到A模块风扇为关闭状态"));
            }
            else if(tempdata.m_uint32data == 1)
            {
                SetTexditData(tr("获取到A模块风扇为开启状态"));
            }

        }
        else if((cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 1)
        {
            if(tempdata.m_uint32data == 0)
            {
                SetTexditData(tr("获取到B模块风扇为关闭状态"));
            }
            else if(tempdata.m_uint32data == 1)
            {
                SetTexditData(tr("获取到B模块风扇为开启状态"));
            }
        }
        break;
         //热盖控制
    case 4:

        if((cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
        {
            if(tempdata.m_uint32data == 0)
            {
                SetTexditData(tr("获取到A模块热盖为关闭状态"));
            }
            else if(tempdata.m_uint32data == 1)
            {
                SetTexditData(tr("获取到A模块热盖为开启状态"));
            }

        }
        else if((cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 1)
        {
            if(tempdata.m_uint32data == 0)
            {
                SetTexditData(tr("获取到B模块热盖为关闭状态"));
            }
            else if(tempdata.m_uint32data == 1)
            {
                SetTexditData(tr("获取到B模块热盖为开启状态"));
            }
        }

        break;
         //报告热改状态
    case 5:
        SetTexditData(tr("热盖达到设定温度"));

        break;
         //实验过程通知
    case 6:
        break;
         //仪器参数读写
    case 7:
        break;
         //热盖设定温度的读写
    case 8:

//        SetTexditData(tr("热盖设定温度为：")+QString(cmdInfo.baCmdContent));
        if((cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
        {
            SetTexditData(tr("获取到A模块热盖设定温度为：：")+QString("%1").arg(tempdata.m_floatdata));
            SetTexditData(tr("获取到A模块热盖设定温度为：：")+QString("%1").arg(tempdata.m_uint32data));
        }
        else if((cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 1)
        {
            SetTexditData(tr("获取到B模块热盖设定温度为：：")+QString("%1").arg(tempdata.m_floatdata));
            SetTexditData(tr("获取到B模块热盖设定温度为：：")+QString("%1").arg(tempdata.m_uint32data));
        }

        break;
         //温度补偿参数读写
    case 9:
        if((cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
        {
            if(cmdInfo.uUartCmd.sUartCmd.unSubCmd == 0)
            {
                SetTexditData(tr("获取到A模块热盖温度补偿参数为：")+QString("%1").arg(tempdata.m_uint32data));
                SetTexditData(tr("获取到A模块热盖温度补偿参数为：")+QString("%1").arg(tempdata.m_floatdata));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unSubCmd == 1)
            {
                SetTexditData(tr("获取到A模块热盖温度补偿参数为：")+QString("%1").arg(tempdata.m_uint32data));
                SetTexditData(tr("获取到A模块热盖温度补偿参数为：")+QString("%1").arg(tempdata.m_floatdata));
            }

        }
        else if((cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 1)
        {
            if(cmdInfo.uUartCmd.sUartCmd.unSubCmd == 0)
            {
                SetTexditData(tr("获取到B模块热盖温度补偿参数为：")+QString("%1").arg(tempdata.m_uint32data));
                SetTexditData(tr("获取到B模块热盖温度补偿参数为：")+QString("%1").arg(tempdata.m_floatdata));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unSubCmd == 1)
            {
                SetTexditData(tr("获取到B模块热盖温度补偿参数为：")+QString("%1").arg(tempdata.m_uint32data));
                SetTexditData(tr("获取到B模块热盖温度补偿参数为：")+QString("%1").arg(tempdata.m_floatdata));
            }
        }
        break;
         //仪器错误报告
    case 10:
        break;
         //读取铜座温度
    case 11:
        SetTexditData(tr("铜座当前温度为：")+QString(cmdInfo.baCmdContent));
        break;
    }
}
//调试界面串口接受函数
void MainWindow::GetDebugControl(SRequestCmdInfo cmdInfo)
{
    QList<DataContent> cmddata;
    cmddata.clear();
    DataContent tempdata;
    for (int i=0; i<cmdInfo.baCmdContent.size()/4; i++) {
        tempdata.m_uint8data[0] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+0));
        tempdata.m_uint8data[1] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+1));
        tempdata.m_uint8data[2] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+2));
        tempdata.m_uint8data[3] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+3));
//        tempdata.m_uint8data[3] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+0));
//        tempdata.m_uint8data[2] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+1));
//        tempdata.m_uint8data[1] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+2));
//        tempdata.m_uint8data[0] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+3));
        cmddata.append(tempdata);
//        qDebug()<<"tempdata.m_uint8data[3]"<<QString("%1").number(tempdata.m_uint8data[3],16);
//        qDebug()<<"tempdata.m_uint8data[2]"<<QString("%1").number(tempdata.m_uint8data[2],16);
//        qDebug()<<"tempdata.m_uint8data[1]"<<QString("%1").number(tempdata.m_uint8data[1],16);
//        qDebug()<<"tempdata.m_uint8data[0]"<<QString("%1").number(tempdata.m_uint8data[0],16);
    }
    switch (cmdInfo.uUartCmd.sUartCmd.unFunId)
    {
    //修改仪器运行模式
    case 1:

//        SetTexditData(tr("仪器当前运行模式为：")+QString(cmddata[0].m_uint32data));
        if(cmdInfo.uUartCmd.sUartCmd.unSubCmd == 0)
        {

        }
        else if(cmdInfo.uUartCmd.sUartCmd.unSubCmd == 1)
        {

        }
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

        break;
        //荧光采集位置的读取和保存
    case 3:

        if(cmdInfo.uUartCmd.sUartCmd.unSubCmd == 0)
        {

        }
        else if(cmdInfo.uUartCmd.sUartCmd.unSubCmd == 1)
        {

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
        SetTexditData(tr("获取到的荧光采集位置：")+QString::number(cmddata[0].mdoubledata));
        qDebug()<<"获取到的荧光采集位置："<<QString::number(cmddata[0].m_uint32data)
                <<QString::number(cmddata[0].mdoubledata)<<cmddata[0].m_uint32data<<cmddata[0].mdoubledata;


        break;
        //电机运动
    case 4:
        SetTexditData(tr("电机当前位置：")+QString::number(cmddata[0].m_uint32data));
//        ui->Led_eles_get_point->setText(QString::number(cmddata[0].m_uint32data));
        //电机返回当前位置后采集荧光
//        if( qAbs(static_cast<quint32>(electric_machinery_point) - cmddata[0].m_uint32data) < 3)
//        {

//        }
        if( ui->Chb_fluo_auto_acqu->checkState() == Qt::Checked)
        {
            //下发采集荧光指令
            on_Chb_fluo_auto_acqu_clicked(true);
        }
        else
        {
            if(electric_machinery_point != 0)
            {
                qDebug()<<"最佳荧光 采集荧光";
                Get_Fluo_Best_Location();
            }
        }


        break;
    default:
        break;

    }
}


//设置温度补偿值
void MainWindow::on_Btn_temp_comp_clicked()
{
    if(ui->Line_temp_comp_set->text().isEmpty())
    {
        return;
    }

    requescmdInfo.uUartCmd.sUartCmd.unFunId=5;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_T_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=1;

    requescmdInfo.baCmdContent="";
    DataContent datacon;
    datacon.m_uint32data=0;
//    datacon.m_uint32data=static_cast<quint32>(ui->Line_temp_comp_set->text().toInt());
    datacon.m_floatdata=static_cast<float>(ui->Line_temp_comp_set->text().toFloat());
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();
    SetTexditData(tr("设置")+ui->comboBox_T_Channel->currentText()+tr("模块温度补偿值：")+ui->Line_temp_comp_set->text());
//    SetTexditData(tr("温度补偿值：")+ui->Line_temp_comp_set->text());
}

void MainWindow::on_Btn_KP_clicked()
{
    if(ui->Line_KP_set->text().isEmpty())
    {
        return;
    }

    requescmdInfo.uUartCmd.sUartCmd.unFunId=5;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_T_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=2;

    requescmdInfo.baCmdContent="";
    DataContent datacon;

//    datacon.m_uint32data=static_cast<quint32>(ui->Line_KP_set->text().toInt());
     datacon.m_floatdata=static_cast<float>(ui->Line_KP_set->text().toFloat());
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();

//    SetTexditData(tr("设置KP值：")+ui->Line_KP_set->text());
        SetTexditData(tr("设置")+ui->comboBox_T_Channel->currentText()+tr("模块PID中P值：")+ui->Line_KP_set->text());
}

void MainWindow::on_Btn_KI_clicked()
{
    if(ui->Line_KI_set->text().isEmpty())
    {
        return;
    }

    requescmdInfo.uUartCmd.sUartCmd.unFunId=5;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_T_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=3;

    requescmdInfo.baCmdContent="";
    DataContent datacon;
//    datacon.m_uint32data=0;
//    datacon.m_uint32data=static_cast<quint32>(ui->Line_KI_set->text().toInt());
     datacon.m_floatdata=static_cast<float>(ui->Line_KI_set->text().toFloat());
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();

//    SetTexditData(tr("设置KI值：")+ui->Line_KI_set->text());
    SetTexditData(tr("设置")+ui->comboBox_T_Channel->currentText()+tr("模块PID中I值：")+ui->Line_KI_set->text());

}

void MainWindow::on_Btn_KD_clicked()
{
    if(ui->Line_KD_set->text().isEmpty())
    {
        return;
    }

    requescmdInfo.uUartCmd.sUartCmd.unFunId=5;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_T_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=4;

    requescmdInfo.baCmdContent="";
    DataContent datacon;
    datacon.m_uint32data=0;
//    datacon.m_uint32data=static_cast<quint32>(ui->Line_KD_set->text().toInt());
    datacon.m_floatdata=static_cast<float>(ui->Line_KD_set->text().toFloat());
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();

//    SetTexditData(tr("设置KD值：")+ui->Line_KD_set->text());
    SetTexditData(tr("设置")+ui->comboBox_T_Channel->currentText()+tr("模块PID中D值：")+ui->Line_KD_set->text());

}
//设置目标温度 Line_temp_target_set
void MainWindow::on_Btn_temp_target_clicked()
{
    if(ui->Line_temp_target_set->text().isEmpty())
    {
        return;
    }

    requescmdInfo.uUartCmd.sUartCmd.unFunId=3;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_T_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    requescmdInfo.baCmdContent="";
    DataContent datacon;
//    datacon.m_uint32data=0;
//    datacon.m_uint32data=static_cast<quint32>(ui->Line_temp_target_set->text().toInt());
    datacon.m_floatdata=static_cast<float>(ui->Line_temp_target_set->text().toFloat());
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();
    SetTexditData(tr("设置")+ui->comboBox_T_Channel->currentText()+tr("模块目标温度值：")+ui->Line_temp_target_set->text());
//    SetTexditData(tr("设置目标温度值：")+ui->Line_temp_target_set->text());
}
//设置溶解曲线类型(溶解曲线只下发命令，让下位机切换溶解PID;扩增命令也是只下发，让下位机切换扩增PID，都不带参数)
//设置升温速率
void MainWindow::on_Btn_temp_up_clicked()
{
    if(ui->Line_temp_up_set->text().isEmpty())
    {
        return;
    }

    requescmdInfo.uUartCmd.sUartCmd.unFunId=6;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_T_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=2;

    requescmdInfo.baCmdContent="";
    DataContent datacon;
    datacon.m_uint32data=0;
//    datacon.m_uint32data=static_cast<quint32>(ui->Line_temp_up_set->text().toInt());
    datacon.m_floatdata=static_cast<float>(ui->Line_temp_up_set->text().toFloat());
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();

    SetTexditData(tr("设置")+ui->comboBox_T_Channel->currentText()+tr("模块升温速率：")+ui->Line_temp_up_set->text());
//    SetTexditData(tr("设置升温速率：")+ui->Line_temp_up_set->text());

}

void MainWindow::on_Btn_temp_down_clicked()
{
    if(ui->Line_temp_down_set->text().isEmpty())
    {
        return;
    }

    requescmdInfo.uUartCmd.sUartCmd.unFunId=6;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_T_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=3;

    requescmdInfo.baCmdContent="";
    DataContent datacon;
    datacon.m_uint32data=0;
//    datacon.m_uint32data=static_cast<quint32>(ui->Line_temp_down_set->text().toInt());
    datacon.m_floatdata=static_cast<float>(ui->Line_temp_down_set->text().toFloat());
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();

//    SetTexditData(tr("设置降温速率：")+ui->Line_temp_down_set->text());
    SetTexditData(tr("设置")+ui->comboBox_T_Channel->currentText()+tr("模块降温速率：")+ui->Line_temp_down_set->text());

}
//溶解曲线起始值
void MainWindow::on_Btn_diss_curve_start_clicked()
{
    if(ui->Line_diss_curve_start_set->text().isEmpty())
    {
        return;
    }

    requescmdInfo.uUartCmd.sUartCmd.unFunId=6;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_T_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=4;

    requescmdInfo.baCmdContent="";
    DataContent datacon;
    datacon.m_uint32data=0;
//    datacon.m_uint32data=static_cast<quint32>(ui->Line_diss_curve_start_set->text().toInt());
    datacon.m_floatdata=static_cast<float>(ui->Line_diss_curve_start_set->text().toFloat());
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();

//    SetTexditData(tr("设置曲线起始值：")+ui->Line_diss_curve_start_set->text());
    SetTexditData(tr("设置")+ui->comboBox_T_Channel->currentText()+tr("模块曲线起始值：")+ui->Line_diss_curve_start_set->text());

}

void MainWindow::on_Btn_diss_curve_end_clicked()
{
    if(ui->Line_diss_curve_end_set->text().isEmpty())
    {
        return;
    }

    requescmdInfo.uUartCmd.sUartCmd.unFunId=6;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_T_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=5;

    requescmdInfo.baCmdContent="";
    DataContent datacon;
    datacon.m_uint32data=0;
//    datacon.m_uint32data=static_cast<quint32>(ui->Line_diss_curve_end_set->text().toInt());
    datacon.m_floatdata=static_cast<float>(ui->Line_diss_curve_end_set->text().toFloat());
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();

//    SetTexditData(tr("设置曲线终止值：")+ui->Line_diss_curve_end_set->text());
    SetTexditData(tr("设置")+ui->comboBox_T_Channel->currentText()+tr("模块曲线终止值：")+ui->Line_diss_curve_end_set->text());

}

void MainWindow::on_Btn_diss_curve_max_clicked()
{
    if(ui->Line_diss_curve_max_set->text().isEmpty())
    {
        return;
    }

    requescmdInfo.uUartCmd.sUartCmd.unFunId=6;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_T_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=6;

    requescmdInfo.baCmdContent="";
    DataContent datacon;
    datacon.m_uint32data=0;
//    datacon.m_uint32data=static_cast<quint32>(ui->Line_diss_curve_max_set->text().toInt());
    datacon.m_floatdata=static_cast<float>(ui->Line_diss_curve_max_set->text().toFloat());
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();

//    SetTexditData(tr("设置溶解曲线最大值：")+ui->Line_diss_curve_max_set->text());
    SetTexditData(tr("设置")+ui->comboBox_T_Channel->currentText()+tr("模块溶解曲线最大值：")+ui->Line_diss_curve_max_set->text());

}
//发送串口数据
//void MainWindow::on_Btn_send_data_clicked()
//{
//    on_Btn_update_head_clicked();
//   emit Sign_SendDriver(send_data);
//}

void MainWindow::GetInitDeviceState()
{
    requescmdInfo.baCmdContent="";
    requescmdInfo.uUartCmd.sUartCmd.unStartTag=0xff;
    requescmdInfo.uUartCmd.sUartCmd.unDataLen_empty=0xff;

    requescmdInfo.uUartCmd.sUartCmd.unDataLen=sizeof(requescmdInfo.uUartCmd) + static_cast<quint32>(requescmdInfo.baCmdContent.size());
    requescmdInfo.uUartCmd.sUartCmd.unCrc=0;

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

void MainWindow::on_comboBox_F_Dyestuff_activated(int index)
{
    m_thiCMD.ThiCmd=static_cast<quint8>(index) ;
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

//    ui->Btn_fan_ctrl->setEnabled(false);
//    ui->Line_lid_ctrl_set->setEnabled(false);
//    ui->Btn_lid_ctrl->setEnabled(false);
//    ui->Line_hot_lid_set->setEnabled(false);
//    ui->Btn_hot_lid->setEnabled(false);
//    ui->Line_temp_compen_set->setEnabled(false);
//    ui->Btn_temp_compen->setEnabled(false);
}
//采样次数设置
void MainWindow::on_Btn_adc_report_clicked()
{

    requescmdInfo.uUartCmd.sUartCmd.unFunId=7;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_F_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    DataContent datacon;
    datacon.m_uint32data=static_cast<quint32>(ui->Line_adc_report_set->text().toInt());
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();

//    SetTexditData(tr("下发采样次数：")+ui->Line_adc_report_set->text());
    SetTexditData(tr("设置")+ui->comboBox_F_Channel->currentText()+tr("模块采样次数：")+ui->Line_adc_report_set->text());


}
//Line_led_intens_set
void MainWindow::on_Btn_led_intens_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=5;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_F_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=1;

    DataContent datacon;
    datacon.m_uint32data=static_cast<quint32>(ui->Line_led_intens_set->text().toInt());
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();
//    SetTexditData(tr("下发采样次数：")+ui->Line_led_intens_set->text());
    SetTexditData(tr("设置")+ui->comboBox_F_Channel->currentText()+tr("模块LED光强：")+ui->Line_led_intens_set->text());
}
//Line_adc_rate_set
void MainWindow::on_Btn_adc_rate_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=5;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_F_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=2;

    DataContent datacon;
    datacon.m_uint32data=static_cast<quint32>(ui->Line_adc_rate_set->text().toInt());
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();
//    SetTexditData(tr("下发采样次数：")+ui->Line_led_intens_set->text());
    SetTexditData(tr("设置")+ui->comboBox_F_Channel->currentText()+tr("模块ADC速率：")+ui->Line_adc_rate_set->text());
}
//Line_adc_nu_set
void MainWindow::on_Btn_adc_nu_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=5;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_F_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=3;

    DataContent datacon;
    datacon.m_uint32data=static_cast<quint32>(ui->Line_adc_nu_set->text().toInt());
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();
//    SetTexditData(tr("下发采样次数：")+ui->Line_adc_nu_set->text());
    SetTexditData(tr("设置")+ui->comboBox_F_Channel->currentText()+tr("模块ADC单位：")+ui->Line_adc_nu_set->text());
}
//Line_led_gap_set
void MainWindow::on_Btn_led_gap_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=5;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_F_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=4;

    DataContent datacon;
    datacon.m_uint32data=static_cast<quint32>(ui->Line_led_gap_set->text().toInt());
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();
//    SetTexditData(tr("下发采样次数：")+ui->Line_led_gap_set->text());
     SetTexditData(tr("设置")+ui->comboBox_F_Channel->currentText()+tr("模块LED采集间隔：")+ui->Line_led_gap_set->text());
}
//Line_delay_time_set
void MainWindow::on_Btn_delay_time_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=5;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_F_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=5;

    DataContent datacon;
    datacon.m_uint32data=static_cast<quint32>(ui->Line_delay_time_set->text().toInt());
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();
//    SetTexditData(tr("下发采样次数：")+ui->Line_delay_time_set->text());
    SetTexditData(tr("设置")+ui->comboBox_F_Channel->currentText()+tr("模块延时时间：")+ui->Line_delay_time_set->text());
}
//Line_channel_set
void MainWindow::on_Btn_channel_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=5;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_F_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=6;

    DataContent datacon;
    datacon.m_uint32data=static_cast<quint32>(ui->Line_channel_set->text().toInt());
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();
//    SetTexditData(tr("下发采样次数：")+ui->Line_channel_set->text());
    SetTexditData(tr("设置")+ui->comboBox_T_Channel->currentText()+tr("模块打开通道：")+ui->Line_channel_set->text());
}

void MainWindow::on_Btn_calcul_nu_clicked()
{

    requescmdInfo.uUartCmd.sUartCmd.unFunId=5;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_F_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=7;

    DataContent datacon;
    datacon.m_uint32data=static_cast<quint32>(ui->Line_calcul_nu_set->text().toInt());
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();
//    SetTexditData(tr("下发采样次数：")+ui->Line_calcul_nu_set->text());
    SetTexditData(tr("设置")+ui->comboBox_F_Channel->currentText()+tr("模块计算单位：")+ui->Line_calcul_nu_set->text());
}

void MainWindow::on_Btn_software_ver_clicked()
{
    //Line_software_ver_set
    requescmdInfo.uUartCmd.sUartCmd.unFunId=5;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_F_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=8;

    DataContent datacon;
    datacon.m_uint32data=static_cast<quint32>(ui->Line_software_ver_set->text().toInt());
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();
    SetTexditData(tr("设置软件版本为：")+ui->Line_software_ver_set->text());
}

void MainWindow::on_Btn_hardware_ver_clicked()
{
    //Line_hardware_ver_set
    requescmdInfo.uUartCmd.sUartCmd.unFunId=5;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_F_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=9;

    DataContent datacon;
    datacon.m_uint32data=static_cast<quint32>(ui->Line_hardware_ver_set->text().toInt());
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();
    SetTexditData(tr("设置硬件版本为：")+ui->Line_hardware_ver_set->text());
}

void MainWindow::on_Btn_production_date_clicked()
{
    //Line_production_date
    requescmdInfo.uUartCmd.sUartCmd.unFunId=5;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_F_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=10;

    DataContent datacon;
    datacon.m_uint32data=static_cast<quint32>(ui->Line_production_date->text().toInt());
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();
    SetTexditData(tr("设置生产日期：")+ui->Line_production_date->text());
}
/*
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
//        ui->lineEditLine_fan_ctrl_set->setEnabled(true);
//        ui->Btn_fan_ctrl->setEnabled(true);
        break;
         //报告风扇状态
    case 3:
        break;
         //热盖控制
    case 4:
//        ui->Line_lid_ctrl_set->setEnabled(true);
//        ui->Btn_lid_ctrl->setEnabled(true);
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
//        ui->Line_temp_compen_set->setEnabled(true);
//        ui->Btn_temp_compen->setEnabled(true);
        break;
         //仪器错误报告
    case 10:
        break;
         //读取铜座温度
    case 11:
        break;
    }
}
*/
//设置温度补偿参数
//void MainWindow::on_Btn_temp_compen_clicked()
//{
//    m_cmd_bit.FunRW=1;
//    requescmdInfo.baCmdContent=ui->Line_temp_compen_set->text().toLocal8Bit();
//}
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
//    if(index == 0)
//    {
//        ui->Wdg_T_mode->Cleardata();
//        check_curve=0;
//        curve_point=0;
//    }
//    else
//    {
//        ui->Wdg_T_mode_B->Cleardata();
//        check_curve_B=0;
//        curve_point_B=0;
//    }




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

//   qDebug()<<"下发数据 baCmdContent"<<requescmdInfo.baCmdContent;

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

//        qDebug()<<"CRC="<<crc_temp;
//        qDebug()<<"下发数据 send_data"<<send_data;
//        quint8  *pPacketData_t = (quint8 *)(send_data.data());
//        memcpy(requescmdInfo.uUartCmd.ucCmd, pPacketData_t, 8);
//        qDebug()<<"0"<<QString("%1").number(requescmdInfo.uUartCmd.ucCmd[0],16);
//        qDebug()<<"1"<<QString("%1").number(requescmdInfo.uUartCmd.ucCmd[1],16);
//        qDebug()<<"2"<<QString("%1").number(requescmdInfo.uUartCmd.ucCmd[2],16);
//        qDebug()<<"3"<<QString("%1").number(requescmdInfo.uUartCmd.ucCmd[3],16);
//        qDebug()<<"4"<<QString("%1").number(requescmdInfo.uUartCmd.ucCmd[4],16);
//        qDebug()<<"5"<<QString("%1").number(requescmdInfo.uUartCmd.ucCmd[5],16);
//        qDebug()<<"6"<<QString("%1").number(requescmdInfo.uUartCmd.ucCmd[6],16);
//        qDebug()<<"7"<<QString("%1").number(requescmdInfo.uUartCmd.ucCmd[7],16);

//        SRequestCmdInfo cmdInfo;
//        cmdInfo.baCmdContent    = send_data.mid(8);
//        DataContent tempdata;
//        for (int i=0; i<cmdInfo.baCmdContent.size()/4; i++)
//        {
//            tempdata.m_uint8data[0] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+0));
//            tempdata.m_uint8data[1] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+1));
//            tempdata.m_uint8data[2] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+2));
//            tempdata.m_uint8data[3] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+3));
//            qDebug()<<"tempdata.m_uint8data[3]"<<QString("%1").number(tempdata.m_uint8data[3],16);
//            qDebug()<<"tempdata.m_uint8data[2]"<<QString("%1").number(tempdata.m_uint8data[2],16);
//            qDebug()<<"tempdata.m_uint8data[1]"<<QString("%1").number(tempdata.m_uint8data[1],16);
//            qDebug()<<"tempdata.m_uint8data[0]"<<QString("%1").number(tempdata.m_uint8data[0],16);
//            qDebug()<<"tempdata.m_uint32data"<<tempdata.m_uint32data;
//            qDebug()<<"tempdata.mdoubledata"<<tempdata.mdoubledata;
//            qDebug()<<"tempdata.m_floatdata"<<tempdata.m_floatdata;
//        }





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
    requescmdInfo.uUartCmd.sUartCmd.unFunId=1;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    //    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->Cbx_modu->currentIndex()<<3);
          requescmdInfo.uUartCmd.sUartCmd.unCh=0;
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=4;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    DataContent datacon;
    datacon.m_uint32data=static_cast<quint32>(ui->Cbx_mode->currentIndex()) ;
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();
//    SetTexditData(tr("设置")+ui->Cbx_modu->currentText()+tr("模块运行模式为：")+ui->Cbx_mode->currentText());
    SetTexditData(tr("设置模式为：")+ui->Cbx_mode->currentText());


}
//读取设备运行模式
void MainWindow::on_Btn_get_mode_clicked()
{

    requescmdInfo.uUartCmd.sUartCmd.unFunId=1;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    //    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->Cbx_modu->currentIndex()<<3);
          requescmdInfo.uUartCmd.sUartCmd.unCh=0;
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=4;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;
    requescmdInfo.baCmdContent="";

    ReceiveCmd();
//    SetTexditData(tr("读取")+ui->Cbx_modu->currentText()+tr("模块运行模式"));
    SetTexditData(tr("读取运行模式"));
}
//荧光采集位置读取和保存 下发设置
void MainWindow::on_Btn_set_fluo_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=3;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
//    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->Cbx_rotate_dire->currentIndex()<<3)+1;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->Cbx_rotate_dire->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=4;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    DataContent datacon;
    datacon.m_uint32data=static_cast<quint32>(ui->Led_fluo_set_point->text().toInt());
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();
//    SetTexditData(tr("设置")+ui->Cbx_modu->currentText()+tr("模块荧光采集位置：")+ui->Led_fluo_set_point->text()+tr("选装方向为+")+ui->Cbx_rotate_dire->currentText());
    SetTexditData(tr("设置荧光采集位置：")+ui->Led_fluo_set_point->text()+tr("选装方向为+")+ui->Cbx_rotate_dire->currentText());
}
//荧光采集位置读取和保存 读取数据
void MainWindow::on_Btn_get_fluo_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=3;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->Cbx_rotate_dire->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=4;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;
    requescmdInfo.baCmdContent="";

    ReceiveCmd();
//     SetTexditData(tr("读取")+ui->Cbx_modu->currentText()+tr("模块荧光采集位置"));
    SetTexditData(tr("读取荧光采集位置"));
}
//采集荧光
void MainWindow::on_Btn_coll_fluo_clicked()
{
//    for (int i=0;i<number;i++)
//    {
//        requescmdInfo.uUartCmd.sUartCmd.unFunId=9;
//        requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
//        requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
//        requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
//        requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->Cbx_modu->currentIndex()<<3);
//        requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
//        requescmdInfo.uUartCmd.sUartCmd.unSubCmd=static_cast<quint8>(ui->Cbx_fluo_type->currentIndex());

//        requescmdInfo.baCmdContent="";

//        ReceiveCmd();
//        SetTexditData(tr("模块")+ui->Cbx_modu->currentText()+tr("开始采集荧光,当前采集第")+QString::number(i+1)+"/"+QString::number(number) +tr("次"));
//        QTimer::singleShot(500,this,SLOT(Get_Point_Fluo()));
//    }
    point_and_flou=1;
    if(fluo_check < ui->Spx_fluo_point_number->value())
    {
        /*
        SetTexditData(tr("开始采集荧光,当前采集第")+QString::number(fluo_check+1)+"/"+QString::number(ui->Spx_fluo_point_number->value()) +tr("次"));
        QTimer::singleShot(1000,this,SLOT(Get_Point_Fluo()));
        fluo_check++;
        */
        SetTexditData(tr("开始采集荧光,当前采集第")+QString::number(fluo_check+1)+"/"+QString::number(ui->Spx_fluo_point_number->value()) +tr("次"));

        qDebug()<<"开始采集荧光,当前采集第"+QString::number(fluo_check+1)+"/"+QString::number(ui->Spx_fluo_point_number->value()) +tr("次");
        Get_Point_Fluo();
        fluo_check++;
    }
    else
    {
        fluo_check=0;
    }
}
//设置电机位置
void MainWindow::on_Btn_elec_set_point_clicked()
{

    requescmdInfo.uUartCmd.sUartCmd.unFunId=4;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
//    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->Cbx_modu->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unCh=0;
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=4;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    DataContent datacon;
    datacon.m_uint32data=static_cast<quint32>(ui->Led_eles_set_point->text().toInt());
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();

    ui->Led_eles_set_point->setText(QString("%1").arg(ui->Spx_elec_step->value()+ui->Led_eles_set_point->text().toInt()));
//    SetTexditData(tr("设置")+ui->Cbx_modu->currentText()+tr("模块电机位置：")+ui->Led_eles_set_point->text());
     SetTexditData(tr("设置电机位置：")+QString("%1").arg(datacon.m_uint32data));

}
//读取电机位置
/*
void MainWindow::on_Btn_elec_get_point_clicked()
{

    requescmdInfo.uUartCmd.sUartCmd.unFunId=4;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=0;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
//    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->Cbx_modu->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unCh=0;
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=4;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;
    requescmdInfo.baCmdContent="";

    ReceiveCmd();
//     SetTexditData(tr("读取")+ui->Cbx_modu->currentText()+tr("模块电机位置"));
     SetTexditData(tr("读取电机位置"));

}
*/
//void MainWindow::on_pushButton_clicked()
//{
//    mythread_t=new MyThrtead_T;
//    mythread_t->setpoint(ui->Spx_fluo_number->value());
//    connect(mythread_t,&MyThrtead_T::Sign_set_point,this,&MainWindow::on_Btn_elec_set_point_clicked);
////    connect(mythread_t,&MyThrtead_T::Sign_get_coll_fluo,this,&MainWindow::on_Btn_coll_fluo_clicked);
//    connect(mythread_t,&MyThrtead_T::Sign_get_coll_fluo,this,&MainWindow::Get_cont_Fluo);
//    connect(mythread_t,&MyThrtead_T::Signmax,this,&MainWindow::Slot_gat_max);
//    connect(this,&MainWindow::Sign_check_state,mythread_t,&MyThrtead_T::Slot_get_check_state);
//    mythread_t->start();

//    SetTexditData(tr("设置")+ui->Cbx_modu->currentText()+tr("模块采集荧光测试 采集次数为：")+QString("%1").arg(ui->Spx_fluo_number->value()));
//    SetTexditData(tr("设置采集荧光测试 采集次数为：")+QString("%1").arg(ui->Spx_fluo_number->value()));
//}

/*Get_cont_Fluo
void MainWindow::on_Chb_passageway_open_clicked(bool checked)
{
    if(checked)
    {
//        ui->Chb_passageway_close->setChecked(false);
//        ui->Chb_passageway_open->setChecked(true);
        requescmdInfo.uUartCmd.sUartCmd.unFunId=1;
        requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
        requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
        requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
        requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_T_Channel->currentIndex()<<3);
        requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
        requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;
        requescmdInfo.baCmdContent="";
        ReceiveCmd();
    }
}

void MainWindow::on_Chb_passageway_close_clicked(bool checked)
{
    if(checked)
    {
//        ui->Chb_passageway_close->setChecked(true);
//        ui->Chb_passageway_open->setChecked(false);
        requescmdInfo.uUartCmd.sUartCmd.unFunId=1;
        requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
        requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
        requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
        requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_T_Channel->currentIndex()<<3);
        requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
        requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;
        requescmdInfo.baCmdContent="";

        ReceiveCmd();
    }
}
*/

void MainWindow::on_Btn_temp_comp_get_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=5;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_T_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=1;

    requescmdInfo.baCmdContent="";

    ReceiveCmd();
    SetTexditData(tr("读取")+ui->comboBox_T_Channel->currentText()+tr("模块温度补偿值"));
}


void MainWindow::on_Btn_temp_target_get_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=3;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_T_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    requescmdInfo.baCmdContent="";

    ReceiveCmd();
    SetTexditData(tr("读取")+ui->comboBox_T_Channel->currentText()+tr("模块目标温度值"));
}



void MainWindow::on_Btn_temp_up_get_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=6;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_T_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=2;

    requescmdInfo.baCmdContent="";

    ReceiveCmd();
    SetTexditData(tr("读取")+ui->comboBox_T_Channel->currentText()+tr("模块升温速率"));
}

void MainWindow::on_Btn_temp_down_get_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=6;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_T_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=3;

    requescmdInfo.baCmdContent="";

    ReceiveCmd();
    SetTexditData(tr("读取")+ui->comboBox_T_Channel->currentText()+tr("模块降温速率"));
}

void MainWindow::on_Btn_diss_curve_start_get_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=6;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_T_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=4;

    requescmdInfo.baCmdContent="";

    ReceiveCmd();
    SetTexditData(tr("读取")+ui->comboBox_T_Channel->currentText()+tr("溶解曲线起始值"));
}

void MainWindow::on_Btn_diss_curve_endget_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=6;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_T_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=5;

    requescmdInfo.baCmdContent="";

    ReceiveCmd();
    SetTexditData(tr("读取")+ui->comboBox_T_Channel->currentText()+tr("溶解曲线终止值"));
}

void MainWindow::on_Btn_diss_curve_max_get_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=6;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_T_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=6;

    requescmdInfo.baCmdContent="";

    ReceiveCmd();
    SetTexditData(tr("读取")+ui->comboBox_T_Channel->currentText()+tr("溶解曲线最大值"));
}

void MainWindow::on_Btn_adc_report_get_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=7;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_F_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    requescmdInfo.baCmdContent="";

    ReceiveCmd();
    SetTexditData(tr("读取")+ui->comboBox_F_Channel->currentText()+tr("模块采样次数"));

}

void MainWindow::on_Btn_calcul_nu_get_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=8;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_F_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=7;

    requescmdInfo.baCmdContent="";

    ReceiveCmd();
     SetTexditData(tr("读取")+ui->comboBox_F_Channel->currentText()+tr("模块计算单位"));
}

void MainWindow::on_Btn_led_intens_get_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=8;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_F_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=1;

    requescmdInfo.baCmdContent="";

    ReceiveCmd();
     SetTexditData(tr("读取")+ui->comboBox_F_Channel->currentText()+tr("模块LED光强"));
}

void MainWindow::on_Btn_adc_rate_get_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=8;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_F_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=2;

    requescmdInfo.baCmdContent="";

    ReceiveCmd();
     SetTexditData(tr("读取")+ui->comboBox_F_Channel->currentText()+tr("模块ADC速率"));
}

void MainWindow::on_Btn_adc_nu_get_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=8;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_F_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=3;

    requescmdInfo.baCmdContent="";

    ReceiveCmd();
     SetTexditData(tr("读取")+ui->comboBox_F_Channel->currentText()+tr("模块ADC单位"));
}

void MainWindow::on_Btn_led_gap_get_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=8;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_F_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=4;

    requescmdInfo.baCmdContent="";

    ReceiveCmd();
     SetTexditData(tr("读取")+ui->comboBox_F_Channel->currentText()+tr("模块LED打开间隔"));
}

void MainWindow::on_Btn_delay_time_get_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=8;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_F_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=5;

    requescmdInfo.baCmdContent="";

    ReceiveCmd();
    SetTexditData(tr("读取")+ui->comboBox_F_Channel->currentText()+tr("模块延时时间"));
}

void MainWindow::on_Btn_channel_get_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=8;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_F_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=6;

    requescmdInfo.baCmdContent="";

    ReceiveCmd();
     SetTexditData(tr("读取")+ui->comboBox_F_Channel->currentText()+tr("模块打开的通道"));
}

void MainWindow::on_Btn_software_ver_get_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=8;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_F_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=8;

    requescmdInfo.baCmdContent="";

    ReceiveCmd();
     SetTexditData(tr("读取软件版本"));
}

void MainWindow::on_Btn_hardware_ver_get_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=8;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_F_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=9;

    requescmdInfo.baCmdContent="";

    ReceiveCmd();
     SetTexditData(tr("读取硬件版本"));
}

void MainWindow::on_Btn_production_date_get_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=8;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_F_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=10;

    requescmdInfo.baCmdContent="";

    ReceiveCmd();
     SetTexditData(tr("读取生产日期"));
}

/*
void MainWindow::on_Btn_fan_ctrl_open_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=2;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_M_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=3;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    DataContent datacon;
    datacon.m_uint32data=1;
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();
}

void MainWindow::on_Btn_fan_ctrl_close_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=2;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_M_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=3;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    DataContent datacon;
    datacon.m_uint32data=0;
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();
}
*/
//热盖温度补偿
void MainWindow::on_Btn_hot_lid_temp_wr_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=9;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_M_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=3;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    DataContent datacon;
     datacon.m_uint32data=static_cast<quint32>(ui->Line_hot_lid_temp_wr->text().toInt());
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();
    SetTexditData(tr("设置")+ui->comboBox_M_Channel->currentText()+tr("模块热盖温度补偿：")+ui->Line_hot_lid_temp_wr->text());
}

void MainWindow::on_Btn_hot_lid_temp_wr_get_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=9;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_M_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=3;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    requescmdInfo.baCmdContent="";

    ReceiveCmd();
    SetTexditData(tr("读取")+ui->comboBox_M_Channel->currentText()+tr("模块热盖温度补偿参数"));
}
//设置热盖温度
void MainWindow::on_Btn_hot_lid_clicked()
{

    requescmdInfo.uUartCmd.sUartCmd.unFunId=8;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_M_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=3;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    DataContent datacon;
     datacon.m_uint32data=static_cast<quint32>(ui->Line_hot_lid_set->text().toInt());
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();
    SetTexditData(tr("设置")+ui->comboBox_M_Channel->currentText()+tr("模块热盖温度：")+ui->Line_hot_lid_set->text());
}
void MainWindow::on_Btn_hot_lid_get_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=8;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_M_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=3;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    requescmdInfo.baCmdContent="";

    ReceiveCmd();
    SetTexditData(tr("读取")+ui->comboBox_M_Channel->currentText()+tr("模块热盖温度"));
}
//开启热盖
/*
void MainWindow::on_Btn_lid_ctrl_open_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=4;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_M_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=3;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    DataContent datacon;
     datacon.m_uint32data=1;
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();
}
//关闭热盖
void MainWindow::on_Btn_lid_ctrl_close_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=4;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_M_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=3;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    DataContent datacon;
     datacon.m_uint32data=0;
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();
}
*/
//设置底座温度补偿
void MainWindow::on_Btn_base_temp_wr_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=9;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_M_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=3;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=1;

    DataContent datacon;
     datacon.m_uint32data=static_cast<quint32>(ui->Line_base_temp_wr->text().toInt());
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();
    SetTexditData(tr("设置")+ui->comboBox_M_Channel->currentText()+tr("模块底座温度补偿：")+ui->Line_base_temp_wr->text());
}

void MainWindow::on_Btn_base_temp_wr_get_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=9;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_M_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=3;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=1;

    requescmdInfo.baCmdContent="";

    ReceiveCmd();

    SetTexditData(tr("读取")+ui->comboBox_M_Channel->currentText()+tr("模块底座温度补偿参数"));

}
/*
void MainWindow::on_Chb_passageway_open_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=1;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
//    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_T_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unCh=(0<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    requescmdInfo.baCmdContent="";
    DataContent datacon;
    datacon.m_uint32data=1;
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();

//    SetTexditData(tr("当前开启")+ui->comboBox_T_Channel->currentText()+tr("模块"));
    SetTexditData(tr("当前开启A模块"));
    ui->Chb_passageway_open->setChecked(true);
    ui->Chb_passageway_close->setChecked(false);
}

void MainWindow::on_Chb_passageway_close_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=1;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
//    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_T_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unCh=(0<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    requescmdInfo.baCmdContent="";
    DataContent datacon;
    datacon.m_uint32data=2;
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();

//    SetTexditData(tr("当前关闭")+ui->comboBox_T_Channel->currentText()+tr("模块"));
    SetTexditData(tr("当前关闭A模块"));
    ui->Chb_passageway_open->setChecked(false);
    ui->Chb_passageway_close->setChecked(true);
}
*/
void MainWindow::on_Btn_passageway_get_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=1;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_T_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;
     requescmdInfo.baCmdContent="";
     ReceiveCmd();
     SetTexditData(tr("读取")+ui->comboBox_T_Channel->currentText()+tr("模块状态"));

}

void MainWindow::on_Btn_KP_get_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=5;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_T_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=2;

    requescmdInfo.baCmdContent="";

    ReceiveCmd();
    SetTexditData(tr("读取")+ui->comboBox_T_Channel->currentText()+tr("模块PID参数中的P值"));
}

void MainWindow::on_Btn_KI_get_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=5;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_T_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=3;

    requescmdInfo.baCmdContent="";

    ReceiveCmd();
    SetTexditData(tr("读取")+ui->comboBox_T_Channel->currentText()+tr("模块PID参数中的I值"));
}

void MainWindow::on_Btn_KD_get_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=5;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_READ;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_T_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=4;

    requescmdInfo.baCmdContent="";

    ReceiveCmd();
    SetTexditData(tr("读取")+ui->comboBox_T_Channel->currentText()+tr("模块PID参数中的D值"));
}

void MainWindow::on_Cbx_fan_ctrl_open_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=2;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_M_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=3;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    DataContent datacon;
    datacon.m_uint32data=1;
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();

    SetTexditData(tr("当前开启")+ui->comboBox_M_Channel->currentText()+tr("模块风扇"));
    ui->Cbx_fan_ctrl_open->setChecked(true);
    ui->Cbx_fan_ctrl_close->setChecked(false);

}

void MainWindow::on_Cbx_fan_ctrl_close_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=2;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_M_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=3;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    DataContent datacon;
    datacon.m_uint32data=0;
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();

    SetTexditData(tr("当前开启")+ui->comboBox_M_Channel->currentText()+tr("模块风扇"));
    ui->Cbx_fan_ctrl_open->setChecked(false);
    ui->Cbx_fan_ctrl_close->setChecked(true);
}

void MainWindow::on_Chb_lid_ctrl_open_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=4;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_M_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=3;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    DataContent datacon;
     datacon.m_uint32data=1;
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();
    ui->Chb_lid_ctrl_open->setChecked(true);
    ui->Chb__lid_ctrl_close->setChecked(false);
    SetTexditData(tr("当前开启")+ui->comboBox_M_Channel->currentText()+tr("模块热盖"));
}

void MainWindow::on_Chb__lid_ctrl_close_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=4;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_M_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=3;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    DataContent datacon;
     datacon.m_uint32data=0;
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();
    ui->Chb_lid_ctrl_open->setChecked(false);
    ui->Chb__lid_ctrl_close->setChecked(true);
    SetTexditData(tr("当前关闭")+ui->comboBox_M_Channel->currentText()+tr("模块热盖"));
}

void MainWindow::on_comboBox_T_Channel_currentTextChanged(const QString &arg1)
{
//    if(arg1 == "A")
//    {
//        ui->Chb_passageway_open_B->setEnabled(false);
//        ui->Chb_passageway_close_B->setEnabled(false);

//        ui->Chb_passageway_open->setEnabled(true);
//        ui->Chb_passageway_close->setEnabled(true);
//    }
//    else
//    {
//        ui->Chb_passageway_open_B->setEnabled(true);
//        ui->Chb_passageway_close_B->setEnabled(true);

//        ui->Chb_passageway_open->setEnabled(false);
//        ui->Chb_passageway_close->setEnabled(false);
//    }
}

void MainWindow::on_Chb_passageway_open_B_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=1;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
//    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_T_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unCh=(1<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    requescmdInfo.baCmdContent="";
    DataContent datacon;
    datacon.m_uint32data=1;
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();

//    SetTexditData(tr("当前开启")+ui->comboBox_T_Channel->currentText()+tr("模块"));
    SetTexditData(tr("当前开启B模块"));
    ui->Chb_passageway_open_B->setChecked(true);
    ui->Chb_passageway_close_B->setChecked(false);
}

void MainWindow::on_Chb_passageway_close_B_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=1;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=(1<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    requescmdInfo.baCmdContent="";
    DataContent datacon;
    datacon.m_uint32data=2;
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();

//    SetTexditData(tr("当前关闭")+ui->comboBox_T_Channel->currentText()+tr("模块"));
    SetTexditData(tr("当前关闭B模块"));
    ui->Chb_passageway_open_B->setChecked(false);
    ui->Chb_passageway_close_B->setChecked(true);
}

void MainWindow::on_Chb_passageway_open_A_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=1;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
//    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->comboBox_T_Channel->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unCh=(1<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    requescmdInfo.baCmdContent="";
    DataContent datacon;
    datacon.m_uint32data=1;
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();

//    SetTexditData(tr("当前开启")+ui->comboBox_T_Channel->currentText()+tr("模块"));
    SetTexditData(tr("当前开启A模块"));
    ui->Chb_passageway_open_A->setChecked(true);
    ui->Chb_passageway_close_A->setChecked(false);
}

void MainWindow::on_Chb_passageway_close_A_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=1;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=(1<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=1;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    requescmdInfo.baCmdContent="";
    DataContent datacon;
    datacon.m_uint32data=2;
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();

//    SetTexditData(tr("当前关闭")+ui->comboBox_T_Channel->currentText()+tr("模块"));
    SetTexditData(tr("当前关闭A模块"));
    ui->Chb_passageway_open_A->setChecked(false);
    ui->Chb_passageway_close_A->setChecked(true);
}

void MainWindow::on_Btn_clear_get_clicked()
{
    ui->Texdit_accept->clear();
}
/*
void MainWindow::on_Btn_fluo_Best_location__clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=3;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->Cbx_rotate_dire->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=4;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    DataContent datacon;
    datacon.m_uint32data=static_cast<quint32>(ui->Led_fluo_Best_location->text().toInt());
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();
//    SetTexditData(tr("设置")+ui->Cbx_modu->currentText()+tr("模块荧光最佳采集位置：")+ui->Led_fluo_set_point->text()+tr("选装方向为+")+ui->Cbx_rotate_dire->currentText());

    SetTexditData(tr("设置荧光最佳采集位置：")+ui->Led_fluo_set_point->text()+tr("选装方向为+")+ui->Cbx_rotate_dire->currentText());
Led_fluo_Best_location
}
*/
/*最佳荧光位置测试流程
 * 1、获取最佳荧光位置(界面输入)，试剂孔宽度(界面输入)
 * 2、计算电机初始位置并下发命令让电机运动到该位置(最佳荧光位置-孔宽/2-10  (-10是为了确保电机运动到孔外边))
 * 3、等待电机到达位置上报信息，下发采集荧光命令
 * 4、等待荧光数据上报，判断电机当前位置是否已运动到孔的另外一边(当前位置>(最佳荧光位置+孔宽/2+10) (+10是为了确保电机运动到孔外边))
 *    未到则继续下发命令让电机继续运动(电机位置=电机当前位置+电机运动步长)，到了退出采集荧光
 * 5、荧光采集结束，计算荧光最大值，并得到该荧光值对应的电机位置
 * 6、设置电机位置到界面上的最佳荧光位置
*/
void MainWindow::on_Btn_fluo_Best_location_clicked()
{
    if(electric_machinery_point == 0)
    {
        ui->Wdg_debug->Cleardata();
        //读取最佳荧光位置和孔宽度
        int fluo_point=ui->Led_fluo_Best_location->text().toInt();
        int fluo_hole_width=ui->Spx_fluo_hole_width->value();
        //计算电机运动位置
        electric_machinery_point= fluo_point-static_cast<int>(fluo_hole_width/2)-10;

    }
    else
    {
       electric_machinery_point=electric_machinery_point+ui->Spx_elec_step->value();
    }
    qDebug()<<"electric_machinery_point"<<electric_machinery_point;

    point_and_flou=0;
    //下发电机运动命令
//    requescmdInfo.uUartCmd.sUartCmd.unFunId=3;
    requescmdInfo.uUartCmd.sUartCmd.unFunId=4;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=static_cast<quint32>(ui->Cbx_rotate_dire->currentIndex()<<3);
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=4;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=0;

    DataContent datacon;
    datacon.m_uint32data=static_cast<quint32>(electric_machinery_point);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();
    SetTexditData(tr("设置荧光采集位置：")+ui->Led_fluo_set_point->text()+tr("运动方向为：")+ui->Cbx_rotate_dire->currentText());
}

void MainWindow::on_Btn_LED_stren_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=5;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
    requescmdInfo.uUartCmd.sUartCmd.unCh=0;
    requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
    requescmdInfo.uUartCmd.sUartCmd.unSubCmd=1;

    DataContent datacon;
    datacon.m_uint32data=ui->Spx_LED_stren->text().toUInt();
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[0]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[1]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[2]);
    requescmdInfo.baCmdContent.append(datacon.m_uint8data[3]);

    ReceiveCmd();
    SetTexditData(tr("设置LED光强植为：")+QString("%1").arg(ui->Spx_LED_stren->value()));
}

void MainWindow::on_Chb_fluo_auto_acqu_clicked(bool checked)
{
    if(checked)
    {
        //开始采集荧光下发采集荧光命令 fluo_auto_acq_number
        fluo_auto_acq_number++;
//        requescmdInfo.uUartCmd.sUartCmd.unFunId=1;
        requescmdInfo.uUartCmd.sUartCmd.unFunId=9;
        requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
        requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
        requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
        requescmdInfo.uUartCmd.sUartCmd.unCh=0;
        requescmdInfo.uUartCmd.sUartCmd.unModuleType=2;
        requescmdInfo.uUartCmd.sUartCmd.unSubCmd=1;

        requescmdInfo.baCmdContent="";

        ReceiveCmd();
    }
    else
    {
//        check_size=0;
        debug_flou_size=0;
    }
}

void MainWindow::on_Btn_get_curve_data_clicked()
{

   ui->Wdg_debug->GetCurveData();
   qDebug()<<"曲线数据"<< ui->Wdg_debug->curve_data;

   //获取系统粘贴板
   QClipboard *clipboard = QApplication::clipboard();
   clipboard->clear();
   clipboard->setText(ui->Wdg_debug->curve_data);

}
