#ifndef RFIDSCAN_H
#define RFIDSCAN_H

#include <QObject>
#include "KL4003.h"
#include <QTimer>
#include <QStandardItemModel>
#include "goodsdatabase.h"
#include <QSettings>

class rfidScan:public QObject
{
    Q_OBJECT
public:
    rfidScan(QStandardItemModel *tableModel);
    int start();
    void sell();
    void test_AddNewRFID();

private:
    enum Table{BIN_CODE = 0, GOODS_NAME, SALE_PRICE, VIP_PRICE, AMOUNT, TOTAL_PRICE};
    QTimer * scanRfidTimer;  //rfid开始扫描的定时器
    KL4003 *rfid = KL4003::Instance();
    int scanTimes;//扫描的次数
    QMap<QString ,QString >displayEPCAndBar;//保存显示的标签的EPC和barCode
    QMap<QString ,QString >newScanEPCAndBar;//保存最新扫描到的标签的EPC和barCode
    void displayInfo( QString bar);//在界面上显示商品信息
    void removeTag(QString code);
    void showPrice();
    goodsDatabase *goodsDB;
    QStandardItemModel *model;


private slots:
    void getRFIDData();
signals:
    void show();
};

#endif // RFIDSCAN_H
