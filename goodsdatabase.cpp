#include "goodsdatabase.h"

goodsDatabase::goodsDatabase()
{
  //打开数据库
  goodsData = QSqlDatabase::addDatabase("QSQLITE", "sqlite");
  goodsData.setDatabaseName("./goodsdata.db");
  if(!goodsData.open())qDebug()<<"error!";

}

int goodsDatabase::query(QString bar,struct goodsInfo *info)
{
  //sqlite> SELECT * FROM COMPANY WHERE NAME LIKE 'Ki%';
  if(!goodsData.isOpen())return -1;

  QSqlQuery sql_query_goodsMesg(goodsData);  //以条形码从数据库中获取商品信息
  QSqlQuery sql_query_goodsPrice(goodsData); //以商品编码从数据库中商品价格
  QSqlQuery sql_query_goodsType(goodsData); //以商品编码从数据库中商品类别
  QString goodsbar    = "select * from Goods where barcode = :bar";
  QString goodsPrice  = "select * from Price where bm = :bm";
  QString goodsType   = "select * from Type where bm = :bm_fl";
  sql_query_goodsMesg.prepare(goodsbar);
  sql_query_goodsPrice.prepare(goodsPrice);
  sql_query_goodsType.prepare(goodsType);
  sql_query_goodsMesg.bindValue(":bar",bar);
  if(!sql_query_goodsMesg.exec())
  {
      qDebug() << sql_query_goodsMesg.lastError();
      return -2;

  }
  else
  {
      while(sql_query_goodsMesg.next())
      {
          info->bincode =  sql_query_goodsMesg.value("bm").toString();
          info->barcode =  sql_query_goodsMesg.value("barcode").toString();
          info->name    =  sql_query_goodsMesg.value("mc").toString();
          info->goodsTypeBinCode = sql_query_goodsMesg.value("bm_fl").toString();

          //以商品编码从数据库中商品价格
          if(!info->bincode.isEmpty() || !info->bincode.isNull())
          {
              sql_query_goodsPrice.bindValue(":bm",info->bincode);
              if(!sql_query_goodsPrice.exec())
              {
                  qDebug() << sql_query_goodsMesg.lastError();
              }
              else
              {
                  while(sql_query_goodsPrice.next())
                  {
                      info->vipPrice = sql_query_goodsPrice.value("dj_hy").toDouble();
                      info->salePrice = sql_query_goodsPrice.value("dj_ls").toDouble();
                  }
               }
          }
          //以商品类别编码从数据库中商品类别名称
          if(!info->goodsTypeBinCode.isEmpty() || !info->goodsTypeBinCode.isNull())
          {
              sql_query_goodsType.bindValue(":bm_fl",info->goodsTypeBinCode);
              if(!sql_query_goodsType.exec())
              {

                  qDebug() << sql_query_goodsType.lastError();
              }
              else
              {
                  while(sql_query_goodsType.next())
                  {
                     info->goodsType = sql_query_goodsType.value("mc").toString();
                  }
               }
          }
      /*   qDebug() <<QString::fromLocal8Bit(" 商品编码 ：")<< info->bincode<<QString::fromLocal8Bit(" 条形码 ：") <<info->barcode<<QString::fromLocal8Bit(" 商品名称 ：" )<<info->name\
               <<QString::fromLocal8Bit( "会员价格:") << QString::number(info->vipPrice )<< QString::fromLocal8Bit("销售价格: ") << QString::number( info->salePrice)\
              <<QString::fromLocal8Bit("商品类别:" )<< info->goodsType;*/
      }
  }
  return 0;
}

