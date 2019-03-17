#include "transmission.h"
#include "serial_handle.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"


Transmission::Transmission()
{

}

void Transmission::Init()
{
    serial = new Serial_Handle();
    serial->moveToThread(&SerialThread);

    connect(&SerialThread, &QThread::finished, serial, &QObject::deleteLater);
    SerialThread.start();



    qDebug()<<"from Transmission:" << QThread::currentThreadId();//打印主线程ID

    Timer_ReadData = new QTimer(this);
    connect(Timer_ReadData, SIGNAL(timeout()), this, SLOT(Read_Serial()));
    Timer_ReadData->start(1);

    Timer_SendMsg = new QTimer(this);
    connect(Timer_SendMsg, SIGNAL(timeout()), this, SLOT(Send_Heartbeart()));
    Timer_SendMsg->start(20);

}

void Transmission::Open_Serial(QString Com, int Baud, uint8_t databits, uint8_t Parity, uint8_t stopbits)
{
    emit serial->Open_Serial(Com,Baud,databits,Parity,stopbits);
}

void Transmission::Close_Serial()
{
    emit serial->Close_Serial();
}

unsigned short Transmission::crc16_calc(unsigned char *buf, int length)
{
        unsigned short Result = 0x2B3B;

        for (int i = 0; i < length; i++)
        {
            Result = (Result >> 8) ^ CRC16_Table[(Result ^ buf[i]) & 0xff];
        }
        return Result;
}


void Transmission::Send_Moter_Data(motor_Params_t motor_params)
{
    char send_buff[20];
    uint16_t crc16;

    send_buff[0] = 0xA5;
    send_buff[1] = 0x5A;
    send_buff[2] = 13;
    send_buff[3] = 0x11;
    send_buff[4] = motor_params.Wheel_diameter;
    send_buff[5] = motor_params.Transmission_gear_ratio;
    send_buff[6] = motor_params.Motor_reduction_ratio ;
    send_buff[7] = motor_params.motor_encoder_resolution;
    send_buff[8] = motor_params.motor_encoder_resolution >> 8;
    send_buff[9] = motor_params.pulley_encoder_resolution;
    send_buff[10] = motor_params.pulley_encoder_resolution >> 8;

    crc16 = crc16_calc((uint8_t *)send_buff,11);

    send_buff[11] = crc16;
    send_buff[12] = crc16 >> 8;

    QByteArray temp;
    temp.append(send_buff,send_buff[2]);
    emit serial->WriteData(temp);
}

void Transmission::Send_Movemoment_Data(movement_params_t movement_params)
{
    char send_buff[20];
    uint16_t crc16;

    send_buff[0] = 0xA5;
    send_buff[1] = 0x5A;
    send_buff[2] = 13;
    send_buff[3] = 0x21;
    send_buff[4] = movement_params.Travelling_Direction;
    send_buff[5] = movement_params.speed;
    send_buff[6] = movement_params.speed >> 8;
    send_buff[7] = movement_params.distance;
    send_buff[8] = movement_params.distance >> 8;
    send_buff[9] = movement_params.accel_value;
    send_buff[10] = movement_params.accel_value >> 8;

    crc16 = crc16_calc((uint8_t *)send_buff,11);

    send_buff[11] = crc16;
    send_buff[12] = crc16 >> 8;

    QByteArray temp;
    temp.append(send_buff,send_buff[2]);
    emit serial->WriteData(temp);
}

void Transmission::Send_Movement_Enable(uint8_t crtl_value)
{
    char send_buff[20];
    uint16_t crc16;

    send_buff[0] = 0xA5;
    send_buff[1] = 0x5A;
    send_buff[2] = 7;
    send_buff[3] = 0x31;
    send_buff[4] = crtl_value;

    crc16 = crc16_calc((uint8_t *)send_buff,5);

    send_buff[5] = crc16;
    send_buff[6] = crc16 >> 8;

    QByteArray temp;
    temp.append(send_buff,send_buff[2]);
    emit serial->WriteData(temp);
}

void Transmission::Send_Data_Frame_Enable(uint8_t ctrl_value)
{
     char send_buff[20];
     uint16_t crc16;


     send_buff[0] = 0xA5;
     send_buff[1] = 0x5A;
     send_buff[2] = 7;

     if(crtl_terminal == HOST_COMPUTER)
     {
         send_buff[3] = 0x41;
         send_buff[4] = ctrl_value;
     }
     else
     {
         send_buff[3] = 0x42;
         send_buff[4] = ctrl_value;
     }

     crc16 = crc16_calc((uint8_t *)send_buff,5);

     send_buff[5] = crc16;
     send_buff[6] = crc16 >> 8;

     QByteArray temp;
     temp.append(send_buff,send_buff[2]);
     emit serial->WriteData(temp);
}

void Transmission::Send_Privilege_Enable(uint8_t crtl_value)
{
    char send_buff[20];
    uint16_t crc16;


    send_buff[0] = 0xA5;
    send_buff[1] = 0x5A;
    send_buff[2] = 7;
    if(crtl_terminal == HOST_COMPUTER)
    {
        send_buff[3] = 0x61;
        send_buff[4] = crtl_value;
    }
    else
    {
        send_buff[3] = 0x62;
        send_buff[4] = crtl_value;
    }

    crc16 = crc16_calc((uint8_t *)send_buff,5);

    send_buff[5] = crc16;
    send_buff[6] = crc16 >> 8;

    QByteArray temp;
    temp.append(send_buff,send_buff[2]);
    emit serial->WriteData(temp);
}


void Transmission::Send_Heartbeart()
{
    char send_buff[20];
    uint16_t crc16;
    static uint8_t cnt1 = 0,cnt2 = 0;

    send_buff[0] = 0xA5;
    send_buff[1] = 0x5A;
    send_buff[2] = 7;

    if(crtl_terminal == HOST_COMPUTER)
    {
       send_buff[3] = 0x51;
       send_buff[4] = cnt1;
       cnt1++;
    }
    else
    {
       send_buff[3] = 0x52;
       send_buff[4] = cnt2;
       cnt2++;
    }

    crc16 = crc16_calc((uint8_t *)send_buff,5);

    send_buff[5] = crc16;
    send_buff[6] = crc16 >> 8;

    QByteArray temp;
    temp.append(send_buff,send_buff[2]);
    emit serial->WriteData(temp);
}


void Transmission::Read_Serial()
{
    QByteArray temp;
    uint8_t buffer[50];
    char i = 0;
    unsigned int len =  serial->GetDataCount();

    if(len >= 6 * 2)
    {
        if((serial->at(0) == 'A' && serial->at(1) == '5'\
           && serial->at(2) == '5' && serial->at(3) == 'A') || (serial->at(0) == 'a' && serial->at(1) == '5'\
           && serial->at(2) == '5' && serial->at(3) == 'a') )
        {
                qDebug()<<"帧头校验成功";
               uint16_t length = ((convertCharToHex(serial->at(4)) << 4 )+ convertCharToHex(serial->at(5)));

                 qDebug()<<"帧长度为："<<length;

                 serial->Rec(&temp,length);

                 qDebug()<<"rx data"<<temp;

                for(char i=0;i< length ; i++)
                {
                   buffer[i] = (convertCharToHex(temp.at( 2 * i )) << 4 )+ convertCharToHex(temp.at(2*i + 1));
                }

                uint16_t crc_result;
                crc_result = crc16_calc(buffer,length);
                qDebug()<<"帧校验结果为："<<crc_result;


                if( crc_result== 0)
                {
                    qDebug()<<"crc校验成功";
                     ack_type = buffer[i+3];
                     Show_Feedback(ack_type,buffer,length);
                }
            }
      }
    else
    {
        serial->Drop(1);
        for(;;)
        {
            if(serial->GetDataCount() > 0)//读至下一帧帧头
            {
                if((serial->at(0) == 'A' && serial->at(1) == '5'\
                   && serial->at(2) == '5' && serial->at(3) == 'A') || (serial->at(0) == 'a' && serial->at(1) == '5'\
                   && serial->at(2) == '5' && serial->at(3) == 'a') )
                 {
                     break;
                 }
                 else
                 {
                     serial->Drop(1);
                 }
            }
            else
            {
                 break;
            }
        }
    }
}

void Transmission::Show_Feedback(uint8_t ack, uint8_t *buff, uint8_t len)
{
    if(ack == 0xf1)
    {
        memcpy(&monitoringdata,&buff[4],len);
        emit UI_Display_Feedback(monitoringdata);
    }
    else
    {
        emit UI_Display_ACK(ack);
    }

}


char Transmission::convertCharToHex(char ch)
{
    /*
    0x30等于十进制的48，48也是0的ASCII值，，
    1-9的ASCII值是49-57，，所以某一个值－0x30，，
    就是将字符0-9转换为0-9

    */
    if((ch >= '0') && (ch <= '9'))
         return ch-0x30;
     else if((ch >= 'A') && (ch <= 'F'))
         return ch-'A'+10;
     else if((ch >= 'a') && (ch <= 'f'))
         return ch-'a'+10;
     else return (-1);
}

