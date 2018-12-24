#include "bagmachine.h"
#include <QThread>
BagMachine::BagMachine()
{
    Bag = new QSerialPort();

    connect(Bag,SIGNAL(readyRead()),this,SLOT(On_readData()));
}
void BagMachine::bagPayout()
{
    char cmd[3];
    cmd[0] = 0x6b;
    cmd[1] = 0x01;
    cmd[2] = 0x01;

    Bag->write(cmd);
}

void BagMachine::bagStatus()
{

    char cmd[3];
    cmd[0] = 0x6c;
    cmd[1] = 0x01;
    cmd[2] = 0xfe;

    Bag->write(cmd);
}


int BagMachine::openCom(QString port)
{

        Bag->setPortName(port);
        if(Bag->open(QIODevice::ReadWrite))
        {

            Bag->setBaudRate(QSerialPort::Baud115200);
            Bag->setParity(QSerialPort::NoParity);
            Bag->setDataBits(QSerialPort::Data8);//8位数据位
            Bag->setStopBits(QSerialPort::OneStop);//1个停止位
            Bag->setFlowControl(QSerialPort::NoFlowControl);
            bagStatus();
        }


}

void BagMachine::On_readData()
{
    QByteArray data =  Bag->readAll();

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
