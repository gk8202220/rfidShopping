#include "pay.h"
#include <QSettings>
#define configFile "config/config.ini"
Pay::Pay()
{
    manager = new QNetworkAccessManager(this);
    QSettings config(configFile,QSettings::IniFormat);
    wxUrl = config.value("MiHuaPay/wxUrl").toString();
    alipayUrl = config.value("MiHuaPay/alipayUrl").toString();
    queryUrl = config.value("MiHuaPay/queryUrl").toString();
}
void Pay::wxpay(QString order,QString amount,QString subject)
{
    //QString weixinurl = "http://liukaihong14.e1.luyouxia.net:28834/shop/WXpay";
    if(wxUrl.isEmpty())
    {
        emit payStatus("error","微信服务器地址没有配置");
        return;
    }
    QNetworkRequest request;
    request.setUrl(QUrl(wxUrl));
    QByteArray postData;
    postData.append("orderNum=");
    postData.append(order);
    postData.append("&subject=");
    postData.append(subject);
    postData.append("&total_fee=");
    postData.append(amount);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    wxReply = manager->post(request,postData);
    connect(wxReply, SIGNAL(readyRead()), this, SLOT(On_wxQRcode()));
}

void Pay::alipay(QString order,QString amount,QString subject)
{
    // QString weixinurl = "http://192.168.6.222:28834/shop/confirmAsllPayWeb/pay";
    // QString weixinurl = "http://liukaihong14.e1.luyouxia.net:28834/shop/confirmAsllPayWeb/pay";
    if(alipayUrl.isEmpty())
    {
        emit payStatus("error","支付宝服务器地址没有配置");
        return;
    }
    QNetworkRequest request;
    request.setUrl(QUrl(alipayUrl));
    QByteArray postData;
    postData.append("outTradeno=");
    postData.append(order);
    postData.append("&subject=");
    postData.append(subject);
    postData.append("&totalAmount=");
    postData.append(amount);
    postData.append("&storeId=");
    postData.append("0001");
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    alipayReply = manager->post(request,postData);
    connect(alipayReply, SIGNAL(readyRead()), this, SLOT(On_alipayQRcode()));
}

void Pay::query(QString order)
{
    // QString weixinurl = "http://liukaihong14.e1.luyouxia.net:28834/shop/selectByOrderNum";

    if(alipayUrl.isEmpty())
    {
        emit payStatus("error","返回结果服务器地址没有配置");
        return;
    }
    QNetworkRequest request;
    QByteArray postData;
    request.setUrl(QUrl(queryUrl));
    postData.append("orderNum=");
    postData.append(order);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    wxReply = manager->post(request,postData);
    connect(wxReply, SIGNAL(readyRead()), this, SLOT(On_payStatus()));
}

void Pay::On_alipayQRcode()
{
    QByteArray data = alipayReply->readAll();
    if(!data.isNull() || !data.isEmpty() )
    {
        if(data.contains("alipay"))  emit payStatus("alipayQRCode",QString(data.mid(1,data.length()-2)));
        else emit payStatus("error","服务器异常，获取支付宝二维码失败");
    }

}
void Pay::On_wxQRcode()
{
    QByteArray data = wxReply->readAll();
    if(!data.isNull() || !data.isEmpty())
    {
        if(data.contains("weixin"))  emit payStatus("weixinQRCode",QString(data.mid(1,data.length()-2)));
      //  else emit payStatus("error","服务器异常，获取微信二维码失败");
    }
}
void Pay::On_payStatus()
{
    QByteArray data = wxReply->readAll();
    //qDebug()<<data;
    if(!data.isEmpty())
    {
        QString ret(data.mid(1,data.length()-2));
        if(ret == "1") emit payStatus("payStatus","支付成功");
        else if(ret  == "0" ) emit payStatus("payStatus","支付失败");
        else emit payStatus("error","服务器异常，查询订单失败");
    }

}
