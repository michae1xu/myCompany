#ifndef CRCUELEMENT_H
#define CRCUELEMENT_H

#include "cbaseelement.h"

class CRcuElement : public CBaseElement
{
    friend class CXmlResolver;
public:
    CRcuElement();

    bool generateCode() override;
    void calcDoneCtrl(const QBitArray &doneCtrl);
    QBitArray calcLastRcuDoneCtrl() const;

    void addBfuElement(CBaseElement *element);
    CBaseElement *findBfu(SIndex sindex);
    void addSboxElement(CBaseElement *element);
    CBaseElement *findSbox(SIndex sindex);
    void addBenesElement(CBaseElement *element);
    CBaseElement *findBenes(SIndex sindex);
    void addMemoryElement(CBaseElement *element);
    CBaseElement *findMemory(SIndex sindex);
    void addSregElement(CBaseElement *element);
    CBaseElement *findSreg(SIndex sindex);
    void addLfsrElement(CBaseElement *element);
    CBaseElement *findLfsr(SIndex sindex);
    void addOutfifoElement(CBaseElement *element);
    CBaseElement *findOutfifo(SIndex sindex);

    void addDefaultElement();
    int getLoopStartEndFlag() const;
    void setLoopStartEndFlag(int value);

    int getLoopTimes() const;
    void setLoopTimes(int value);

    int getLoopStartEndFlag2() const;
    void setLoopStartEndFlag2(int value);

    int getLoopTimes2() const;
    void setLoopTimes2(int value);

private:
    QList<CBaseElement*> m_lfsrList;
    QList<CBaseElement*> m_bfuList;
    QList<CBaseElement*> m_sboxList;
    QList<CBaseElement*> m_benesList;
    QList<CBaseElement*> m_memoryList;
    QList<CBaseElement*> m_sregList;
    QList<CBaseElement*> m_outfifoList;

    QBitArray m_doneCtrl;
    int loopStartEndFlag = 0;
    int loopTimes = 0;
    int loopStartEndFlag2 = 0;
    int loopTimes2 = 0;
};

#endif // CRCUELEMENT_H
