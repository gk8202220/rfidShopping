#ifndef GOODSDATABASE_H
#define GOODSDATABASE_H
#include <QString>
#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
/*
 * 从商品的数据库中获取商品信息
 * 本地数据库文件在当前程序运行的目录./goodsdata.db
 *
 */
struct goodsInfo{
    QString barcode  //条形码
    QString name;       //商品名称
    QString bincode;    //商品编码
    QString goodsType;  //商品类别
    QString goodsTypeBinCode;  //商品类别编码
    double vipPrice;    //会员价格
    double salePrice;   //销售价格
};

class goodsDatabase
{
public:
    goodsDatabase();

    int query(QString bar,struct goodsInfo *info);//1.QString bar为传入的条形码,struct goodsInfo *info为返回的商品信息
private:
    QSqlDatabase goodsData;
};

#endif // GOODSDATABASE_H
