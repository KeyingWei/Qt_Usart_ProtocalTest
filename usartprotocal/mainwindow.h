#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QWidget>
#include <QDebug>
#include "serial_handle.h"
#include "mytransmission.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void Display_Feedback_Data(monitoringDat1_t monitor_data);
    void Display_ACK(int ack);
    ~MainWindow();



private slots:
    void onStartOrStopButtonclicked();

    void onRes_data_buttonclicked();

    void onCtr_source_switch_currentIndexChanged(int index);

    void onSet_moment_paramsclicked();

    void onStart_or_stop_buttonclicked();

    void onEmergency_stop_buttonclicked();

    void onSet_motor_paramsclicked();

    void onReles_crtl_aulclicked();

    void EnableButton(bool enable);

    void SetFeedbackReadOnly();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    Read_data();
    ScanfSerialPort();
    QTimer *timer;
    motor_Params_t moter_params;
    movement_params_t movement_params;
    myTransmission *transmission;

};

#endif // MAINWINDOW_H
