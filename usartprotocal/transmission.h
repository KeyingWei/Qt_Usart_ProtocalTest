#ifndef TRANSMISSION_H
#define TRANSMISSION_H

#include "serial_handle.h"
#include <QObject>
#include <QTimer>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QSerialPortInfo>
#include "mainwindow.h"





class Transmission:public QObject
{
    Q_OBJECT
public:
    Transmission();



    unsigned short crc16_calc(unsigned char *buf, int length);
    void Send_Moter_Data(motor_Params_t motor_params);
    void Send_Movemoment_Data(movement_params_t movement_params);
    void Send_Movement_Enable(uint8_t crtl_value);
    void Send_Data_Frame_Enable(uint8_t ctrl_value);
    void Send_Privilege_Enable(uint8_t crtl_value);

    void Show_Feedback(uint8_t ack,uint8_t *buff,uint8_t len);

    void Open_Serial(QString Com, int Baud,uint8_t databits,uint8_t Parity,uint8_t stopbits);
    void Close_Serial();
    uint8_t crtl_terminal;

private:


    uint8_t ack_type;
    monitoringDat1_t monitoringdata;



     Serial_Handle *serial;
     QThread SerialThread;
     QTimer *Timer_ReadData;
     QTimer *Timer_SendMsg;

signals:
     void UI_Display_ACK(uint8_t ack);
     void UI_Display_Feedback(monitoringDat1_t data);

public  slots:
    void Init();
    void Send_Heartbeart();

private slots:

    void Read_Serial();
    char convertCharToHex(char ch);

};

#endif // TRANSMISSION_H
