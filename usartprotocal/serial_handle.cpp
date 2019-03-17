#include "serial_handle.h"

Serial_Handle::Serial_Handle()
{
    QObject::connect(this,   &Serial_Handle::WriteData,    this, &Serial_Handle::Write);
    QObject::connect(this,   &Serial_Handle::Close_Serial, this, &Serial_Handle::Close);

}


Serial_Handle::~Serial_Handle()
{
    serial->close();
}

void Serial_Handle::Close()
{
    serial->clear();
    serial->close();
    serial->deleteLater();
}

void Serial_Handle::Open(QString Com, int Baud,char databits,char Parity,char stopbits)
{
    serial = new QSerialPort;
    //设置串口名
    serial->setPortName(Com);           //下位机串口
    //打开串口
    serial->open(QIODevice::ReadWrite);
    //设置波特率
    serial->setBaudRate(Baud);
    //设置数据位数
    switch(databits)
     {
         case 8:serial->setDataBits(QSerialPort::Data8);break;
         case 7:serial->setDataBits(QSerialPort::Data7);break;
         case 6:serial->setDataBits(QSerialPort::Data6);break;
         case 5:serial->setDataBits(QSerialPort::Data5);break;
     }

    //设置奇偶校验
    switch(Parity)
    {
         case 0:serial->setParity(QSerialPort::NoParity);break;
         case 1:serial->setParity(QSerialPort::EvenParity);break;
         case 2:serial->setParity(QSerialPort::OddParity);break;
    default:break;
    }
    //设置停止位
    switch(stopbits)
    {
         case 0:serial->setStopBits(QSerialPort::OneStop);break;
         case 1:serial->setStopBits(QSerialPort::OneAndHalfStop);break;
         case 2:serial->setStopBits(QSerialPort::TwoStop);break;
       default:break;
    }
    //设置流控制
    serial->setFlowControl(QSerialPort::NoFlowControl);

    QObject::connect(serial, &QSerialPort::readyRead,   this, &Serial_Handle::Read);

    qDebug() << "com = " << Com << " baud = " << Baud;
    qDebug()<<"from serial :" << QThread::currentThreadId();
}

void Serial_Handle::Read()
{
    ReadBuffer.append(serial->readAll().toHex());
}

void Serial_Handle::Write(QByteArray data)
{
    WriteBuffer.append(data);
    int len = serial->write(WriteBuffer);
    WriteBuffer.remove(0, len);
   // qDebug()<<"remove  "<<len<<"  data";
}

unsigned int Serial_Handle::GetDataCount()
{
    return ReadBuffer.size();
}

char Serial_Handle::at(unsigned int index)
{
    return ReadBuffer.at(index);
}

void Serial_Handle::Rec(QByteArray *buffer, unsigned int len)
{
    *buffer = ReadBuffer.mid(0, len);
    ReadBuffer.remove(0, len);
}

void Serial_Handle::Free()
{
    ReadBuffer.clear();
}

void Serial_Handle::Drop(unsigned int len)
{
    ReadBuffer.remove(0, len);
}

