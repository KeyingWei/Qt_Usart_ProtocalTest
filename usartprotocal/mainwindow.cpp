#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QList>
#include <QQueue>
#include <stdlib.h>
#include <qformlayout.h>
#include "mytransmission.h"
#include "qt_ui_style.h"
#include <qstylefactory.h>


/*设置不同功能按键的样式表*/
QString open_button_style = "QPushButton{background-color:green;\
                                   color: white;   border-radius: 10px;  border: 2px groove gray;\
                                   border-style: outset;}";
QString close_button_style = "QPushButton{background-color:red;\
                             color: white;   border-radius: 10px;  border: 2px groove gray;\
                             border-style: outset;}";

                                 //  "QPushButton:hover{background-color:white; color: black;}"\
                                     "QPushButton:pressed{background-color:rgb(85, 170, 255);\
                                     border-style: inset; }";

 QString set_button_style = "QPushButton{background-color:green;\
                              color: white;   border-radius: 10px;  border: 2px groove gray;\
                              border-style: outset;}"\
                              "QPushButton:hover{background-color:white; color: black;}"\
                              "QPushButton:pressed{background-color:rgb(85, 170, 255);\
                              border-style: inset; }";



 QString mode_select = "QPushButton{background-color:blue;\
                             color: white;\
                               }";
 QString mode_deselect = "QPushButton{background-color:gray;\
                             color: black;\
                               }";


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("轨道车通讯测试上位机DEMO"));

    ui->baudBox->setCurrentIndex(0);
    ui->DataBitsBox->setCurrentIndex(0);
    ui->ParityBox->setCurrentIndex(0);
    ui->StopBitsBox->setCurrentIndex(0);

    timer =new QTimer(this);//创建一个定时器
    connect(timer,&QTimer::timeout,this,&MainWindow::ScanfSerialPort);//信号连接
    //开启计时，100ms检测是否有串口设备接入
    timer->start(100);

    transmission = new myTransmission;

     QT_UI_Style::style_set(":/new/prefix1/QT_STYLE_DARK_BLUE.qss");

      /*连接按钮的信号槽*/
     connect(ui->StartOrStopButton,&QPushButton::clicked,this,&MainWindow::onStartOrStopButtonclicked);
     connect(ui->res_data_button,&QPushButton::clicked,this,&MainWindow::onRes_data_buttonclicked);
     connect(ui->start_or_stop_button,&QPushButton::clicked,this,&MainWindow::onStart_or_stop_buttonclicked);
     connect(ui->emergency_stop_button,&QPushButton::clicked,this,&MainWindow::onEmergency_stop_buttonclicked);
     connect(ui->set_moment_params,&QPushButton::clicked,this,&MainWindow::onSet_moment_paramsclicked);
     connect(ui->set_motor_params,&QPushButton::clicked,this,&MainWindow::onSet_motor_paramsclicked);
     connect(ui->ctr_source_switch,SIGNAL(currentIndexChanged(int)),this,SLOT(onCtr_source_switch_currentIndexChanged(int)));
     connect(ui->reles_crtl_aul,&QPushButton::clicked,this,&MainWindow::onReles_crtl_aulclicked);

     /*连接反馈数据显示的信号槽*/
     QObject::connect(transmission,&myTransmission::UI_Display_ACK,this,&MainWindow::Display_ACK);
     QObject::connect(transmission,&myTransmission::UI_Display_Feedback,this,&MainWindow::Display_Feedback_Data);


     ScanfSerialPort();


     ui->ack_plainTextEdit->setReadOnly(true);
     ui->ack_plainTextEdit->setFixedHeight(40);
     setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);
     setFixedSize(this->width(),this->height());

//     ui->groupBox->setFixedHeight(50);
//     ui->groupBox_2->setFixedHeight(50);
//     ui->groupBox_3->setFixedHeight(50);
//     ui->groupBox_28->setFixedHeight(75);
//     ui->groupBox_4->setFixedHeight(75);


     ui->res_data_button->setText(tr("开启数据帧"));
     ui->reles_crtl_aul->setText( tr("申请控制权限"));
     ui->start_or_stop_button->setText(tr("启动"));

     qDebug()<<"UI线程ID"<<QThread::currentThreadId();


     /*设置界面风格为暗黑*/
     qApp->setStyle(QStyleFactory::create("Fusion"));
     QPalette palette;
     palette.setColor(QPalette::Window, QColor(53,53,53));
     palette.setColor(QPalette::WindowText, Qt::white);
     palette.setColor(QPalette::Base, QColor(15,15,15));
     palette.setColor(QPalette::AlternateBase, QColor(53,53,53));
     palette.setColor(QPalette::ToolTipBase, Qt::white);
     palette.setColor(QPalette::ToolTipText, Qt::white);
     palette.setColor(QPalette::Text, Qt::white);
     palette.setColor(QPalette::Button, QColor(53,53,53));
     palette.setColor(QPalette::ButtonText, Qt::white);
     palette.setColor(QPalette::BrightText, Qt::red);
     palette.setColor(QPalette::Highlight, QColor(142,45,197).lighter());
     palette.setColor(QPalette::HighlightedText, Qt::black);
     qApp->setPalette(palette);

     /*失能设置按钮*/
     EnableButton(false);

     ui->set_moment_params->setStyleSheet(set_button_style);
     ui->set_motor_params->setStyleSheet(set_button_style);
     ui->res_data_button->setStyleSheet(open_button_style);
     ui->reles_crtl_aul->setStyleSheet(open_button_style);
     ui->start_or_stop_button->setStyleSheet(open_button_style);
     ui->emergency_stop_button->setStyleSheet(close_button_style);

     SetFeedbackReadOnly();
 //    ui->label_2->setOpenExternalLinks(true);

     qDebug()<<tr("界面设置成功");
}

MainWindow::~MainWindow()
{
    delete ui;
}

MainWindow::ScanfSerialPort()
{
    static int serila_num_curr = 0,serial_num_last = 0;
    QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
    if(infos.isEmpty())
    {
         ui->PortNumber->clear();
         serila_num_curr = 0;
         ui->StartOrStopButton->setEnabled(false);
    }
    else
    {
       QStringList list;
       ui->StartOrStopButton->setEnabled(true);
       serial_num_last = serila_num_curr;
        foreach(QSerialPortInfo info,infos)
        {
             list << info.portName();
        }
        serila_num_curr = list.length();
         if(serila_num_curr != serial_num_last)
         {
             ui->PortNumber->clear();
             qSort(list.begin(),list.end());
             ui->PortNumber->addItems(list);
         }

    }

    return 0;
}


void MainWindow::Display_Feedback_Data(monitoringDat1_t monitor_data)
{

    ui->power_voltage->setText(QString::number(monitor_data.power_voltage));
    ui->power_current->setText(QString::number(monitor_data.power_current));
    ui->motor_drive_current->setText(QString::number(monitor_data.motor_drive_current));
    ui->power_voltage_threshold->setText(QString::number(monitor_data.power_voltage_threshold));
    ui->power_current_threshold->setText(QString::number(monitor_data.power_current_threshold));
    ui->motor_current_threshold->setText(QString::number(monitor_data.motor_current_threshold));
    ui->power_vltage_state->setText(QString::number(monitor_data.power_vltage_state));
    ui->motor_running_state->setText(QString::number(monitor_data.motor_running_state));
    ui->control_usart_state->setText(QString::number(monitor_data.control_usart_state));
    ui->running_light_state->setText(QString::number(monitor_data.running_light_state));
    ui->deive_state->setText(QString::number(monitor_data.deive_state));
    ui->pulley_speed->setText(QString::number((int16_t)monitor_data.pulley_speed));
    ui->encodera_accVal->setText(QString::number((int32_t)monitor_data.encodera_accVal));
    ui->movement_distance->setText(QString::number((int32_t)monitor_data.movement_distance));
    ui->Wheel_diameter->setText(QString::number(monitor_data.Wheel_diameter));
    ui->Transmission_gear_ratio->setText(QString::number(monitor_data.Transmission_gear_ratio));
    ui->Motor_reduction_ratio_2->setText(QString::number(monitor_data.Motor_reduction_ratio));
    ui->motor_encoder_resolution_2->setText(QString::number(monitor_data.motor_encoder_resolution));
    ui->pulley_encoder_resolution_2->setText(QString::number(monitor_data.pulley_encoder_resolution));
    ui->Privilege_identifier->setText(QString::number(monitor_data.Privilege_identifier));
    ui->Travelling_Direction->setText(QString::number(monitor_data.Travelling_Direction));

}

void MainWindow::Display_ACK(int ack)
{
    ui->ack_plainTextEdit->clear();
    switch(ack)
    {
        case 0xa1:
          ui->ack_plainTextEdit->insertPlainText(tr("成功设置电机参数!"));
          break;
        case 0xb1:
           ui->ack_plainTextEdit->insertPlainText(tr("成功设置运动参数!"));
            break;
        case 0xc1:
           ui->ack_plainTextEdit->insertPlainText(tr("成功设置运动使能!"));
            break;
        case 0xd1:
           ui->ack_plainTextEdit->insertPlainText(tr("上位机成功设置数据帧使能!"));
            break;
        case 0xd2:
           ui->ack_plainTextEdit->insertPlainText(tr("面板成功设置数据帧使能!"));
            break;
        case 0xe0:
           ui->ack_plainTextEdit->insertPlainText(tr("上位机获取控制权限失败!"));
            break;
        case 0xe1:
           ui->ack_plainTextEdit->insertPlainText(tr("上位机获取控制权限成功!"));
            break;
        case 0xe2:
           ui->ack_plainTextEdit->insertPlainText(tr("上位机释放控制权限成功!"));
            break;
        case 0xe3:
           ui->ack_plainTextEdit->insertPlainText(tr("面板获取控制权限失败!"));
            break;
        case 0xe4:
           ui->ack_plainTextEdit->insertPlainText(tr("面板获取控制权限成功!"));
           break;
        case 0xe5:
           ui->ack_plainTextEdit->insertPlainText(tr("面板释放控制权限成功!"));
            break;
        default :
            break;
    }
}



void MainWindow::onStartOrStopButtonclicked()
{
    if(ui->StartOrStopButton->text() == tr("打开串口"))
       { 

           transmission->Open_Serial(ui->PortNumber->currentText(),ui->baudBox->currentText().toInt(),\
                                      ui->DataBitsBox->currentText().toInt(),ui->ParityBox->currentIndex(),ui->StopBitsBox->currentIndex());

           transmission->serial_stata = 1;

           EnableButton(true);

             ui->baudBox->setEnabled(false);
             ui->StopBitsBox->setEnabled(false);
             ui->ParityBox->setEnabled(false);
             ui->DataBitsBox->setEnabled(false);
             ui->PortNumber->setEnabled(false);
             ui->StartOrStopButton->setText(tr("关闭串口"));
    }
    else
    {
        transmission->Close_Serial();
         transmission->serial_stata = 0;

         EnableButton(false);

        ui->baudBox->setEnabled(true);
        ui->StopBitsBox->setEnabled(true);
        ui->ParityBox->setEnabled(true);
        ui->DataBitsBox->setEnabled(true);
        ui->PortNumber->setEnabled(true);
        ui->StartOrStopButton->setText(tr("打开串口"));
    }
}




void MainWindow::onCtr_source_switch_currentIndexChanged(int index)
{
    qDebug()<<"curr index is "<<index;

    transmission->crtl_terminal = index;
}



void MainWindow::onRes_data_buttonclicked()
{
   qDebug()<<"设置接收数据帧 "<<ui->res_data_button->text() ;

  if(ui->res_data_button->text() == tr("开启数据帧"))
   {
      transmission->Send_Data_Frame_Enable(1);
      ui->res_data_button->setStyleSheet(close_button_style);
      ui->res_data_button->setText(tr("停止数据帧"));
   }
  else
  {
        transmission->Send_Data_Frame_Enable(0);
         ui->res_data_button->setStyleSheet(open_button_style);
         ui->res_data_button->setText(tr("开启数据帧"));
  }
}


void MainWindow::onSet_moment_paramsclicked()
{


    movement_params.Travelling_Direction = ui->set_direction->text().toInt();
    movement_params.speed = ui->setspeed->text().toInt();
    movement_params.distance = ui->setdistance->text().toInt();
    movement_params.accel_value = ui->setaccel_value->text().toInt();

   transmission->Send_Movemoment_Data(movement_params);
}


void MainWindow::onStart_or_stop_buttonclicked()
{

    if(ui->start_or_stop_button->text() == tr("启动"))
     {
         transmission->Send_Movement_Enable(0x01);
         ui->start_or_stop_button->setStyleSheet(close_button_style);
         ui->start_or_stop_button->setText(tr("停止"));
     }
    else
    {
         transmission->Send_Movement_Enable(0x00);
          ui->start_or_stop_button->setStyleSheet(open_button_style);
         ui->start_or_stop_button->setText(tr("启动"));
    }
}

void MainWindow::onEmergency_stop_buttonclicked()
{
    transmission->Send_Movement_Enable(0x03);
}

void MainWindow::onSet_motor_paramsclicked()
{
    moter_params.Wheel_diameter = ui->set_wheel_diameter->text().toInt();
    moter_params.Transmission_gear_ratio = ui->set_gear_ratio->text().toInt();
    moter_params.Motor_reduction_ratio = ui->set_Motor_reduction_ratio->text().toInt();
    moter_params.motor_encoder_resolution = ui->set_motor_encoder_resolution->text().toInt();
    moter_params.pulley_encoder_resolution = ui->set_pulley_encoder_resolution->text().toInt();

   transmission->Send_Moter_Data(moter_params);

}

void MainWindow::onReles_crtl_aulclicked()
{
    if(ui->reles_crtl_aul->text() == tr("申请控制权限"))
        {

           ui->reles_crtl_aul->setStyleSheet(close_button_style);
           transmission->Send_Privilege_Enable(1);
           ui->reles_crtl_aul->setText(tr("释放控制权限"));
        }
    else
    {
        transmission->Send_Privilege_Enable(2);
        ui->reles_crtl_aul->setStyleSheet(open_button_style);
        ui->reles_crtl_aul->setText(tr("申请控制权限"));
    }
}

void MainWindow::EnableButton(bool enable)
{
    if(enable ==  true)
    {
        ui->res_data_button->setEnabled(true);
        ui->reles_crtl_aul->setEnabled(true);
        ui->start_or_stop_button->setEnabled(true);
        ui->emergency_stop_button->setEnabled(true);
        ui->set_moment_params->setEnabled(true);
        ui->set_motor_params->setEnabled(true);

    }
    else
    {
        ui->res_data_button->setEnabled(false);
        ui->reles_crtl_aul->setEnabled(false);
        ui->start_or_stop_button->setEnabled(false);
        ui->emergency_stop_button->setEnabled(false);
        ui->set_moment_params->setEnabled(false);
        ui->set_motor_params->setEnabled(false);
    }

}

void MainWindow::SetFeedbackReadOnly()
{
    ui->power_voltage->setReadOnly(true);
    ui->power_current->setReadOnly(true);
    ui->motor_drive_current->setReadOnly(true);
    ui->power_voltage_threshold->setReadOnly(true);
    ui->power_current_threshold->setReadOnly(true);
    ui->motor_current_threshold->setReadOnly(true);
    ui->power_vltage_state->setReadOnly(true);
    ui->motor_running_state->setReadOnly(true);
    ui->control_usart_state->setReadOnly(true);
    ui->running_light_state->setReadOnly(true);
    ui->deive_state->setReadOnly(true);
    ui->pulley_speed->setReadOnly(true);
    ui->encodera_accVal->setReadOnly(true);
    ui->movement_distance->setReadOnly(true);
    ui->Wheel_diameter->setReadOnly(true);
    ui->Transmission_gear_ratio->setReadOnly(true);
    ui->Motor_reduction_ratio_2->setReadOnly(true);
    ui->motor_encoder_resolution_2->setReadOnly(true);
    ui->pulley_encoder_resolution_2->setReadOnly(true);
    ui->Privilege_identifier->setReadOnly(true);
    ui->Travelling_Direction->setReadOnly(true);
}

