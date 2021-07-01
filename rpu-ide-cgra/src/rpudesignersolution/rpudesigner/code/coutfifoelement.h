#ifndef COUTFIFOELEMENT_H
#define COUTFIFOELEMENT_H

#include "cbaseelement.h"

class COutfifoElement : public CBaseElement
{
    friend class CXmlResolver;
public:
    COutfifoElement();

    bool generateCode() override;
    int getCodePos() const override;

private:
};

#endif // COUTFIFOELEMENT_H
