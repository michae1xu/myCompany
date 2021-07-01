#include "cbaseelement.h"
#include <string>
#include <QFileInfo>
#include <QDateTime>
#include <QTextStream>

uint8_t g_dataSource = 0;
QBitArray getDataSource()
{
    QBitArray bitArray(7, false);
    BasePe::i2b(bitArray, g_dataSource++, 7, 0, true);
    return bitArray;
}

uint8_t CBaseElement::m_Rcu = 0;

QList<QBitArray> CBaseElement::s_rlmTable;
QList<QBitArray> CBaseElement::s_rcuTable;
QList<QBitArray> CBaseElement::s_rcuTableComplete;
QList<QBitArray> CBaseElement::s_bfuTable;
QList<QBitArray> CBaseElement::s_bfuTableComplete;
QList<QBitArray> CBaseElement::s_memoryTable;
QList<QBitArray> CBaseElement::s_routerTable;
QList<QPair<QString, QBitArray>> CBaseElement::s_sboxTable;
QList<QPair<QString, QBitArray>> CBaseElement::s_benesTable;

QList<QBitArray> CBaseElement::s_benesCompressTable;
QList<QPair<int,int>> CBaseElement::s_benesIndexTable;
QList<QPair<QString,int>> CBaseElement::s_benesPathTable;
QList<QBitArray> CBaseElement::s_sboxCompressTable;
QList<QPair<QString,CBaseElement::sboxIndex>> CBaseElement::s_sboxPathTable;

CBaseElement::SrcCodeType CBaseElement::s_dataSourceCodeMap
{
    { {-1,-1}, QBitArray(7, true)},
    { { 0, 0}, getDataSource()},
    { { 0, 1}, getDataSource()},
    { { 1, 0}, getDataSource()},
    { { 1, 1}, getDataSource()},
    { { 2, 0}, getDataSource()},
    { { 2, 1}, getDataSource()},
    { { 3, 0}, getDataSource()},
    { { 3, 1}, getDataSource()},
    { { 4, 0}, getDataSource()},
    { { 4, 1}, getDataSource()},
    { { 5, 0}, getDataSource()},
    { { 5, 1}, getDataSource()},
    { { 6, 0}, getDataSource()},
    { { 6, 1}, getDataSource()},
    { { 7, 0}, getDataSource()},
    { { 7, 1}, getDataSource()},
    { { 8, 0}, getDataSource()},
    { { 8, 1}, getDataSource()},
    { { 8, 2}, getDataSource()},
    { { 8, 3}, getDataSource()},
    { { 9, 0}, getDataSource()},
    { { 9, 1}, getDataSource()},
    { { 9, 2}, getDataSource()},
    { { 9, 3}, getDataSource()},
    { {10, 0}, getDataSource()},
    { {10, 1}, getDataSource()},
    { {10, 2}, getDataSource()},
    { {10, 3}, getDataSource()},
    { {11, 0}, getDataSource()},
    { {11, 1}, getDataSource()},
    { {11, 2}, getDataSource()},
    { {11, 3}, getDataSource()},
    { {12, 0}, getDataSource()},
    { {12, 1}, getDataSource()},
    { {12, 2}, getDataSource()},
    { {12, 3}, getDataSource()},
    { {13, 0}, getDataSource()},
    { {13, 1}, getDataSource()},
    { {13, 2}, getDataSource()},
    { {13, 3}, getDataSource()},
    { {14, 0}, getDataSource()},
    { {14, 1}, getDataSource()},
    { {14, 2}, getDataSource()},
    { {14, 3}, getDataSource()},
    { {15, 0}, getDataSource()},
    { {15, 1}, getDataSource()},
    { {15, 2}, getDataSource()},
    { {15, 3}, getDataSource()},
    { {15, 4}, getDataSource()},
    { {15, 5}, getDataSource()},
    { {15, 6}, getDataSource()},
    { {15, 7}, getDataSource()},
    { {15, 8}, getDataSource()},
    { {15, 9}, getDataSource()},
    { {15,10}, getDataSource()},
    { {15,11}, getDataSource()},
    { {15,12}, getDataSource()},
    { {15,13}, getDataSource()},
    { {15,14}, getDataSource()},
    { {15,15}, getDataSource()},
    { {16, 0}, getDataSource()},
    { {16, 1}, getDataSource()},
    { {16, 2}, getDataSource()},
    { {16, 3}, getDataSource()},
    { {17, 0}, getDataSource()},//65
    { {17, 1}, getDataSource()},
    { {17, 2}, getDataSource()},
    { {17, 3}, getDataSource()},
    { {17, 4}, getDataSource()},
    { {17, 5}, getDataSource()},
    { {17, 6}, getDataSource()},
    { {17, 7}, getDataSource()},
    { {17, 8}, getDataSource()},
    { {17, 9}, getDataSource()},
    { {17,10}, getDataSource()},
    { {17,11}, getDataSource()},
    { {17,12}, getDataSource()},
    { {17,13}, getDataSource()},
    { {17,14}, getDataSource()},
    { {17,15}, getDataSource()},
};

void CBaseElement::clearTable()
{
    s_rlmTable.clear();
    s_rcuTable.clear();
    s_rcuTableComplete.clear();
    s_bfuTable.clear();
    s_bfuTableComplete.clear();
    s_sboxTable.clear();
    s_benesTable.clear();
    s_memoryTable.clear();
    s_routerTable.clear();

    s_benesCompressTable.clear();
    s_benesIndexTable.clear();
    s_benesPathTable.clear();
    s_sboxCompressTable.clear();
    s_sboxPathTable.clear();
}

void CBaseElement::bitNegation(QBitArray &bitArray, int length, int bitPos)
{
    QBitArray bitCode = bitArray;
    bool posValue =false;
    for (int i = 0; i < length/2; ++i)
    {
        posValue = bitCode.at(bitPos+i);
        bitCode.setBit(bitPos+i,bitCode.at(bitPos+length-i-1));
        bitCode.setBit(bitPos+length-i-1,posValue);
    }
    bitArray = bitCode;
}

CBaseElement::sboxCIndex CBaseElement::getSboxCI() const
{
    return m_sboxCI;
}

void CBaseElement::setSboxCI(const sboxCIndex &sboxCI)
{
    m_sboxCI = sboxCI;
}

QString CBaseElement::getPeCode() const
{
    return m_peCode;
}

void CBaseElement::setPeCode(const QString &peCode)
{
    m_peCode = peCode;
}

QBitArray CBaseElement::getCode() const
{
    return m_code;
}

void CBaseElement::concact(QBitArray &bitArray, int source, int length, bool bNegation,bool b,int bitPos)
{
    bool bAdd = false;//初始位置为零，赋值使用后，还需加上length
    if(bitPos != -1)
    {
        if(m_bitPos == 0)
            bAdd = true;
        m_bitPos = bitPos;
    }
    if(bNegation)
        i2b(bitArray,source,length,m_bitPos,bNegation);
    else
        i2b(bitArray,source,length,m_bitPos);

    if(bitPos == -1 || bAdd)
        m_bitPos += length;
}

void CBaseElement::printRcuBinCode(QString sourceName)
{
    return;
    QFile myfile("D:\\binCode.txt");
    if (myfile.open(QFile::WriteOnly|QFile::Append))
    {
        QString sBlank = m_Rcu>9?"":" ";
        if(m_Rcu == 0)
        {
            QString sPrint = "\n\n" +
                    QDateTime::currentDateTime().toString("dd.MM hh:mm") +
                    "(rcu逐步 ):"+sBlank+QString::number(m_Rcu)+" "+sourceName+" ";
            myfile.write(sPrint.toUtf8());
            m_Rcu++;
        }
        else if(m_Rcu != 0)
        {
            QString sPrint = "\n                      "
                    +sBlank+QString::number(m_Rcu)+" "+sourceName+" ";
            myfile.write(sPrint.toUtf8());
            m_Rcu++;
        }

        QBitArray bitTemp = getCode();

        int iSize = bitTemp.size()%8;
        if(iSize != 0)
            bitTemp.resize(bitTemp.size()+ 8 - iSize);

        for (int j = bitTemp.size() - 1; j >= 0; --j) {
            if(j!=bitTemp.size() - 1 && (j+1)%8 == 0)
                myfile.write(" ");
            if(bitTemp.at(j))
                myfile.write("1");
            else
                myfile.write("0");
        }
        QTextStream out(&myfile);
    }
}

bool CBaseElement::getBDefaultBinCode() const
{
    return bDefaultBinCode;
}

void CBaseElement::setBDefaultBinCode(bool value)
{
    bDefaultBinCode = value;
}


