#include "crcuelement.h"
#include "cbfuelement.h"
#include "cmemoryelement.h"
#include "csboxelement.h"
#include "cbeneselement.h"
#include "csregelement.h"
#include "coutfifoelement.h"

#include <ElementBfu.h>

CRcuElement::CRcuElement()
{
    m_code.resize(CODE_LENGTH_RCU);
    m_Rcu = 0;
}

bool CRcuElement::generateCode()
{
    m_code.fill(false, 0, m_code.count());
    const auto fillCode = [this](const QList<CBaseElement*> &list,const QString peName,QString& strPeCode) -> bool {
        for(int i = 0; i < list.count(); ++i)
        {
//            const auto* pe = dynamic_cast<CBfuElement*>(list.at(i));
            auto* pe = list.at(i);
            if(pe == nullptr)
                continue;
            auto* memory = dynamic_cast<CMemoryElement*>(list.at(i));
            if(memory != nullptr)
                memory->setDoneCtrl(m_doneCtrl);
            if(!pe->generateCode()) {
                m_lastError = pe->getLastError();
                return false;
            }
            const auto pos = pe->getCodePos();
            const auto &code = pe->getCode();
            for(int j = 0; j < code.count(); ++j)
            {
                if(code.at(j))
                    m_code.setBit(pos + j);
            }
            if(peName == "BFU")
            {
                if(i==7 && false)
                {
                    static int iRcu = 8;
                    if(iRcu==-1)
                        iRcu = 8;
                    QString strPrint;
                    for(int i = 0;i<80;i++)
                        strPrint+=m_code.at(i)==0?"0":"1";
                    if(iRcu == 0)
                        strPrint+="\n\n";
                    QFile file("D:/rcu_bfu_80.txt");
                    if(file.open(QFile::WriteOnly|QFile::Append)) {
                        file.write((QString::number(iRcu)+" "+strPrint+"\n").toUtf8());
                    }
                    iRcu--;
                }

               CBfuElement* cBfu = dynamic_cast<CBfuElement*>(pe);
               strPeCode+="BFU      "+QString::number(i,10)+": "+pe->getPeCode()+ " bfuIndex: "+QString::number(cBfu->bfuIndex,10)+"\n";
            }
            else if(peName == "SBOX")
            {
                CSboxElement* cSbox = dynamic_cast<CSboxElement*>(pe);
                this->setSboxCI(cSbox->getSboxCI());
                strPeCode+="SBOX: "+pe->getPeCode()+"\n";
            }
            else if(peName == "BENES")
                strPeCode+="BENES: "+pe->getPeCode()+"\n";
            else if(peName == "SREG")
                strPeCode+="SREG: "+pe->getPeCode()+"\n";
            else if(peName == "OFIFO")
                strPeCode+="OFIFO: "+pe->getPeCode()+"\n";
            else if(peName == "LFSR")
                strPeCode+="LFSR: "+pe->getPeCode()+"\n";
        }
        return true;
    };

    addDefaultElement();

    if(m_bfuList.size() == 8)
    {
        QList<CBaseElement*> m_temp_bfuList;
        for(int i = 0; i < 8; i++)
        {
            for(int j = 0; j < 8; j++)
            {
                if(i == m_bfuList.at(j)->getIndex().peIndex)
                {
                    m_temp_bfuList.append(m_bfuList.at(j));
                    break;
                }
            }
        }
        m_bfuList = m_temp_bfuList;
    }

    QString strPeCode;
    bool ret = fillCode(m_bfuList,"BFU",strPeCode);
    ret = ret && fillCode(m_sboxList,"SBOX",strPeCode);
    ret = ret && fillCode(m_benesList,"BENES",strPeCode);
    ret = ret && fillCode(m_memoryList,"MEM",strPeCode);
    ret = ret && fillCode(m_sregList,"SREG",strPeCode);
    ret = ret && fillCode(m_outfifoList,"OFIFO",strPeCode);
    ret = ret && fillCode(m_lfsrList,"LFSR",strPeCode);
    setPeCode(strPeCode);
    return ret;
}

void CRcuElement::calcDoneCtrl(const QBitArray &doneCtrl)
{
    const auto fillCode = [this](const QList<CBaseElement*> &_list, int curRcuIndex){
        for(auto i = 0; i < _list.count(); ++i)
        {
//            const auto* pe = dynamic_cast<CBfuElement*>(list.at(i));
            auto* pe = _list.at(i);
            if(pe == nullptr)
                continue;

            for(auto j = 0; j < pe->getInputSource().count(); ++j)
            {
//                auto index = 0;
                const auto& port = pe->getInputSource().at(j);
                if(port.first.rcuIndex != curRcuIndex)
                    continue;

//                if(port.first.peIndex < 8) {
//                    index = port.first.peIndex * 2 + port.second;
//                } else if(port.first.peIndex < 12) {
//                    index = 8 * 2 + (port.first.peIndex - 8) * 4 + port.second;
//                } else if(port.first.peIndex < 13) {
//                    index = 8 * 2 + 4 * 4 + (port.first.peIndex - 12) * 4 + port.second;
//                } else {
//                    continue;
//                }
//                m_code.setBit(index);
                m_code.setBit(port.first.peIndex);
                m_doneCtrl.setBit(port.first.peIndex);//新增
            }
        }
    };

    m_doneCtrl.resize(13);
    m_doneCtrl.fill(false, 0, m_doneCtrl.size());
    m_doneCtrl |= doneCtrl;
    fillCode(m_memoryList, m_index.rcuIndex);
    fillCode(m_sregList, m_index.rcuIndex);
    fillCode(m_outfifoList, m_index.rcuIndex);
}

QBitArray CRcuElement::calcLastRcuDoneCtrl() const
{
    QBitArray code(13, false);
    const auto fillCode = [&code](const QList<CBaseElement*> &list, int curRcuIndex){
        for(int i = 0; i < list.count(); ++i)
        {
//            const auto* pe = dynamic_cast<CBfuElement*>(list.at(i));
            auto* pe = list.at(i);
            if(pe == nullptr)
                continue;

            for(auto j = 0; j < pe->getInputSource().count(); ++j)
            {
//                auto index = 0;
                const auto &port = pe->getInputSource().at(j);
                if(port.first.rcuIndex != curRcuIndex - 1 || port.first.bcuIndex == -1)
                    continue;

//                if(port.first.peIndex < 8) {
//                    index = port.first.peIndex * 2 + port.second;
//                } else if(port.first.peIndex < 12) {
//                    index = 8 * 2 + (port.first.peIndex - 8) * 4 + port.second;
//                } else if(port.first.peIndex < 13) {
//                    index = 8 * 2 + 4 * 4 + (port.first.peIndex - 12) * 4 + port.second;
//                } else {
//                    continue;
//                }
//                code.setBit(index);
                code.setBit(port.first.peIndex);
            }
        }
    };
    fillCode(m_bfuList, m_index.rcuIndex);
    fillCode(m_sboxList, m_index.rcuIndex);
    fillCode(m_benesList, m_index.rcuIndex);
    return code;
}

void CRcuElement::addBfuElement(CBaseElement *element)
{
    if(element)
        m_bfuList.append(element);
}

CBaseElement* CRcuElement::findBfu(SIndex sindex)
{
    CBaseElement *ele = nullptr;
    foreach(CBaseElement *_ele, m_bfuList) {
        if(_ele->getIndex() == sindex)
        {
            ele = _ele;
            break;
        }
    }
    return ele;
}

void CRcuElement::addSboxElement(CBaseElement *element)
{
    if(element)
        m_sboxList.append(element);
}

CBaseElement *CRcuElement::findSbox(SIndex sindex)
{
    CBaseElement *ele = nullptr;
    foreach(CBaseElement *_ele, m_sboxList) {
        if(_ele->getIndex() == sindex)
        {
            ele = _ele;
            break;
        }
    }
    return ele;
}

void CRcuElement::addBenesElement(CBaseElement *element)
{
    if(element)
        m_benesList.append(element);
}

CBaseElement *CRcuElement::findBenes(SIndex sindex)
{
    CBaseElement *ele = nullptr;
    foreach(CBaseElement *_ele, m_benesList) {
        if(_ele->getIndex() == sindex)
        {
            ele = _ele;
            break;
        }
    }
    return ele;
}

void CRcuElement::addMemoryElement(CBaseElement *element)
{
    if(element)
        m_memoryList.append(element);
}

CBaseElement *CRcuElement::findMemory(SIndex sindex)
{
    CBaseElement *ele = nullptr;
    foreach(CBaseElement *_ele, m_memoryList) {
        if(_ele->getIndex() == sindex)
        {
            ele = _ele;
            break;
        }
    }
    return ele;
}

void CRcuElement::addSregElement(CBaseElement *element)
{
    if(element)
        m_sregList.append(element);
}

CBaseElement *CRcuElement::findSreg(SIndex sindex)
{
    CBaseElement *ele = nullptr;
    foreach(CBaseElement *_ele, m_sregList) {
        if(_ele->getIndex() == sindex)
        {
            ele = _ele;
            break;
        }
    }
    return ele;
}

void CRcuElement::addLfsrElement(CBaseElement *element)
{
    if(element)
        m_lfsrList.append(element);
}

CBaseElement *CRcuElement::findLfsr(BasePe::SIndex sindex)
{
    CBaseElement *ele = nullptr;
    foreach(CBaseElement *_ele, m_lfsrList) {
        if(_ele->getIndex() == sindex)
        {
            ele = _ele;
            break;
        }
    }
    return ele;
}

void CRcuElement::addOutfifoElement(CBaseElement *element)
{
    if(element)
        m_outfifoList.append(element);
}

CBaseElement *CRcuElement::findOutfifo(SIndex sindex)
{
    CBaseElement *ele = nullptr;
    foreach(CBaseElement *_ele, m_outfifoList) {
        if(_ele->getIndex() == sindex)
        {
            ele = _ele;
            break;
        }
    }
    return ele;
}

void CRcuElement::addDefaultElement()
{
    //bfu
    QList<int> listPe{0,1,2,3,4,5,6,7};
    for(int i = 0; i < m_bfuList.size(); i++)
    {
        auto bfu = m_bfuList.at(i);
        int index = bfu->getIndex().peIndex;
        listPe.removeOne(index);
    }

    int iListCount = m_bfuList.count();
    int iAddCount = 8 - iListCount;
    for(int i = 0;i<iAddCount;i++)
    {
        CBfuElement* peTemp = new CBfuElement();
        peTemp->setBDefaultBinCode(true);
        peTemp->setIndex(m_index.bcuIndex,m_index.rcuIndex,listPe.at(i));
        addBfuElement(peTemp);
    }
    //sbox
    iListCount = m_sboxList.count();
    iAddCount = 1 - iListCount;
    for(int i = 0;i<iAddCount;i++)
    {
        CSboxElement* peTemp = new CSboxElement();
        peTemp->setBDefaultBinCode(true);
        peTemp->setIndex(m_index.bcuIndex,m_index.rcuIndex,8);
        addSboxElement(peTemp);
    }
    //benes
    iListCount = m_benesList.count();
    iAddCount = 1 - iListCount;
    for(int i = 0;i<iAddCount;i++)
    {
        CBenesElement* peTemp = new CBenesElement();
        peTemp->setBDefaultBinCode(true);
        peTemp->setIndex(m_index.bcuIndex,m_index.rcuIndex,8+4+iListCount+i);
        addBenesElement(peTemp);
    }
}

int CRcuElement::getLoopStartEndFlag() const
{
    return loopStartEndFlag;
}

void CRcuElement::setLoopStartEndFlag(int value)
{
    loopStartEndFlag = value;
}

int CRcuElement::getLoopTimes() const
{
    return loopTimes;
}

void CRcuElement::setLoopTimes(int value)
{
    loopTimes = value;
}

int CRcuElement::getLoopStartEndFlag2() const
{
    return loopStartEndFlag2;
}

void CRcuElement::setLoopStartEndFlag2(int value)
{
    loopStartEndFlag2 = value;
}

int CRcuElement::getLoopTimes2() const
{
    return loopTimes2;
}

void CRcuElement::setLoopTimes2(int value)
{
    loopTimes2 = value;
}








