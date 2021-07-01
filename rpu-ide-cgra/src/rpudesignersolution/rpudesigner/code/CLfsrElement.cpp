#include "CLfsrElement.h"

CLfsrElement::CLfsrElement()
{
    m_code.resize(CODE_LENGTH_LFSR);
    m_inputSource = {
        {{-1, -1, -1}, -1},
    };
}

bool CLfsrElement::generateCode()
{
    //source 7
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
    setPeCode(strSrc.left(strSrc.length()-1));

    QBitArray router_false(32,false);
    router_false.setBit(0,true);
    router_false.setBit(1,true);
    router_false.setBit(2,true);
    router_false.setBit(3,true);
    router_false.setBit(4,true);
    router_false.setBit(5,true);
    router_false.setBit(6,true);

    QBitArray router_temp(32,true);
    router_temp.setBit(28,false);
    router_temp.setBit(29,false);
    router_temp.setBit(30,false);
    router_temp.setBit(31,false);
    int index;
    if(router==router_false)
        index = s_routerTable.indexOf(router_temp);
    else
        index = s_routerTable.indexOf(router);

    if(index == -1) {
            s_routerTable.append(router);
        index = s_routerTable.count() - 1;
    }

    if(router != router_false)
        i2b(m_code, index, 7, 0);
    else
        m_code.fill(true,0,m_code.size());

    printRcuBinCode("lfsr ");
    return true;
}

int CLfsrElement::getCodePos() const
{
    return CODE_LENGTH_BFU * BFU_COUNT +
            CODE_LENGTH_SBOX * 1 +
            CODE_LENGTH_BENES * BENES_COUNT +
            CODE_LENGTH_MEMORY * MEMORY_COUNT +
            CODE_LENGTH_SREG * SREG_COUNT +
            CODE_LENGTH_OUTFIFO * OUTFIFO_COUNT +
            CODE_LENGTH_LFSR * 0;
}
