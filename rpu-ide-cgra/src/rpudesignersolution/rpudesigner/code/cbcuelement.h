#ifndef CBCUELEMENT_H
#define CBCUELEMENT_H

#include "cbaseelement.h"

extern int macroPos;
class CProjectResolver;

typedef struct memoryDataInfo
{
    int offset = 0;
    QString memData;
}memDI;

class CBcuElement : public CBaseElement
{
    friend class CXmlResolver;
public:
    typedef struct sboxCompress8Index
    {
        singleSboxCI sbox0_group0;
        singleSboxCI sbox0_group1;
        singleSboxCI sbox1_group0;
        singleSboxCI sbox1_group1;
        singleSboxCI sbox2_group0;
        singleSboxCI sbox2_group1;
        singleSboxCI sbox3_group0;
        singleSboxCI sbox3_group1;
    }sbox8CIndex;

public:
    CBcuElement() = default;

    bool generateCode();
//   void parseCode(const QBitArray &code);

    QList<QBitArray>& getRcuCodeList();

    CBaseElement *findRcu(int bcuIndex, int rcuIndex);
    void addRcuElement(CBaseElement *element);
    void setRlmMacro(const QBitArray bitArray);
    QString getProjectName() const;
    void setProjectName(const QString &value);

    QString getXmlName() const;
    void setXmlName(const QString &value);

    QString getRlmMacro() const;
    void setRlmMacro(const QString &value);

    QString getBenesIndex() const;
    void setBenesIndex();

    QString getSboxIndex() const;
    void setSboxIndex();

    sbox8CIndex getSbox8CI() const;
    void setSbox8CI(const sbox8CIndex &value);

    QString getImdData() const;
    void setImdData(const QString &value);

    memoryDataInfo getMemoryDI() const;
    void setMemoryDI(const memoryDataInfo &value);

    QString getImdSrcConfig() const;
    void setImdSrcConfig(const QString &value);

    QMap<QString, QString> getResourceMap() const;
    void setResourceMap(const QMap<QString, QString> &value);

    QString getInfifoData() const;
    void setInfifoData(const QString &value);

private:
    int burst = 0;
    int gap = 0;
    int sort = 0;
    int readImdMode128_32 = 0;

    QString rlmMacro;
    QString imdData;
    memoryDataInfo memoryDI;
    QString infifoData;
    QString projectName;
    QString xmlName;
    QString benesIndex;
    QString sboxIndex;

    QString memorySrcConfig;
    QString inFifoSrcConfig;
    QString imdSrcConfig;
    QMap<QString, QString> resourceMap;

    QList<CBaseElement*> m_rcuList;
    QList<QBitArray> m_rcuCodeList;
    sbox8CIndex sbox8CI;

    void generateImd(const QString &imdSrcConfig);
    void generateMemory(const QString &memorySrcConfig);
    void generateInfifo(const QString &inFifoSrcConfig);

};

#endif // CBCUELEMENT_H
