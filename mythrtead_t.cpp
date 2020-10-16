#include "mythrtead_t.h"
#include<QtDebug>
MyThrtead_T::MyThrtead_T()
{
    qDebug()<<"MyThrtead_T"<<QThread::currentThreadId();
    check_state=true;
    check_error=0;
    check_size=0;
    check_step_number=50;
}

void MyThrtead_T::setpoint(int number)
{
    check_step_number=number;
    qDebug()<<"check_step_number"<<check_step_number;
}

//void MyThrtead_T::setpoint()
//{
//    qDebug()<<"setpoint";

//}

void MyThrtead_T::getcoll()
{
   //  Sign_get_coll_fluo();
    qDebug()<<"getcoll";
}

void MyThrtead_T::Slot_get_check_state(bool checkstate)
{
    check_state=checkstate;
}

void MyThrtead_T::run()
{
//    qDebug()<<"MyThrtead_T run"<<QThread::currentThreadId();
     qDebug()<<"check_step_number"<<check_step_number;
    while(1)
    {
        if(check_state)
        {
            if((check_size >= check_step_number ))
            {
                check_error=0;
                emit Signmax();
                return;
            }
            //下发电机位置
            //setpoint();
            emit Sign_set_point();

            QThread::msleep(1000);
            //下发采集荧光
//            getcoll();
           emit Sign_get_coll_fluo();
            check_size++;
            check_state=false;
            check_error=0;
            qDebug()<<"check_size"<<check_size;


        }
        else
        {
            if(check_error >10 )
            {
                return;
            }
            check_error++;
            check_state=false;
            qDebug()<<"check_error="<<check_error;
        }
        QThread::msleep(1000);
    }
}
