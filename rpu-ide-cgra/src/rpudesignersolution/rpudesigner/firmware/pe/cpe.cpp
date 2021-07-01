#include "cpe.h"

CPE::CPE(int bcu, int rcu, int id, int type):
    m_bcu(bcu),
    m_rcu(rcu),
    m_id(id),
    m_peType((EPeType)type)
{
    for(int i = 0; i < 4; ++i){
        m_inputType[i] = EInputNull;
        m_input[i] = nullptr;
        m_output[i] = new unsigned int;
    }
}

CPE::~CPE()
{
    for(int i = 0; i < 4; ++i){
        delete m_output[i];
    }
}

void CPE::refreshAllValue(uint32_t *allPeValue)
{
    for(int i = 0; i < 4; ++i){
        *m_output[i] = allPeValue[i];
    }
    if(m_peType == EPeBfu)
    {
        uint32_t tmp = *m_output[0];
        *m_output[0] = *m_output[1];
        *m_output[1] = tmp;
    }
}

void CPE::updatePeValue(uint32_t *value)
{
    for(int i = 0; i < 4; ++i){
        *m_output[i] = value[i];
    }
}

QString CPE::completeLength(const uint32_t value)
{
    QString str = QString::number(value, 16).toLower();
    int length = 8 - str.length();
    for(int i = 0; i < length; ++i)
    {
        str.insert(0, "0");
    }
    return str;
}

CBfu::CBfu(int bcu, int rcu, int id) :
    CPE(bcu, rcu, id, EPeBfu)
{
}

QString CBfu::getValue()
{
    //BFU  (0,0,0):0 [00000000,00000000,00000000] [00000000,00000000]
    QString str = QString("BFU  (%0,%1,%2):1 [")
            .arg(m_bcu).arg(m_rcu).arg(m_id);
    for(int i = 0; i < 3; ++i)
    {
        if(m_input[i])
            str += QString("%0,").arg(completeLength(*m_input[i]));
        else if(m_inputType[i] == EInFifo)
            str += QString("InFifo,");
        else if(m_inputType[i] == EMemory)
            str += QString("Memory,");
        else
            str += QString("00000000,");
    }
    str.chop(1);
    str += QString("] [%0,%1]").arg(completeLength(*m_output[0]))
            .arg(completeLength(*m_output[1]));
    return str;
}

CSbox::CSbox(int bcu, int rcu, int id) :
    CPE(bcu, rcu, id, EPeSbox)
{
    m_peType = EPeSbox;
}

QString CSbox::getValue()
{
    //SBOX  (0,0,0):0 [00000000] [00000000,00000000,00000000,00000000]
    QString str = QString("SBOX  (%0,%1,%2):1 [")
            .arg(m_bcu).arg(m_rcu).arg(m_id - 8);
    if(m_input[0])
        str += completeLength(*m_input[0]);
    else if(m_inputType[0] == EInFifo)
        str += QString("InFifo,");
    else if(m_inputType[0] == EMemory)
        str += QString("Memory,");
    else
        str += QString("00000000");

    str += QString("] [%0,%1,%2,%3]")
            .arg(completeLength(*m_output[0]))
            .arg(completeLength(*m_output[1]))
            .arg(completeLength(*m_output[2]))
            .arg(completeLength(*m_output[3]));
    return str;
}

CBenes::CBenes(int bcu, int rcu, int id) :
    CPE(bcu, rcu, id, EPeBenes)
{
    m_peType = EPeBenes;
}

QString CBenes::getValue()
{
    //BENES  (0,0,0):0 [00000000,00000000,00000000,00000000] [00000000,00000000,00000000,00000000]
    QString str = QString("BENES  (%0,%1,%2):1 [")
            .arg(m_bcu).arg(m_rcu).arg(m_id - 12);
    for(int i = 0; i < 4; ++i)
    {
        if(m_input[i])
            str += QString("%0,").arg(completeLength(*m_input[i]));
        else if(m_inputType[i] == EInFifo)
            str += QString("InFifo,");
        else if(m_inputType[i] == EMemory)
            str += QString("Memory,");
        else
            str += QString("00000000,");
    }
    str.chop(1);
    str += QString("] [%0,%1,%2,%3]")
            .arg(completeLength(*m_output[0]))
            .arg(completeLength(*m_output[1]))
            .arg(completeLength(*m_output[2]))
            .arg(completeLength(*m_output[3]));
    return str;
}

