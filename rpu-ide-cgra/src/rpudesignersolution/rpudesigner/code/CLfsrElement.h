#ifndef CLFSRELEMENT_H
#define CLFSRELEMENT_H

#include "cbaseelement.h"

class CLfsrElement : public CBaseElement
{
    friend class CXmlResolver;
public:
    CLfsrElement();

    bool generateCode() override;
    int getCodePos() const override;
};

#endif // CLFSRELEMENT_H
