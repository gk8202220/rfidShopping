#include "bagmachine.h"
#include <QThread>
#include <QDebug>
#include <QSettings>
#define configFile "config/config.ini"
BagMachine::BagMachine()
{
    bag = new QSerialPort();
    connect(bag,SIGNAL(readyRead()),this,SLOT(On_readData()));
    QSettings setting(configFile,QSettings::IniFormat);
    comPort = setting.value("bag/com").toString();
}
void BagMachine::bagPayout()
{
    char cmd[3];
    cmd[0] = 0x6b;
    cmd[1] = 0x01;
    cmd[2] = 0x01;
    bag->write(cmd);
}

void BagMachine::bagStatus()
{

    char cmd[3];
    cmd[0] = 0x6c;
    cmd[1] = 0x01;
    cmd[2] = 0xfe;
    bag->write(cmd);
}

void BagMachine::bagRelax()
{
    char cmd[3];
    cmd[0] = 0x6b;
    cmd[1] = 0x01;
    cmd[2] = 0x02;
    bag->write(cmd);
}


int BagMachine::openCom()
{

    if(comPort.isEmpty())return -1;
    foreach (QSerialPortInfo info, QSerialPortInfo::availablePorts()) {
        if(info.portName() == comPort)bag->setPort(info);
        if(bag->open(QIODevice::ReadWrite))
        {

            bag->setBaudRate(QSerialPort::Baud115200);
            bag->setParity(QSerialPort::NoParity);
            bag->setDataBits(QSerialPort::Data8);//8位数据位
            bag->setStopBits(QSerialPort::OneStop);//1个停止位
            bag->setFlowControl(QSerialPort::NoFlowControl);
            return 0;
            //bagStatus();
        }
}
return -1;
}

void BagMachine::On_readData()
{
    QByteArray data =  bag->readAll();

    if(!data.isEmpty())
    {
        if(data.count()!=3){
            readComData.append(data);
            return;
        }
        readComData.clear();
        if(data.at(0)  == 0x6b)
        {
            if(data.at(2) == 0x01)
            {
                //出袋成功
               // ui->label_status->setText("出袋成功!");
            }else
            {
               // ui->label_status->setText("出袋失败!");
            }
        }
        if(data.at(0) == 0x6c)
        {
            if(data.at(2) == 0x01)
            {
                //出袋成功
//                ui->label_status->setText("有袋子!");
            }else
            {
              //  ui->label_status->setText("没有袋子了");
            }

        }
    }
}
