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
//������Ϣ���ܺ���
void MainWindow::SlotUsartData2Mcu(SRequestCmdInfo cmdInfo)
{

    qDebug()<<"������Ϣ���ܺ��� unFunId"<<cmdInfo.uUartCmd.sUartCmd.unFunId;
    qDebug()<<"������Ϣ���ܺ��� unRorW"<<cmdInfo.uUartCmd.sUartCmd.unRorW;
    qDebug()<<"������Ϣ���ܺ��� unModuleType"<<cmdInfo.uUartCmd.sUartCmd.unModuleType;
    qDebug()<<"������Ϣ���ܺ��� unCh"<<cmdInfo.uUartCmd.sUartCmd.unCh;
    qDebug()<<"������Ϣ���ܺ��� unDataLen_empty"<<cmdInfo.uUartCmd.sUartCmd.unDataLen_empty;
    qDebug()<<"������Ϣ���ܺ��� unDataLen"<<cmdInfo.uUartCmd.sUartCmd.unDataLen;

//    if(cmdInfo.uUartCmd.sUartCmd.unStatusAck == 0)
//    {
//        return;
//    }
    switch (cmdInfo.uUartCmd.sUartCmd.unModuleType)
    {
        //�¿�
    case 1:
        GetTempControl(cmdInfo);
        break;
        //ӫ��
    case 2:
        GetFluorControl(cmdInfo);
        break;
        //����
    case 3:
        GetMainControl(cmdInfo);
        break;
        //ϵͳ����
    case 4:
        GetDebugControl(cmdInfo);
        break;
        //����
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
    //�������ֵ
    qSort(fluo_size.begin(),fluo_size.end());

    qDebug()<<fluo_size;
    qDebug()<<"max="<< fluo_size[fluo_size.size()-1]<<fluo_size.size()<<"list"<<list.size();
    ui->Texdit_accept->append("���ֵΪ��"+QString("%1").arg(fluo_size[fluo_size.size()-1]));

    for(auto mlist:list)
    {
        if(mlist == fluo_size[fluo_size.size()-1])
        {
            qDebug()<<"λ��"<<siz;
            int max_point=(siz*ui->Spx_elec_step->value())+(ui->Led_fluo_Best_location->text().toInt()-ui->Spx_fluo_hole_width->value()/2);
            ui->Led_fluo_Best_location->setText(QString("%1").arg(max_point));
            ui->Texdit_accept->append("���λ��Ϊ"+QString("%1").arg(max_point));
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
    on_Btn_coll_fluo_clicked();
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
        //�ɼ�ӫ��
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
    //�������֡ͷ
    switch (ui->tabWidget->currentIndex())
    {
    case 0:
        return;
        //�¿�
    case 1:
        break;
        //ӫ��
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
        //������
    case 3:
        break;
        //ϵͳ����
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
    //�жϵ�ǰҳ��
    switch (ui->tabWidget->currentIndex())
    {
    case 0:
        break;
        //�¿�
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
        //ӫ��
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
        //����
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
        //����
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
//    SetTexditData(tr("�յ�����Ϊ��")+QString(cmdInfo.baCmdContent));
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
                SetTexditData(tr("Aģ���¿�ͨ������"));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("Bģ���¿�ͨ������"));
            }

            break;
        case 2:
            if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 0)
            {
                SetTexditData(tr("Aģ���¿�ͨ���ر�"));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("Bģ���¿�ͨ���ر�"));
            }

            break;
        default:
            if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 0)
            {
                SetTexditData(tr("Aģ���¿�ͨ�����ݷ��ش���"));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("Bģ���¿�ͨ�����ݷ��ش���"));
            }

            break;

        }


        break;
    case 2:
        break;
        //��ȡĿ���趨�¶�
    case 3:
//        ui->Line_temp_targe->setText(QString(cmdInfo.baCmdContent));
//        SetTexditData(tr("Ŀ���¶�����ֵΪ��")+QString(cmdInfo.baCmdContent));



       // qDebug()<<"check_curve="<<check_curve<<"curve_point="<<curve_point<<"check_curve%4="<<check_curve%4<<"curve_point/4="<<curve_point/4;
        if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
        {
            SetTexditData(tr("Aģ��Ŀ���¶�����ֵΪ��")+QString("%1").arg(tempdata.m_floatdata));
            SetTexditData(tr("Aģ��Ŀ���¶�����ֵΪ��")+QString("%1").arg(tempdata.m_uint32data));
            //�����¶ȼ������ tempdata.m_floatdata
//             ui->Wdg_T_mode->SLot_AddPointToCurve(check_curve%4,curve_point/4,QString(cmdInfo.baCmdContent).toDouble());
            ui->Wdg_T_mode->SLot_AddPointToCurve(check_curve%4,curve_point/4,static_cast<double>(tempdata.m_floatdata));
            check_curve++;
            curve_point++;
        }
        else if((cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 1)
        {
            SetTexditData(tr("Bģ��Ŀ���¶�����ֵΪ��")+QString("%1").arg(tempdata.m_floatdata));
            SetTexditData(tr("Bģ��Ŀ���¶�����ֵΪ��")+QString("%1").arg(tempdata.m_uint32data));
            //�����¶ȼ������
            ui->Wdg_T_mode_B->SLot_AddPointToCurve(check_curve_B%4,curve_point_B/4,static_cast<double>(tempdata.m_floatdata));
            check_curve_B++;
            curve_point_B++;
        }
        break;
    case 4:
//         SetTexditData(tr("���յ����¶�ֵΪ��")+QString(cmdInfo.baCmdContent));

         if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 0)
         {
             SetTexditData(tr("Aģ����յ����¶�ֵΪ��")+QString("%1").arg(tempdata.m_floatdata));
             SetTexditData(tr("Aģ����յ����¶�ֵΪ��")+QString("%1").arg(tempdata.m_uint32data));
             //�����¶ȼ������ tempdata.m_floatdata
//             ui->Wdg_T_mode->SLot_AddPointToCurve(check_curve%4,curve_point/4,QString(cmdInfo.baCmdContent).toDouble());
             ui->Wdg_T_mode->SLot_AddPointToCurve(check_curve%4,curve_point/4,static_cast<double>(tempdata.m_floatdata));
             check_curve++;
             curve_point++;
         }
         else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
         {
             SetTexditData(tr("Bģ����յ����¶�ֵΪ��")+QString("%1").arg(tempdata.m_floatdata));
             SetTexditData(tr("Bģ����յ����¶�ֵΪ��")+QString("%1").arg(tempdata.m_uint32data));
             //�����¶ȼ������
             ui->Wdg_T_mode_B->SLot_AddPointToCurve(check_curve_B%4,curve_point_B/4,static_cast<double>(tempdata.m_floatdata));
             check_curve_B++;
             curve_point_B++;
         }

        break;
        //ģ���������
    case 5:
        switch (cmdInfo.uUartCmd.sUartCmd.unSubCmd)
        {
        case 1:
            //�����²�
//            ui->Line_temp_comp->setText(QString(cmdInfo.baCmdContent));
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("���յ���Aģ���¶Ȳ���ֵΪ��")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("���յ���Aģ���¶Ȳ���ֵΪ��")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("���յ���Bģ���¶Ȳ���ֵΪ��")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("���յ���Bģ���¶Ȳ���ֵΪ��")+QString("%1").arg(tempdata.m_uint32data));
            }


            break;
        case 2:
//            ui->Line_KP->setText(QString(cmdInfo.baCmdContent));
//            SetTexditData(tr("���յ���PID�����е�PֵΪ��")+QString(cmdInfo.baCmdContent));
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("���յ���Aģ��PID�����е�PֵΪ��")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("���յ���Aģ��PID�����е�PֵΪ��")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("���յ���Bģ��PID�����е�PֵΪ��")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("���յ���Bģ��PID�����е�PֵΪ��")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        case 3:
//            ui->Line_KI->setText(QString(cmdInfo.baCmdContent));
//            SetTexditData(tr("���յ���PID�����е�IֵΪ��")+QString(cmdInfo.baCmdContent));
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("���յ���Aģ��PID�����е�IֵΪ��")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("���յ���Aģ��PID�����е�IֵΪ��")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("���յ���Bģ��PID�����е�IֵΪ��")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("���յ���Bģ��PID�����е�IֵΪ��")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        case 4:
//            ui->Line_KD->setText(QString(cmdInfo.baCmdContent));
//            SetTexditData(tr("���յ���PID�����е�DֵΪ��")+QString(cmdInfo.baCmdContent));
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("���յ���Aģ��PID�����е�DֵΪ��")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("���յ���Aģ��PID�����е�DֵΪ��")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("���յ���Bģ��PID�����е�DֵΪ��")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("���յ���Bģ��PID�����е�DֵΪ��")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        default:
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("Aģ��������ݴ��󣡣�")+QString(cmdInfo.baCmdContent));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("Bģ��������ݴ��󣡣�")+QString(cmdInfo.baCmdContent));
            }

            break;
        }
        break;
        //ʵ���������
    case 6:
        switch (cmdInfo.uUartCmd.sUartCmd.unSubCmd)
        {
        case 1:
            //�ܽ���������

            break;
            //��������
        case 2:
//            ui->Line_temp_up->setText(QString(cmdInfo.baCmdContent));
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("���յ���Aģ����������ֵΪ��")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("���յ���Aģ����������ֵΪ��")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("���յ���Bģ����������ֵΪ��")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("���յ���Bģ����������ֵΪ��")+QString("%1").arg(tempdata.m_uint32data));

            }

            break;
            //��������
        case 3:
//            ui->Line_temp_down->setText(QString(cmdInfo.baCmdContent));
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("���յ���Aģ�齵������ֵΪ��")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("���յ���Aģ�齵������ֵΪ��")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("���յ���Bģ�齵������ֵΪ��")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("���յ���Bģ�齵������ֵΪ��")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
            //�ܽ�������ʼֵ
        case 4:
//            ui->Line_diss_curve_start->setText(QString(cmdInfo.baCmdContent));
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("���յ�Aģ���ܽ�������ʼֵֵΪ��")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("���յ�Aģ���ܽ�������ʼֵֵΪ��")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("���յ�Bģ���ܽ�������ʼֵֵΪ��")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("���յ�Bģ���ܽ�������ʼֵֵΪ��")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
            //�ܽ�������ֹ��
        case 5:
//            ui->Line_diss_curve_end->setText(QString(cmdInfo.baCmdContent));
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("���յ���Aģ���ܽ�������ֵֹΪ��")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("���յ���Aģ���ܽ�������ֵֹΪ��")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("���յ���Bģ���ܽ�������ֵֹΪ��")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("���յ���Bģ���ܽ�������ֵֹΪ��")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
            //�ܽ��������ֵ
        case 6:
//            ui->Line_diss_curve_max->setText(QString(cmdInfo.baCmdContent));
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("���յ���Aģ���ܽ��������ֵΪ��")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("���յ���Aģ���ܽ��������ֵΪ��")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("���յ���Bģ���ܽ��������ֵΪ��")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("���յ���Bģ���ܽ��������ֵΪ��")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        default:
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("Aģ��ʵ��������ݴ��󣡣�")+QString(cmdInfo.baCmdContent));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("Bģ��ʵ��������ݴ��󣡣�")+QString(cmdInfo.baCmdContent));
            }

            break;
        }
        break;
        //�ϱ����¶�
    case 7:
//        ��ʾ��ǰ�ϱ����¶�ֵ
//        SetTexditData(tr("�豸��ǰ�¶ȣ�")+QString(cmdInfo.baCmdContent));
        if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 0)
        {
            SetTexditData(tr("���յ���Aģ���豸��ǰ�¶�Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
             SetTexditData(tr("���յ���Aģ���豸��ǰ�¶�Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
        }
        else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
        {
            SetTexditData(tr("���յ���Bģ���豸��ǰ�¶�Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
             SetTexditData(tr("���յ���Bģ���豸��ǰ�¶�Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
        }

        break;
        //����״̬
    case 8:
        if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 0)
        {
//             SetTexditData(tr("Aģ���豸����������Ϣ��")+QString(cmdInfo.baCmdContent));
        }
        else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
        {
             SetTexditData(tr("Bģ���豸����������Ϣ��")+QString(cmdInfo.baCmdContent));
        }

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
    //�жϴ���������A��Bģ�������Ƿ�͵�ǰѡ���ģ��һ��
//        if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 != ui->comboBox_T_Channel->currentIndex())
//        {
//            return;
//        }
//    SetTexditData(tr("�յ�����Ϊ��")+QString(cmdInfo.baCmdContent));

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
    //��ʼ�ɼ�ӫ��
    case 1:
        break;
        //ӫ�����ݱ���
    case 2:
        if(ui->Chb_fluo_auto_acqu->isChecked())
        {
            ui->Wdg_debug->SLot_AddPointToCurve(0,check_size,cmddata[0].m_uint32data);

            check_size++;
            SetTexditData(tr("���ܵ���ӫ������Ϊ��")+QString("%1").arg(cmddata[0].m_floatdata));
            SetTexditData(tr("���ܵ���ӫ������Ϊ��")+QString("%1").arg(cmddata[0].m_uint32data));
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

        //����ӫ������
//        ui->Wdg_F_mode->SLot_AddPointToCurve(check_curve_flou%4,curve_point_flou/4,QString(cmdInfo.baCmdContent).toDouble());
          ui->Wdg_F_mode->SLot_AddPointToCurve(check_curve_flou%4,curve_point_flou/4,static_cast<double>(tempdata.m_floatdata));
        check_curve_flou++;
        curve_point_flou++;
//        SetTexditData(tr("���ܵ���ӫ������Ϊ��")+QString(cmdInfo.baCmdContent));

        if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
        {
            SetTexditData(tr("���ܵ���Aģ��ӫ������Ϊ��")+QString("%1").arg(cmddata[0].m_floatdata));
             SetTexditData(tr("���ܵ���Aģ��ӫ������Ϊ��")+QString("%1").arg(cmddata[0].m_uint32data));
        }
        else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
        {
            SetTexditData(tr("���ܵ���Bģ��ӫ������Ϊ��")+QString("%1").arg(cmddata[0].m_floatdata));
             SetTexditData(tr("���ܵ���Bģ��ӫ������Ϊ��")+QString("%1").arg(cmddata[0].m_uint32data));
        }
*/



        break;
        //ADC�����ϱ�
    case 3:
//        SetTexditData(tr("���ܵ���ADC����Ϊ��")+QString(cmdInfo.baCmdContent));
        if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
        {
            SetTexditData(tr("���ܵ���Aģ��ADC����Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
             SetTexditData(tr("���ܵ���AmokADC����Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
        }
        else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
        {
            SetTexditData(tr("���ܵ���Bģ��ADC����Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
             SetTexditData(tr("���ܵ���Bģ��ADC����Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
        }


        break;
        //��ǿ�����ϱ�
    case 4:
//        SetTexditData(tr("���ܵ��Ĺ�ǿ����Ϊ��")+QString(cmdInfo.baCmdContent));
        if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
        {
            SetTexditData(tr("���ܵ���Aģ���ǿ����Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
             SetTexditData(tr("���ܵ���Aģ���ǿ����Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
        }
        else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
        {
            SetTexditData(tr("���ܵ���Bģ���ǿ����Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
             SetTexditData(tr("���ܵ���Bģ���ǿ����Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
        }

        break;
        //ģ���������
    case 5:
        switch (cmdInfo.uUartCmd.sUartCmd.unSubCmd)
        {
        case 1:
//             SetTexditData(tr("���ܵ���LED��ǿ����Ϊ��")+QString(cmdInfo.baCmdContent));
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("���ܵ���Aģ��LED��ǿ����Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("���ܵ���Aģ��LED��ǿ����Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("���ܵ���Bģ��LED��ǿ����Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("���ܵ���Bģ��LED��ǿ����Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        case 2:
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("���ܵ���Aģ��ADC����Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("���ܵ���Aģ��ADC����Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("���ܵ���Bģ��ADC����Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
                SetTexditData(tr("���ܵ���Bģ��ADC����Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        case 3:
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("���ܵ���Aģ��ADC��λΪ��")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("���ܵ���Aģ��ADC��λΪ��")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("���ܵ���Bģ��ADC��λΪ��")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("���ܵ���Bģ��ADC��λΪ��")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        case 4:
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("���ܵ���Aģ��LED�򿪼��Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("���ܵ���Aģ��LED�򿪼��Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("���ܵ���Bģ��LED�򿪼��Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("���ܵ���Bģ��LED�򿪼��Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        case 5:
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("���ܵ���Aģ����ʱʱ��Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("���ܵ���Aģ����ʱʱ��Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("���ܵ���Bģ����ʱʱ��Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("���ܵ���Bģ����ʱʱ��Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        case 6:
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("���ܵ���Aģ���ͨ��Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("���ܵ���Aģ���ͨ��Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("���ܵ���Bģ���ͨ��Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("���ܵ���Bģ���ͨ��Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        case 7:
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("���ܵ��ļ�Aģ������Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("���ܵ���Aģ�������Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("���ܵ���Bģ�������Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("���ܵ���Bģ�������Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        case 8:
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("���ܵ���Aģ������汾Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("���ܵ���Aģ������汾Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("���ܵ���Bģ������汾Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("���ܵ���Bģ������汾Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        case 9:
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("���ܵ���Aģ��Ӳ���汾Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("���ܵ���Aģ��Ӳ���汾Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("���ܵ���Bģ��Ӳ���汾Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("���ܵ���Bģ��Ӳ���汾Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        case 10:
            if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
            {
                SetTexditData(tr("���ܵ���Aģ����������Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("���ܵ���Aģ����������Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
            {
                SetTexditData(tr("���ܵ���Bģ����������Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
                 SetTexditData(tr("���ܵ���Bģ����������Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
            }

            break;
        default:
            SetTexditData(tr("���ܵ���ģ���������ݴ���")+QString(cmdInfo.baCmdContent));
            break;

        }
        break;
        //����״̬�ϱ�
    case 6:
        if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
        {
            SetTexditData(tr("���ܵ���Aģ��ӫ��״̬������Ϣ��")+QString::number(cmddata[0].m_uint32data));
        }
        else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
        {
            SetTexditData(tr("���ܵ���Bģ��ӫ��״̬������Ϣ��")+QString::number(cmddata[0].m_uint32data));
        }

        break;
        //����������������
    case 7:
//        ui->Line_adc_report->setText(QString(cmdInfo.baCmdContent));
        if( (cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
        {
        SetTexditData(tr("���ܵ���Aģ��������������Ϊ��")+QString::number(cmddata[0].m_uint32data));
        }
        else if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
        {
        SetTexditData(tr("���ܵ���Bģ��������������Ϊ��")+QString::number(cmddata[0].m_uint32data));
        }

        break;
        //ģ�������ȡ
    case 8:
        break;
        //�ɼ�����ӫ������
    case 9:
    {

        SetTexditData(tr("��ȡ����ӫ��ֵΪ��")+QString::number(cmddata[0].m_uint32data));
//        double fluordata;
//        QString("%1").arg(cmddata[0].m_uint32data);
        qDebug()<<"m_uint32data"<<cmddata[0].m_uint32data
               <<"m_floatdata"<<cmddata[0].m_floatdata
              <<"mdoubledata"<<cmddata[0].mdoubledata
              <<"mdoubledata"<<static_cast<double>(cmddata[0].m_uint32data);
        ui->Wdg_debug->SLot_AddPointToCurve(0,check_size,cmddata[0].m_uint32data);
        fluo_size.append(cmddata[0].m_uint32data);
        check_size++;

        if(electric_machinery_point == 0)
        {
           emit Sign_check_state(true);

        }
        else
        {
            qDebug()<<"���ӫ�� �˶����";

            //�жϵ�ǰӫ��λ���Ƿ����˶������ⲿ
            if(electric_machinery_point > (ui->Led_fluo_Best_location->text().toInt()+ui->Spx_fluo_hole_width->value()/2+10))
            {
                electric_machinery_point=0;
                check_size=0;
                //��ȡ�������
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

void MainWindow::GetMainControl(SRequestCmdInfo cmdInfo)
{
    //    if(cmdInfo.uUartCmd.sUartCmd.unStatusAck == 0)
    //    {
    //        return;
    //    }
    //�жϴ���������A��Bģ�������Ƿ�͵�ǰѡ���ģ��һ��
//        if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 != ui->comboBox_T_Channel->currentIndex())
//        {
//            return;
//        }
//    SetTexditData(tr("�յ�����Ϊ��")+QString(cmdInfo.baCmdContent));
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
    //��������
    case 1:
        //��ʾӲ���ź�����汾��
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
         //���ȿ���
    case 2:
//            SetTexditData(tr("��ȡ����ӫ��ֵΪ��")+QString::number(cmddata[0].m_uint32data));
        if((cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
        {
            if(tempdata.m_uint32data == 0)
            {
                SetTexditData(tr("��ȡ��Aģ�����Ϊ�ر�״̬"));
            }
            else if(tempdata.m_uint32data == 1)
            {
                SetTexditData(tr("��ȡ��Aģ�����Ϊ����״̬"));
            }

        }
        else if((cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 1)
        {
            if(tempdata.m_uint32data == 0)
            {
                SetTexditData(tr("��ȡ��Bģ�����Ϊ�ر�״̬"));
            }
            else if(tempdata.m_uint32data == 1)
            {
                SetTexditData(tr("��ȡ��Bģ�����Ϊ����״̬"));
            }
        }

        break;
         //�������״̬
    case 3:
        if((cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
        {
            if(tempdata.m_uint32data == 0)
            {
                SetTexditData(tr("��ȡ��Aģ�����Ϊ�ر�״̬"));
            }
            else if(tempdata.m_uint32data == 1)
            {
                SetTexditData(tr("��ȡ��Aģ�����Ϊ����״̬"));
            }

        }
        else if((cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 1)
        {
            if(tempdata.m_uint32data == 0)
            {
                SetTexditData(tr("��ȡ��Bģ�����Ϊ�ر�״̬"));
            }
            else if(tempdata.m_uint32data == 1)
            {
                SetTexditData(tr("��ȡ��Bģ�����Ϊ����״̬"));
            }
        }
        break;
         //�ȸǿ���
    case 4:

        if((cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
        {
            if(tempdata.m_uint32data == 0)
            {
                SetTexditData(tr("��ȡ��Aģ���ȸ�Ϊ�ر�״̬"));
            }
            else if(tempdata.m_uint32data == 1)
            {
                SetTexditData(tr("��ȡ��Aģ���ȸ�Ϊ����״̬"));
            }

        }
        else if((cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 1)
        {
            if(tempdata.m_uint32data == 0)
            {
                SetTexditData(tr("��ȡ��Bģ���ȸ�Ϊ�ر�״̬"));
            }
            else if(tempdata.m_uint32data == 1)
            {
                SetTexditData(tr("��ȡ��Bģ���ȸ�Ϊ����״̬"));
            }
        }

        break;
         //�����ȸ�״̬
    case 5:
        SetTexditData(tr("�ȸǴﵽ�趨�¶�"));

        break;
         //ʵ�����֪ͨ
    case 6:
        break;
         //����������д
    case 7:
        break;
         //�ȸ��趨�¶ȵĶ�д
    case 8:

//        SetTexditData(tr("�ȸ��趨�¶�Ϊ��")+QString(cmdInfo.baCmdContent));
        if((cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
        {
            SetTexditData(tr("��ȡ��Aģ���ȸ��趨�¶�Ϊ����")+QString("%1").arg(tempdata.m_floatdata));
            SetTexditData(tr("��ȡ��Aģ���ȸ��趨�¶�Ϊ����")+QString("%1").arg(tempdata.m_uint32data));
        }
        else if((cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 1)
        {
            SetTexditData(tr("��ȡ��Bģ���ȸ��趨�¶�Ϊ����")+QString("%1").arg(tempdata.m_floatdata));
            SetTexditData(tr("��ȡ��Bģ���ȸ��趨�¶�Ϊ����")+QString("%1").arg(tempdata.m_uint32data));
        }

        break;
         //�¶Ȳ���������д
    case 9:
        if((cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 0)
        {
            if(cmdInfo.uUartCmd.sUartCmd.unSubCmd == 0)
            {
                SetTexditData(tr("��ȡ��Aģ���ȸ��¶Ȳ�������Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
                SetTexditData(tr("��ȡ��Aģ���ȸ��¶Ȳ�������Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unSubCmd == 1)
            {
                SetTexditData(tr("��ȡ��Aģ���ȸ��¶Ȳ�������Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
                SetTexditData(tr("��ȡ��Aģ���ȸ��¶Ȳ�������Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
            }

        }
        else if((cmdInfo.uUartCmd.sUartCmd.unCh>>3) == 1)
        {
            if(cmdInfo.uUartCmd.sUartCmd.unSubCmd == 0)
            {
                SetTexditData(tr("��ȡ��Bģ���ȸ��¶Ȳ�������Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
                SetTexditData(tr("��ȡ��Bģ���ȸ��¶Ȳ�������Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
            }
            else if(cmdInfo.uUartCmd.sUartCmd.unSubCmd == 1)
            {
                SetTexditData(tr("��ȡ��Bģ���ȸ��¶Ȳ�������Ϊ��")+QString("%1").arg(tempdata.m_uint32data));
                SetTexditData(tr("��ȡ��Bģ���ȸ��¶Ȳ�������Ϊ��")+QString("%1").arg(tempdata.m_floatdata));
            }
        }
        break;
         //�������󱨸�
    case 10:
        break;
         //��ȡͭ���¶�
    case 11:
        SetTexditData(tr("ͭ����ǰ�¶�Ϊ��")+QString(cmdInfo.baCmdContent));
        break;
    }
}

void MainWindow::GetDebugControl(SRequestCmdInfo cmdInfo)
{
    QList<DataContent> cmddata;
    cmddata.clear();
    DataContent tempdata;
    for (int i=0; i<cmdInfo.baCmdContent.size()/4; i++) {
//        tempdata.m_uint8data[0] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+0));
//        tempdata.m_uint8data[1] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+1));
//        tempdata.m_uint8data[2] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+2));
        tempdata.m_uint8data[3] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+0));
        tempdata.m_uint8data[2] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+1));
        tempdata.m_uint8data[1] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+2));
        tempdata.m_uint8data[0] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+3));
        cmddata.append(tempdata);
    }
    switch (cmdInfo.uUartCmd.sUartCmd.unFunId)
    {
    //�޸���������ģʽ
    case 1:

//        SetTexditData(tr("������ǰ����ģʽΪ��")+QString(cmddata[0].m_uint32data));
        if(cmdInfo.uUartCmd.sUartCmd.unSubCmd == 0)
        {

        }
        else if(cmdInfo.uUartCmd.sUartCmd.unSubCmd == 1)
        {

        }
        if(cmddata[0].m_uint32data == 1)
        {
           SetTexditData(tr("������ǰ����ģʽΪ������ģʽ"));
        }
        else
        {
           SetTexditData(tr("������ǰ����ģʽΪ������ģʽ"));
        }
        break;
        //�ɼ�ӫ��
    case 2:

        break;
        //ӫ��ɼ�λ�õĶ�ȡ�ͱ���
    case 3:

        if(cmdInfo.uUartCmd.sUartCmd.unSubCmd == 0)
        {

        }
        else if(cmdInfo.uUartCmd.sUartCmd.unSubCmd == 1)
        {

        }
        if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 0)
        {
            SetTexditData(tr("��ǰ�����ת����Ϊ������"));
        }
        else  if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 == 1)
        {
            SetTexditData(tr("��ǰ�����ת����Ϊ������"));
        }

//        SetTexditData(tr("��ȡ����ӫ��ɼ�λ�ã�")+QString("%1").arg( cmddata[0].m_uint32data));
        SetTexditData(tr("��ȡ����ӫ��ɼ�λ�ã�")+QString::number(cmddata[0].m_uint32data));

        break;
        //����˶�
    case 4:
        SetTexditData(tr("�����ǰλ�ã�")+QString::number(cmddata[0].m_uint32data));
//        ui->Led_eles_get_point->setText(QString::number(cmddata[0].m_uint32data));
        //������ص�ǰλ�ú�ɼ�ӫ��
//        if( qAbs(static_cast<quint32>(electric_machinery_point) - cmddata[0].m_uint32data) < 3)
//        {

//        }
        if(electric_machinery_point != 0)
        {
            qDebug()<<"���ӫ�� �ɼ�ӫ��";
            Get_Fluo_Best_Location();
        }
        break;
    default:
        break;

    }
}


//�����¶Ȳ���ֵ
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
    SetTexditData(tr("����")+ui->comboBox_T_Channel->currentText()+tr("ģ���¶Ȳ���ֵ��")+ui->Line_temp_comp_set->text());
//    SetTexditData(tr("�¶Ȳ���ֵ��")+ui->Line_temp_comp_set->text());
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

//    SetTexditData(tr("����KPֵ��")+ui->Line_KP_set->text());
        SetTexditData(tr("����")+ui->comboBox_T_Channel->currentText()+tr("ģ��PID��Pֵ��")+ui->Line_KP_set->text());
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

//    SetTexditData(tr("����KIֵ��")+ui->Line_KI_set->text());
    SetTexditData(tr("����")+ui->comboBox_T_Channel->currentText()+tr("ģ��PID��Iֵ��")+ui->Line_KI_set->text());

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

//    SetTexditData(tr("����KDֵ��")+ui->Line_KD_set->text());
    SetTexditData(tr("����")+ui->comboBox_T_Channel->currentText()+tr("ģ��PID��Dֵ��")+ui->Line_KD_set->text());

}
//����Ŀ���¶� Line_temp_target_set
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
    SetTexditData(tr("����")+ui->comboBox_T_Channel->currentText()+tr("ģ��Ŀ���¶�ֵ��")+ui->Line_temp_target_set->text());
//    SetTexditData(tr("����Ŀ���¶�ֵ��")+ui->Line_temp_target_set->text());
}
//�����ܽ���������(�ܽ�����ֻ�·��������λ���л��ܽ�PID;��������Ҳ��ֻ�·�������λ���л�����PID������������)
//������������
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

    SetTexditData(tr("����")+ui->comboBox_T_Channel->currentText()+tr("ģ���������ʣ�")+ui->Line_temp_up_set->text());
//    SetTexditData(tr("�����������ʣ�")+ui->Line_temp_up_set->text());

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

//    SetTexditData(tr("���ý������ʣ�")+ui->Line_temp_down_set->text());
    SetTexditData(tr("����")+ui->comboBox_T_Channel->currentText()+tr("ģ�齵�����ʣ�")+ui->Line_temp_down_set->text());

}
//�ܽ�������ʼֵ
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

//    SetTexditData(tr("����������ʼֵ��")+ui->Line_diss_curve_start_set->text());
    SetTexditData(tr("����")+ui->comboBox_T_Channel->currentText()+tr("ģ��������ʼֵ��")+ui->Line_diss_curve_start_set->text());

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

//    SetTexditData(tr("����������ֵֹ��")+ui->Line_diss_curve_end_set->text());
    SetTexditData(tr("����")+ui->comboBox_T_Channel->currentText()+tr("ģ��������ֵֹ��")+ui->Line_diss_curve_end_set->text());

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

//    SetTexditData(tr("�����ܽ��������ֵ��")+ui->Line_diss_curve_max_set->text());
    SetTexditData(tr("����")+ui->comboBox_T_Channel->currentText()+tr("ģ���ܽ��������ֵ��")+ui->Line_diss_curve_max_set->text());

}
//���ʹ�������
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
        //��ȡģ���������
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
        //��ȡʵ���������
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
//������������
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

//    SetTexditData(tr("�·�����������")+ui->Line_adc_report_set->text());
    SetTexditData(tr("����")+ui->comboBox_F_Channel->currentText()+tr("ģ�����������")+ui->Line_adc_report_set->text());


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
//    SetTexditData(tr("�·�����������")+ui->Line_led_intens_set->text());
    SetTexditData(tr("����")+ui->comboBox_F_Channel->currentText()+tr("ģ��LED��ǿ��")+ui->Line_led_intens_set->text());
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
//    SetTexditData(tr("�·�����������")+ui->Line_led_intens_set->text());
    SetTexditData(tr("����")+ui->comboBox_F_Channel->currentText()+tr("ģ��ADC���ʣ�")+ui->Line_adc_rate_set->text());
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
//    SetTexditData(tr("�·�����������")+ui->Line_adc_nu_set->text());
    SetTexditData(tr("����")+ui->comboBox_F_Channel->currentText()+tr("ģ��ADC��λ��")+ui->Line_adc_nu_set->text());
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
//    SetTexditData(tr("�·�����������")+ui->Line_led_gap_set->text());
     SetTexditData(tr("����")+ui->comboBox_F_Channel->currentText()+tr("ģ��LED�ɼ������")+ui->Line_led_gap_set->text());
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
//    SetTexditData(tr("�·�����������")+ui->Line_delay_time_set->text());
    SetTexditData(tr("����")+ui->comboBox_F_Channel->currentText()+tr("ģ����ʱʱ�䣺")+ui->Line_delay_time_set->text());
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
//    SetTexditData(tr("�·�����������")+ui->Line_channel_set->text());
    SetTexditData(tr("����")+ui->comboBox_T_Channel->currentText()+tr("ģ���ͨ����")+ui->Line_channel_set->text());
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
//    SetTexditData(tr("�·�����������")+ui->Line_calcul_nu_set->text());
    SetTexditData(tr("����")+ui->comboBox_F_Channel->currentText()+tr("ģ����㵥λ��")+ui->Line_calcul_nu_set->text());
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
    SetTexditData(tr("��������汾Ϊ��")+ui->Line_software_ver_set->text());
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
    SetTexditData(tr("����Ӳ���汾Ϊ��")+ui->Line_hardware_ver_set->text());
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
    SetTexditData(tr("�����������ڣ�")+ui->Line_production_date->text());
}
/*
void MainWindow::on_comboBox_M_FunID_activated(int index)
{
    SetMainControl();
     m_cmd_bit.FunRW=0;
    m_cmd_bit.FunID = static_cast<quint8>(index+1);
    switch (index+1)
    {
    //��������
    case 1:

        break;
         //���ȿ���
    case 2:
//        ui->lineEditLine_fan_ctrl_set->setEnabled(true);
//        ui->Btn_fan_ctrl->setEnabled(true);
        break;
         //�������״̬
    case 3:
        break;
         //�ȸǿ���
    case 4:
//        ui->Line_lid_ctrl_set->setEnabled(true);
//        ui->Btn_lid_ctrl->setEnabled(true);
        break;
         //�����ȸ�״̬
    case 5:
        break;
         //ʵ�����֪ͨ
    case 6:
        break;
         //����������д
    case 7:
        break;
         //�ȸ��趨�¶ȵĶ�д
    case 8:
        ui->Line_hot_lid_set->setEnabled(true);
        ui->Btn_hot_lid->setEnabled(true);
        break;
         //�¶Ȳ���������д
    case 9:
//        ui->Line_temp_compen_set->setEnabled(true);
//        ui->Btn_temp_compen->setEnabled(true);
        break;
         //�������󱨸�
    case 10:
        break;
         //��ȡͭ���¶�
    case 11:
        break;
    }
}
*/
//�����¶Ȳ�������
//void MainWindow::on_Btn_temp_compen_clicked()
//{
//    m_cmd_bit.FunRW=1;
//    requescmdInfo.baCmdContent=ui->Line_temp_compen_set->text().toLocal8Bit();
//}
//ͨ��ѡ��
void MainWindow::on_comboBox_M_Channel_activated(int index)
{
    m_cmd_bit.FunRW=1;
    m_cH.Channel=static_cast<quint8>(index<<3);
}
//ѡ��A��Bͨ��
void MainWindow::on_comboBox_T_Channel_currentIndexChanged(int index)
{
    m_cH.Channel=static_cast<quint8>(index<<3);
    qDebug()<<"index A_B="<<index;
    //���֮ǰ������
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




    //�������Ϊ�´λ�ͼ��׼��
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

//   qDebug()<<"�·����� baCmdContent"<<requescmdInfo.baCmdContent;

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

        qDebug()<<"CRC="<<crc_temp;
//        qDebug()<<"�·����� send_data"<<send_data;
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
//�����豸����ģʽ
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
//    SetTexditData(tr("����")+ui->Cbx_modu->currentText()+tr("ģ������ģʽΪ��")+ui->Cbx_mode->currentText());
    SetTexditData(tr("����ģʽΪ��")+ui->Cbx_mode->currentText());


}
//��ȡ�豸����ģʽ
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
//    SetTexditData(tr("��ȡ")+ui->Cbx_modu->currentText()+tr("ģ������ģʽ"));
    SetTexditData(tr("��ȡ����ģʽ"));
}
//ӫ��ɼ�λ�ö�ȡ�ͱ��� �·�����
void MainWindow::on_Btn_set_fluo_clicked()
{
    requescmdInfo.uUartCmd.sUartCmd.unFunId=3;
    requescmdInfo.uUartCmd.sUartCmd.unRorW=M_WRITE;
    requescmdInfo.uUartCmd.sUartCmd.unNodeType=7;
    requescmdInfo.uUartCmd.sUartCmd.unStatusAck=0;
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
//    SetTexditData(tr("����")+ui->Cbx_modu->currentText()+tr("ģ��ӫ��ɼ�λ�ã�")+ui->Led_fluo_set_point->text()+tr("ѡװ����Ϊ+")+ui->Cbx_rotate_dire->currentText());
    SetTexditData(tr("����ӫ��ɼ�λ�ã�")+ui->Led_fluo_set_point->text()+tr("ѡװ����Ϊ+")+ui->Cbx_rotate_dire->currentText());
}
//ӫ��ɼ�λ�ö�ȡ�ͱ��� ��ȡ����
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
//     SetTexditData(tr("��ȡ")+ui->Cbx_modu->currentText()+tr("ģ��ӫ��ɼ�λ��"));
    SetTexditData(tr("��ȡӫ��ɼ�λ��"));
}
//�ɼ�ӫ��
void MainWindow::on_Btn_coll_fluo_clicked()
{
    int number=ui->Spx_fluo_point_number->value();
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
//        SetTexditData(tr("ģ��")+ui->Cbx_modu->currentText()+tr("��ʼ�ɼ�ӫ��,��ǰ�ɼ���")+QString::number(i+1)+"/"+QString::number(number) +tr("��"));
//        QTimer::singleShot(500,this,SLOT(Get_Point_Fluo()));
//    }

    if(fluo_check < number)
    {
//        SetTexditData(tr("ģ��")+ui->Cbx_modu->currentText()+tr("��ʼ�ɼ�ӫ��,��ǰ�ɼ���")+QString::number(fluo_check+1)+"/"+QString::number(number) +tr("��"));
        SetTexditData(tr("��ʼ�ɼ�ӫ��,��ǰ�ɼ���")+QString::number(fluo_check+1)+"/"+QString::number(number) +tr("��"));
        QTimer::singleShot(1000,this,SLOT(Get_Point_Fluo()));
        fluo_check++;
    }
    else
    {
        fluo_check=0;
    }
}
//���õ��λ��
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
//    SetTexditData(tr("����")+ui->Cbx_modu->currentText()+tr("ģ����λ�ã�")+ui->Led_eles_set_point->text());
     SetTexditData(tr("���õ��λ�ã�")+ui->Led_eles_set_point->text());

}
//��ȡ���λ��
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
//     SetTexditData(tr("��ȡ")+ui->Cbx_modu->currentText()+tr("ģ����λ��"));
     SetTexditData(tr("��ȡ���λ��"));

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

//    SetTexditData(tr("����")+ui->Cbx_modu->currentText()+tr("ģ��ɼ�ӫ����� �ɼ�����Ϊ��")+QString("%1").arg(ui->Spx_fluo_number->value()));
//    SetTexditData(tr("���òɼ�ӫ����� �ɼ�����Ϊ��")+QString("%1").arg(ui->Spx_fluo_number->value()));
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
    SetTexditData(tr("��ȡ")+ui->comboBox_T_Channel->currentText()+tr("ģ���¶Ȳ���ֵ"));
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
    SetTexditData(tr("��ȡ")+ui->comboBox_T_Channel->currentText()+tr("ģ��Ŀ���¶�ֵ"));
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
    SetTexditData(tr("��ȡ")+ui->comboBox_T_Channel->currentText()+tr("ģ����������"));
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
    SetTexditData(tr("��ȡ")+ui->comboBox_T_Channel->currentText()+tr("ģ�齵������"));
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
    SetTexditData(tr("��ȡ")+ui->comboBox_T_Channel->currentText()+tr("�ܽ�������ʼֵ"));
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
    SetTexditData(tr("��ȡ")+ui->comboBox_T_Channel->currentText()+tr("�ܽ�������ֵֹ"));
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
    SetTexditData(tr("��ȡ")+ui->comboBox_T_Channel->currentText()+tr("�ܽ��������ֵ"));
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
    SetTexditData(tr("��ȡ")+ui->comboBox_F_Channel->currentText()+tr("ģ���������"));

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
     SetTexditData(tr("��ȡ")+ui->comboBox_F_Channel->currentText()+tr("ģ����㵥λ"));
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
     SetTexditData(tr("��ȡ")+ui->comboBox_F_Channel->currentText()+tr("ģ��LED��ǿ"));
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
     SetTexditData(tr("��ȡ")+ui->comboBox_F_Channel->currentText()+tr("ģ��ADC����"));
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
     SetTexditData(tr("��ȡ")+ui->comboBox_F_Channel->currentText()+tr("ģ��ADC��λ"));
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
     SetTexditData(tr("��ȡ")+ui->comboBox_F_Channel->currentText()+tr("ģ��LED�򿪼��"));
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
    SetTexditData(tr("��ȡ")+ui->comboBox_F_Channel->currentText()+tr("ģ����ʱʱ��"));
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
     SetTexditData(tr("��ȡ")+ui->comboBox_F_Channel->currentText()+tr("ģ��򿪵�ͨ��"));
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
     SetTexditData(tr("��ȡ����汾"));
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
     SetTexditData(tr("��ȡӲ���汾"));
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
     SetTexditData(tr("��ȡ��������"));
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
//�ȸ��¶Ȳ���
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
    SetTexditData(tr("����")+ui->comboBox_M_Channel->currentText()+tr("ģ���ȸ��¶Ȳ�����")+ui->Line_hot_lid_temp_wr->text());
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
    SetTexditData(tr("��ȡ")+ui->comboBox_M_Channel->currentText()+tr("ģ���ȸ��¶Ȳ�������"));
}
//�����ȸ��¶�
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
    SetTexditData(tr("����")+ui->comboBox_M_Channel->currentText()+tr("ģ���ȸ��¶ȣ�")+ui->Line_hot_lid_set->text());
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
    SetTexditData(tr("��ȡ")+ui->comboBox_M_Channel->currentText()+tr("ģ���ȸ��¶�"));
}
//�����ȸ�
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
//�ر��ȸ�
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
//���õ����¶Ȳ���
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
    SetTexditData(tr("����")+ui->comboBox_M_Channel->currentText()+tr("ģ������¶Ȳ�����")+ui->Line_base_temp_wr->text());
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

    SetTexditData(tr("��ȡ")+ui->comboBox_M_Channel->currentText()+tr("ģ������¶Ȳ�������"));

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

//    SetTexditData(tr("��ǰ����")+ui->comboBox_T_Channel->currentText()+tr("ģ��"));
    SetTexditData(tr("��ǰ����Aģ��"));
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

//    SetTexditData(tr("��ǰ�ر�")+ui->comboBox_T_Channel->currentText()+tr("ģ��"));
    SetTexditData(tr("��ǰ�ر�Aģ��"));
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
     SetTexditData(tr("��ȡ")+ui->comboBox_T_Channel->currentText()+tr("ģ��״̬"));

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
    SetTexditData(tr("��ȡ")+ui->comboBox_T_Channel->currentText()+tr("ģ��PID�����е�Pֵ"));
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
    SetTexditData(tr("��ȡ")+ui->comboBox_T_Channel->currentText()+tr("ģ��PID�����е�Iֵ"));
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
    SetTexditData(tr("��ȡ")+ui->comboBox_T_Channel->currentText()+tr("ģ��PID�����е�Dֵ"));
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

    SetTexditData(tr("��ǰ����")+ui->comboBox_M_Channel->currentText()+tr("ģ�����"));
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

    SetTexditData(tr("��ǰ����")+ui->comboBox_M_Channel->currentText()+tr("ģ�����"));
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
    SetTexditData(tr("��ǰ����")+ui->comboBox_M_Channel->currentText()+tr("ģ���ȸ�"));
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
    SetTexditData(tr("��ǰ�ر�")+ui->comboBox_M_Channel->currentText()+tr("ģ���ȸ�"));
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

//    SetTexditData(tr("��ǰ����")+ui->comboBox_T_Channel->currentText()+tr("ģ��"));
    SetTexditData(tr("��ǰ����Bģ��"));
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

//    SetTexditData(tr("��ǰ�ر�")+ui->comboBox_T_Channel->currentText()+tr("ģ��"));
    SetTexditData(tr("��ǰ�ر�Bģ��"));
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

//    SetTexditData(tr("��ǰ����")+ui->comboBox_T_Channel->currentText()+tr("ģ��"));
    SetTexditData(tr("��ǰ����Aģ��"));
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

//    SetTexditData(tr("��ǰ�ر�")+ui->comboBox_T_Channel->currentText()+tr("ģ��"));
    SetTexditData(tr("��ǰ�ر�Aģ��"));
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
//    SetTexditData(tr("����")+ui->Cbx_modu->currentText()+tr("ģ��ӫ����Ѳɼ�λ�ã�")+ui->Led_fluo_set_point->text()+tr("ѡװ����Ϊ+")+ui->Cbx_rotate_dire->currentText());

    SetTexditData(tr("����ӫ����Ѳɼ�λ�ã�")+ui->Led_fluo_set_point->text()+tr("ѡװ����Ϊ+")+ui->Cbx_rotate_dire->currentText());
Led_fluo_Best_location
}
*/
/*���Ӯ���������
 * 1����ȡ���ӫ��λ��(��������)���Լ��׿��(��������)
 * 2����������ʼλ�ò��·������õ���˶�����λ��(���ӫ��λ��-�׿�/2-10  (-10��Ϊ��ȷ������˶��������))
 * 3���ȴ��������λ���ϱ���Ϣ���·��ɼ�ӫ������
 * 4���ȴ�ӫ�������ϱ����жϵ����ǰλ���Ƿ����˶����׵�����һ��(��ǰλ��>(���ӫ��λ��+�׿�/2+10) (+10��Ϊ��ȷ������˶��������))
 *    δ��������·������õ�������˶�(���λ��=�����ǰλ��+����˶�����)�������˳��ɼ�ӫ��
 * 5��ӫ��ɼ�����������ӫ�����ֵ�����õ���ӫ��ֵ��Ӧ�ĵ��λ��
 * 6�����õ��λ�õ������ϵ����ӫ��λ��
*/
void MainWindow::on_Btn_fluo_Best_location_clicked()
{
    if(electric_machinery_point == 0)
    {
        //��ȡ���ӫ��λ�úͿ׿��
        int fluo_point=ui->Led_fluo_Best_location->text().toInt();
        int fluo_hole_width=ui->Spx_fluo_hole_width->value();
        //�������˶�λ��
        electric_machinery_point= fluo_point-static_cast<int>(fluo_hole_width/2)-10;

    }
    else
    {
       electric_machinery_point=electric_machinery_point+ui->Spx_elec_step->value();
    }
    qDebug()<<"electric_machinery_point"<<electric_machinery_point;
    //�·�����˶�����
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
    SetTexditData(tr("����ӫ��ɼ�λ�ã�")+ui->Led_fluo_set_point->text()+tr("�˶�����Ϊ��")+ui->Cbx_rotate_dire->currentText());
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
    SetTexditData(tr("����LED��ǿֲΪ��")+QString("%1").arg(ui->Spx_LED_stren->value()));
}

void MainWindow::on_Chb_fluo_auto_acqu_clicked(bool checked)
{
    if(checked)
    {
        //��ʼ�ɼ�ӫ���·��ɼ�ӫ������ fluo_auto_acq_number
        fluo_auto_acq_number++;
        requescmdInfo.uUartCmd.sUartCmd.unFunId=1;
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
        check_size=0;
    }
}

void MainWindow::on_Btn_get_curve_data_clicked()
{
   qDebug()<<"��������"<< ui->Wdg_debug->curve_data;
}
