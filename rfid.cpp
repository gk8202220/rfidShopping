#include "rfid.h"
#define stopTimer  10000
#define scanGoodsAnt 0x80 //扫描商品的天线
#define scanTime 2 //扫描的间隔时间
#define scanTimesTotal 3 // 扫描的次数
#include <QDebug>

Rfid::Rfid(QStandardItemModel *ItemModel)
{
    this->model =  ItemModel;
    goodsDB = new goodsDatabase();
    scanRfidTimer = new QTimer();
    connect(scanRfidTimer,SIGNAL(timeout()),this,SLOT(getRFIDData()));
    scanTimes = 0;
}

int Rfid::start()
{
    if( rfid->isLoadLib())
    {
        int ret  =  rfid->RR_AutoOpenComPort();
        if(ret != 0)return -1;
        else
        {
            qDebug() << "已连接RFID";
            scanRfidTimer->start(scanTime);
            return 0;
        }
    }
    return -2; //加载RFID动态库失败，检查路径是否正确
}

void Rfid::getRFIDData()
{/*
 * 使用实时扫描方式
 */
    QMap <QString ,QString >newScanEPCAndBarTemp;
    QMap <QString ,QString >newScanEPCAndBar_out;
    int outAnt;
    if(scanTimes<scanTimesTotal)
    {
        int ret = rfid->query(&outAnt,&newScanEPCAndBarTemp,&newScanEPCAndBar_out); //一次只能收到4个标签的数据。过多的数据会分别发送过来。
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
        //qDebug()<< "新的扫描标签 " <<newScanEPCAndBar.keys() <<newScanEPCAndBar.values();
        //qDebug()<< "显示的标签 " <<displayEPCAndBar.keys() <<displayEPCAndBar.values();
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
        // showPrice();
        emit show();
        newScanEPCAndBar.clear();
    }
}

void Rfid::displayInfo(QString bar)
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
            model->setItem(i,AMOUNT,new QStandardItem(QString::number(goodsCount+1))); //数量+1
            model->setItem(i,TOTAL_PRICE,new QStandardItem(QString::number(TotalPrice,'f',2))); //改变对应的总计
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
    model->setItem(rowCount,BIN_CODE,new QStandardItem(bar));
    model->setItem(rowCount,GOODS_NAME,new QStandardItem(info.name));
    model->setItem(rowCount,SALE_PRICE,new QStandardItem(QString::number(info.salePrice,'f',2)));
    model->setItem(rowCount,VIP_PRICE,new QStandardItem(QString::number(info.vipPrice,'f',2)));
    model->setItem(rowCount,AMOUNT,new QStandardItem(QString::number(1)));
    model->setItem(rowCount,TOTAL_PRICE,new QStandardItem(QString::number(info.salePrice,'f',2)));


}
void Rfid::sell()
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
void Rfid::test_AddNewRFID()
{
    scanRfidTimer->stop();
    //从rfidtobar.txt文件中读取到的RFID标签和所对应的条码
    QSettings settings("config/rfidTobar.txt",QSettings::IniFormat);
    QStringList  rfidList =   settings.allKeys();
    QMap<QString,QString> rfidBarFile;
    foreach (QString key, rfidList) {
        rfidBarFile.insert(key,settings.value(key).toString());
        qDebug()<<key << "=" <<settings.value(key).toString();
    }

    int outAnt;
    QMap<QString,QString>EPCAndBar;
    QMap <QString ,QString >EPCAndBar_out;
    int ret =  rfid->query(&outAnt,&EPCAndBar,&EPCAndBar_out);
    if(ret<0)return;
    if(outAnt == 1)
    {
        foreach (QString EPC, EPCAndBar_out.keys()) {
            //qDebug()<<"扫描到的EPC"<<EPC;
            if(rfidList.contains(EPC))
            {
                int ret = rfid->writeTagData(EPC,rfidBarFile.value(EPC));
                if(ret!=0)rfid->writeTagData(EPC,rfidBarFile.value(EPC));
            }
        }
    }
    scanRfidTimer->start();
}
void Rfid::removeTag(QString code)
{
    for(int i=0;i<model->rowCount();i++)
    {
        if (model->data(model->index(i,0)).toString() == code)
        {
            int goodsCount = model->index(i,AMOUNT).data().toInt();
            if(goodsCount>1)model->setItem(i,AMOUNT,new QStandardItem( QString::number(goodsCount-1))); //数量-1
            else model->removeRow(i);
        }
    }
}
