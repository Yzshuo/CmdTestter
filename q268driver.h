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
     QSerialPort	*deviceSerial;//��������

public slots:
     //��ȡ��λ���·��Ĵ�����Ϣ
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
     //���ֲ���
     void OpenDriver(void);
     //CRCУ��
      quint8  CrcErrorCheck(quint8 *ptr,quint32 len);

      //ͨ�����ڷ������ݣ�ˢ�´��ڻ�����
      void    SendCmd2Intrument(QByteArray baData);

private slots:
     //���մ�����Ϣ
     void SlotReadyData(void);
     //������ݲ����н����ַ�
     void SlotCheckDataAndParser(QByteArray baPaecket);
     //������������װ��QByteArray
     bool    CmdPackage(SRequestCmdInfo cmdInfo);


private:
     //���ò����ʴ򿪴���
     void SetParament(QString serialPortName, int unBaudRate);
};

#endif // Q268DRIVER_H
