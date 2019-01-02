#ifndef RFID_H
#define RFID_H
#include "goodsdatabase.h"
#include "KL4003.h"
#include <QObject>
#include <QStandardItem>
#include <QTimer>
#include <QTime>
#include <QSettings>
enum Table{BIN_CODE , GOODS_NAME, SALE_PRICE, VIP_PRICE, AMOUNT, TOTAL_PRICE};
class Rfid:public QObject
{
    Q_OBJECT
public:
    Rfid(QStandardItemModel *ItemModel);
    void sell();
    void test_AddNewRFID();
    int  start();
private:
    void displayInfo(QString bar);//在界面上显示商品信息
    void removeTag(QString code);
    QStandardItemModel *model;
    QTimer *scanRfidTimer;  //rfid开始扫描的定时器
    int scanTimes;//扫描的次数
    KL4003 *rfid;// = KL4003::Instance();
    QMap<QString ,QString >displayEPCAndBar;//保存显示的标签的EPC和barCode
    QMap<QString ,QString >newScanEPCAndBar;//保存最新扫描到的标签的EPC和barCode
    goodsDatabase *goodsDB;


signals:
   void show();
private slots:
    void getRFIDData();
};

#endif // RFID_H
