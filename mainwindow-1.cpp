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
//������Ϣ���ܺ���
void MainWindow::SlotUsartData2Mcu(SRequestCmdInfo cmdInfo)
{
    qDebug()<<"������Ϣ���ܺ���"<<cmdInfo.uUartCmd.sUartCmd.unStartTag;
    qDebug()<<"������Ϣ���ܺ��� unFunId"<<cmdInfo.uUartCmd.sUartCmd.unFunId;
    qDebug()<<"������Ϣ���ܺ��� unRorW"<<cmdInfo.uUartCmd.sUartCmd.unRorW;
    qDebug()<<"������Ϣ���ܺ��� unModuleType"<<cmdInfo.uUartCmd.sUartCmd.unModuleType;
    qDebug()<<"������Ϣ���ܺ��� unCh"<<cmdInfo.uUartCmd.sUartCmd.unCh;
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
    //�������֡ͷ
    switch (ui->tabWidget->currentIndex())
    {
    case 0:
        return;
        //�¿�
    case 1:
//        requescmdInfo.uUartCmd.sUartCmd.unFunId=m_cmd_bit.FunID;
//        requescmdInfo.uUartCmd.sUartCmd.unRorW=m_cmd_bit.FunRW;
//        requescmdInfo.uUartCmd.sUartCmd.unNodeType=m_cmd_bit.NodeType;

//        requescmdInfo.uUartCmd.sUartCmd.unCh= static_cast<quint32>(m_cH.Channel<<3)+m_cH.TEC_ID;
//        requescmdInfo.uUartCmd.sUartCmd.unModuleType=m_cH.Module;
//        requescmdInfo.uUartCmd.sUartCmd.unSubCmd=static_cast<quint32>(m_thiCMD.TempGap<<4)+m_thiCMD.ThiCmd;
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
//        requescmdInfo.uUartCmd.sUartCmd.unModuleType=m_cH.Module;
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
    //�жϵ�ǰҳ��
    switch (ui->tabWidget->currentIndex())
    {
    case 0:
        break;
        //�¿�
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
        //ӫ��
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
        //����
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
    switch (cmdInfo.uUartCmd.sUartCmd.unFunId)
    {
    case 1:
        break;
    case 2:
        break;
        //��ȡĿ���趨�¶�
    case 3:
        ui->Line_temp_targe->setText(QString(cmdInfo.baCmdContent));
        //�����¶ȼ������
        ui->Wdg_T_mode->SLot_AddPointToCurve(check_curve%4,curve_point/4,QString(cmdInfo.baCmdContent).toDouble());
        check_curve++;
        curve_point++;
       // qDebug()<<"check_curve="<<check_curve<<"curve_point="<<curve_point<<"check_curve%4="<<check_curve%4<<"curve_point/4="<<curve_point/4;

        break;
    case 4:
        break;
        //ģ���������
    case 5:
        switch (cmdInfo.uUartCmd.sUartCmd.unSubCmd)
        {
        case 1:
            //�����²�
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
        //ʵ���������
    case 6:
        switch (cmdInfo.uUartCmd.sUartCmd.unSubCmd)
        {
        case 1:
            //�ܽ���������

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
        //�ϱ����¶�
    case 7:
//        ��ʾ��ǰ�ϱ����¶�ֵ
        SetTexditData(tr("�豸��ǰ�¶ȣ�")+QString(cmdInfo.baCmdContent));
        break;
        //����״̬
    case 8:
        SetTexditData(tr("�豸����������Ϣ��")+QString(cmdInfo.baCmdContent));
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
    switch (cmdInfo.uUartCmd.sUartCmd.unFunId)
    {
    //��ʼ�ɼ�ӫ��
    case 1:
        break;
        //ӫ�����ݱ���
    case 2:
        //����ӫ������
        ui->Wdg_F_mode->SLot_AddPointToCurve(check_curve_flou%4,curve_point_flou/4,QString(cmdInfo.baCmdContent).toDouble());
        check_curve_flou++;
        curve_point_flou++;



        break;
        //ADC�����ϱ�
    case 3:
        break;
        //��ǿ�����ϱ�
    case 4:
        break;
        //ģ���������
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
        //����״̬�ϱ�
    case 6:
        break;
        //����������������
    case 7:
        ui->Line_adc_report->setText(QString(cmdInfo.baCmdContent));
        break;
        //ӫ��ģ�����
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
    //�жϴ���������A��Bģ�������Ƿ�͵�ǰѡ���ģ��һ��
//        if(cmdInfo.uUartCmd.sUartCmd.unCh>>3 != ui->comboBox_T_Channel->currentIndex())
//        {
//            return;
//        }
//    SetTexditData(tr("�յ�����Ϊ��")+QString(cmdInfo.baCmdContent));
    switch (cmdInfo.uUartCmd.sUartCmd.unFunId)
    {
    //��������
    case 1:
        //��ʾӲ���ź�����汾��
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
         //���ȿ���
    case 2:

        ui->Line_fan_ctrl->setText(QString(cmdInfo.baCmdContent));
        break;
         //�������״̬
    case 3:
        break;
         //�ȸǿ���
    case 4:

        ui->Line_lid_ctrl->setText(QString(cmdInfo.baCmdContent));
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

        ui->Line_hot_lid->setText(QString(cmdInfo.baCmdContent));
        break;
         //�¶Ȳ���������д
    case 9:

        ui->Line_temp_compen->setText(QString(cmdInfo.baCmdContent));
        break;
         //�������󱨸�
    case 10:
        break;
         //��ȡͭ���¶�
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
    //�޸���������ģʽ
    case 1:
        for (int i=0; i<cmdInfo.baCmdContent.size()/4; i++) {
            tempdata.m_uint8data[0] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+0));
            tempdata.m_uint8data[1] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+1));
            tempdata.m_uint8data[2] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+2));
            tempdata.m_uint8data[3] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+3));
            cmddata.append(tempdata);
        }
//        SetTexditData(tr("������ǰ����ģʽΪ��")+QString(cmddata[0].m_uint32data));
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
        check_cmd=true;
        for (int i=0; i<cmdInfo.baCmdContent.size()/4; i++) {
            tempdata.m_uint8data[0] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+0));
            tempdata.m_uint8data[1] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+1));
            tempdata.m_uint8data[2] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+2));
            tempdata.m_uint8data[3] =  static_cast<quint8>(cmdInfo.baCmdContent.at(i*4+3));
            cmddata.append(tempdata);
        }
        SetTexditData(tr("��ȡ����ӫ��ֵΪ��")+QString::number(cmddata[0].m_uint32data));
        break;
        //ӫ��ɼ�λ�õĶ�ȡ�ͱ���
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
        SetTexditData(tr("�����ǰλ�ã�")+QString::number(cmddata[0].m_uint32data));
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
    //��/�ر�ͨ��
    case 0:
        break;
    case 1:
        break;
        //Ŀ���¶�����
    case 2:
        ui->Line_temp_target_set->setEnabled(true);
        ui->Btn_temp_target->setEnabled(true);
        break;
        //�¶��ϱ�
    case 3:
        break;
        //ģ���������
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
        //ʵ���������
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
        //�¶��ϱ� ����ʱʹ��
    case 6:
        break;
        //����״̬
    case 7:
        break;
    }

}

//ѡ���¶ȴ��������
void MainWindow::on_comboBox_T_TEC_ID_activated(int index)
{
     m_cH.TEC_ID=static_cast<quint8>(index+1);
}

//�����¶Ȳ���ֵ
void MainWindow::on_Btn_temp_comp_clicked()
{
    if(ui->Line_temp_comp_set->text().isEmpty())
    {
        return;
    }

    m_thiCMD.ThiCmd= static_cast<quint8>(1);
     m_cmd_bit.FunRW=1;

    requescmdInfo.baCmdContent=ui->Line_temp_comp_set->text().toLocal8Bit();
    SetTexditData(tr("�����¶Ȳ���ֵ��")+ui->Line_temp_comp_set->text());
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
    SetTexditData(tr("����KPֵ��")+ui->Line_KP_set->text());
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

    SetTexditData(tr("����KIֵ��")+ui->Line_KI_set->text());
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
    SetTexditData(tr("����KDֵ��")+ui->Line_KD_set->text());
    ui->Line_KD->setText(ui->Line_KD_set->text());
}
//����Ŀ���¶� Line_temp_target_set
void MainWindow::on_Btn_temp_target_clicked()
{
    if(ui->Line_temp_target_set->text().isEmpty())
    {
        return;
    }

    m_cmd_bit.FunRW=1;
    requescmdInfo.baCmdContent=ui->Line_temp_target_set->text().toLocal8Bit();
    SetTexditData(tr("����Ŀ���¶�ֵ��")+ui->Line_temp_target_set->text());
    ui->Line_temp_targe->setText(ui->Line_temp_target_set->text());
}
//�����ܽ���������(�ܽ�����ֻ�·��������λ���л��ܽ�PID;��������Ҳ��ֻ�·�������λ���л�����PID������������)
void MainWindow::on_Btn_diss_curve_clicked()
{

    m_thiCMD.ThiCmd= static_cast<quint8>(1);
     m_cmd_bit.FunRW=1;
    SetTexditData(tr("�����ܽ���������"));

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
    SetTexditData(tr("�����������ʣ�")+ui->Line_temp_up_set->text());
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
    SetTexditData(tr("���ý������ʣ�")+ui->Line_temp_down_set->text());
    ui->Line_temp_down->setText(ui->Line_temp_down_set->text());
}
//�ܽ�������ʼֵ
void MainWindow::on_Btn_diss_curve_start_clicked()
{
    if(ui->Line_diss_curve_start_set->text().isEmpty())
    {
        return;
    }
        m_thiCMD.ThiCmd= static_cast<quint8>(4);
         m_cmd_bit.FunRW=1;

   requescmdInfo.baCmdContent=ui->Line_diss_curve_start_set->text().toLocal8Bit();
    SetTexditData(tr("����������ʼֵ��")+ui->Line_diss_curve_start_set->text());
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
    SetTexditData(tr("����������ֵֹ��")+ui->Line_diss_curve_end_set->text());
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
    SetTexditData(tr("�����ܽ��������ֵ��")+ui->Line_diss_curve_max_set->text());
    ui->Line_diss_curve_max->setText(ui->Line_diss_curve_max_set->text());
}
//���ʹ�������
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
    //ģ���������
    case 4:
        ui->comboBox_F_Modeoption->setEnabled(true);
        on_comboBox_F_Modeoption_activated(inde);
        m_cmd_bit.FunRW=1;
        break;
//     ����������������
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
//����Ƶ������
void MainWindow::on_Btn_adc_report_clicked()
{
    m_cmd_bit.FunID=7;
    //����
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
    //��������
    case 1:

        break;
         //���ȿ���
    case 2:
        ui->lineEditLine_fan_ctrl_set->setEnabled(true);
        ui->Btn_fan_ctrl->setEnabled(true);
        break;
         //�������״̬
    case 3:
        break;
         //�ȸǿ���
    case 4:
        ui->Line_lid_ctrl_set->setEnabled(true);
        ui->Btn_lid_ctrl->setEnabled(true);
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
        ui->Line_temp_compen_set->setEnabled(true);
        ui->Btn_temp_compen->setEnabled(true);
        break;
         //�������󱨸�
    case 10:
        break;
         //��ȡͭ���¶�
    case 11:
        break;
    }
}
//���ȿ���
void MainWindow::on_Btn_fan_ctrl_clicked()
{
    m_cmd_bit.FunRW=1;
    requescmdInfo.baCmdContent=ui->lineEditLine_fan_ctrl_set->text().toLocal8Bit();
}
//�ȸǿ���
void MainWindow::on_Btn_lid_ctrl_clicked()
{
    m_cmd_bit.FunRW=1;
    requescmdInfo.baCmdContent=ui->Line_lid_ctrl_set->text().toLocal8Bit();
}
//�����ȸ��¶�
void MainWindow::on_Btn_hot_lid_clicked()
{
    m_cmd_bit.FunRW=1;
    requescmdInfo.baCmdContent=ui->Line_hot_lid_set->text().toLocal8Bit();
}
//�����¶Ȳ�������
void MainWindow::on_Btn_temp_compen_clicked()
{
    m_cmd_bit.FunRW=1;
    requescmdInfo.baCmdContent=ui->Line_temp_compen_set->text().toLocal8Bit();
}
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
    ui->Wdg_T_mode->Cleardata();
    check_curve=0;
    curve_point=0;

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
//�����豸����ģʽ
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
//��ȡ�豸����ģʽ
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
//ӫ��ɼ�λ�ö�ȡ�ͱ��� �·�����
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
//ӫ��ɼ�λ�ö�ȡ�ͱ��� ��ȡ����
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
//�ɼ�ӫ��
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
//���õ��λ��
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
//��ȡ���λ��
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
            //�·����λ��
           // on_Btn_elec_set_point_clicked();
            QThread::msleep(100);
            //�·��ɼ�ӫ��
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
