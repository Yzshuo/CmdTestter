#include "q268driver.h"



#pragma execution_character_set("utf-8")
Q268Driver::Q268Driver()
{
    deviceSerial=new QSerialPort;
    connect(this,SIGNAL(SendSerialPortData(QByteArray)),this,SLOT(SlotCheckDataAndParser(QByteArray)));
    Init();
}

Q268Driver::~Q268Driver()
{

}

void Q268Driver::Init()
{
 //   qDebug()<<"Q268Driver";
    QStringList list;
//    list<<"600"<<"1200"<<"2400"<<"4800"<<"9600"<<"19200"<<"28800"<<"38400"<<"57600"<<"115200"<<"128000"<<"256000";
    list<<"1200"<<"2400"<<"4800"<<"9600"<<"19200"<<"38400"<<"57600"<<"115200";
    QDialog     *PortSelect = new QDialog();
    QLabel      LabelInfo;
    QLabel      LabelCom;
    QPushButton OK_Button;
    QPushButton Cancel_Button;
    QComboBox   *PortComboBox = new QComboBox;
     QComboBox  *BaudRate = new QComboBox;
     BaudRate->addItems(list);
     BaudRate->setCurrentText("115200");
    QVBoxLayout Layout_VBox;
    QHBoxLayout Layout_HBox;

    OK_Button.setText(tr("ȷ��"));
    Cancel_Button.setText(tr("ȡ��"));
    LabelInfo.setText(tr("ѡ����Ҫ�򿪵Ĵ���"));
    LabelCom.setText(tr("ѡ������"));
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        PortComboBox->addItem(info.portName());
    }

    Layout_HBox.addWidget(&OK_Button);
    Layout_HBox.addWidget(&Cancel_Button);

    Layout_VBox.addWidget(&LabelInfo);
    Layout_VBox.addWidget(PortComboBox);
    Layout_VBox.addWidget(&LabelCom);
    Layout_VBox.addWidget(BaudRate);
    Layout_VBox.addLayout(&Layout_HBox);

    PortSelect->setLayout(&Layout_VBox);

    connect(&OK_Button,&QPushButton::clicked,[=](){
        auto comName = PortComboBox->currentText();//��ô�����

//        SetParament(comName,115200);
        SetParament(comName,BaudRate->currentText().toInt());
        qDebug()<<"����"<<comName<<"������"<<BaudRate->currentText().toInt();
        PortSelect->close();
    });

    connect(&Cancel_Button,&QPushButton::clicked,[=](){

        qDebug()<<"Cancel_Button";
        driver_close();

        PortSelect->close();
    });

    PortSelect->exec();
}

void Q268Driver::SlotGetUpDriver(QByteArray infodata)
{
    SendCmd2Intrument(infodata);
}

void Q268Driver::driver_close()
{
    qDebug()<<"driver_close";
    emit Sign_close();
}

void Q268Driver::OpenDriver()
{
    SRequestCmdInfo handShake;
    handShake.uUartCmd.sUartCmd.unFunId         = 1;
    handShake.uUartCmd.sUartCmd.unRorW          = M_WRITE;
    handShake.uUartCmd.sUartCmd.unNodeType      = 7;
    handShake.uUartCmd.sUartCmd.unStatusAck     = 0;
    handShake.uUartCmd.sUartCmd.unCh            = 0;
    handShake.uUartCmd.sUartCmd.unModuleType    = 3;
    handShake.uUartCmd.sUartCmd.unSubCmd        = 0;
//    handShake.baCmdContent="";
    CmdPackage(handShake);
}

quint8 Q268Driver::CrcErrorCheck(quint8 *ptr, quint32 len)
{
    quint8 crc;
    quint8 i;

    crc = 0;
    while(len--)
    {
        crc ^= *ptr++;
        for(i = 0;i < 8;i++)
        {
            if(crc & 0x01)
            {
                crc = (crc >> 1) ^ 0x8C;
            }else crc >>= 1;
        }
    }
    return crc;
}
//������������װ��QByteArray
bool Q268Driver::CmdPackage(SRequestCmdInfo cmdInfo)
{
    QByteArray send_data;
    cmdInfo.uUartCmd.sUartCmd.unStartTag      = 0xFF;
    cmdInfo.uUartCmd.sUartCmd.unDataLen_empty = 0;
    cmdInfo.uUartCmd.sUartCmd.unDataLen       = sizeof(cmdInfo.uUartCmd) + static_cast<quint32>(cmdInfo.baCmdContent.size());



//    qDebug()<<"0"<<QString("%1").number(cmdInfo.uUartCmd.ucCmd[0],16);
//    qDebug()<<"1"<<QString("%1").number(cmdInfo.uUartCmd.ucCmd[1],16);
//    qDebug()<<"2"<<QString("%1").number(cmdInfo.uUartCmd.ucCmd[2],16);
//    qDebug()<<"3"<<QString("%1").number(cmdInfo.uUartCmd.ucCmd[3],16);
//    qDebug()<<"4"<<QString("%1").number(cmdInfo.uUartCmd.ucCmd[4],16);
//    qDebug()<<"5"<<QString("%1").number(cmdInfo.uUartCmd.ucCmd[5],16);
//    qDebug()<<"6"<<QString("%1").number(cmdInfo.uUartCmd.ucCmd[6],16);
//    qDebug()<<"7"<<QString("%1").number(cmdInfo.uUartCmd.ucCmd[7],16);



    for (int i=0; i<8; i++)
    {
        send_data.append(static_cast<char>(cmdInfo.uUartCmd.ucCmd[i]));
    }
    send_data.append(cmdInfo.baCmdContent);
    quint8  *pPacketData = (quint8 *)(send_data.data());
     char crc_temp = static_cast<char>(CrcErrorCheck(pPacketData+4, static_cast<quint32>(send_data.size()-4)));
    send_data.remove(3,1);
    send_data.insert(3,crc_temp);

//    qDebug()<<"crc_temp"<<crc_temp;

    SendCmd2Intrument(send_data);

    return true;
}

void Q268Driver::SendCmd2Intrument(QByteArray baData)
{
//    qDebug()<<"�·�����"<<baData;
    deviceSerial->write(baData);
    deviceSerial->flush();
}

void Q268Driver::SlotReadyData()
{
    qint64     unSize;
    quint8      *pucData;
    bool        bCountinue = false;
    qDebug()<<"read COM OK";
//    m_readData.clear();

    do
    {
//        qDebug()<<"m_bNewCmd"<<m_bNewCmd<<"m_unNeedReadNu"<<m_unNeedReadNu;
        // 1����ȡ��ͷ 0xFF DLC
        if (m_bNewCmd)
        {
            while (1)
            {
//                qDebug()<<"deviceSerial->bytesAvailable()"<<deviceSerial->bytesAvailable();
                //�������ݹ�С
                if (deviceSerial->bytesAvailable() < 3)
                {
                    qDebug()<<"�������ݹ�С";
                    break;
                }
                m_readData.clear();
                m_readData = deviceSerial->read(3);
                pucData = (quint8 *)(m_readData.data());

                if ((pucData[0] == 0xFF) & (pucData[2] > 3))
                {
                    m_unNeedReadNu = pucData[2]-3;
                    m_bNewCmd      = false;
                    break;
                }
            }
        }
        // 2����ȡ����������
        unSize = deviceSerial->bytesAvailable();
//        qDebug()<<"ʵ�����ݳ���"<<unSize<<"��ȡ���ݳ���"<<m_unNeedReadNu;
        if (m_unNeedReadNu <= unSize)
        {
            m_readData.append(deviceSerial->read(m_unNeedReadNu));
        }
        else
        {
            m_readData.append(deviceSerial->read(unSize));
//            qDebug()<<"m_readData"<<m_readData;
            m_unNeedReadNu = m_unNeedReadNu - unSize;
            break;
        }
//        qDebug()<<"���ڶ�ȡ��������"<<m_readData;
        // 3���������ݸ����������
        emit SendSerialPortData(m_readData);

        m_bNewCmd       = true;
        m_unNeedReadNu  = 0;

        // 4����⴮�ڣ������Ƿ������ݿ��Ա�����
        if (deviceSerial->bytesAvailable() > 2)
        {
            bCountinue = true;
        }
    } while (bCountinue);

}
//������ݲ����н����ַ�
void Q268Driver::SlotCheckDataAndParser(QByteArray baPaecket)
{
    if(baPaecket.size()<8)
    {
        return;
    }

    quint8  *pPacketData = (quint8 *)(baPaecket.data());
    SRequestCmdInfo cmdInfo;
    if (pPacketData[3] == CrcErrorCheck(pPacketData + 4,static_cast<quint32>(baPaecket.size() - 4)))//crcУ��
    {
        cmdInfo.sCmdId.unCmdMainId  = CMD_MAIN_ID_UART_DATA_PACKET;
        cmdInfo.sCmdId.unCmdSubId   = UART_DATA_PACKET_CMD_SUB_ID_RX;
        memcpy(cmdInfo.uUartCmd.ucCmd, pPacketData, 8);   //
        cmdInfo.baCmdContent        = baPaecket.mid(8);
        emit SendUsartData2Mcu(cmdInfo);
//        qDebug()<<"cmdInfo.uUartCmd unDataLen"<<cmdInfo.uUartCmd.sUartCmd.unDataLen;
    }
}



void Q268Driver::SetParament(QString serialPortName, int unBaudRate)
{
    //���ô��ڶ˿�
    deviceSerial->setPortName(serialPortName);
    //���ò�����
    deviceSerial->setBaudRate(unBaudRate);
    deviceSerial->setParity(QSerialPort::NoParity);//���ü���
    deviceSerial->setDataBits(QSerialPort::Data8);//��������λ
    deviceSerial->setStopBits(QSerialPort::OneStop); //����һ��ֹͣλ
    deviceSerial->setFlowControl(QSerialPort::NoFlowControl);//����û��������
    //�Զ�д��ʽ�򿪴���
    if(deviceSerial->open(QIODevice::ReadWrite))
    {
        qDebug()<<"���ڴ򿪳ɹ�";
        connect(deviceSerial,SIGNAL(readyRead()),this,SLOT(SlotReadyData()));
        //�������ֲ���
        OpenDriver();
    }
    else
    {
        //���ڴ�ʧ�ܣ�
        QMessageBox::question(nullptr,tr("����"),tr("���ڴ�ʧ��!"),QMessageBox::Yes);

    }
}
