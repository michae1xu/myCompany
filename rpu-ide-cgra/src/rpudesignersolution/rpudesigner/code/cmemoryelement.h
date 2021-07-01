#ifndef CMEMORYELEMENT_H
#define CMEMORYELEMENT_H

#include "cbaseelement.h"

class CMemoryElement : public CBaseElement
{
    friend class CXmlResolver;
public:
    CMemoryElement();

    bool generateCode() override;
    int getCodePos() const override;

    void setDoneCtrl(const QBitArray &bitArray);

private:
    QBitArray m_doneCtrl;

    int writeMemOffset;
    int writeMemMask;
    int writeMemMode;
    int writeMemThreashold;
    int writeMemAddr;

    int readMemOffset;
    int readMemMode;
    int readMemMode128_32;
    int readMemThreashold;
    int readMemAddr1;
    int readMemAddr2;
    int readMemAddr3;
    int readMemAddr4;
};

#endif // CMEMORYELEMENT_H
