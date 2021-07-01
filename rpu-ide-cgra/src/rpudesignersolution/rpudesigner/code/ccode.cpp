#include "ccode.h"
#include "fileresolver/cprojectresolver.h"
#include "fileresolver/cxmlresolver.h"
#include "cbcuelement.h"
#include <QFileInfo>
#include <QDataStream>

CCode::CCode(const QString &outDirPath) :
    m_outDirPath(outDirPath)
{
    start();
}

void CCode::start()
{
    m_lastError = "NO ERROR";
    CBaseElement::clearTable();
}

void CCode::finish()
{
//    auto xx = CBaseElement::s_rcuTable;
//    auto yy = CBaseElement::s_bfuTable;
//    auto zz = CBaseElement::s_sboxTable;
//    auto aa = CBaseElement::s_benesTable;
//    auto bb = CBaseElement::s_memoryTable;
//    auto cc = CBaseElement::s_routerTable;

//    printBinCode(CBaseElement::s_rcuTable,"rcu   ");//鎵撳嵃鏈弽杞墠
//    printBinCode(CBaseElement::s_bfuTable,"bfu   ");
//    printBinCode(CBaseElement::s_memoryTable,"memory");
//    printBinCode(CBaseElement::s_routerTable,"router");

    printTableCode();

//    CodeNegation(CBaseElement::s_rcuTable);
//    CodeNegation(CBaseElement::s_bfuTable);
//    CodeNegation(CBaseElement::s_memoryTable);
//    CodeNegation(CBaseElement::s_routerTable);

//    b2f(CBaseElement::s_rcuTable,    m_outDirPath + "/rcu.bin"    , m_lastError);
//    b2f(CBaseElement::s_bfuTable,    m_outDirPath + "/bfu.bin"    , m_lastError);
//    bp2f(CBaseElement::s_sboxTable,  m_outDirPath + "/sbox.bin"   , m_lastError);
//    bp2f(CBaseElement::s_benesTable, m_outDirPath + "/benes.bin"  , m_lastError);
//    b2f(CBaseElement::s_memoryTable, m_outDirPath + "/memory.bin" , m_lastError);
//    b2f(CBaseElement::s_routerTable, m_outDirPath + "/router.bin" , m_lastError);
}

bool CCode::addProject(const CProjectResolver *projectResolver)
{
    const auto* projectParameter = projectResolver->getProjectParameter();
    Q_ASSERT(projectParameter);
    const auto& projectName = QFileInfo(projectParameter->projectName).completeBaseName();

    for(int i = 0; i < projectParameter->xmlParameterList->count(); ++i)
    {
        const auto& xmlParameter = projectParameter->xmlParameterList->at(i);
        if(xmlParameter.selected == Qt::Unchecked)
            continue;

        qDeleteAll(m_bcuList);
        m_bcuList.clear();

        CXmlResolver xml(xmlParameter.xmlPath);
        xml.initXmlToCode(*this, projectParameter);

        foreach(auto* bcu, m_bcuList) {
            bcu->setProjectName(projectName);
            bcu->setXmlName(QFileInfo(xmlParameter.xmlPath).completeBaseName());
            bcu->setResourceMap(projectParameter->resourceMap);
            if(!bcu->generateCode()) {
                m_lastError = bcu->getLastError();
                return false;
            }
            setPeCode(peCode() + bcu->getPeCode());
            setRlmMacro(rlmMacro() + bcu->getRlmMacro());
            setBenesIndex(benesIndex() + bcu->getBenesIndex());
            setSboxIndex(sboxIndex() + bcu->getSboxIndex());
            s_imdTable.append({bcu->getProjectName(),bcu->getImdData()});
            s_memoryTable.append({bcu->getProjectName(),bcu->getMemoryDI()});
            s_infifoTable.append({bcu->getProjectName(),bcu->getInfifoData()});
//            if(!b2f(bcu->getRcuCodeList(), m_outDirPath + "/" + projectName + "-" + QFileInfo(xmlParameter.xmlPath).completeBaseName() + ".bin", m_lastError))
//            {
//                return false;
//            }
        }
    }
    return true;
}

QString CCode::lastError() const
{
    return m_lastError;
}

void CCode::addBcuElement(CBcuElement *element)
{
    if(element)
        m_bcuList.append(element);
}

CBcuElement* CCode::findBcu(int bcuIndex)
{
    CBcuElement *ele = nullptr;
    foreach(auto* _ele, m_bcuList) {
        if(_ele->getIndex().bcuIndex == bcuIndex)
        {
            ele = _ele;
            break;
        }
    }
    return ele;
}

QString CCode::rlmMacro() const
{
    return m_rlmMacro;
}

void CCode::setRlmMacro(const QString &rlmMacro)
{
    m_rlmMacro = rlmMacro;
}

QString CCode::benesIndex() const
{
    return m_benesIndex;
}

void CCode::setBenesIndex(const QString &benesIndex)
{
    m_benesIndex = benesIndex;
}

QString CCode::sboxIndex() const
{
    return m_sboxIndex;
}

void CCode::setSboxIndex(const QString &sboxIndex)
{
    m_sboxIndex = sboxIndex;
}

QString CCode::peCode() const
{
    return m_peCode;
}

void CCode::setPeCode(const QString &peCode)
{
    m_peCode = peCode;
}

QList<QPair<QString, QString> > CCode::getImdTable() const
{
    return s_imdTable;
}

void CCode::setImdTable(const QList<QPair<QString, QString> > &value)
{
    s_imdTable = value;
}

QList<QPair<QString, memoryDataInfo> > CCode::getMemoryTable() const
{
    return s_memoryTable;
}

void CCode::setMemoryTable(const QList<QPair<QString, memoryDataInfo> > &value)
{
    s_memoryTable = value;
}

QList<QPair<QString, QString> > CCode::getInfifoTable() const
{
    return s_infifoTable;
}

void CCode::setInfifoTable(const QList<QPair<QString, QString> > &value)
{
    s_infifoTable = value;
}

void CCode::CodeNegation(QList<QBitArray> &s_peTable)
{
    QList<QBitArray> s_peTableTemp;
    for (int i = 0; i < s_peTable.size(); ++i)
    {
        QBitArray peCode = s_peTable.at(i);
        bool posValue =false;
        for (int i = 0; i < peCode.count()/8; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                posValue = peCode.at(j+i*8);
                peCode.setBit(j+i*8,peCode.at(7+i*8-j));
                peCode.setBit(7+i*8-j,posValue);
            }
        }

        for (int i = 0; i < peCode.count()/2; ++i)
        {
            posValue = peCode.at(i);
            peCode.setBit(i,peCode.at(peCode.count()-1-i));
            peCode.setBit(peCode.count()-1-i,posValue);
        }
        s_peTableTemp.append(peCode);
    }
    if(!s_peTableTemp.isEmpty())
        s_peTable = s_peTableTemp;
}

void CCode::printBinCode(QList<QBitArray> list, QString sourceName)
{
    return;
//    QFile myfile("C:\\Users\\xuf\\Desktop\\binCode.txt");//鍒涘缓涓€涓緭鍑烘枃浠剁殑鏂囨。
    QFile myfile("D:\\binCode.txt");
    if (myfile.open(QFile::WriteOnly|QFile::Append))
    {
        if(sourceName == "rcu   ")
            myfile.write("\n\n");
        QString stime = "\n" + QDateTime::currentDateTime().toString("dd.MM hh:mm") + "("+sourceName + "): ";
        myfile.write(stime.toUtf8());
        for (int i = 0; i < list.size(); ++i) {
            if(list.size() > 1 && i != 0)
                myfile.write("\n                     ");
            QBitArray bitTemp = list.at(i);

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
        }
        QTextStream out(&myfile);
    }
}

QString printCommonTableCode(const QString m_outDirPath, const QString strTableName,const QList<QBitArray> &s_TargetTable, const int iLength)
{
    QString strLineAppend;
    QString strTalbeCode = strTableName + "='{\n";
    for(int i = 0; i < s_TargetTable.size();i++)
    {
        auto _tempArray = s_TargetTable.at(i);
        QBitArray bitArray(iLength,false);
        for (int j = 0; j < iLength; ++j) {
            if(j < _tempArray.size())
                bitArray.setBit(j,_tempArray.at(j));
        }

        QString strResult = "", strTemp = "";
        int iAddTime = 0;
        QBitArray subBitArray(4,false);
        for (int j = iLength - 1; j > 0; )
        {
            for (int k = 0; k < 4; ++k)
                subBitArray.setBit(k,bitArray.at(j-k));
            auto strBin = bitToHexStr(subBitArray);
            iAddTime++;
            if(iAddTime%8 == 0)
            {
                strTemp += BinToHex(strBin);
                strResult = "32'h" + strTemp + ","+strResult;
                strTemp = "";
            }
            else
              strTemp += BinToHex(strBin);
            j=j-4;
        }
        strTalbeCode += strResult +"\n";
    }
    strTalbeCode = strTalbeCode.left(strTalbeCode.length() - 2) + "\n};\n\n";

    QFile file(m_outDirPath + "/rpu_cfg_table.sv");
    if(file.open(QFile::WriteOnly|QFile::Append)) {
        file.write(strTalbeCode.toUtf8());
        QTextStream out(&file);
    }

    strLineAppend = strTalbeCode;
    strLineAppend = strLineAppend.right(strLineAppend.size()-(strTableName + "='{\n").length());
    strLineAppend = strLineAppend.left(strLineAppend.size()-QString("\n};\n\n").length());
    return strLineAppend;
}

QString printRlmTableCode(const QString m_outDirPath, const QString strTableName,const QList<QBitArray> &s_rlmTable)
{
    QString strLineAppend;
    QString strTalbeCode = strTableName + "='{\n";
    for (int i = 0; i < s_rlmTable.size(); ++i) {
        QBitArray bitArray = s_rlmTable.at(i);
        //姣00bit涓轰竴缁勶紝涓嶈冻100bit琛ラ浂
        int iTimes = bitArray.size()/100;
        int iCount = bitArray.size()%100;
        if(iCount != 0)
        {
            bitArray.resize((iTimes+1)*100);
            iTimes++;
        }
        QList<QBitArray> s_rlm_table;
        for (int j = 0; j < iTimes; ++j)
        {
            QBitArray bitTemp(100,false);
            for (int k = 0; k < 100; ++k)
                bitTemp.setBit(k,bitArray.at(j*100+k));
            s_rlm_table.append(bitTemp);
        }

        QString strTimes = "a", strTemp = "", strRlm0 = "",strRlm1 = "",strRlm2 = "",strRlm3 = "";
        int iAddTime = 0,iLastTime = iTimes,iNowCount = 0;

        for (int j = 0; j < s_rlm_table.size(); ++j) {
            QBitArray subBitArray = s_rlm_table.at(j);
            QBitArray bitTemp(4,false);
            //鍊掑簭鎴彇4bit杞崲鎴6杩涘埗
            for(int k = subBitArray.size()-1; k >= 0;)
            {
                for (int l = 0; l < 4; ++l)
                    bitTemp.setBit(l,subBitArray.at(k-l));
                auto strBin = bitToHexStr(bitTemp);
                iAddTime++;
                if(iAddTime == 1)
                {
                    iLastTime--;
                    if(iLastTime == 0 && iCount != 0)
                        strTimes = QString::number(iCount/10, 10);

                    strTemp += BinToHex(strBin);

                    QString strCount = QString::number(iNowCount,16).right(2);
                    if(strCount.length() == 1)
                        strTemp = "32'hffff"+strTimes+"0"+strCount+strTemp+",";
                    else
                        strTemp = "32'hffff"+strTimes+    strCount+strTemp+",";
                    iNowCount += 10;

                    strRlm3 = strTemp;
                    strTemp = "";
                }
                else if(iAddTime == 9 || iAddTime == 17 || iAddTime == 25)
                {
                    strTemp += BinToHex(strBin);
                    strTemp = "32'h" + strTemp + ",";
                    if(strRlm2 == "")
                        strRlm2 = strTemp;
                    else if(strRlm1 == "")
                        strRlm1 = strTemp;
                    else if(strRlm0 == "")
                    {
                        strRlm0 = strTemp;
                        strTalbeCode += strRlm0+strRlm1+strRlm2+strRlm3+"\n";
                        strRlm0 = strRlm1 = strRlm2 = strRlm3 = "";
                        iAddTime = 0;
                    }
                    strTemp = "";
                }
                else
                    strTemp += BinToHex(strBin);
                k=k-4;
            }
        }
    }

    strTalbeCode = strTalbeCode.left(strTalbeCode.length() - 2) + "\n};\n\n";
    strLineAppend = strTalbeCode;
    QRegExp rx(",");
    strTalbeCode.replace(rx," ");
    rx.setPattern("32.h");
    strTalbeCode.replace(rx,"0x");
    QFile file(m_outDirPath + "/rpu_cfg_table.sv");
    if(file.open(QFile::WriteOnly|QFile::Append)) {
        file.write(strTalbeCode.toUtf8());
        QTextStream out(&file);
    }

    strLineAppend = strLineAppend.right(strLineAppend.size()-(strTableName + "='{\n").length());
    strLineAppend = strLineAppend.left(strLineAppend.size()-QString("\n};\n\n").length());
    return strLineAppend;
}

void printMacroTableCode(const QString m_outDirPath,const QString strMacro)
{
    QString strPrint = strMacro;
    QRegExp rx("#");
    strPrint.replace(rx,"`");
    strPrint += "\n";
    QFile file(m_outDirPath + "/rpu_cfg_table.sv");
    if(file.open(QFile::WriteOnly|QFile::Append)) {
        file.write(strPrint.toUtf8());
        QTextStream out(&file);
    }
}

QString getTableCodeLength(const CCode::RpuTableBin& rpuTableBin,bool bComplete,CCode::RpuTableLength& rtLength)
{
    QString strLength;
    CCode::RpuTableBin rpuTableBinCopy = rpuTableBin;
    if(bComplete)
    {
        rpuTableBinCopy.adjustComplete();
        strLength += "#define RLM_TABLE_LEN ("      +QString::number(rpuTableBinCopy.strRlm.size()/(4*8))+"*4*4)\n";
        strLength += "#define RCU_TABLE_LEN ("      +QString::number(rpuTableBinCopy.strRcu.size()/(8*8))+"*8*4)\n";
        strLength += "#define BFU_TABLE_LEN ("      +QString::number(rpuTableBinCopy.strBfu.size()/(4*8))+"*4*4)\n";
        strLength += "#define ROUTE_TABLE_LEN ("    +QString::number(rpuTableBinCopy.strRoute.size()/(4*8))+"*4*4)\n";
        strLength += "#define MEM_CTRL_TABLE_LEN (" +QString::number(rpuTableBinCopy.strMemCtrl.size()/(4*8))+"*4*4)\n";
        strLength += "#define BENES_TABLE_LEN (" +QString::number(rpuTableBinCopy.strBenes.size()/(2*128))+"*128)\n";
    }
    else
    {
        rpuTableBinCopy.adjustSimply();
        strLength += "#define RLM_TABLE_LEN ("      +QString::number(rpuTableBinCopy.strRlm.size()/(4*8))+"*4*4)\n";
        strLength += "#define RCU_TABLE_LEN ("      +QString::number(rpuTableBinCopy.strRcu.size()/(7*8))+"*7*4)\n";
        strLength += "#define BFU_TABLE_LEN ("      +QString::number(rpuTableBinCopy.strBfu.size()/(2*8))+"*2*4)\n";
        strLength += "#define ROUTE_TABLE_LEN ("    +QString::number(rpuTableBinCopy.strRoute.size()/(1*8))+"*1*4)\n";
        strLength += "#define MEM_CTRL_TABLE_LEN (" +QString::number(rpuTableBinCopy.strMemCtrl.size()/(4*8))+"*4*4)\n";
        strLength += "#define BENES_CTRL_TABLE_LEN (" +QString::number(rpuTableBinCopy.strBenes.size()/(2*128))+"*128)\n";
    }
    rtLength.iRlm = (rpuTableBinCopy.strRlm.size()*4)/8;
    rtLength.iRcu = (rpuTableBinCopy.strRcu.size()*4)/8;
    rtLength.iBfu = (rpuTableBinCopy.strBfu.size()*4)/8;
    rtLength.iRoute = (rpuTableBinCopy.strRoute.size()*4)/8;
    rtLength.iMemCtrl = (rpuTableBinCopy.strMemCtrl.size()*4)/8;
    rtLength.iBenes = (rpuTableBinCopy.strBenes.size()*4)/8;
    return strLength;
}

QString getImdAddr(CCode::RpuTableLength &rtLength,const QList<QPair<QString, QString>> &s_imdTable)
{
    int iAddPos = rtLength.iBenes+rtLength.iBfu+rtLength.iMemCtrl+rtLength.iRcu+rtLength.iRlm+rtLength.iRoute;
    QString strImdAddr;
    int iImdLength = 0;
    strImdAddr += "\nstruct cmd_imd_addr\n{\n    char *algo_name;\n    unsigned int addr;\n    unsigned int length;\n};\n\n";
    strImdAddr += "struct cmd_imd_addr cmd_imd_addr_t[]={\n";
    QString strTemp;
    for (int i = 0; i < s_imdTable.size(); ++i)
    {
        auto imd = s_imdTable.at(i);
        int iLength = imd.second.length()*4/8;
        strTemp = "    {\""+imd.first.toUpper()+"\", 0x"+QString::number(iAddPos,16)+", "+QString::number(iLength)+"},\n";
        if(i == s_imdTable.size()-1)
            strTemp = strTemp.left(strTemp.length()-2) + "\n};\n";
        iAddPos+=iLength;
        iImdLength += iLength;
        strImdAddr+=strTemp;
    }
    rtLength.iImd = iImdLength;
    return strImdAddr;
}

QString getMemAddr(CCode::RpuTableLength &rtLength,const QList<QPair<QString, memoryDataInfo>> &s_memTable)
{
    int iAddPos = rtLength.iBenes+rtLength.iBfu+rtLength.iMemCtrl+rtLength.iRcu+rtLength.iRlm+rtLength.iRoute+rtLength.iImd;
    QString strMemAddr;
    int iMemLength = 0;
    strMemAddr += "\nstruct cmd_mem_addr\n{\n    char *algo_name;\n    unsigned int offset_addr;\n    unsigned int length;\n    unsigned int addr;\n};\n\n";
    strMemAddr += "struct cmd_mem_addr cmd_mem_addr_t[]={\n";
    QString strTemp;
    for (int i = 0; i < s_memTable.size(); ++i)
    {
        auto mem = s_memTable.at(i);
        int iLength = mem.second.memData.length()*4/8;
        strTemp = "    {\""+mem.first.toUpper()+"\", 0x"+QString::number(mem.second.offset,16)+
                ", "+QString::number(iLength)+", 0x"+QString::number(iAddPos,16)+"},\n";
        if(i == s_memTable.size()-1)
            strTemp = strTemp.left(strTemp.length()-2) + "\n};\n\n";
        iAddPos+=iLength;
        iMemLength += iLength;
        strMemAddr+=strTemp;
    }
    rtLength.iMem = iMemLength;
    return strMemAddr;
}

QString getInfifoData(const QList<QPair<QString, QString>> &s_infifoTable)
{
    QString strInfifo;
    for (int i = 0; i < s_infifoTable.size(); ++i)
    {
         strInfifo+="int "+s_infifoTable.at(i).first.toLower()+"_infifo[]={\n"+s_infifoTable.at(i).second+"\n};\n";
    }
    return strInfifo;
}

void printRpuCfgH(const QString &m_outDirPath,
                  const QString &strRlmMacro,
                  const QString &strBenesIndex,
                  const QString &strSboxIndex,
                  const CCode::RpuTableBin& rpuTableBin,
                  const QList<QPair<QString, QString>> &s_imdTable,
                  const QList<QPair<QString, memoryDataInfo> > &s_memTable,
                  const QList<QPair<QString, QString>> &s_infifoTable)
{
//    QString strSboxIndexEdit = "int static sbox[";
//    QStringList strList = strSboxIndex.split("\n");
//    strSboxIndexEdit += QString::number((strList.size()-1)*8)+"][4]={\n" + strSboxIndex;
//    strSboxIndexEdit = strSboxIndexEdit.left(strSboxIndexEdit.length()-2) + "\n};\n";

    CCode::RpuTableLength rtLengthComplete,rtLengthSimply;
    QString strDataComplete = "#ifndef __RPU_CFG_TABLE_H__\n#define __RPU_CFG_TABLE_H__\n\n";
    strDataComplete += getTableCodeLength(rpuTableBin,true,rtLengthComplete)+strRlmMacro;
    strDataComplete += getImdAddr(rtLengthComplete,s_imdTable);
    strDataComplete += getMemAddr(rtLengthComplete,s_memTable)+getInfifoData(s_infifoTable)+strBenesIndex+strSboxIndex+"\n";

//    qDebug() << "iRlm: 0x" << QString::number(0,16);
//    qDebug() << "iRcu: 0x" << QString::number(rtLengthComplete.iRlm,16);
//    qDebug() << "iBfu: 0x" << QString::number(rtLengthComplete.iRlm+rtLengthComplete.iRcu,16);
//    qDebug() << "iRoute: 0x" << QString::number(rtLengthComplete.iRlm+rtLengthComplete.iRcu+rtLengthComplete.iBfu,16);
//    qDebug() << "iMemCtrl: 0x" << QString::number(rtLengthComplete.iRlm+rtLengthComplete.iRcu+rtLengthComplete.iBfu+rtLengthComplete.iRoute,16);
//    qDebug() << "iBenes: 0x" << QString::number(rtLengthComplete.iRlm+rtLengthComplete.iRcu+rtLengthComplete.iBfu+rtLengthComplete.iRoute+rtLengthComplete.iMemCtrl,16);
//    qDebug() << "iImd: 0x" << QString::number(rtLengthComplete.iRlm+rtLengthComplete.iRcu+rtLengthComplete.iBfu+rtLengthComplete.iRoute+rtLengthComplete.iMemCtrl+rtLengthComplete.iBenes,16);
//    qDebug() << "iMem: 0x" << QString::number(rtLengthComplete.iRlm+rtLengthComplete.iRcu+rtLengthComplete.iBfu+rtLengthComplete.iRoute+rtLengthComplete.iMemCtrl+rtLengthComplete.iBenes+rtLengthComplete.iImd,16);
//    qDebug() << "iTotalLength: 0x" << QString::number(rtLengthComplete.iRlm+rtLengthComplete.iRcu+rtLengthComplete.iBfu+rtLengthComplete.iRoute+rtLengthComplete.iMemCtrl+rtLengthComplete.iBenes+rtLengthComplete.iImd+rtLengthComplete.iMem,16);

    QString strDataSimply = "#ifndef __RPU_CFG_TABLE_H__\n#define __RPU_CFG_TABLE_H__\n\n";
    strDataSimply += getTableCodeLength(rpuTableBin,false,rtLengthSimply)+strRlmMacro;
    strDataSimply += getImdAddr(rtLengthSimply,s_imdTable);
    strDataSimply += getMemAddr(rtLengthSimply,s_memTable)+getInfifoData(s_infifoTable)+strBenesIndex+strSboxIndex+"\n";

    QString strDataCommon;
    QRegExp rx("32'h");
    QString str0=rpuTableBin.strRlm,
            str1=rpuTableBin.strRcu,
            str2=rpuTableBin.strBfu,
            str3=rpuTableBin.strRoute,
            str4=rpuTableBin.strMemCtrl;
    strDataCommon += "static const unsigned int rlm_table[]={\n"+str0.replace(rx,"0x")+"\n};\n";
    strDataCommon += "static const unsigned int rcu_table[]={\n"+str1.replace(rx,"0x")+"\n};\n";
    strDataCommon += "static const unsigned int bfu_table[]={\n"+str2.replace(rx,"0x")+"\n};\n";
    strDataCommon += "static const unsigned int route_table[]={\n"+str3.replace(rx,"0x")+"\n};\n";
    strDataCommon += "static const unsigned int mem_table[]={\n"+str4.replace(rx,"0x")+"\n};\n#endif";;

    QFile file(m_outDirPath + "/rpu_cfg_table_complete.h");
    if(file.open(QFile::WriteOnly|QFile::Append)) {
        file.write((strDataComplete+strDataCommon).toUtf8());
        QTextStream out(&file);
    }

    QFile file2(m_outDirPath + "/rpu_cfg_table_simply.h");
    if(file2.open(QFile::WriteOnly|QFile::Append)) {
        file2.write((strDataSimply+strDataCommon).toUtf8());
        QTextStream out(&file);
    }
    //rlm瀹忓垵濮嬩綅缃綊闆
    macroPos = 0;
}

void printRpuCfgBin(const QString m_outDirPath,const CCode::RpuTableBin& rpuTableBin,
                    const QList<QPair<QString, QString>> &s_imdTable,
                    const QList<QPair<QString, memoryDataInfo> > &s_memTable)
{
    QString strImdMem;
    for (int i = 0; i < s_imdTable.size(); ++i)
        strImdMem+=s_imdTable.at(i).second;
    for (int i = 0; i < s_imdTable.size(); ++i)
        strImdMem+=s_memTable.at(i).second.memData;

    QString strPrint;
    CCode::RpuTableBin rpuTableComplete = rpuTableBin;
    CCode::RpuTableBin rpuTableSimply = rpuTableBin;
    rpuTableComplete.adjustComplete();
    strPrint=rpuTableComplete.strRlm+rpuTableComplete.strRcu+rpuTableComplete.strBfu+
            rpuTableComplete.strRoute+rpuTableComplete.strMemCtrl+rpuTableComplete.strBenes+strImdMem;

    QFile file(m_outDirPath + "/rpu_cfg_table_complete.bin");
    if(file.open(QFile::WriteOnly|QFile::Append)) {
        file.write(QByteArray::fromHex(strPrint.toLocal8Bit()));
    }

    rpuTableSimply.adjustSimply();
    strPrint=rpuTableSimply.strRlm+rpuTableSimply.strRcu+rpuTableSimply.strBfu+
            rpuTableSimply.strRoute+rpuTableSimply.strMemCtrl+rpuTableSimply.strBenes+strImdMem;
    QFile file2(m_outDirPath + "/rpu_cfg_table_simply.bin");
    if(file2.open(QFile::WriteOnly|QFile::Append)) {
        file2.write(QByteArray::fromHex(strPrint.toLocal8Bit()));
    }
}

QString getListBitArrayTo16Str(const QList<QBitArray>& list)
{
    QString strTemp;
    for (auto var : list)
    {
        QBitArray bitTemp(4,false);
        for (int i = 0; i < var.size();)
        {
            for (int j = 0; j < 4; ++j)
                bitTemp.setBit(j,var.at(i+j));
            strTemp += BinToHex(bitToHexStr(bitTemp));
            i = i + 4;
        }
    }
    return strTemp;
}

QString printCompleteTable(const QList<QBitArray>& list,const QString tableName = "")
{
    QString strTemp;
    int iNum = 0;
    if(tableName == "rcu")
        iNum = list.size() - 1;
    for (auto var : list)
    {
        QBitArray bitArray(var.size(),false);
        for(int i = 0; i< var.size();i++)
            bitArray.setBit(i,var.at(var.size()-i-1));
        QString strNowTemp = (iNum>9?QString::number(iNum,10):" "+QString::number(iNum,10)) + " ";
        QBitArray bitTemp(4,false);
        for (int i = 0; i < bitArray.size();)
        {
            for (int j = 0; j < 4; ++j)
                bitTemp.setBit(j,bitArray.at(i+j));
            strNowTemp += BinToHex(bitToHexStr(bitTemp));
            i = i + 4;
        }

        strTemp+= strNowTemp + "\n";
        if(tableName == "rcu")
            iNum--;
        else
            iNum++;
    }
    return strTemp;
}

QString ChangeIndexBy8Bit(const QString str)
{
    QString strTemp = str;
    int iLength = strTemp.length();
    QString strLeft8,strResult;
    for (int i = 0; i < iLength/8; ++i)
    {
        strLeft8 = strTemp.left(8);
        for (int j = 0; j < 4; ++j)
        {
            strResult+=strLeft8.right(2);
            strLeft8 = strLeft8.left(8-(j+1)*2);
        }
        strTemp = strTemp.right(strTemp.length()-8);
    }
    return strResult;
}

void printRpuSboxBin(const QString m_outDirPath)
{
    QString strPrint = getListBitArrayTo16Str(CBaseElement::s_sboxCompressTable);
    strPrint = ChangeIndexBy8Bit(strPrint);

    QFile file(m_outDirPath + "/rpu_cfg_sbox.bin");
    if(file.open(QFile::WriteOnly|QFile::Append)) {
        file.write(QByteArray::fromHex(strPrint.toLocal8Bit()));
    }
}

CCode::RpuTableBin CCode::printRpuCfgSV(const QString m_outDirPath)
{
    printMacroTableCode(m_outDirPath,rlmMacro());
    RpuTableBin rpuTableBin;
    rpuTableBin.strRlm = printRlmTableCode(m_outDirPath,"rcm_mem_rlm_array",CBaseElement::s_rlmTable);
    QList<QBitArray> rcuTable;
    rpuTableBin.strRcu = printCommonTableCode(m_outDirPath,"rcm_mem_rcu_array",CBaseElement::s_rcuTable,256);
    rpuTableBin.strBfu = printCommonTableCode(m_outDirPath,"rcm_mem_bfu_array",CBaseElement::s_bfuTable,128);
    rpuTableBin.strRoute = printCommonTableCode(m_outDirPath,"rcm_mem_route_array",CBaseElement::s_routerTable,128);
    rpuTableBin.strMemCtrl = printCommonTableCode(m_outDirPath,"rcm_mem_mem_ctrl_array",CBaseElement::s_memoryTable,128);
    rpuTableBin.strBenes = getListBitArrayTo16Str(CBaseElement::s_benesCompressTable);
    return rpuTableBin;
}

void CCode::printRpuPeCode(const QString m_outDirPath)
{
    QList<QBitArray> bfuRemoveDuplicate;
    bool bDuplicate = false;
    for(auto var : CBaseElement::s_bfuTableComplete)
    {
        bDuplicate = false;
        for(auto bfu : bfuRemoveDuplicate)
            if(var == bfu)
            {
                bDuplicate = true;
                break;
            }
        if(!bDuplicate)
            bfuRemoveDuplicate.append(var);
    }
    QString strBfuRemoveDuplicate = "\nbfu_table_RemoveDuplicate:\n";
    strBfuRemoveDuplicate += printCompleteTable(bfuRemoveDuplicate);
    QString strBfuComplete = "\nbfu_table_complete:\n";
    strBfuComplete += printCompleteTable(CBaseElement::s_bfuTableComplete);
    QString strRcuComplete = "\nrcu_table_complete:\n";
    strRcuComplete += printCompleteTable(CBaseElement::s_rcuTableComplete,"rcu");
    QFile file(m_outDirPath + "/rpu_cfg_peCode.txt");
    if(file.open(QFile::WriteOnly|QFile::Append)) {
        file.write((peCode()+strBfuRemoveDuplicate+strBfuComplete+strRcuComplete).toUtf8());
        QTextStream out(&file);
    }
}

void CCode::printTableCode()
{
    printRpuPeCode(m_outDirPath);
    RpuTableBin rpuTableBin = printRpuCfgSV(m_outDirPath);
    printRpuCfgH(m_outDirPath,rlmMacro(),benesIndex(),sboxIndex(),rpuTableBin,getImdTable(),getMemoryTable(),getInfifoTable());
    printRpuCfgBin(m_outDirPath,rpuTableBin,getImdTable(),getMemoryTable());
    printRpuSboxBin(m_outDirPath);
}







