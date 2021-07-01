#include <QStringList>
#include "cpemodel.h"
#include "pe/cpe.h"
#include "fileresolver/cxmlresolver.h"

CPeModel::CPeModel(QString fileName, int index) :
    m_fileName(fileName),
    m_index(index)
{
    generatePe(fileName);
}

CPeModel::~CPeModel()
{
    for(int bcuid = 0; bcuid < BCUMAX; bcuid++)
    {
        for(int rcuid = 0; rcuid < RCUMAX; rcuid++)
        {
            for(int peid = 0; peid < BFUMAX+SBOXMAX+BENESMAX; peid++)
            {
                delete m_peList[bcuid][rcuid][peid];
            }
        }
    }
}

void CPeModel::updatePeValue(int bcu, int rcu, int id, uint32_t *value)
{
    m_peList[bcu][rcu][id]->updatePeValue(value);
}

void CPeModel::generatePe(const QString &fileName)
{
    CXmlResolver xml(fileName);
    for(int bcuid = 0; bcuid < BCUMAX; bcuid++)
    {
        for(int rcuid = 0; rcuid < RCUMAX; rcuid++)
        {
            for(int peid = 0; peid < BFUMAX; peid++)
            {
                m_peList[bcuid][rcuid][peid] = new CBfu(bcuid, rcuid, peid);
            }
            for(int peid = BFUMAX; peid < SBOXMAX + BFUMAX; peid++)
            {
                m_peList[bcuid][rcuid][peid] = new CSbox(bcuid, rcuid, peid);
            }
            m_peList[bcuid][rcuid][SBOXMAX + BFUMAX] = new CBenes(bcuid, rcuid, SBOXMAX + BFUMAX);
        }
    }

    for(int bcuid = 0; bcuid < BCUMAX; bcuid++)
    {
        for(int rcuid = 0; rcuid < RCUMAX; rcuid++)
        {
            for(int peid = 0; peid < BFUMAX; peid++)
            {
                if(xml.isElementExist(bcuid, rcuid, ELEMENT_BFU, peid))
                {
                    for(int i = 0; i < 3; ++i)
                    {
                        conectPeFrom(&xml, bcuid, rcuid, peid, i, ELEMENT_BFU, BfuInputA_Type + i,(int)BfuInputA_Index + i);
                    }
                }
            }
            for(int peid = BFUMAX; peid < SBOXMAX + BFUMAX; peid++)
            {
                if(xml.isElementExist(bcuid, rcuid, ELEMENT_SBOX, peid - BFUMAX))
                {
                    conectPeFrom(&xml, bcuid, rcuid, peid, 0, ELEMENT_SBOX, (int)SboxInput_Type,(int)SboxInput_Index);
                }
            }
            if(xml.isElementExist(bcuid, rcuid, ELEMENT_BENES, 0))
            {
                for(int i = 0; i < 4; ++i)
                {
                    conectPeFrom(&xml, bcuid, rcuid, SBOXMAX + BFUMAX, i, ELEMENT_BENES, (int)BenesInput0_Type + i,(int)BenesInput0_Index + i);
                }
            }
        }
    }
}

void CPeModel::conectPeFrom(CXmlResolver *xml, int toPeBcuid, int toPeRcuid, int toPePeid, int port, int elementType, int attrType, int attrIndex)
{
    CPE *toPe = m_peList[toPeBcuid][toPeRcuid][toPePeid];
    int fromPeBcuid = 0, fromPeRcuid = 0, fromPePeid = 0;
    int elementid = toPePeid;
    switch (ElementType(elementType)) {
    case ELEMENT_SBOX:
        elementid = elementid - BFUMAX;
        break;
    case ELEMENT_BENES:
        elementid = elementid - BFUMAX - SBOXMAX;
        break;
    default:
        break;
    }
    int inputType = xml->getElementAttributeValue(toPeBcuid, toPeRcuid, ElementType(elementType), elementid, (AttributeID)attrType);
    switch (inputType) {
    case Type0:
        toPe->m_inputType[port] = CPE::EInputNull;
        break;
    case Type1:
    {
        fromPeBcuid = toPeBcuid;
        fromPeRcuid = toPeRcuid;
        int inputIndex = xml->getElementAttributeValue(toPeBcuid, toPeRcuid, ElementType(elementType), elementid, (AttributeID)attrIndex);
        fromPePeid = inputIndex;
        CPE *fromPe = m_peList[fromPeBcuid][fromPeRcuid][fromPePeid];
        toPe->m_input[port] = fromPe->m_output[0];
    }
        break;
    case Type2:
    {
        fromPeBcuid = toPeBcuid;
        fromPeRcuid = toPeRcuid;
        int inputIndex = xml->getElementAttributeValue(toPeBcuid, toPeRcuid, ElementType(elementType), elementid, (AttributeID)attrIndex);
        fromPePeid = inputIndex;
        CPE *fromPe = m_peList[fromPeBcuid][fromPeRcuid][fromPePeid];
        toPe->m_input[port] = fromPe->m_output[1];
    }
        break;
    case Type3:
    {
        fromPeBcuid = toPeBcuid;
        fromPeRcuid = toPeRcuid;
        int inputIndex = xml->getElementAttributeValue(toPeBcuid, toPeRcuid, ElementType(elementType), elementid, (AttributeID)attrIndex);
        fromPePeid = inputIndex / 4 + BFUMAX;
        CPE *fromPe = m_peList[fromPeBcuid][fromPeRcuid][fromPePeid];
        toPe->m_input[port] = fromPe->m_output[inputIndex % 4];
    }
        break;
    case Type4:
    {
        fromPeBcuid = toPeBcuid;
        fromPeRcuid = toPeRcuid;
        fromPePeid = BFUMAX + SBOXMAX;
        int inputIndex = xml->getElementAttributeValue(toPeBcuid, toPeRcuid, ElementType(elementType), elementid, (AttributeID)attrIndex);
        CPE *fromPe = m_peList[fromPeBcuid][fromPeRcuid][fromPePeid];
        toPe->m_input[port] = fromPe->m_output[inputIndex % 4];
    }
        break;
    case Type5:
    {
        if(toPeRcuid == 0)
        {
            fromPeBcuid = xml->getElementAttributeValue(toPeBcuid, toPeRcuid, ElementType(elementType), elementid, BCUROUTER);
            fromPeRcuid = RCUMAX - 1;
        }
        else
        {
            fromPeBcuid = toPeBcuid;
            fromPeRcuid = toPeRcuid - 1;
        }
        int inputIndex = xml->getElementAttributeValue(toPeBcuid, toPeRcuid, ElementType(elementType), elementid, (AttributeID)attrIndex);
        fromPePeid = inputIndex;
        CPE *fromPe = m_peList[fromPeBcuid][fromPeRcuid][fromPePeid];
        toPe->m_input[port] = fromPe->m_output[0];
    }
        break;
    case Type6:
    {
        if(toPeRcuid == 0)
        {
            fromPeBcuid = xml->getElementAttributeValue(toPeBcuid, toPeRcuid, ElementType(elementType), elementid, BCUROUTER);
            fromPeRcuid = RCUMAX - 1;
        }
        else
        {
            fromPeBcuid = toPeBcuid;
            fromPeRcuid = toPeRcuid - 1;
        }
        int inputIndex = xml->getElementAttributeValue(toPeBcuid, toPeRcuid, ElementType(elementType), elementid, (AttributeID)attrIndex);
        fromPePeid = inputIndex;
        CPE *fromPe = m_peList[fromPeBcuid][fromPeRcuid][fromPePeid];
        toPe->m_input[port] = fromPe->m_output[1];
    }
        break;
    case Type7:
    {
        if(toPeRcuid == 0)
        {
            fromPeBcuid = xml->getElementAttributeValue(toPeBcuid, toPeRcuid, ElementType(elementType), elementid, BCUROUTER);
            fromPeRcuid = RCUMAX - 1;
        }
        else
        {
            fromPeBcuid = toPeBcuid;
            fromPeRcuid = toPeRcuid - 1;
        }
        int inputIndex = xml->getElementAttributeValue(toPeBcuid, toPeRcuid, ElementType(elementType), elementid, (AttributeID)attrIndex);
        fromPePeid = inputIndex / 4 + BFUMAX;
        CPE *fromPe = m_peList[fromPeBcuid][fromPeRcuid][fromPePeid];
        toPe->m_input[port] = fromPe->m_output[inputIndex % 4];
    }
        break;
    case Type8:
    {
        if(toPeRcuid == 0)
        {
            fromPeBcuid = xml->getElementAttributeValue(toPeBcuid, toPeRcuid, ElementType(elementType), elementid, BCUROUTER);
            fromPeRcuid = RCUMAX - 1;
        }
        else
        {
            fromPeBcuid = toPeBcuid;
            fromPeRcuid = toPeRcuid - 1;
        }
        fromPePeid = BFUMAX + SBOXMAX;
        int inputIndex = xml->getElementAttributeValue(toPeBcuid, toPeRcuid, ElementType(elementType), elementid, (AttributeID)attrIndex);
        CPE *fromPe = m_peList[fromPeBcuid][fromPeRcuid][fromPePeid];
        toPe->m_input[port] = fromPe->m_output[inputIndex % 4];
    }
        break;
    case Type9:
        toPe->m_inputType[port] = CPE::EInFifo;
        break;
    case Type10:
        toPe->m_inputType[port] = CPE::EMemory;
        break;
    default:
        break;
    }
}

void CPeModel::refreshAllValue(uint32_t *allPeValue)
{
    for(int bcuid = 0; bcuid < BCUMAX; bcuid++)
    {
        for(int rcuid = 0; rcuid < RCUMAX; rcuid++)
        {
            for(int peid = 0; peid < BFUMAX+SBOXMAX+BENESMAX; peid++)
            {
                m_peList[bcuid][rcuid][peid]->refreshAllValue(allPeValue + (bcuid*(RCUMAX*(BFUMAX+SBOXMAX+BENESMAX)) + rcuid*(BFUMAX+SBOXMAX+BENESMAX) + peid) * 4);
            }
        }
    }
}

QStringList CPeModel::getAllValue()
{
    QStringList peValueList;
    for(int bcuid = 0; bcuid < BCUMAX; bcuid++)
    {
        for(int rcuid = 0; rcuid < RCUMAX; rcuid++)
        {
            for(int peid = 0; peid < BFUMAX+SBOXMAX+BENESMAX; peid++)
            {
                peValueList.append(m_peList[bcuid][rcuid][peid]->getValue());
            }
        }
    }
    return peValueList;
}

