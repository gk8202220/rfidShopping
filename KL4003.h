#ifndef KL4003_H
#define KL4003_H
#include <windows.h>
#include <QMap>
typedef int (*FUN1)(int,BYTE*,BYTE,int* );
typedef int (*FUN2)(BYTE*,BYTE*,BYTE*,BYTE*,BYTE*,BYTE*,BYTE*,BYTE*,BYTE*,BYTE*,BYTE*,BYTE*,int);
typedef int(*FUN3)(BYTE*,int *,int);
typedef int(*FUN4)(BYTE*,BYTE,int);
typedef int	(*FUN5)(BYTE*,BYTE,BYTE,BYTE,BYTE*,BYTE,BYTE*,BYTE,BYTE,BYTE,BYTE,BYTE,BYTE,BYTE,BYTE,BYTE*,BYTE*,int *,int *,int);
typedef int	(*FUN6)(BYTE*,BYTE,BYTE,BYTE,BYTE*,BYTE,BYTE*,BYTE,BYTE,BYTE*,
                    BYTE,BYTE*,BYTE,BYTE,BYTE,BYTE,BYTE*,BYTE*,int *,int *,int);
typedef int (*FUN7)(int);
typedef int (*FUN8)(BYTE* ,BYTE , int );
typedef int (*FUN9)(BYTE*, BYTE*, BYTE, BYTE, BYTE, BYTE, BYTE*, BYTE, BYTE*, BYTE, BYTE*, BYTE*, int*, int);
typedef int (*FUN10)(BYTE *,BYTE*, BYTE, BYTE, BYTE, BYTE,BYTE*,BYTE*,BYTE,BYTE*,BYTE,BYTE*,int*,int);
typedef int (*FUN11)(int* , BYTE* , BYTE , int* );
typedef int	(*FUN12)(BYTE * ,BYTE ,BYTE ,BYTE ,BYTE *,BYTE ,BYTE *,BYTE ,BYTE ,BYTE ,BYTE ,BYTE ,BYTE ,BYTE ,BYTE  ,int * ,int *,int );
typedef int	(*FUN13)(BYTE *, int *, int *, BYTE *, int );
typedef int	(*FUN14)(BYTE *, int );
#include <QObject>
class KL4003:public QObject
{
    Q_OBJECT

public:
    KL4003();
    static KL4003 *Instance()
    {
        if (!_instance) {
            _instance = new KL4003;
        }
        return _instance;
    }
    int RR_OpenComPort();
    int RR_AutoOpenComPort();
    int  query(int *ReoutAnt,QMap <QString,QString> *EPCAndBar,QMap <QString,QString> *EPCAndBar_out);
    BYTE ComAdr;
    BYTE Ant;
    int FrmHandle;
    bool EnbleFlag = false;
    byte * HexStringToByteArray(QString src);
    bool lodeLib();
    bool isLoadLib();
    QString ByteArrayToHexString(byte* srcData);
    int readTagData(QString EPC,QString *re_Bar);
    int writeTagData(QString CurrentEPC, QString WriteDataString);
    int TagSell(QString CurrentEPC,QString bar);//将标签进行售出标志
    int Buffquery();//缓冲查询模式
    int readBuff();
    int clearbuff();
    QMap<QString,QString> tagBars;
    bool enbleScan;

private:

    static KL4003 *_instance;
    FUN1 OpenComPort;//打开端口
    FUN2 GetReaderInformation;//获取阅读器参商信息
    FUN3 GetBufferCnt_G2;
    FUN4 SetAntennaMultiplexing;//配置天线
    FUN5 Inventory_G2;//实时查询
    FUN6 InventoryMix_G2;//混合查询
    FUN7 CloseSpecComPort;//关闭端口
    FUN8 SetBeepNotification; //设置蜂鸣器
    FUN9 ReadData_G2;	 //读数据
    FUN10 WriteData_G2;
    FUN11 AutoOpenComPort;//
    FUN12 InventoryBuffer_G2;//缓存询查命令
    FUN13 ReadBuffer_G2; //读取缓冲数据
    FUN14 ClearBuffer_G2;//清空缓存 
    bool islodeLib;
};

#endif // KL4003_H
