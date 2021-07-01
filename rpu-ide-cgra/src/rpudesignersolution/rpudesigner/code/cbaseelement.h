#ifndef CBASEELEMENT_H
#define CBASEELEMENT_H

#include <stdint.h>
#include "../basepe/basepe.h"

#define CODE_LENGTH_RCU     188
#define CODE_LENGTH_BFU      10
#define CODE_LENGTH_SBOX     33
#define CODE_LENGTH_BENES    13
#define CODE_LENGTH_MEMORY   19
#define CODE_LENGTH_SREG     27
#define CODE_LENGTH_LFSR      7
#define CODE_LENGTH_OUTFIFO   9
#define CODE_LENGTH_LOOPCTRL 20

class CBaseElement : public BasePe
{
public:
    CBaseElement() = default;
    typedef struct SBOXINDEX
    {
        int index_0 = -1;
        int index_1 = -1;
        int index_2 = -1;
        int index_3 = -1;
    }sboxIndex;

    struct singleSboxCI
    {
      int iSbox = -1;
      int iGroup= -1;
      sboxIndex sIndex;
    };
    typedef struct sboxCompressIndex
    {
        singleSboxCI sbox0;
        singleSboxCI sbox1;
        singleSboxCI sbox2;
        singleSboxCI sbox3;
    }sboxCIndex;

    QBitArray getCode() const;

    virtual bool generateCode() = 0;
//    virtual bool parseCode(const QBitArray &code) = 0;
    virtual int getCodePos() const {
        return 0;
    }

public:
    void concact(QBitArray &bitArray, int source, int length, bool bNegation = false, bool = false, int bitPos = -1);
    static void clearTable();

    static QList<QBitArray> s_rlmTable;
    static QList<QBitArray> s_rcuTable;
    static QList<QBitArray> s_rcuTableComplete;
    static QList<QBitArray> s_bfuTable;
    static QList<QBitArray> s_bfuTableComplete;
    static QList<QBitArray> s_memoryTable;
    static QList<QBitArray> s_routerTable;
    static QList<QPair<QString, QBitArray>> s_sboxTable; //废弃
    static QList<QPair<QString, QBitArray>> s_benesTable;

    static QList<QBitArray> s_benesCompressTable;           //全局，每128B压缩
    static QList<QPair<int,int>> s_benesIndexTable;         //单个算法，每个benes的序号集合
    static QList<QPair<QString,int>> s_benesPathTable;      //全局，路径对应的benes序号集合

    static QList<QBitArray> s_sboxCompressTable;            //全局，每1KB压缩
    static QList<QPair<QString,sboxIndex>> s_sboxPathTable; //全局，路径对应的sbox序号集合

    bool getBDefaultBinCode() const;
    void setBDefaultBinCode(bool value);

    QString getPeCode() const;
    void setPeCode(const QString &peCode);

    sboxCIndex getSboxCI() const;
    void setSboxCI(const sboxCIndex &sboxCI);

protected:
    QBitArray m_code;

    using SrcCodeType = QMap<QPair<int, int>, QBitArray>;
    static SrcCodeType s_dataSourceCodeMap;

    static uint8_t m_Rcu;
    void printRcuBinCode(QString sourceName);
    void bitNegation(QBitArray &bitArray, int length, int bitPos);
private:
    bool bDefaultBinCode = false;
    int m_bitPos = 0;//待修改配置码起始位
    QString m_peCode;//未压缩配置码
    sboxCIndex m_sboxCI;//sbox压缩index

};

#endif // CBASEELEMENT_H
