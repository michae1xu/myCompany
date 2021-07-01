#ifndef CSREGELEMENT_H
#define CSREGELEMENT_H

#include "cbaseelement.h"

class CSRegElement : public CBaseElement
{
    friend class CXmlResolver;
public:
    CSRegElement();

    bool generateCode() override;
    int getCodePos() const override;

private:
    int rchMode;
    int writeRchAddr1 = 0;
    int writeRchAddr2 = 0;
    int writeRchAddr3 = 0;
    int writeRchAddr4 = 0;
};

#endif // CSREGELEMENT_H
