#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QFileDialog>
#include <QTimer>
#include <QSettings>
#include <QObject>
#include <QSettings>
#include "goodsdatabase.h"
#include "KL4003.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    int start();
    void sell();
    void test_AddNewRFID();

    ~MainWindow();

private:
   // enum Table{BIN_CODE = 0, GOODS_NAME, SALE_PRICE, VIP_PRICE, AMOUNT, TOTAL_PRICE};

    QTimer *scanRfidTimer;  //rfid开始扫描的定时器
    KL4003 *rfid = KL4003::Instance();

    int scanTimes;//扫描的次数
    QMap<QString ,QString >displayEPCAndBar;//保存显示的标签的EPC和barCode
    QMap<QString ,QString >newScanEPCAndBar;//保存最新扫描到的标签的EPC和barCode
    void displayInfo( QString bar);//在界面上显示商品信息
    void removeTag(QString code);
    void showPrice();
    goodsDatabase *goodsDB;

    QStandardItemModel * model;
    struct goodsInfo1 *p;
    QList<QStandardItem*> m_item_list;
    void setCss();

    void setItem(int row,int col,QString text);
    QStandardItem* getItem(int row, int col);
    void disPlay();


    Ui::MainWindow *ui;

private slots:
    void on_save_clicked();
    void getRFIDData();

/*signals:
    void show();*/
};


#endif // MAINWINDOW_H
