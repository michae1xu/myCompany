/******************************************************************************
* Description: 算子通用特性基类，用于一下工程：配置码、IDE编辑器、仿真工具rcs
* Author:
* Created by QtCreator 2016-08-15T11:12:27
******************************************************************************/

#ifndef BASEPE_H
#define BASEPE_H

#include <QString>
#include <QBitArray>
#include <QPair>
#include <QList>
#include <QMap>

#define BFU_COUNT     8
#define SBOX_COUNT    4
#define BENES_COUNT   1
#define MEMORY_COUNT  1
#define SREG_COUNT    1
#define LFSR_COUNT    1
#define OUTFIFO_COUNT 1

typedef unsigned long long U64;
typedef unsigned int       U32;

enum LBCType
{
    LBC_A,
    LBC_NA,
    LBC_AXORB,
    LBC_AANDB,
    LBC_AORB,
    LBC_NAXORB,
    LBC_NAANDB,
    LBC_NAORB,
};

enum MUXType
{
    MUX_A,
    MUX_AXORB
};

enum SHIFT_Type
{
    SHIFT_LOGICR = 0,
    SHIFT_ARITHR = 1,
    SHIFT_LAL    = 2,
    SHIFT_LOOPL  = 3
};

struct LUAttr
{
    LBCType lbc[6];
};

struct SUAttr
{
    LBCType lbc[3];
    SHIFT_Type shift_mode[2];
    U32 shift[2];
    U32 mux[2];
};
struct AUAttr
{
    U32 mux[2];
//    U32 add;
    U32 carry;
    U32 mod;
    LBCType lbc;
};

bool aea(const std::string &, AUAttr &);
bool ael(const std::string &, LUAttr &);
bool aes(const std::string &, SUAttr &);
bool aes(const std::string &, const std::string &, SUAttr &);

bool b2f(const QList<QBitArray> &, const QString &, QString &);
bool bp2f(const QList<QPair<QString, QBitArray>> &, const QString &, QString &);

class BasePe
{
public:
    struct SIndex {
        int bcuIndex;
        int rcuIndex;
        int peIndex;
        bool operator ==(const BasePe::SIndex &i) const
        {
            return (bcuIndex == i.bcuIndex && rcuIndex == i.rcuIndex && peIndex == i.peIndex);
        }
        bool operator <(const BasePe::SIndex &right) const
        {
            return ((bcuIndex < right.bcuIndex) ||
                    (bcuIndex == right.bcuIndex && rcuIndex < right.rcuIndex) ||
                    (bcuIndex == right.bcuIndex && rcuIndex == right.rcuIndex && peIndex < right.peIndex));
        }
        QString getString(){
            return QString("PE[%1, %2, %3]")
                .arg(QString::number(bcuIndex))
                .arg(QString::number(rcuIndex))
                .arg(QString::number( peIndex));
        }
    };

    using InputSourceType = QList<QPair<SIndex, int>>;

    BasePe();
    virtual ~BasePe();

    bool operator ==(const BasePe &) const;
    bool operator <(const BasePe &) const;

    void setIndex(int, int, int);
    void setIndex(const SIndex &);
    SIndex getIndex() const;
    InputSourceType getInputSource();

    void setLastError(const QString &);
    QString getLastError() const;

    static void i2b(QBitArray &, int, int, int, bool = false);

protected:
    SIndex m_index;
    QString m_lastError;
    InputSourceType m_inputSource;
};


QString BinToHex(const QString strBin);
QString bitToHexStr(QBitArray &bitArray);

#endif // BASEPE_H
