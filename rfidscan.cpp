#include "rfidscan.h"
#include <QDebug>
#define stopTimer  10000
#define scanGoodsAnt 0x80 //ɨ����Ʒ������
#define scanTime 2 //ɨ��ļ��ʱ��
#define scanTimesTotal 3 // ɨ��Ĵ���
rfidScan::rfidScan(QStandardItemModel *tableModel)
{
    this->model = tableModel;
    goodsDB = new goodsDatabase();
    scanRfidTimer = new QTimer();
    connect(scanRfidTimer,SIGNAL(timeout()),this,SLOT(getRFIDData()));
    scanTimes = 0;

}

int rfidScan::start()
{
    if( rfid->isLoadLib())
    {
        int ret  =  rfid->RR_AutoOpenComPort();
        if(ret != 0)
        {
            //��RFID���ɹ�
         //this->close();
            return -1;
        }else
        {
            scanRfidTimer->start(scanTime);
            return 0;

        }
    }
    return -2; //����RFID��̬��ʧ�ܣ����·���Ƿ���ȷ
}


//ɨ���ǩ
void rfidScan::getRFIDData()
{
/*
 * ʹ��ʵʱɨ�跽ʽ
 */
QMap <QString ,QString >newScanEPCAndBarTemp;
int outAnt;
    if(scanTimes<scanTimesTotal)
    {
        int ret =  rfid->query(&outAnt,&newScanEPCAndBarTemp); //һ��ֻ���յ�4����ǩ�����ݡ���������ݻ�ֱ��͹�����
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
            //����ͨ����������������Ƿ���
            if(!newScanEPCAndBar.isEmpty())
            {
                //���߼�⵽��û�й������Ʒ�����б���
                qDebug()<<"***********����*****************";
                foreach(QString bar,newScanEPCAndBar.values())qDebug()<<bar;
                qDebug()<<"***********����*****************";
            }

        }//end else scanTimes
    }else
    {
        scanTimes = 0;

        // qDebug()<< "�µ�ɨ���ǩ " <<newScanEPCAndBar.keys() <<newScanEPCAndBar.values();
        //qDebug()<< "��ʾ�ı�ǩ " <<displayEPCAndBar.keys() <<displayEPCAndBar.values();

        //��ȡ������1�ı�ǩ(����̨������)
        //  qDebug()<< "��ǩ���� "<< EPCAndBar.count();
        //qDebug()<<"Ŀǰ��ʾ����Ʒ " << dispalyBar.count() <<dispalyBar.keys()<<dispalyBar.values();

        if(displayEPCAndBar.isEmpty())
        {
            //��ʱ��ʾ�б�Ϊ�գ�����ʾ���еõ��ı�ǩ
            foreach (QString bar, newScanEPCAndBar.values()) {
                // qDebug()<< "ȫ�µı�ǩ " <<bar;
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

            //2.�����ɨ�赽�ı�ǩ����ʾ�����ϵ��б��д����򲻴���
            //3. ��ɨ�赽�ı�ǩ���� -- ��ʾ�����ϵı�ǩ�в����� > �����
            foreach (QString newEPC, newScanEPCAndBar.keys()) {
                if(!displayEPCAndBar.contains(newEPC))
                {
                    //  qDebug()<<"����ӵ���Ʒ "<< newScanEPCAndBar.value(newEPC);
                   displayEPCAndBar.insert(newEPC,newScanEPCAndBar.value(newEPC));
                   displayInfo(newScanEPCAndBar.value(newEPC));
                }
            }

        }
        newScanEPCAndBar.clear();
        emit show();
    }
}

void rfidScan::displayInfo( QString bar)
{
    if(bar.isEmpty())return;
    if(model == NULL )
    {
        qDebug()<<"�봫��tableView Model";
        return;
    }
    int rowCount = model->rowCount();
        for(int i = 0;i <= rowCount;i++)//ѭ����ѯ����ѯ�Ƿ�����ͬ����Ʒ���
        {
          QString data_string = model->index(i,BIN_CODE).data().toString();
            if(data_string ==  bar)//�������ͬ�ı��
            {
                //�Ѿ�����ͬ���Ƶĵ���Ʒ�������ϼ���1
                int goodsCount  = model->index(i,AMOUNT).data().toInt();
                float TotalPrice  = model->index(i,TOTAL_PRICE).data().toFloat();
                float salePrice  = model->index(i,SALE_PRICE).data().toFloat();
                TotalPrice = salePrice*goodsCount;
                model->setItem(i,AMOUNT,new QStandardItem(QString::number(goodsCount+1))); //����+1
                model->setItem(i,TOTAL_PRICE,new QStandardItem(QString::number(TotalPrice,'f',2))); //�ı��Ӧ���ܼ�
                 return;
            }
        }
       struct goodsInfo info;
       goodsDB->query(bar,&info);//�����ݿ��ȡ��Ʒ��Ϣ
       if(info.barcode.isEmpty())
       {
          // QMessageBox::warning(this,QString("��ʾ"),tr("�޴���Ʒ��������ɨ��"),QMessageBox::NoButton,QMessageBox::NoButton);
           return ;
       }

        model->setItem(rowCount,BIN_CODE,new QStandardItem(bar));
        model->setItem(rowCount,GOODS_NAME,new QStandardItem(info.name));
        model->setItem(rowCount,SALE_PRICE,new QStandardItem(QString::number(info.salePrice,'f',2)));
        model->setItem(rowCount,VIP_PRICE,new QStandardItem(QString::number(info.vipPrice,'f',2)));
        model->setItem(rowCount,AMOUNT,new QStandardItem(QString::number(1)));
        model->setItem(rowCount,TOTAL_PRICE,new QStandardItem(QString::number(info.salePrice,'f',2)));
        //model->setItem(rowCount,GOODS_TYPE,new QStandardItem(info.goodsType));

 }
void rfidScan::removeTag(QString code)
{
  for(int i=0;i<model->rowCount();i++)
    {
      if (model->data(model->index(i,0)).toString() == code)
      {
          int goodsCount = model->index(i,AMOUNT).data().toInt();
          if(goodsCount>1)
              model->setItem(i,AMOUNT,new QStandardItem(QString::number(goodsCount-1))); //����-1
          else
          model->removeRow(i);
      }
  }

}

//�ͻ��Ѿ������֮��Ա�ǩ���з������
void rfidScan::sell()
{
    scanRfidTimer->stop();    //ֹͣ��ȡrfid
   qDebug()<<"��Ҫ�۳������б�ǩ"<<displayEPCAndBar.keys();
   int destroyTimes = 0;//���б�־ʧ�ܵĴ���
    do
    {
         foreach (QString EPC, displayEPCAndBar.keys()) {
             QString displayBar =displayEPCAndBar.value(EPC);
             int ret =  rfid->TagSell(EPC,displayBar);
             if(ret != 0)ret = rfid->TagSell(EPC,displayBar);
             QString bar;
             rfid->readTagData(EPC,&bar);
//              qDebug()<<"����Ҫ�۳��ı�ǩ"<<EPC<<displayBar;
//              qDebug()<<"����д����ѯ�Ľ��EPC"<<EPC<<"bar:"<<bar;
             int flag= bar.mid(0,1).toInt(nullptr,16);
             int barLen = bar.mid(1,1).toInt(nullptr,16);
             if(flag == 0 && barLen>10)
             {
                 qDebug()<<"��ǩд��ɹ�";
                 displayEPCAndBar.remove(EPC);
                 removeTag(displayBar);
             }
             destroyTimes++;
         }

    }while(!displayEPCAndBar.isEmpty() || destroyTimes>10);
   if(!displayEPCAndBar.isEmpty())
   {
       //����Ʒû�����гɹ��ı�־���ɽ���δ��־�ı�ǩ��Ϣ֪ͨ���ͨ�������ߡ�
   }
   scanRfidTimer->start();  //���¿�ʼ��ȡrfid
}

void rfidScan::test_AddNewRFID()
{
    scanRfidTimer->stop();
    //��rfidtobar.txt�ļ��ж�ȡ����RFID��ǩ������Ӧ������
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
            qDebug()<<"ɨ�赽��EPC"<<EPC;
            if(rfidList.contains(EPC))
            {
                int ret = rfid->writeTagData(EPC,rfidBarFile.value(EPC));
                if(ret!=0)rfid->writeTagData(EPC,rfidBarFile.value(EPC));
            }

        }
    }
    scanRfidTimer->start();
}

