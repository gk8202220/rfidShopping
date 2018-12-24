#include "bagmachine.h"
#include <QThread>
#include <QDebug>
BagMachine::BagMachine()
{
    bag = new QSerialPort();

    connect(bag,SIGNAL(readyRead()),this,SLOT(On_readData()));
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


int BagMachine::openCom(QString port)
{
    foreach (QSerialPortInfo info, QSerialPortInfo::availablePorts()) {

        if(info.portName() == port)
        {
              bag->setPort(info);
              qDebug()<< "com11";
        }


        if(bag->open(QIODevice::ReadWrite))
        {

            bag->setBaudRate(QSerialPort::Baud115200);
            bag->setParity(QSerialPort::NoParity);
            bag->setDataBits(QSerialPort::Data8);//8位数据位
            bag->setStopBits(QSerialPort::OneStop);//1个停止位
            bag->setFlowControl(QSerialPort::NoFlowControl);
            bagStatus();
        }
}

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
