#ifndef CPE_H
#define CPE_H

#include <QString>

class CPE
{
public:
    CPE(int bcu, int rcu, int id, int type);
    virtual ~CPE();
    enum EPeType {EPeBfu, EPeSbox, EPeBenes};
    enum EInputType {EInputNull, ECurRowPe, ELastRowPe, EInFifo, EMemory};

    int m_bcu,m_rcu,m_id;
    unsigned int *m_input[4];
    EInputType m_inputType[4];
    unsigned int *m_output[4];
    EPeType m_peType;

    virtual QString getValue() = 0;
    virtual void refreshAllValue(uint32_t *allPeValue);
    virtual void updatePeValue(uint32_t *value);
    QString completeLength(const uint32_t value);
};

class CBfu : public CPE
{
public:
    CBfu(int bcu, int rcu, int id);

    QString getValue() override;
};

class CSbox : public CPE
{
public:
    CSbox(int bcu, int rcu, int id);

    QString getValue() override;
};

class CBenes : public CPE
{
public:
    CBenes(int bcu, int rcu, int id);

    QString getValue() override;
};
#endif // CPE_H
