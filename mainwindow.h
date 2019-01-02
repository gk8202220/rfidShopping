#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QFileDialog>
#include <QTimer>
#include <QTime>
#include <QSettings>
#include <QObject>
#include <QSettings>
#include <QMessageBox>
#include "rfid.h"
#include "bagmachine.h"
#include "pay.h"
#include  "qrcodegenerate.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    void sell();
    void test_AddNewRFID();
    ~MainWindow();

private: 
    QStandardItemModel * model;
    struct goodsInfo1 *p;
    QList<QStandardItem*> m_item_list;
    void openDevice(); //打开设备
    void setCss();
    void setItem(int row,int col,QString text);
    void setButtonCss();
    QStandardItem* getItem(int row, int col);
    void disPlay();

    void hideLabel();//隐藏显示微信支付宝支付的控件
    void showLabel();//显示微信支付宝支付控件

    Ui::MainWindow *ui;
    BagMachine bag; //出袋机
    Pay *pay;//支付
    QString trade_num;
    QrcodeGenerate *QRCode;
    Rfid *rfid;

private slots:
    void on_payStatus(QString status,QString data);
    void showPrice();
    void on_bagOut_clicked();
    void on_restart_clicked();
    void on_sell_clicked();
};


#endif // MAINWINDOW_H
