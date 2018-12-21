#ifndef GOODSDATABASE_H
#define GOODSDATABASE_H
#include <QString>
#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
/*
 * ����Ʒ�����ݿ��л�ȡ��Ʒ��Ϣ
 * �������ݿ��ļ��ڵ�ǰ�������е�Ŀ¼./goodsdata.db
 *
 */
struct goodsInfo{
    QString barcode;    //������
    QString name;       //��Ʒ����
    QString bincode;    //��Ʒ����
    QString goodsType;  //��Ʒ���
    QString goodsTypeBinCode;  //��Ʒ������
    double vipPrice;    //��Ա�۸�
    double salePrice;   //���ۼ۸�
};

class goodsDatabase
{
public:
    goodsDatabase();

    int query(QString bar,struct goodsInfo *info);//1.QString barΪ�����������,struct goodsInfo *infoΪ���ص���Ʒ��Ϣ
private:
    QSqlDatabase goodsData;
};

#endif // GOODSDATABASE_H
