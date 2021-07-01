#ifndef CBENESELEMENT_H
#define CBENESELEMENT_H

#include "cbaseelement.h"

class CBenesElement : public CBaseElement
{
    friend class CXmlResolver;
public:
    CBenesElement();

    bool generateCode() override;
    int getCodePos() const override;

private:
    QString srcConfig;
    void generateBenesIndex();
};

#endif // CBENESELEMENT_H
