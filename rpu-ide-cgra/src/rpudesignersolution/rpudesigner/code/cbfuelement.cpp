#include "cbfuelement.h"
#include <string>

using namespace std;

CBfuElement::CBfuElement()
{
    m_code.resize(CODE_LENGTH_BFU);
    m_inputSource = {
        {{-1, -1, -1}, -1},
        {{-1, -1, -1}, -1},
        {{-1, -1, -1}, -1},
    };
}

bool CBfuElement::generateCode()
{
    QBitArray compressCode(56, false);

    concact(compressCode, bypassIndex,     2,true);
    concact(compressCode, (int)funcIndex,  2,true);

    switch (funcIndex) {
    case Func_Au_Index:
    {
        AUAttr auAttr = {{0,0}, 0, 0, LBC_A};
        if(!aea(funcExp.toStdString(), auAttr))
        {
            setLastError(m_index.getString() + QStringLiteral("解析表达式错误"));
            return false;
        }
        concact(compressCode,auAttr.mux[0],     1,true);
        concact(compressCode,auAttr.carry,      1,true);
        concact(compressCode,funcAuModIndex,    2,true);
        concact(compressCode,auAttr.lbc,        3,true);
        concact(compressCode,auAttr.mux[1],     1,true);
        break;
    }
    case Func_Lu_Index:
    {
        LUAttr luAttr = {LBC_A, LBC_A, LBC_A, LBC_A, LBC_A, LBC_A};
        if(!ael(funcExp.toStdString(), luAttr))
        {
            setLastError(m_index.getString() + QStringLiteral("解析表达式错误"));
            return false;
        }
        concact(compressCode, luAttr.lbc[0],   3,true);
        concact(compressCode, luAttr.lbc[1],   3,true);
        concact(compressCode, luAttr.lbc[2],   3,true);
        concact(compressCode, luAttr.lbc[3],   3,true);
        concact(compressCode, luAttr.lbc[4],   3,true);
        concact(compressCode, luAttr.lbc[5],   3,true);
        break;
    }
    case Func_Su_Index:
    {
        SUAttr suAttr = {{LBC_A, LBC_A, LBC_A}, {SHIFT_LOGICR, SHIFT_LOGICR}, {0, 0 }, {0, 0}};
        if(!aes(funcExp.toStdString(), bypassExp.toStdString(), suAttr))
        {
            setLastError(m_index.getString() + QStringLiteral("解析表达式错误"));
            return false;
        }
        concact(compressCode, suAttr.lbc[0],   3,true);
        concact(compressCode, suAttr.lbc[1],   3,true);
        concact(compressCode, suAttr.shift[0], 5,true);
        concact(compressCode, suAttr.shift_mode[0], 2,true);
        concact(compressCode, suAttr.shift[1], 5,true);
        concact(compressCode, suAttr.shift_mode[1], 2,true);
        concact(compressCode, suAttr.mux[0],   1,true);
        concact(compressCode, suAttr.lbc[2],   3,true);
        concact(compressCode, suAttr.mux[1],   1,true);
        break;
    }
    case Func_Mu_Index:
    {
        concact(compressCode, funcMuModIndex==0?1 : 0,   1,true);
        break;
    }
    default:
        break;
    }


    QString strCfg = "cfg=";
    QBitArray bitArray(32,false);
    for (int i = 0; i < 29; ++i)
        bitArray.setBit(i+3,compressCode.at(28-i));
    for (int i = 0; i < bitArray.size()/4; ++i) {
        QBitArray bitTemp(4,false);
        for (int j = 0; j < 4; ++j)
            bitTemp.setBit(j,bitArray.at(i*4+j));
        strCfg+=BinToHex(bitToHexStr(bitTemp));
    }
    QString strSrc = "src=";
    for(int i = 0; i < m_inputSource.count(); ++i)
    {
        auto _temp = s_dataSourceCodeMap.value({m_inputSource.at(i).first.peIndex, m_inputSource.at(i).second});
        _temp.setBit(6, m_inputSource.at(i).first.peIndex == -1 || m_index.rcuIndex - m_inputSource.at(i).first.rcuIndex == 1);
        for(int j = 0; j < _temp.count(); ++j)
        {
            if(_temp.at(j))
                compressCode.setBit(29 + i * _temp.count() + j);
        }

        QBitArray bitArray(8,false);
        for (int i = 0; i < 7; ++i)
            bitArray.setBit(i+1,_temp.at(6-i));
        QString strTemp;
        for (int i = 0; i < bitArray.size()/4; ++i) {
            QBitArray bitTemp(4,false);
            for (int j = 0; j < 4; ++j)
                bitTemp.setBit(j,bitArray.at(i*4+j));
            strTemp+=BinToHex(bitToHexStr(bitTemp));
        }
        strSrc+=strTemp+" ";
    }

    setPeCode(strSrc.left(strSrc.length()-1)+", "+strCfg);

    s_bfuTableComplete.append(compressCode);
    auto index = s_bfuTable.indexOf(compressCode);
    if(index == -1) {
        s_bfuTable.append(compressCode);
        index = s_bfuTable.count() - 1;
    }

    bfuIndex = index;
    m_code.fill(0, 0, m_code.count());
    const auto& _temp = QBitArray::fromBits((const char*)(&index), 10);
    for(int i = 0; i < m_code.count(); ++i)
    {
        if(_temp.at(i))
            m_code.setBit(m_code.count() -1 - i);
    }

    printRcuBinCode("bfu  ");
    return true;
}

int CBfuElement::getCodePos() const
{
    return m_index.peIndex * CODE_LENGTH_BFU;
}

