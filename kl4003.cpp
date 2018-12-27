#include "kl4003.h"
#define libPath "KL4003_32.dll"
#include <QLibrary>
#include <QDebug>
#include <QThread>
KL4003 *KL4003::_instance = 0;
KL4003::KL4003()
{
 islodeLib = lodeLib();
 ComAdr = 1;
FrmHandle = -1;//最大询查时间
enbleScan = true;
}
bool KL4003::lodeLib()
{
    QLibrary lib(libPath); //加载动态库
    if (lib.load()) {
        qDebug() << "load "<<libPath<< " ok!";

        OpenComPort = (FUN1)lib.resolve("OpenComPort"); //加载动态库的函数
        if (!OpenComPort)   qDebug() << "load FUN(OpenComPort) error!";

        GetReaderInformation = (FUN2)lib.resolve("GetReaderInformation"); //加载动态库的函数
        if (!GetReaderInformation)qDebug() << "load FUN(GetReaderInformation) error!";

        GetBufferCnt_G2 = (FUN3)lib.resolve("GetBufferCnt_G2"); //加载动态库的函数
        if (!GetBufferCnt_G2) qDebug() << "load FUN(GetBufferCnt_G2) error!";

        SetAntennaMultiplexing = (FUN4)lib.resolve("SetAntennaMultiplexing"); //加载动态库的函数
        if (!SetAntennaMultiplexing)qDebug() << "load FUN(SetAntennaMultiplexing) error!";

        Inventory_G2 = (FUN5)lib.resolve("Inventory_G2"); //加载动态库的函数
        if (!Inventory_G2)qDebug() << "load FUN(Inventory_G2) error!";

        InventoryMix_G2 = (FUN6)lib.resolve("InventoryMix_G2"); //加载动态库的函数
        if (!InventoryMix_G2)qDebug() << "load FUN(InventoryMix_G2) error!";

        CloseSpecComPort = (FUN7)lib.resolve("CloseSpecComPort"); //加载动态库的函数
        if (!CloseSpecComPort)qDebug() << "load FUN(CloseSpecComPort) error!";

        SetBeepNotification = (FUN8)lib.resolve("SetBeepNotification"); //加载动态库的函数
        if (!SetBeepNotification)qDebug() << "load FUN(SetBeepNotification) error!";

        ReadData_G2 = (FUN9)lib.resolve("ReadData_G2"); //加载动态库的函数
        if (!ReadData_G2) qDebug() << "load FUN(ReadData_G2) error!";

        WriteData_G2 = (FUN10)lib.resolve("WriteData_G2"); //加载动态库的函数
        if (!WriteData_G2)qDebug() << "load FUN(WriteData_G2) error!";

        AutoOpenComPort = (FUN11)lib.resolve("AutoOpenComPort"); //加载动态库的函数
        if (!AutoOpenComPort)qDebug() << "load FUN(AutoOpenComPort) error!";

        InventoryBuffer_G2 = (FUN12)lib.resolve("InventoryBuffer_G2"); //加载动态库的函数
        if (!InventoryBuffer_G2)qDebug() << "load FUN(InventoryBuffer_G2) error!";

        ReadBuffer_G2 = (FUN13)lib.resolve("ReadBuffer_G2"); //加载动态库的函数
        if (!ReadBuffer_G2)qDebug() << "load FUN(ReadBuffer_G2) error!";

        ClearBuffer_G2 = (FUN14)lib.resolve("ClearBuffer_G2"); //加载动态库的函数
        if (!ClearBuffer_G2)qDebug() << "load FUN(ClearBuffer_G2) error!";
        return true;

    }else
    {
        qDebug() << "load "<<libPath<<"  error!";
        return false;
    }
}

bool KL4003::isLoadLib()
{
    return islodeLib;
}
int KL4003::RR_OpenComPort()
{
    int port = 3;
    ComAdr = 255;
    BYTE baud = 5;
    FrmHandle = 0;//最大询查时间          
    int ret = OpenComPort(port,&ComAdr,baud,&FrmHandle);
    qDebug() << "ret = " <<ret<<"ComAdr"<<ComAdr;

    return ret;
}

int KL4003::RR_AutoOpenComPort()
{
    int port = 0;
    ComAdr = 255;
    BYTE baud = 5;
    FrmHandle = 0;//最大询查时间
    int ret = AutoOpenComPort(&port, &ComAdr,  baud, &FrmHandle);
    qDebug() << "ret = " <<ret<<"ComAdr"<<ComAdr;
    return ret;
}


int KL4003::query(int *ReoutAnt,QMap <QString,QString> *EPCAndBar,QMap <QString,QString> *EPCAndBar_out)
{
    QString EPC, tagData;
    int CardNum = 0;
    int Totallen = 0;
    byte MaskMem = 0;
    byte MaskAdr[2] = {0};
    byte MaskLen = 0;
    byte MaskData[100] = {0};
    byte MaskFlag = 0;
    BYTE Target=0;
    byte Scantime=200;
    byte FastFlag=1;//0 – 不启用，不需要要指定Target,Inant,Scantime 3个参数，默认0即可;1 – 启用，此时要指定Target,Inant,Scantime 3个参数。
    int InAnt=0x80;
    BYTE Session= 0;
    byte QValue = 4;
    byte  ReadMem = 3; //1个字节，掩码区。1 – EPC存储区；2 – TID存储区；3 –用户存储区。
    byte ReadAdr[2] ={0x00,0x00};
    byte ReadLen = 4;
    byte Psd[4] = {0x00,0x00,0x00,0x00};
    byte EPCList[20000];
    byte OutAnt; //返回天线号。值1 2 4 8 分别对应 1 2 3 4号天线
   int ret =InventoryMix_G2(&ComAdr, QValue, Session, MaskMem,MaskAdr, MaskLen,MaskData,
                              MaskFlag, ReadMem,ReadAdr, ReadLen, Psd, Target,
                              InAnt, Scantime, FastFlag,EPCList,&OutAnt,&Totallen,&CardNum,FrmHandle);
    *ReoutAnt = OutAnt;

// qDebug() << "fcmdRet"<<ret  <<"InAnt"<<InAnt << "OutAnt "  <<OutAnt <<"CardNum"<<CardNum <<
//         "Psd" <<Psd  << "ReadAdr" <<ReadAdr;//000c300833b2ddd901400000  8108000000000000
  //
    if(CardNum>0 )
    {
     //   qDebug()<< "天线"<<*ReoutAnt;
        int m=0;
        int EPClen=0;  //EPC长度
        int rollCode =0; //滚码
        //byte RSSI; //信号强度
        for (int CardIndex = 0;CardIndex<CardNum;CardIndex++)
        {


            QByteArray data;
            byte EPC1[250];
            rollCode = EPCList[m];
            EPClen = EPCList[m +1];
            memcpy(EPC1,&EPCList[m+2], EPClen);
            //RSSI = EPCList[m+2+EPClen];
            m +=  EPClen + 3;
            //得到标签的EPC号
            for(int n=0;n<EPClen;n++)
            {
                data.append(EPC1[n]);
            }

           //int rollCode_low =rollCode&0x7f;
           int rollCode_high = rollCode>>7;//最高位0为EPC 1为数据
//           qDebug()<<"滚码";
//           qDebug()<<rollCode;
//          qDebug()<<"最高位：";
//           qDebug()<<rollCode_high;
//           qDebug()<<"低七位:";
//           qDebug()<<rollCode_low;
            if(rollCode_high==1)
            {
                tagData=data.toHex().data();

                /*
                 * 重点说明：
                 * 读取到的标签的数据（user区 地址0 长度16）
                 * 第1位表示的是是否已经被购买 1为未购买 。第2位表示条码的长度。
                 */
                //读取条码成功才添加进入列表
                int flag    = tagData.mid(0,1).toInt(nullptr,16);
                int codeLen = tagData.mid(1,1).toInt(nullptr,16);
                   //通道1是就进行收银台结算的
                    if(flag == 0 && codeLen >10)
                    {
                        QString code = tagData.mid(2,codeLen);
                        EPCAndBar_out->insert(EPC,code);  //已经售出的标签
                    }else if(flag == 1 && codeLen >0)
                    {
                        QString code = tagData.mid(2,codeLen);
                        EPCAndBar->insert(EPC,code);
                    }
               
              //(OutAnt == 1)
//                emit getRfidTag(EPC,tagData);
               // updataList(EPC,tagData,OutAnt);
            }
            else
            {
                EPC =data.toHex().data();
            }


       }
        //qDebug()<< "标签的数量" << tagBars.count();

       // tagBars.clear();
    }

    return ret;
}

byte *KL4003::HexStringToByteArray(QString src)
{
     byte *buffer = new byte[src.length()/ 2];
    for(int i=0;i<src.length();i +=2)
    {
       buffer[i/2]  = src.mid(i,2).toInt(nullptr,16);
         //  qDebug("%x",buffer[i/2]);
    }
    return buffer;
}


QString KL4003::ByteArrayToHexString(byte *srcData)
{

    QByteArray data;
    for(int i = 0;i<sizeof(srcData)*2;i++)
    {
        data.append(srcData[i]);
    }
    return data.toHex().data();
}


int KL4003::readTagData(QString EPC,QString *re_Bar)
{
    BYTE ENum;//EPC长度
    BYTE Mem; //读取区域
    BYTE WordPtr;//起始地址
    BYTE Num; //读取长度
    byte *Password  = new byte[4];
    BYTE MaskLen = 0;
    BYTE MaskData[100] = {0};
    BYTE MaskMem = 1;
    BYTE MaskAdr[2] = {0};
    int Errorcode;
    //memset(EPC,0,sizeof(EPC));
    QString currenyTag = EPC;
    // QString currenyTag = "300833B2DDD9014000000000";
    ENum = currenyTag.length()/4;
    byte *currenyEPC = new byte[ENum*2];
    currenyEPC = HexStringToByteArray(currenyTag);
    Mem = 3; //选择要读取的存储区。0x00：保留区；0x01：EPC存储区；0x02：TID存储区；0x03：用户存储区
    WordPtr = 0;//指定要读取的字起始地址0
    Num = 4;  //要读取的字的个数4。
    BYTE Data[200];
    QString dataPsd = "00000000";
    if(dataPsd.length()!=8)
    {
       // QMessageBox::warning(this,"密码","请正确输入8位数！","确定");
        return  -1;
    }

    Password= HexStringToByteArray(dataPsd);
    QByteArray returnData;
    memset(Data,0,sizeof(Data));
    int ret = ReadData_G2(&ComAdr,currenyEPC,ENum,Mem,WordPtr,Num,Password,MaskMem,MaskAdr,MaskLen,MaskData,Data,&Errorcode,FrmHandle);
    //qDebug()<<"ret"<<ret << "rfid->ComAdr" <<ComAdr <<"ENum" <<ENum<<"Mem" <<Mem <<"Num"<< Num <<"WordPtr" <<WordPtr<<"Errorcode"<<Errorcode;
    for(int i=0;i<Num*2 ;i++)returnData.append(Data[i]);
    re_Bar->append(returnData.toHex());

    return ret;
}


int KL4003::writeTagData(QString CurrentEPC, QString WriteDataString)
{
    BYTE Enum = CurrentEPC.length()/4;
    byte *EPC = new byte[Enum*2];
    EPC = HexStringToByteArray(CurrentEPC);
    BYTE Wnum = 4;
    BYTE Mem = 3;
    BYTE  WordPtr = 0;
    BYTE*Writedata = new BYTE[WriteDataString.length() /2];
    Writedata = HexStringToByteArray(WriteDataString);
    QString Psddata = "00000000";
    BYTE MaskLen = 0;
    BYTE MaskData[100] = {0};
    BYTE MaskMem = 1;
    BYTE MaskAdr[2] = {0};
     int Errorcode;
    if(Psddata.length()!=8)
    {
        return -1;
    }
    byte *Password  = new byte[4];
    Password= HexStringToByteArray(Psddata);
    int ret =  WriteData_G2(&ComAdr, EPC,  Wnum,  Enum,  Mem,  WordPtr,Writedata, Password,  MaskMem,MaskAdr, MaskLen,MaskData,&Errorcode,FrmHandle);
   // qDebug()<<"ret"<<ret << "rfid->ComAdr" <<ComAdr <<"ENum" <<Enum<<"Wnum" <<Wnum <<"WordPtr" <<WordPtr<<"Errorcode"<<Errorcode;
   // qDebug()<<CurrentEPC;

//  if(ret == 0)qDebug()<<"售出标志写入成功！";
//  else qDebug()<<"售出标志写入失败!";
  return ret;
}

int KL4003::TagSell(QString CurrentEPC,QString bar)
{

    QString sellFlag;
    //将条码的首位置0
    sellFlag.append('0');
    sellFlag.append(QString::number( bar.length(),16));
    sellFlag.append(bar);
   // qDebug()<<"将要进行售出的标签"<<sellFlag;
    return writeTagData(CurrentEPC,sellFlag);
}


int KL4003::Buffquery()
{
    int CardNum = 0; //本次询查中读取标签的次数，不区分是否多次读取同一张标签
    byte MaskMem = 0;
    byte MaskAdr[2] = {0};
    byte MaskLen = 0;
    byte MaskData[100] = {0};
    byte MaskFlag = 0;
    byte Scantime=0;
    byte FastFlag=0;
    byte InAnt=0x01;
    byte Session= 0;
    byte QValue = 4;
    byte AdrTID = 0;
    byte LenTID = 0;//询查TID区的数据字数范围0~15。
    byte TIDFlag = 0;//TID询查标志位。0 – 读EPC 1 – 读TID
    byte Target = 0;//查询标签的Target值。 0 – Target  1 – Target B。
    int BufferCount; //缓存中记录的标签总数，相同 EPC/TID数据的标签将被视为同一张标签。若未清空缓存，标签数量为多次询查操作的数量累加。
    int ret =InventoryBuffer_G2( &ComAdr,  QValue,  Session,  MaskMem,   MaskAdr,  MaskLen,   MaskData,  MaskFlag,  AdrTID,
                             LenTID,  TIDFlag,  Target,  InAnt,  Scantime,  FastFlag , &BufferCount,&CardNum,FrmHandle);
    if(FrmHandle == 0x01)
    qDebug()  <<"本次读取的标签数:"<<CardNum;
    else if(FrmHandle == 0xF8)qDebug()<<"天线连接检测错误";
    return ret;
}


int KL4003::readBuff()
{
   int Totallen;//读取到的pEPCList的长度
   int CardNum = 0;//读取到的标签的张数
   byte pEPCList[4096];//Totallen个字节，读取到的标签数据，由多个标签组成，每个标签的格式为： ANT+LEN+EPC/TID+RSSI+Count;
   int ret  =  ReadBuffer_G2(&ComAdr, &Totallen, &CardNum,pEPCList, FrmHandle);
   qDebug ()<< "读取到的pEPCList的长度:" << Totallen << "读取到的标签的张数:" <<CardNum;
   if(Totallen>0)
   {
        int i = 0;
        for(int num = 0;num<CardNum;num++)
        {
            QByteArray data;
            byte EPC[256];
            int EPCLen = 16;
           // rollCode = EPCList[m];
           // EPClen = EPCList[i +1];
            memcpy(EPC,&pEPCList[i], EPCLen);
            i  +=EPCLen;
            for (int n = 0; n< EPCLen;n++)data.append(EPC[n]);
            QString tagData=data.toHex().data();
            QString ant  = tagData.mid(0,2);
            QString len = tagData.mid(2,2);
            //qDebug()<<ant <<len.toInt(nullptr,16) << "读取到的EPC数据"<< tagData.mid(4,(len.toInt(nullptr,16))*2);
          // if(!tagData.isEmpty()) emit getRfidTag(tagData,tagData);

        }
   }
   return ret;
}


int KL4003::clearbuff()
{
    return  ClearBuffer_G2(&ComAdr, FrmHandle);
}
