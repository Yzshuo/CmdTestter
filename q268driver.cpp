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

    OK_Button.setText(tr("确认"));
    Cancel_Button.setText(tr("取消"));
    LabelInfo.setText(tr("选择需要打开的串口"));
    LabelCom.setText(tr("选择波特率"));
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
        auto comName = PortComboBox->currentText();//获得串口名

//        SetParament(comName,115200);
        SetParament(comName,BaudRate->currentText().toInt());
        qDebug()<<"串口"<<comName<<"波特率"<<BaudRate->currentText().toInt();
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
//将命令打包，封装成QByteArray
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
//    qDebug()<<"下发数据"<<baData;
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
        // 1、读取包头 0xFF DLC
        if (m_bNewCmd)
        {
            while (1)
            {
//                qDebug()<<"deviceSerial->bytesAvailable()"<<deviceSerial->bytesAvailable();
                //串口数据过小
                if (deviceSerial->bytesAvailable() < 3)
                {
                    qDebug()<<"串口数据过小";
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
        // 2、读取整个包数据
        unSize = deviceSerial->bytesAvailable();
//        qDebug()<<"实际数据长度"<<unSize<<"读取数据长度"<<m_unNeedReadNu;
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
//        qDebug()<<"串口读取到的数据"<<m_readData;
        // 3、发送数据给命令解析器
        emit SendSerialPortData(m_readData);

        m_bNewCmd       = true;
        m_unNeedReadNu  = 0;

        // 4、检测串口，看看是否还有数据可以被处理
        if (deviceSerial->bytesAvailable() > 2)
        {
            bCountinue = true;
        }
    } while (bCountinue);

}
//检查数据并进行解析分发
void Q268Driver::SlotCheckDataAndParser(QByteArray baPaecket)
{
    if(baPaecket.size()<8)
    {
        return;
    }

    quint8  *pPacketData = (quint8 *)(baPaecket.data());
    SRequestCmdInfo cmdInfo;
    if (pPacketData[3] == CrcErrorCheck(pPacketData + 4,static_cast<quint32>(baPaecket.size() - 4)))//crc校验
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
    //设置串口端口
    deviceSerial->setPortName(serialPortName);
    //设置波特率
    deviceSerial->setBaudRate(unBaudRate);
    deviceSerial->setParity(QSerialPort::NoParity);//设置极性
    deviceSerial->setDataBits(QSerialPort::Data8);//设置数据位
    deviceSerial->setStopBits(QSerialPort::OneStop); //设置一个停止位
    deviceSerial->setFlowControl(QSerialPort::NoFlowControl);//设置没有流控制
    //以读写方式打开串口
    if(deviceSerial->open(QIODevice::ReadWrite))
    {
        qDebug()<<"串口打开成功";
        connect(deviceSerial,SIGNAL(readyRead()),this,SLOT(SlotReadyData()));
        //进行握手操作
        OpenDriver();
    }
    else
    {
        //串口打开失败！
        QMessageBox::question(nullptr,tr("错误"),tr("串口打开失败!"),QMessageBox::Yes);

    }
}
