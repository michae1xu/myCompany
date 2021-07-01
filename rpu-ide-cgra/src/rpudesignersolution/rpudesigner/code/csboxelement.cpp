#include "csboxelement.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

CSboxElement::CSboxElement()
{
    m_code.resize(CODE_LENGTH_SBOX);
    m_inputSource = {
        {{-1, -1, -1}, -1},
        {{-1, -1, -1}, -1},
        {{-1, -1, -1}, -1},
        {{-1, -1, -1}, -1},
    };
}

int CSboxElement::generateSbox(const QString& sboxSrc) const
{
    auto index = -1;
    for(auto i = 0; i < s_sboxTable.count(); ++i)
    {
        if(s_sboxTable.at(i).first == sboxSrc)
        {
            index = i;
            break;
        }
    }
    if(index == -1)
    {
        QFile file(sboxSrc);
        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QBitArray sboxConfig(1024*4*32, false);
            QTextStream in(&file);
            auto writePos = 0;
            while(!in.atEnd())
            {
                const auto& line = in.readLine().split(" ");
                if(line.count() == 2)
                {
                    auto ok = false;
                    auto value = line.at(1).toUInt(&ok, 16);
                    if(ok)
                    {
                        i2b(sboxConfig, value, 32, writePos, true);
                        writePos += 32;
                    }
                }
            }
            s_sboxTable.append({sboxSrc, sboxConfig});
            index = s_sboxTable.count() - 1;
            file.close();
        }
    }
    return index;
}

void CSboxElement::generateSboxIndex(int iSbox,sboxCIndex& sboxCI,const QString& sboxSrc) const
{
    singleSboxCI sSboxCI;
    sSboxCI.iSbox = iSbox;
    sSboxCI.iGroup = sboxGroup.at(iSbox);

    for(auto i = 0; i < s_sboxPathTable.count(); ++i)
    {
        if(s_sboxPathTable.at(i).first == sboxSrc)
        {
            sSboxCI.sIndex = s_sboxPathTable.at(i).second;
            if(iSbox == 0)
                sboxCI.sbox0 = sSboxCI;
            else if(iSbox == 1)
                sboxCI.sbox1 = sSboxCI;
            else if(iSbox == 2)
                sboxCI.sbox2 = sSboxCI;
            else if(iSbox == 3)
                sboxCI.sbox3 = sSboxCI;
            return;
        }
    }

    QFile file(sboxSrc);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //提取sbox文件中数据，有效长度为4KB
        QBitArray sboxConfig(4*1024*8, false);
        QTextStream in(&file);
        auto writePos = 0;
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
                    i2b(sboxConfig, value, 32, writePos);
                    writePos += 32;
                    if(writePos == 4*1024*8)
                        break;
                }
            }
        }
        //以1KB分割，压缩插入全局sbox集合
        for(int i = 0;i < 4;i++)
        {
            QBitArray bitArray(1024*8,false);
            for (int j = 0; j < bitArray.size(); ++j)
                bitArray.setBit(j,sboxConfig.at(j+1024*8*i));
            bool bHave = false;
            for (auto var : s_sboxCompressTable) {
                if(var == bitArray)
                    bHave = true;
            }
            if(!bHave)
                s_sboxCompressTable.append(bitArray);
        }
        //以1KB分割，在压缩的全局sbox集合中，提取序号
        sboxIndex sIndex;
        for(int i = 0;i < 4;i++)
        {
            QBitArray bitArray(1024*8,false);
            for (int j = 0; j < bitArray.size(); ++j)
                bitArray.setBit(j, sboxConfig.at(j+1024*8*i));
            for (int j = 0; j < s_sboxCompressTable.size(); ++j) {
                if(s_sboxCompressTable.at(j) == bitArray)
                {
                    if(i == 0)
                    {
                        sIndex.index_0 = j;
                        break;
                    }
                    else if(i == 1)
                    {
                        sIndex.index_1 = j;
                        break;
                    }
                    else if(i == 2)
                    {
                        sIndex.index_2 = j;
                        break;
                    }
                    else if(i == 3)
                    {
                        sIndex.index_3 = j;
                        break;
                    }
                }
            }

        }
        //存储序号
        s_sboxPathTable.append({sboxSrc, sIndex});
        //sboxCompressIndex 赋值
        sSboxCI.sIndex = sIndex;
        if(iSbox == 0)
            sboxCI.sbox0 = sSboxCI;
        else if(iSbox == 1)
            sboxCI.sbox1 = sSboxCI;
        else if(iSbox == 2)
            sboxCI.sbox2 = sSboxCI;
        else if(iSbox == 3)
            sboxCI.sbox3 = sSboxCI;
        file.close();
    }
}

bool CSboxElement::generateCode()
{
    //source 9
    QString strSrc = "src=";
    QBitArray router(32, false);
    for(auto i =0; i < m_inputSource.count(); ++i)
    {
        auto _temp = s_dataSourceCodeMap.value({m_inputSource.at(i).first.peIndex, m_inputSource.at(i).second});
        _temp.setBit(6, m_inputSource.at(i).first.peIndex == -1 || m_index.rcuIndex - m_inputSource.at(i).first.rcuIndex == 1);
        for(int j = 0; j < _temp.count(); ++j)
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
    m_code.fill(0, 0, m_code.count());
    i2b(m_code, index, 9, 0);

    //sbox
    sboxCIndex sboxCI;
    for(auto i = 0; i < srcConfig.count(); ++i)
    {
//        sboxConfigIndex.replace(i, generateSbox(srcConfig.at(i)));
        generateSboxIndex(i,sboxCI,srcConfig.at(i));
    }
    this->setSboxCI(sboxCI);

    QBitArray uncompressCode(24, false);
    for(auto i = 0; i < sboxMode.count(); ++i)
    {
        auto mode = sboxMode.at(i);
        auto group = sboxGroup.at(i);
        auto byte = sboxByte.at(i);
        i2b(uncompressCode, mode,  3, i*6, true);
        i2b(uncompressCode, group, 1, i*6 + 3, true);
        i2b(uncompressCode, byte,  2, i*6 + 3 + 1, true);
    }
    QString strCfg = "cfg=";
    QBitArray bitArray(24,false);
    for (int i = 0; i < 24; ++i)
        bitArray.setBit(i,uncompressCode.at(23-i));
    for (int i = 0; i < bitArray.size()/4; ++i) {
        QBitArray bitTemp(4,false);
        for (int j = 0; j < 4; ++j)
            bitTemp.setBit(j,bitArray.at(i*4+j));
        strCfg+=BinToHex(bitToHexStr(bitTemp));
    }
    setPeCode(strSrc.left(strSrc.length()-1)+", "+strCfg);
    //mode 24
    for(auto i = 0; i < uncompressCode.count(); ++i)
    {
        if(uncompressCode.at(i))
            m_code.setBit(m_code.size()-i-1);
    }

    printRcuBinCode("sbox ");
    return true;
}

int CSboxElement::getCodePos() const
{
    return CODE_LENGTH_BFU * BFU_COUNT +
            CODE_LENGTH_SBOX * (m_index.peIndex - BFU_COUNT);
}
