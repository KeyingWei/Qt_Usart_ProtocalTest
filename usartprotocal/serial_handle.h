#ifndef SERIAL_HANDLE_H
#define SERIAL_HANDLE_H

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QSerialPortInfo>
#include <QObject>
#include <QDebug>
#include <QThread>



class Serial_Handle:public QObject
{
    Q_OBJECT
  public:
    Serial_Handle();
    ~Serial_Handle();

    char at(unsigned int index);

    void Rec(QByteArray *buffer, unsigned int len);

    unsigned int GetDataCount();

    void Free();

    void Drop(unsigned int len);

signals://信号槽
    void WriteData(QByteArray data);

    void Open_Serial(QString Com, int Baud,char databits,char Parity,char stopbits);
    void Close_Serial();


public  slots://公有触发槽

    void Open(QString Com, int Baud,char databits,char Parity,char stopbits);

    void Close();//关闭串口

    void Write(QByteArray data);//写数据

private slots://私有触发槽

    void Read();//读取串口数据

private:

    QSerialPort *serial;
    QByteArray ReadBuffer;//接收到的数据
    QByteArray WriteBuffer;//接收到的数据
};

#endif // SERIAL_HANDLE_H
