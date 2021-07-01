#ifndef CBFUELEMENT_H
#define CBFUELEMENT_H

#include "cbaseelement.h"

class CBfuElement : public CBaseElement
{
    friend class CXmlResolver;
    friend class CRcuElement;
public:
    enum BfuFuncIndex {
        Func_Au_Index,
        Func_Su_Index,
        Func_Lu_Index,
        Func_Mu_Index,
    };

    CBfuElement();

    int bfuIndex = 0;
    bool generateCode() override;
    int getCodePos() const override;

private:
    BfuFuncIndex funcIndex = Func_Au_Index;
    QString funcExp = "";
    int funcAuModIndex = 0;
    bool funcAuCarry = false; // remove
    int funcMuModIndex = 0;
    int bypassIndex = 0;
    QString bypassExp;
};

#endif // CBFUELEMENT_H
