#include "csregelement.h"

CSRegElement::CSRegElement()
{
    m_code.resize(CODE_LENGTH_SREG);
    m_inputSource = {
        {{-1, -1, -1}, -1},
        {{-1, -1, -1}, -1},
        {{-1, -1, -1}, -1},
        {{-1, -1, -1}, -1},
    };
}

bool CSRegElement::generateCode()
{
    //source 9
    QString strSrc = "src=";
    m_code.fill(false, 0, m_code.count());
    QBitArray router(32, false);
    for(auto i = 0; i < m_inputSource.count(); ++i)
    {
        auto _temp = s_dataSourceCodeMap.value({m_inputSource.at(i).first.peIndex, m_inputSource.at(i).second});
        _temp.setBit(6, m_inputSource.at(i).first.peIndex == -1 || m_index.rcuIndex - m_inputSource.at(i).first.rcuIndex == 1);
        for(auto j = 0; j < _temp.count(); ++j)
        {
            if(_temp.at(j))
                router.setBit(i * _temp.count() + j);
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

    auto index = s_routerTable.indexOf(router);
    if(index == -1) {
        s_routerTable.append(router);
        index = s_routerTable.count() - 1;
    }

    i2b(m_code, index, 9, 0);

    //sreg
    i2b(m_code,rchMode,2,m_code.count() -2);
    QList<int> addrList = { writeRchAddr1, writeRchAddr2, writeRchAddr3, writeRchAddr4 };
    for(auto i = 0; i < addrList.count(); ++i)
    {
        i2b(m_code, addrList.at(i), 4, m_code.count() -2 - 4 * (i+1));
    }

    QString strCfg = "cfg=";
    QBitArray bitArray(20,false);
    for (int i = 0; i < 18; ++i)
        bitArray.setBit(i+2,m_code.at(9+i));
    for (int i = 0; i < bitArray.size()/4; ++i) {
        QBitArray bitTemp(4,false);
        for (int j = 0; j < 4; ++j)
            bitTemp.setBit(j,bitArray.at(i*4+j));
        strCfg+=BinToHex(bitToHexStr(bitTemp));
    }
    setPeCode(strSrc.left(strSrc.length()-1)+", "+strCfg);

    printRcuBinCode("sreg ");
    return true;
}

int CSRegElement::getCodePos() const
{
    return CODE_LENGTH_BFU * BFU_COUNT +
            CODE_LENGTH_SBOX * 1 +
            CODE_LENGTH_BENES * BENES_COUNT +
            CODE_LENGTH_MEMORY * MEMORY_COUNT +
            CODE_LENGTH_SREG * 0;
}
