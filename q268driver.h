#ifndef Q268DRIVER_H
#define Q268DRIVER_H

#include <QObject>
#include<QDebug>
#include<QtSerialPort/QSerialPort>
#include<QtSerialPort/QSerialPortInfo>
#include<QPushButton>
#include<QDialog>
#include<QLabel>
#include<QComboBox>
#include<QVBoxLayout>
#include<QMessageBox>


#include"Globals.h"
class Q268Driver : public QObject
{
    Q_OBJECT
public:
     Q268Driver();
    ~Q268Driver();

     void Init(void);

     QByteArray		m_readData;
     QByteArray		m_recvData;
     QSerialPort	*deviceSerial;//仪器串口

public slots:
     //获取上位机下发的串口信息
     void SlotGetUpDriver(QByteArray infodata);
private:
     quint32		m_unNeedReadNu;
     bool			m_bNewCmd;

signals:
     void    SendSerialPortData(QByteArray baData);
     void    SendUsartData2Mcu(SRequestCmdInfo cmdInfo);
     void  Sign_close(void);

private:
     void driver_close(void);
     //握手操作
     void OpenDriver(void);
     //CRC校验
      quint8  CrcErrorCheck(quint8 *ptr,quint32 len);

      //通过串口发送数据，刷新串口缓冲区
      void    SendCmd2Intrument(QByteArray baData);

private slots:
     //接收串口消息
     void SlotReadyData(void);
     //检查数据并进行解析分发
     void SlotCheckDataAndParser(QByteArray baPaecket);
     //将命令打包，封装成QByteArray
     bool    CmdPackage(SRequestCmdInfo cmdInfo);


private:
     //设置波特率打开串口
     void SetParament(QString serialPortName, int unBaudRate);
};

#endif // Q268DRIVER_H
