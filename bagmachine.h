#ifndef BAGMACHINE_H
#define BAGMACHINE_H
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QObject>
class BagMachine:public QObject
{
    Q_OBJECT
public:
    BagMachine();
    void bagPayout();
    void bagStatus();
    void bagRelax();
    int openCom();
    QSerialPort *bag;
    QByteArray readComData;
private:
    QString comPort;
private slots:
    void On_readData();

};

#endif // BAGMACHINE_H
