#include "cbcuelement.h"
#include "crcuelement.h"

#include <QTextStream>

int macroPos;

void appendSboxCompressIndex(const CBaseElement::sboxCIndex& sboxCI,CBcuElement::sbox8CIndex& sbox8CI)
{
    for(int i = 0;i < 4;i++)
    {
        if(i == 0)
        {
            if(sboxCI.sbox0.iGroup == 0)
                sbox8CI.sbox0_group0 = sboxCI.sbox0;
            else if(sboxCI.sbox0.iGroup == 1)
                sbox8CI.sbox0_group1 = sboxCI.sbox0;
        }
        else if(i == 1)
        {
            if(sboxCI.sbox1.iGroup == 0)
                sbox8CI.sbox1_group0 = sboxCI.sbox1;
            else if(sboxCI.sbox1.iGroup == 1)
                sbox8CI.sbox1_group1 = sboxCI.sbox1;
        }
        else if(i == 2)
        {
            if(sboxCI.sbox2.iGroup == 0)
                sbox8CI.sbox2_group0 = sboxCI.sbox2;
            else if(sboxCI.sbox2.iGroup == 1)
                sbox8CI.sbox2_group1 = sboxCI.sbox2;
        }
        else if(i == 3)
        {
            if(sboxCI.sbox3.iGroup == 0)
                sbox8CI.sbox3_group0 = sboxCI.sbox3;
            else if(sboxCI.sbox3.iGroup == 1)
                sbox8CI.sbox3_group1 = sboxCI.sbox3;
        }
    }
}

bool CBcuElement::generateCode()
{
    static const auto sortj = [](int &i, QList<QBitArray>& list, QList<QPair<uint8_t, uint8_t>> &config) -> int {
        QList<QBitArray> temp1;
        temp1.append(list.at(i));

        QList<QBitArray> temp2;
        static auto times = 0;

        for(auto j = i + 1; j < (i + 10 < list.length() ? i+10 : list.length()); j++)
        {
            if(list.at(j) == list.at(i))
            {
                temp2.clear();
                for(auto k = 0; k < temp1.length(); k++)
                {
                    if(j+k >= list.length())
                    {
                        break;
                    }
                    temp2.append(list.at(j+k));
                }

                if(temp1 == temp2)
                {
                    times++;
                    temp2.clear();
                    for (auto m = 2; m < list.length() - i -1; m++) {

                        auto n = 0;

                        while(n < temp1.length())
                        {
                            temp2.append(list.at(i + m*temp1.length() + n));
                            n++;
                        }

                        if(temp2 != temp1)
                        {
                            break;
                        }
                        else
                        {
                            times++;
                            temp2.clear();
                        }

                    }
                    auto s = 0;
                    auto times_temp = times;
                    while(times_temp >0)
                    {
                        s = temp1.length();

                        while(s > 0)
                        {
                            list.removeAt(i);
                            config.removeAt(i);
                            s--;
                        }
                        times_temp--;
                    }

                    config.replace(i, {times, 0b0000010});
                    if(i == i + temp1.length() - 1)
                        config.replace(i + temp1.length() - 1, {times, 0b0000011});
                    else
                        config.replace(i + temp1.length() - 1, {0, 0b0000001});

                    i = i + temp1.length() - 1;

                    goto end;
                }
                else
                {
                    temp1.append(list.at(j));
                }
                continue;
            }
            else
            {
                temp1.append(list.at(j));
            }
        }
    end:
        times = 0;
        return 0;
    };

    std::sort(m_rcuList.begin(), m_rcuList.end(), [](const CBaseElement* l, const CBaseElement* r){return *l<*r;});
    QBitArray lastRcuDoneCtrl(13, false);
    QList<QBitArray> codeList;
    QString strPeCode = "==============="+this->projectName+"_"+this->xmlName+"=========================\n";
    QList<QBitArray> codeListIndex;
    sbox8CIndex sbox8CI;
    for(int i = m_rcuList.count() - 1; i >= 0 ; --i)
    {
        QString strPeCodeTemp = "========================================\n";
        strPeCodeTemp+=         "==Config begin=cache_id:   "+QString::number(i)+"============";
        auto *rcu = dynamic_cast<CRcuElement*>(m_rcuList.at(i));
        Q_ASSERT(rcu);
        rcu->calcDoneCtrl(lastRcuDoneCtrl);
        lastRcuDoneCtrl = rcu->calcLastRcuDoneCtrl();
        if(!rcu->generateCode()) {
            m_lastError = rcu->getLastError();
            return false;
        }

        appendSboxCompressIndex(rcu->getSboxCI(),sbox8CI);

        QString strMid;
        strMid = rcu->getPeCode();
        auto code = rcu->getCode();
        QBitArray _code(208, false);
        for(int i = 0; i< CODE_LENGTH_LFSR;i++)
            _code.setBit(i,code.at(CODE_LENGTH_RCU-CODE_LENGTH_LFSR+i));
        for(int i = 0; i< CODE_LENGTH_RCU-CODE_LENGTH_LFSR;i++)
            _code.setBit(i+CODE_LENGTH_LFSR,code.at(i));
        //loop_ctrl 20(2次折叠)
        i2b(_code, rcu->getLoopStartEndFlag(),  2, _code.size() -2);
        i2b(_code, rcu->getLoopTimes(),  8, _code.size() -10);
        i2b(_code, rcu->getLoopStartEndFlag2(),  2, _code.size() -12);
        i2b(_code, rcu->getLoopTimes2(),  8, _code.size() -20);
        QString strLoopPeCode = "LOOP: ";
        strLoopPeCode+="LOOP1=("
                +QString::number((rcu->getLoopStartEndFlag()==1||rcu->getLoopStartEndFlag()==3)?1:0)+","
                +QString::number((rcu->getLoopStartEndFlag()==2||rcu->getLoopStartEndFlag()==3)?1:0)+", "
                +QString::number(rcu->getLoopTimes())+"),";
        strLoopPeCode+="LOOP2=("
                +QString::number((rcu->getLoopStartEndFlag2()==1||rcu->getLoopStartEndFlag2()==3)?1:0)+","
                +QString::number((rcu->getLoopStartEndFlag2()==2||rcu->getLoopStartEndFlag2()==3)?1:0)+", "
                +QString::number(rcu->getLoopTimes2())+")\n";

		//rcu配置码-反转
        bitNegation(_code,_code.count(),0);
        codeList.append(_code);

        auto index = codeListIndex.indexOf(_code);
        if(index == -1) {
            codeListIndex.append(_code);
            index = codeListIndex.count() - 1;
        }

        strPeCode+= strPeCodeTemp +" rcuIndex: "+ QString::number(index)+"\n" + strMid+strLoopPeCode;

    }
    setPeCode(strPeCode);
    setSbox8CI(sbox8CI);

    s_benesTable.clear();
    CBaseElement::s_rcuTableComplete = codeList;
    m_rcuCodeList.clear();
    for(int i = 0; i < codeList.count(); ++i)
    {
        const auto& rcu = codeList.at(i);
        auto index = s_rcuTable.indexOf(rcu);
        if(index == -1) {
            s_rcuTable.append(rcu);
            index = s_rcuTable.count() - 1;
        }

        const auto& _temp = QBitArray::fromBits((const char*)(&index), 32);
        QBitArray code(10, false);
        for(int i = 0; i < 10; ++i)
        {
            if(_temp.at(i))
                code.setBit(i);
        }
        m_rcuCodeList.append(code);
    }
    //将list中10bit拼接在一起
    int iSetPos = 0;
    QBitArray bitArray(m_rcuCodeList.size()*10,false);
    if(false)
    {
        for (int i = 0; i < m_rcuCodeList.size(); ++i)
        {
            QBitArray subBitArray = m_rcuCodeList.at(i);
            for(int j = 0; j < subBitArray.size(); ++j)
            {
                bitArray.setBit(iSetPos,subBitArray.at(j));
                iSetPos++;
            }
        }
    }
    else
    {
        for (int i = m_rcuCodeList.size() -1; i >= 0; i--)
        {
            QBitArray subBitArray = m_rcuCodeList.at(i);
            for(int j = 0; j < subBitArray.size(); ++j)
            {
                bitArray.setBit(iSetPos,subBitArray.at(j));
                iSetPos++;
            }
        }
    }
    s_rlmTable.append(bitArray);
    //.h rlm宏
    setRlmMacro(bitArray);
    //.h benes序号
    setBenesIndex();
    //.h sbox序号
    setSboxIndex();

    generateImd(imdSrcConfig);
    generateMemory(memorySrcConfig);
    generateInfifo(inFifoSrcConfig);
    return true;
}

void CBcuElement::generateImd(const QString &imdSrcConfig)
{
    QFile file(getResourceMap().value(imdSrcConfig));
    int iMaxLine = 0,iMaxPos = 0;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        while(!in.atEnd())
        {
            QString strLine = in.readLine();
            if(strLine.right(1) == " ")
                strLine = strLine.left(strLine.length() - 1);
            const auto& line = strLine.split(" ");
            if(line.count() == 2)
            {
                if(line.at(0) == "#")
                    continue;
                auto ok = false;
                auto value = line.at(1).toUInt(&ok, 16);
                if(ok)
                {
                    iMaxPos++;
                    if(value != 0)
                        iMaxLine = iMaxPos;
                }
            }
        }
        file.close();
    }

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        QString strImd;
        while(!in.atEnd())
        {
            QString strLine = in.readLine();
            if(strLine.right(1) == " ")
                strLine = strLine.left(strLine.length() - 1);
            const auto& line = strLine.split(" ");
            if(line.count() == 2)
            {
                if(line.at(0) == "#")
                    continue;
                auto ok = false;
                auto value = line.at(1).toUInt(&ok, 16);
                if(ok)
                {
                    iMaxLine--;
                    if(iMaxLine == -1)
                        break;
                    QString strTemp = line.at(1).right(8);
                    for(int i=0;i<4;i++)
                    {
                        strImd += strTemp.right(2);
                        strTemp = strTemp.left((4-1-i)*2);
                    }
                }
            }
        }
        setImdData(strImd);
        file.close();
    }
}

void CBcuElement::generateMemory(const QString &memorySrcConfig)
{
    QFile file(getResourceMap().value(memorySrcConfig));
    bool bStartCount = false;

    int iAddLine = -1,iOffsetPos = 0,iValidLine = 0;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        memoryDataInfo memDI;
        while(!in.atEnd())
        {
            QString strLine = in.readLine();
            if(strLine.right(1) == " ")
                strLine = strLine.left(strLine.length() - 1);
            const auto& line = strLine.split(" ");
            if(line.count() == 5)
            {
                if(line.at(0) == "#")
                    continue;
                bool bLineValied = false;
                QString strLineData;
                for(int i = 1; i < 5; ++i)
                {
                    auto ok = false;
                    auto value = line.at(i).toUInt(&ok, 16);
                    if(ok)
                    {
                        QString strTemp = line.at(i).right(8);
                        for(int j=0;j<4;j++)
                        {
                            strLineData += strTemp.right(2);
                            strTemp = strTemp.left((4-1-j)*2);
                        }
                        if(value != 0)
                            bLineValied = true;
                    }
                }

                iAddLine++;
                if(!bStartCount && bLineValied)
                {
                    iOffsetPos = iAddLine;
                    bStartCount = true;
                    memDI.memData+=strLineData;
                }
                else if(bLineValied)
                    memDI.memData+=strLineData;
            }
        }
        memDI.offset = iOffsetPos*16;
        setMemoryDI(memDI);
        file.close();
    }
}

void CBcuElement::generateInfifo(const QString &inFifoSrcConfig)
{
    QFile file(getResourceMap().value(inFifoSrcConfig));

    QString strInfifo="    ";
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        int i = 0;
        while(!in.atEnd())
        {
            QString strLine = in.readLine();
            if(strLine.right(1) == " ")
                strLine = strLine.left(strLine.length() - 1);
            const auto& line = strLine.split(" ");
            if(line.count() == 5)
            {
                if(line.at(0) == "#")
                    continue;
                for(int i = 1; i < 5; ++i)
                {
                    auto ok = false;
                    auto value = line.at(i).toUInt(&ok, 16);
                    if(ok)
                        strInfifo+=line.at(i)+",";
                }
                strInfifo+="\n    ";
                i++;
                if(i == 11)
                    break;
            }
        }
        setInfifoData(strInfifo.left(strInfifo.length()-6));
        file.close();
    }
}

QList<QBitArray>& CBcuElement::getRcuCodeList()
{
    return m_rcuCodeList;
}

CBaseElement *CBcuElement::findRcu(int bcuIndex, int rcuIndex)
{
    CBaseElement *ele = nullptr;
    foreach(auto* _ele, m_rcuList) {
        if(_ele->getIndex().bcuIndex == bcuIndex && _ele->getIndex().rcuIndex == rcuIndex)
        {
            ele = _ele;
            break;
        }
    }
    return ele;
}

void CBcuElement::addRcuElement(CBaseElement *element)
{
    if(element)
        m_rcuList.append(element);
}

void CBcuElement::setRlmMacro(const QBitArray bitArray)
{
    QString strRlm;
    QString strPrefix = "#define " + getProjectName().toUpper();//+"_"+getXmlName().toUpper();

    int iStartPos = macroPos;
    int iEndPos = iStartPos+bitArray.size()/100+(bitArray.size()%100==0?0:1)-1;
    int iNum = bitArray.size()/100*10+bitArray.size()%100/10;

    strRlm  = strPrefix+"_STR_ADDR " +QString::number(iStartPos) +"\n";
    strRlm += strPrefix+"_END_ADDR " +QString::number(iEndPos)   +"\n";
    strRlm += strPrefix+"_CACHE_NUM "+QString::number(iNum)      +"\n";

    setRlmMacro(strRlm);
    macroPos = ++iEndPos;
}

QString CBcuElement::getProjectName() const
{
    return projectName;
}

void CBcuElement::setProjectName(const QString &value)
{
    projectName = value;
}

QString CBcuElement::getXmlName() const
{
    return xmlName;
}

void CBcuElement::setXmlName(const QString &value)
{
    xmlName = value;
}

QString CBcuElement::getRlmMacro() const
{
    return rlmMacro;
}

void CBcuElement::setRlmMacro(const QString &value)
{
    rlmMacro = value;
}

QString CBcuElement::getBenesIndex() const
{
    return benesIndex;
}

void CBcuElement::setBenesIndex()
{
    if(s_benesIndexTable.size()==0)
        return;
    QString strPrefix,strMid;
    QString strProject = this->projectName;
    strPrefix = "int static " + strProject.toLower()+"_benes[";
    QList<int> benesList;
    for(auto var : s_benesIndexTable)
    {
        bool bExist = false;
        for(auto i : benesList)
        {
            if(i == var.second)
            {
                bExist = true;
                break;
            }
        }
        if(!bExist)
            benesList.append(var.second);
    }


    strPrefix +=QString::number(benesList.size())+"]={\n    ";
    for(auto var : benesList)
        strMid+=QString::number(var)+",";
    strMid = strMid.left(strMid.length()-1) + "\n};\n";

    benesIndex = strPrefix+strMid;
    s_benesIndexTable.clear();
}

QString CBcuElement::getSboxIndex() const
{
    return sboxIndex;
}

void CBcuElement::setSboxIndex()
{
    QString strSboxLine = "    ";
    auto sbox8CI = getSbox8CI();
    for(int i = 0;i<8;i++)
    {
        CBaseElement::sboxIndex sboxIndex;
        if(i == 0)
            sboxIndex = getSbox8CI().sbox0_group0.sIndex;
        else if(i == 1)
            sboxIndex = getSbox8CI().sbox0_group1.sIndex;
        else if(i == 2)
            sboxIndex = getSbox8CI().sbox1_group0.sIndex;
        else if(i == 3)
            sboxIndex = getSbox8CI().sbox1_group1.sIndex;
        else if(i == 4)
            sboxIndex = getSbox8CI().sbox2_group0.sIndex;
        else if(i == 5)
            sboxIndex = getSbox8CI().sbox2_group1.sIndex;
        else if(i == 6)
            sboxIndex = getSbox8CI().sbox3_group0.sIndex;
        else if(i == 7)
            sboxIndex = getSbox8CI().sbox3_group1.sIndex;
        strSboxLine +="{";
        for(int j = 0;j<4;j++)
        {
            if(j == 0)
                strSboxLine += QString::number(sboxIndex.index_0) + ",";
            else if(j == 1)
                strSboxLine += QString::number(sboxIndex.index_1) + ",";
            else if(j == 2)
                strSboxLine += QString::number(sboxIndex.index_2) + ",";
            else if(j == 3)
                strSboxLine += QString::number(sboxIndex.index_3) + "},";
        }
    }
    QString strProject = this->projectName;
    sboxIndex = "\nint static "+strProject.toLower()+"_sbox[8][4]={\n"+strSboxLine.left(strSboxLine.length()-1)+"\n};\n";
}

CBcuElement::sbox8CIndex CBcuElement::getSbox8CI() const
{
    return sbox8CI;
}

void CBcuElement::setSbox8CI(const sbox8CIndex &value)
{
    sbox8CI = value;
}

QString CBcuElement::getImdData() const
{
    return imdData;
}

void CBcuElement::setImdData(const QString &value)
{
    imdData = value;
}

memoryDataInfo CBcuElement::getMemoryDI() const
{
    return memoryDI;
}

void CBcuElement::setMemoryDI(const memoryDataInfo &value)
{
    memoryDI = value;
}

QString CBcuElement::getImdSrcConfig() const
{
    return imdSrcConfig;
}

void CBcuElement::setImdSrcConfig(const QString &value)
{
    imdSrcConfig = value;
}

QMap<QString, QString> CBcuElement::getResourceMap() const
{
    return resourceMap;
}

void CBcuElement::setResourceMap(const QMap<QString, QString> &value)
{
    resourceMap = value;
}

QString CBcuElement::getInfifoData() const
{
    return infifoData;
}

void CBcuElement::setInfifoData(const QString &value)
{
    infifoData = value;
}



