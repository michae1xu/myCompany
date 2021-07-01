#ifndef CSBOXELEMENT_H
#define CSBOXELEMENT_H

#include "cbaseelement.h"

class CSboxElement : public CBaseElement
{
    friend class CXmlResolver;
public:
    CSboxElement();

    bool generateCode() override;
    int getCodePos() const override;

private:
    QList<QString> srcConfig = {"", "", "", ""};
    QList<int> sboxConfigIndex = {0, 0, 0, 0};
    QList<int> sboxMode = {0, 0, 0, 0};
    QList<int> sboxGroup = {0, 0, 0, 0};
    QList<int> sboxByte = {0, 0, 0, 0};

    void generateSboxIndex(int iSbox,sboxCIndex& sboxCI,const QString &sboxSrc) const;
    int generateSbox(const QString &sboxSrc) const;
};

#endif // CSBOXELEMENT_H
