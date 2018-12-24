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
    QSerialPort *bag;
    int openCom(QString port);
    QByteArray readComData;
    bool isOpen;
private slots:
    void On_readData();
};

#endif // BAGMACHINE_H
