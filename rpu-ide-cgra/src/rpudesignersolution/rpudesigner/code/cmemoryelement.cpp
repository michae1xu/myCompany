#include "cmemoryelement.h"

CMemoryElement::CMemoryElement()
{
    m_code.resize(CODE_LENGTH_MEMORY);
    m_inputSource = {
        {{-1, -1, -1}, -1},
        {{-1, -1, -1}, -1},
        {{-1, -1, -1}, -1},
        {{-1, -1, -1}, -1},
    };
}

bool CMemoryElement::generateCode()
{
    //source 9
    QBitArray router(32, false);
    for(auto i = 0; i < m_inputSource.count(); ++i)
    {
        auto _temp = s_dataSourceCodeMap.value({m_inputSource.at(i).first.peIndex, m_inputSource.at(i).second});
        _temp.setBit(6, m_inputSource.at(i).first.peIndex == -1 || m_index.rcuIndex - m_inputSource.at(i).first.rcuIndex == 1);
        for(int j = 0; j < _temp.count(); ++j)
        {
            if(_temp.at(j))
                router.setBit(i * _temp.count() + j);
        }
    }

    auto index = s_routerTable.indexOf(router);
    if(index == -1) {
        s_routerTable.append(router);
        index = s_routerTable.count() - 1;
    }

    i2b(m_code, index, 9, 0);

    //mem
    QBitArray memoryConfig(104, false);

    concact(memoryConfig, readMemMode128_32==0?1:0,1);
    concact(memoryConfig, readMemMode,       2,true);
    concact(memoryConfig, readMemAddr1,     10,true);
    concact(memoryConfig, readMemAddr2,     10,true);
    concact(memoryConfig, readMemAddr3,     10,true);
    concact(memoryConfig, readMemAddr4,     10,true);
    concact(memoryConfig, readMemOffset,     8,true);
    concact(memoryConfig, readMemThreashold, 8,true);

    concact(memoryConfig, writeMemMode,       2,true);
    concact(memoryConfig, writeMemAddr,       8,true);
    concact(memoryConfig, writeMemMask,       4,true);
    concact(memoryConfig, writeMemOffset,     8,true);
    concact(memoryConfig, writeMemThreashold, 8,true);

    for(auto i = 0; i < m_doneCtrl.count(); ++i)
    {
        if(m_doneCtrl.at(i))
            memoryConfig.setBit(89+i);
    }

    index = s_memoryTable.indexOf(memoryConfig);
    if(index == -1) {
        s_memoryTable.append(memoryConfig);
        index = s_memoryTable.count() - 1;
    }
    //mode 10
    i2b(m_code, index, 10, 9);

    printRcuBinCode("mem  ");
    return true;
}

int CMemoryElement::getCodePos() const
{
    return CODE_LENGTH_BFU * BFU_COUNT +
            CODE_LENGTH_SBOX * 1 +
            CODE_LENGTH_BENES * BENES_COUNT +
            CODE_LENGTH_MEMORY * 0;
}

void CMemoryElement::setDoneCtrl(const QBitArray &bitArray)
{
    m_doneCtrl = bitArray;
}
