#ifndef CPEMODEL_H
#define CPEMODEL_H

#include <QString>

class CPE;
class CXmlResolver;

class CPeModel
{
public:
    CPeModel(QString fileName, int index);
    ~CPeModel();

    void updatePeValue(int bcu, int rcu, int id, uint32_t *value);
    void refreshAllValue(uint32_t *allPeValue);
    QStringList getAllValue();

private:
    QString m_fileName;
    int m_index;
    CPE *m_peList[4][4][13];

    enum {
        Type0,
        Type1 = 1,  //同行BFU 0口输出，此时index代表同行第几个BFU（0～7）
        Type2,      //同行BFU 1口输出，此时index代表同行第几个BFU
        Type3,      //同行SBOX 任意口输出，此时index代表同行第几个SBOX的第几个输出口（0~15）
        Type4,      //同行Benes 任意口输出，此时index代表同行Benes（benens一行就一个）的第几个输出口（0~3）
        Type5,      //上一行BFU 0口输出（第一行即为上一个BCU的最后一行），此时index代表同行第几个BFU（0～7）
        Type6,      //上一行BFU 1口输出，此时index代表同行第几个BFU
        Type7,      //上一行SBOX 任意口输出，此时index代表同行第几个SBOX的第几个输出口（0~15）
        Type8,      //上一行Benes 任意口输出，此时index代表同行Benes（benens一行就一个）的第几个输出口（0~3）
        Type9,      //当前BCU的FIFO输出，此时index代表FIFO读第几个输出口（0~3）
        Type10      //当前BCU的MEMORY输出，此时index代表MEMORY读第几个输出口（0~3）
    };

    void generatePe(const QString &fileName);
    void conectPeFrom(CXmlResolver *xml, int toPeBcuid, int toPeRcuid, int toPePeid, int port, int elementType, int attrType, int attrIndex);
};

#endif // CPEMODEL_H
