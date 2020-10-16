#ifndef MYTHRTEAD_T_H
#define MYTHRTEAD_T_H

#include <QObject>
#include<QThread>

class MyThrtead_T : public QThread
{
    Q_OBJECT
public:
    MyThrtead_T();
    void setpoint(int number);
    void getcoll(void);



signals:

     void Sign_set_point(void);
     void Sign_get_coll_fluo(void);
     void Signmax(void);

public slots:
     void Slot_get_check_state(bool checkstate);
protected:
    void run();
private:
    bool check_state;
    int  check_size;//��ǰ���в�����
    int  check_error;//δ�ӵ���Ϣ����
    int  check_step_number;//���в�������
    int  check_wait_time;//����ȴ�ʱ��
};

#endif // MYTHRTEAD_T_H
