#include "mainwindow.h"
#include "ui_mainwindow.h"
//电脑串口是57600 其它默认（8 奇偶是无 停止位1 流控制无）

#include <QDebug>
#define stopTimer  10000
#define scanGoodsAnt 0x80 //扫描商品的天线
#define scanTime 2 //扫描的间隔时间
#define scanTimesTotal 3 // 扫描的次数


QString trade_num;
Pay *pay;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(1700,1000);  
    model = new QStandardItemModel();
    ui->tableView->setModel(model);
    disPlay();

    goodsDB = new goodsDatabase();
    scanRfidTimer = new QTimer();
    connect(scanRfidTimer,SIGNAL(timeout()),this,SLOT(getRFIDData()));
    scanTimes = 0;
    start();
    setButtonCss();
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
    setCss();

    //ui->widget->generateString("https://qr.alipay.com/bax04712rrptisv1la1820dc");
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    trade_num = QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + QString::number(100 + qrand() % (999 - 100));

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


void MainWindow::displayInfo(QString bar)
{
    if(bar.isEmpty())return;
    if(model == NULL )
    {
        qDebug()<<"请传入tableView Model";
        return;
    }
    int rowCount = model->rowCount();
        for(int i = 0;i <= rowCount;i++)//循环查询，查询是否想相同的商品编号
        {
           QString data_string = model->index(i,BIN_CODE).data().toString();
            if(data_string ==  bar)//如果有相同的编号
            {
                //已经有相同名称的的商品则数量上加上1
                int goodsCount  = model->index(i,AMOUNT).data().toInt();
                float TotalPrice  = model->index(i,TOTAL_PRICE).data().toFloat();
                float salePrice  = model->index(i,SALE_PRICE).data().toFloat();
                TotalPrice = salePrice*goodsCount;
                setItem(i,AMOUNT,QString::number(goodsCount+1)); //数量+1
                setItem(i,TOTAL_PRICE,QString::number(TotalPrice,'f',2)); //改变对应的总计
                 return;
            }
        }

       struct goodsInfo info;
       goodsDB->query(bar,&info);//从数据库获取商品信息
       if(info.barcode.isEmpty())
       {
          // QMessageBox::warning(this,QString("提示"),tr("无此商品，请重新扫描"),QMessageBox::NoButton,QMessageBox::NoButton);
           return ;
       }


        setItem(rowCount,BIN_CODE,bar);
        setItem(rowCount,GOODS_NAME,info.name );
        setItem(rowCount,SALE_PRICE,QString::number(info.salePrice,'f',2) );
        setItem(rowCount,SALE_PRICE,QString::number(info.vipPrice,'f',2) );
        setItem(rowCount,AMOUNT, QString::number(1));
        setItem(rowCount,TOTAL_PRICE,QString::number(info.salePrice,'f',2));

        model->item(rowCount,BIN_CODE)->setTextAlignment(Qt::AlignCenter);
        model->item(rowCount,GOODS_NAME)->setTextAlignment(Qt::AlignCenter);
        model->item(rowCount,SALE_PRICE)->setTextAlignment(Qt::AlignCenter);
        model->item(rowCount,VIP_PRICE)->setTextAlignment(Qt::AlignCenter);
        model->item(rowCount,AMOUNT)->setTextAlignment(Qt::AlignCenter);
        model->item(rowCount,TOTAL_PRICE)->setTextAlignment(Qt::AlignCenter);


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

}


void MainWindow::removeTag(QString code)
{
    for(int i=0;i<model->rowCount();i++)
      {
        if (model->data(model->index(i,0)).toString() == code)
        {
            int goodsCount = model->index(i,AMOUNT).data().toInt();
            if(goodsCount>1)
               setItem(i,AMOUNT,QString::number(goodsCount-1)); //数量-1
            else
            model->removeRow(i);
        }
    }
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

    if( ( totalPriceBefore != total_price&&total_price!=0 ) || totalAmountBefore != total_amount)
    {
        totalPriceBefore  = total_price;
        totalAmountBefore = total_amount;
        ui->label_total->setText("总计:"+QString::number(totalPriceBefore)+"元"+"\n"+"数量:"+QString::number(totalAmountBefore)+"件"); //总价
        pay->wxpay(trade_num,"0.01","rfid");

    }else if(total_price == 0){
        totalPriceBefore  = 0;
        totalAmountBefore = 0;
        ui->label_total->setText("总计:"+QString::number(totalPriceBefore)+"元"+"\n"+"数量:"+QString::number(totalAmountBefore)+"件"); //总价
        ui->widget->hide();
    }else
    {
        //商品没有变化，查询是否已经支付
        pay->query(trade_num);
    }
}


int MainWindow::start()
{
    if( rfid->isLoadLib())
    {
        int ret  =  rfid->RR_AutoOpenComPort();
        if(ret != 0)
        {
            //打开RFID不成功
         //this->close();
            qDebug() << "没有连接RFID";
            return -1;
        }else
        {
            qDebug() << "连接RFID";
            scanRfidTimer->start(scanTime);
            return 0;

        }
    }
    return -2; //加载RFID动态库失败，检查路径是否正确
}


void MainWindow::getRFIDData()
{/*
 * 使用实时扫描方式
 */
QMap <QString ,QString >newScanEPCAndBarTemp;
int outAnt;
    if(scanTimes<scanTimesTotal)
    {
        int ret = rfid->query(&outAnt,&newScanEPCAndBarTemp); //一次只能收到4个标签的数据。过多的数据会分别发送过来。
        if(ret<0)return;
        if(outAnt == 1)
        {
            foreach (QString EPC, newScanEPCAndBarTemp.keys()) {
                if(!newScanEPCAndBar.contains(EPC))
                 newScanEPCAndBar.insert(EPC,newScanEPCAndBarTemp.value(EPC));
            }
            scanTimes++;
        }//end if(outAnt == 1)
        else
        {
            //其他通道的天线用来检测是否购买
            if(!newScanEPCAndBar.isEmpty())
            {
                //天线检测到还没有购买的商品，进行报警
                qDebug()<<"***********报警*****************";
                foreach(QString bar,newScanEPCAndBar.values())qDebug()<<bar;
                qDebug()<<"***********报警*****************";
            }

        }//end else scanTimes
    }else
    {
        scanTimes = 0;

        // qDebug()<< "新的扫描标签 " <<newScanEPCAndBar.keys() <<newScanEPCAndBar.values();
        //qDebug()<< "显示的标签 " <<displayEPCAndBar.keys() <<displayEPCAndBar.values();

        //获取到天线1的标签(收银台的天线)
        //  qDebug()<< "标签数量 "<< EPCAndBar.count();
        //qDebug()<<"目前显示的商品 " << dispalyBar.count() <<dispalyBar.keys()<<dispalyBar.values();

        if(displayEPCAndBar.isEmpty())
        {
            //此时显示列表为空，将显示锁有得到的标签
            foreach (QString bar, newScanEPCAndBar.values()) {
                // qDebug()<< "全新的标签 " <<bar;
                displayEPCAndBar = newScanEPCAndBar;
                 displayInfo(bar);
            }
        }else
        {

            foreach (QString dispalyEPCTemp, displayEPCAndBar.keys()) {
                if(!newScanEPCAndBar.contains(dispalyEPCTemp))
                {
                    removeTag(displayEPCAndBar.value(dispalyEPCTemp));
                    displayEPCAndBar.remove(dispalyEPCTemp);
                }

            }

            //2.如果新扫描到的标签在显示界面上的列表中存在则不处理
            //3. 新扫描到的标签存在 -- 显示界面上的标签中不存在 > 则添加
            foreach (QString newEPC, newScanEPCAndBar.keys()) {
                if(!displayEPCAndBar.contains(newEPC))
                {
                    //  qDebug()<<"新添加的商品 "<< newScanEPCAndBar.value(newEPC);
                   displayEPCAndBar.insert(newEPC,newScanEPCAndBar.value(newEPC));
                     displayInfo(newScanEPCAndBar.value(newEPC));
                }
            }
        }     
      newScanEPCAndBar.clear();
    }
}


void MainWindow::sell()
{
   scanRfidTimer->stop();    //停止读取rfid
   qDebug()<<"将要售出的所有标签"<<displayEPCAndBar.keys();
   int destroyTimes = 0;//进行标志失败的次数
    do
    {
         foreach (QString EPC, displayEPCAndBar.keys()) {
             QString displayBar =displayEPCAndBar.value(EPC);
             int ret =  rfid->TagSell(EPC,displayBar);
             if(ret != 0)ret = rfid->TagSell(EPC,displayBar);
             QString bar;
             rfid->readTagData(EPC,&bar);//读取数据
//              qDebug()<<"本次要售出的标签"<<EPC<<displayBar;
//              qDebug()<<"本次写入后查询的结果EPC"<<EPC<<"bar:"<<bar;
             int flag= bar.mid(0,1).toInt(nullptr,16);
             int barLen = bar.mid(1,1).toInt(nullptr,16);
             if(flag == 0 && barLen>10)
             {
                 qDebug()<<"标签写入成功";
                 displayEPCAndBar.remove(EPC);
                 removeTag(displayBar);
             }
             destroyTimes++;
         }

    }while(!displayEPCAndBar.isEmpty() || destroyTimes>10);
   if(!displayEPCAndBar.isEmpty())
   {
       //有商品没法进行成功的标志，可将此未标志的标签信息通知检测通道的天线。 今年
   }
   scanRfidTimer->start();  //重新开始读取rfid*/
 }

void MainWindow::test_AddNewRFID()
{
    scanRfidTimer->stop();
    //从rfidtobar.txt文件中读取到的RFID标签和所对应的条码
    QSettings settings("config/rfidTobar.txt",QSettings::IniFormat);
    QStringList  rfidList =   settings.allKeys();
    QMap<QString,QString> rfidBarFile;
    foreach (QString key, rfidList) {
        rfidBarFile.insert(key,settings.value(key).toString());
        //qDebug()<<key << "=" <<settings.value(key).toString();
    }

    int outAnt;
    QMap<QString,QString>EPCAndBar;
    QMap <QString ,QString >EPCAndBar_out;
    int ret =  rfid->query(&outAnt,&EPCAndBar);
    if(ret<0)return;
    if(outAnt == 1)
    {
        foreach (QString EPC, EPCAndBar_out.keys()) {
            qDebug()<<"扫描到的EPC"<<EPC;
            if(rfidList.contains(EPC))
            {
                int ret = rfid->writeTagData(EPC,rfidBarFile.value(EPC));
                if(ret!=0)rfid->writeTagData(EPC,rfidBarFile.value(EPC));
            }
        }
    }
    scanRfidTimer->start();
}

//出袋按钮
void MainWindow::on_bagOut_clicked()
{

}


//重新启动
void MainWindow::on_restart_clicked()
{
      test_AddNewRFID();
}


//付款
void MainWindow::on_sell_clicked()
{
    sell();


 }
