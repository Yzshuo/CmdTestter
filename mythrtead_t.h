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
    int  check_size;//当前运行步骤数
    int  check_error;//未接到消息次数
    int  check_step_number;//运行步骤总数
    int  check_wait_time;//步骤等待时间
};

#endif // MYTHRTEAD_T_H
