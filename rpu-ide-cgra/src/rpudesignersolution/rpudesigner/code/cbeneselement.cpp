#include "cbeneselement.h"
#include <QFile>
#include <QTextStream>

CBenesElement::CBenesElement()
{
    m_code.resize(CODE_LENGTH_BENES);
    m_inputSource = {
        {{-1, -1, -1}, -1},
        {{-1, -1, -1}, -1},
        {{-1, -1, -1}, -1},
        {{-1, -1, -1}, -1},
    };
}

void CBenesElement::generateBenesIndex()
{
    for(auto i = 0; i < s_benesPathTable.count(); ++i)
    {
        if(s_benesPathTable.at(i).first == srcConfig)
        {
            s_benesIndexTable.append({s_benesIndexTable.size()+1,s_benesPathTable.at(i).second});
            return;
        }
    }

    QFile file(srcConfig);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //提取benes文件中数据，有效长度为128B
        QBitArray benesConfig(128*8, false);
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
                    i2b(benesConfig, value, 32, writePos);
                    writePos += 32;
                }
            }
        }
        //压缩插入全局benes集合
        bool bHave = false;
        for (auto var : s_benesCompressTable) {
            if(var == benesConfig)
                bHave = true;
        }
        if(!bHave)
            s_benesCompressTable.append(benesConfig);
        //在压缩的全局benes集合中，提取序号
        int iIndex;
        for (int j = 0; j < s_benesCompressTable.size(); ++j) {
            if(s_benesCompressTable.at(j) == benesConfig)
            {
                iIndex = j;
                break;
            }
        }
        //存储序号
        s_benesIndexTable.append({s_benesIndexTable.size(),iIndex});
        s_benesPathTable.append({srcConfig, iIndex});
        file.close();
    }
}

bool CBenesElement::generateCode()
{
    //source 9
    QString strSrc = "src=";
    QBitArray router(32, false);
    for(int i =0; i < m_inputSource.count(); ++i)
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

    i2b(m_code, index, 9, 0);

    //benes
    index = -1;
    for(int i = 0; i < s_benesTable.count(); ++i)
    {
        if(s_benesTable.at(i).first == srcConfig)
        {
            index = i;
            break;
        }
    }
    if(index == -1)
    {
        QFile file(srcConfig);
        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QBitArray benesConfig(832, false);
            QTextStream in(&file);
            int writePos = 0;
            while(!in.atEnd())
            {
                const auto& line = in.readLine().split(" ");
                if(line.count() == 2)
                {
                    bool ok;
                    auto value = line.at(1).toUInt(&ok, 16);
                    if(ok)
                    {
                        i2b(benesConfig, value, 32, writePos, true);
                        writePos += 32;
                    }
                }
            }
            s_benesTable.append({srcConfig, benesConfig});
            index = s_benesTable.count() - 1;
            file.close();
        }
        else
            index = 0;
    }

    QString strCfg = "cfg=";
    QBitArray bitTemp(4,false);
    i2b(bitTemp,index,4,0);
    strCfg+=BinToHex(bitToHexStr(bitTemp));
    setPeCode(strSrc.left(strSrc.length()-1)+", "+strCfg);

    generateBenesIndex();

    i2b(m_code, index, 4, 9);

    printRcuBinCode("benes");
    return true;
}

int CBenesElement::getCodePos() const
{
    return CODE_LENGTH_BFU * BFU_COUNT +
            CODE_LENGTH_SBOX * 1 +
            CODE_LENGTH_BENES * (m_index.peIndex - BFU_COUNT - SBOX_COUNT);
}
