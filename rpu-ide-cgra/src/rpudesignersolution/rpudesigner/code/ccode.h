#ifndef CCODE_H
#define CCODE_H

#include <QString>
#include <QBitArray>
#include "cbcuelement.h"
#include "datafile.h"

class CProjectResolver;
class CBcuElement;

class CCode
{
    friend class XmlResolver;
public:
    struct RpuTableBin
    {
        QString strRlm;
        QString strRcu;
        QString strBfu;
        QString strRoute;
        QString strMemCtrl;
        QString strBenes;
        //清除前缀及逗号+每8位变序拼接
        void adjust(QString& str,int iCol = 0,int iStep = 0)
        {
            QString strTemp = str;
            QRegExp rx("32'h");
            strTemp.replace(rx,"");
            rx.setPattern(",");
            strTemp.replace(rx,"");
            rx.setPattern("\n");
            strTemp.replace(rx,"");

            if(iStep != 0 && strTemp.length()%(iCol+iStep) != 0)
                return;

            int iLength = strTemp.length();
            int iRow = 0;
            QString strLeft8,strResult;
            for (int i = 0; i < iLength/8; )
            {
                if(iStep != 0 && i == iCol+(iCol+iStep)*iRow)
                {
                    for (int j = 0; j < iStep; ++j)
                        strTemp = strTemp.right(strTemp.length()-8);
                    i=i+iStep;iRow++;
                    continue;
                }
                strLeft8 = strTemp.left(8);
                for (int j = 0; j < 4; ++j)
                {
                    strResult+=strLeft8.right(2);
                    strLeft8 = strLeft8.left(8-(j+1)*2);
                }
                strTemp = strTemp.right(strTemp.length()-8);
                i++;
            }
            str = strResult;
        }
        void adjustComplete()
        {
            adjust(strRlm);
            adjust(strRcu);
            adjust(strBfu);
            adjust(strRoute);
            adjust(strMemCtrl);
            adjust(strBenes);
        }
        void adjustSimply()
        {
            adjust(strRlm);
            adjust(strRcu,7,1);
            adjust(strBfu,2,2);
            adjust(strRoute,1,3);
            adjust(strMemCtrl);
            adjust(strBenes,26,6);
        }
    };

    struct RpuTableLength
    {
      int iRlm = 0;
      int iRcu = 0;
      int iBfu = 0;
      int iRoute = 0;
      int iMemCtrl = 0;
      int iBenes = 0;
      int iImd = 0;
      int iMem = 0;
    };

    explicit CCode(const QString& outDirPath);

    void start();
    void finish();
    bool addProject(const CProjectResolver *projectResolver);

    QString lastError() const;

    void addBcuElement(CBcuElement* element);
    CBcuElement *findBcu(int bcuIndex);

    QString rlmMacro() const;
    void setRlmMacro(const QString &rlmMacro);

    QString benesIndex() const;
    void setBenesIndex(const QString &benesIndex);

    QString sboxIndex() const;
    void setSboxIndex(const QString &sboxIndex);

    QString peCode() const;
    void setPeCode(const QString &peCode);

    QList<QPair<QString, QString> > getImdTable() const;
    void setImdTable(const QList<QPair<QString, QString> > &value);

    QList<QPair<QString, memoryDataInfo> > getMemoryTable() const;
    void setMemoryTable(const QList<QPair<QString, memoryDataInfo> > &value);

    QList<QPair<QString, QString> > getInfifoTable() const;
    void setInfifoTable(const QList<QPair<QString, QString> > &value);

private:
    QString m_outDirPath;
    QString m_lastError = "NO ERROR";
    QList<CBcuElement*> m_bcuList;

    QString m_rlmMacro;
    QString m_benesIndex;
    QString m_sboxIndex;
    QString m_peCode;

    QList<QPair<QString, QString>> s_imdTable;
    QList<QPair<QString, memoryDataInfo>> s_memoryTable;
    QList<QPair<QString, QString>> s_infifoTable;

private:
    void CodeNegation(QList<QBitArray>& s_peTable);
    void printBinCode(QList<QBitArray> list, QString sourceName);
    void printTableCode();
    void printRpuPeCode(const QString m_outDirPath);
    RpuTableBin printRpuCfgSV(const QString m_outDirPath);
};

#endif // CCODE_H
