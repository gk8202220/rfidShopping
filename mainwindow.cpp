#include "mainwindow.h"
#include "ui_mainwindow.h"
//电脑串口是57600 其它默认（8 奇偶是无 停止位1 流控制无）

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(1700,1000);
    disPlay();
    openDevice();
    setButtonCss();
    pay = new Pay();
    connect(pay,SIGNAL(payStatus(QString,QString)),this,SLOT(on_payStatus(QString,QString)));
}

void MainWindow::openDevice()
{
      int ret;
    rfid = new Rfid(model);
    connect(rfid,SIGNAL(show()),this,SLOT(showPrice()));
    ret = rfid->start();//开启rfid扫描仪
    if(ret < 0)qDebug() <<"打开rfid扫描仪失败";
    ret =  bag.openCom();//开启出袋机

    if(ret < 0)qDebug() <<"打开出袋机失败";
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::setItem(int row, int col, QString text)
{
    QStandardItem* itemTemp = new QStandardItem(text);
    model->setItem(row, col, itemTemp);
    m_item_list.push_back(itemTemp);
}


QStandardItem *MainWindow::getItem(int row, int col)
{
    return model->item(row,col)? model->item(row, col):new QStandardItem();
}

void MainWindow::disPlay()
{
    model = new QStandardItemModel();
    ui->tableView->setModel(model);
    model->setHorizontalHeaderItem(BIN_CODE,new QStandardItem(tr("商品编号")));
    model->setHorizontalHeaderItem(GOODS_NAME,new QStandardItem(tr("商品名称")));
    model->setHorizontalHeaderItem(SALE_PRICE,new QStandardItem(tr("原价")));
    model->setHorizontalHeaderItem(VIP_PRICE,new QStandardItem(tr("会员价")));
    model->setHorizontalHeaderItem(AMOUNT,new QStandardItem(tr("数量")));
    model->setHorizontalHeaderItem(TOTAL_PRICE,new QStandardItem(tr("小计")));
    ui->tableView->setColumnWidth(BIN_CODE,250);
    ui->tableView->setColumnWidth(SALE_PRICE,100);
    ui->tableView->setColumnWidth(VIP_PRICE,100);
    ui->tableView->setColumnWidth(AMOUNT,100);
    ui->tableView->setColumnWidth(TOTAL_PRICE,100);
    ui->tableView->horizontalHeader()->setSectionResizeMode(BIN_CODE,QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setSectionResizeMode(SALE_PRICE,QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setSectionResizeMode(VIP_PRICE,QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setSectionResizeMode(AMOUNT,QHeaderView::Fixed);//固定
    ui->tableView->horizontalHeader()->setSectionResizeMode(TOTAL_PRICE,QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setSectionResizeMode(GOODS_NAME,QHeaderView::Stretch);//设置伸缩
    ui->tableView->resizeColumnToContents(GOODS_NAME);
    ui->tableView->verticalHeader()->setVisible(false);//隐藏列表头
    ui->tableView->setShowGrid(false);//去除表格线

    QRCode = new QrcodeGenerate(ui->widget);
    setCss();
}


void MainWindow::setCss()
{
    ui->tableView->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:#ffe4c4;color: #000000;border: 0px solid;border-color:#cacaca;font:18pt Adobe Arabic;padding:5px 2px 5px 2px;border-radius:5}");
    ui->tableView->setStyleSheet("QTableView#tableView{"
                                 "background-color:#d0e3ff;"
                                 "border-radius:10;"
                                 "color:#000000;"
                                 "gridline-color: rgb(111, 156, 207);"
                                 "font:14pt Adobe Arabic}"
                                 );
}

void MainWindow::setButtonCss()
{
    this->setStyleSheet("QPushButton{"
                        "background-color:#55aaff;"
                        "color:#FFFFFF;"
                        "border-width:1;"
                        "border-style:outset;"
                        "border-radius:10px;"
                        "padding:2px 5px 2px 5px;"
                        "font:12pt Adobe Arabic}"
                        "QPushButton::hover{background-color:#ffaa7f}"
                        );


}

//在界面上显示价格以及发起微信收款请求
void MainWindow::showPrice()
{
    static float totalPriceBefore;
    static int totalAmountBefore;
    int   total_amount = 0; //商品总量
    float total_price  = 0;//总价格
    for(int i = 0;i < model->rowCount();i++)
    {
        int amount = model->index(i,AMOUNT).data().toInt();
        total_amount += amount;
        total_price  += model->index(i,TOTAL_PRICE).data().toFloat() * amount;
    }
    //如果商品产生变化，重新生成新的订单
    if( ( totalPriceBefore != total_price&&total_price!=0 ) || totalAmountBefore != total_amount)
    {
        totalPriceBefore  = total_price;
        totalAmountBefore = total_amount;
        ui->label_total->setText("总计:"+QString::number(totalPriceBefore)+"元"+"\n"+"数量:"+QString::number(totalAmountBefore)+"件"); //总价
        //生成新的二维码支付订单
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        trade_num = QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + QString::number(100 + qrand() % (999 - 100));
        pay->wxpay(trade_num,"0.01","rfid");
    }else if(total_price == 0){
        totalPriceBefore  = 0;
        totalAmountBefore = 0;
        ui->label_total->setText("总计:"+QString::number(totalPriceBefore)+"元"+"\n"+"数量:"+QString::number(totalAmountBefore)+"件"); //总价
        ui->widget->hide();
    }
}




void MainWindow::on_payStatus(QString status, QString data)
{

    if(status == "weixinQRCode")QRCode->generateString(data);
    else if(status == "alipayQRCode")ui->widget->generateString(data);
    else if(status == "payStatus")
    {
        if(data=="支付成功")
        {
            rfid->sell();//把标签去除
            bag.bagRelax();//释放袋子
        }
    }
    else if(status == "error")
    {
        qDebug()<<data; //失败信息
    }
}

//出袋按钮
void MainWindow::on_bagOut_clicked()
{
    bag.bagPayout();
}


//重新启动
void MainWindow::on_restart_clicked()
{
    rfid->test_AddNewRFID();
    bag.bagRelax();
}


//付款
void MainWindow::on_sell_clicked()
{


}
