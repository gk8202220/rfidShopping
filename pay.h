/*
 * 1.此为米花微信和支付宝服务器接口
 * 2.在config/config.ini下配置服务器的地址
 * 3.status状态码  data数据
 *   alipay       支付宝二维码链接
 *   weixin       微信二维码链接
 *   payStatus    0 支付失败 /1 支付成功
 *   error        错误信息
 *
 */
#ifndef PAY_H
#define PAY_H
#include <QtNetwork>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QObject>
/*
 *
 * totalAmount  价格
    subject       商品
    outTradeno    订单号
    storeId   商户门店编号
*/
class Pay:public QObject
{
    Q_OBJECT
public:
    Pay();
    void wxpay(QString order,QString amount,QString subject);//返回微信二维码链接
    void alipay(QString order,QString amount,QString subject);//返回支付宝二维码链接

private:
    QNetworkAccessManager *manager;
    QNetworkReply *wxReply;
    QNetworkReply *alipayReply;
    QString wxUrl;
    QString alipayUrl;
    QString queryUrl;
    QTimer *payQueryTime;
    QString order;
private slots:
    void On_wxQRcode();
    void On_alipayQRcode();
    void On_payStatus();
    void query();//根据订单号来查询
signals:
    void payStatus(QString status,QString data);

};

#endif // PAY_H
