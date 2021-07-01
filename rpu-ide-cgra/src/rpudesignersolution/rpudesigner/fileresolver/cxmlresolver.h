#ifndef CXMLRESOLVER_H
#define CXMLRESOLVER_H

#include <QObject>
#include <QDomElement>
#include <QMap>
#include "typedef.h"
//#include "datafile.h"
#include "ModuleBcu.h"
#include "ModuleRcu.h"
#include "ModuleReadImd.h"
#include "ModuleReadRch.h"
#include "ElementBfu.h"
#include "ElementSbox.h"
#include "ElementBenes.h"
#include "RcaGraphScene.h"
#include "RcaGraphDef.h"
#include "code/ccode.h"

using namespace rca_space;

class QDomDocument;
class QFile;

class CXmlResolver : public QObject
{
    Q_OBJECT
public:
    explicit CXmlResolver(const QString &filename);
    ~CXmlResolver();
    QString m_curFile;
    QFile *m_file;
    QDomDocument *m_document;
    QDomElement m_root;
    QDomElement m_rca;
    QDomElement m_rsm;
    QDomElement m_uiVer;
    bool isAutoResetPos;    //是否需要重新排版，如果版本不同，则重新排版。
//    QDomElement m_para; //modified by zhangjun at 2016/8/30
//    SParaInfo *m_paraInfo;
//    SCMDWordInfo *m_cmdWordInfo;

//    QString m_algrithmName;
    QMap<int,QString> m_inputTypeMap; //记录input类型的选择号和字符串的关系
    QMap<int,QString> m_bypassMap;
    QMap<int,QString> m_funcMap;
    QMap<int,QString> m_sboxModeMap;
    QMap<int,QString> m_auModMap;
    QMap<int,QString> m_memModeMap;

    //命令行参数配置
//    QDomElement findCycleElement();
//    QDomElement findIntervalElement();
//    QDomElement findFifoElement();
//    QDomElement findFifoInput(int id);
//    QDomElement findFifoOutput(int id);
//    QDomElement findMemElement();
//    QDomElement findMemInput(int id);
//    QDomElement findMemOutput(int id);
//    QDomElement getCodeElement();
//    QDomElement getCodeChildElement(QString childName);
//    QDomElement getCodeSourceElement();
//    QDomElement getCodeDestElement();

//    void initParaInfo();  //从xml文件初始化命令行参数
//    SParaInfo *getParaInfo();
//    SCMDWordInfo *getCmdWordInfo();
//    SXmlParameter *getXmlParameter(){return m_xmlParameter;}
//    void setParaElementAttribute();

    QDomElement findBCUElement(int bcuid);
    QDomElement findRCULoopElement(int bcuid, int rcuid);
    QDomElement findRCUMemWTElement(int bcuid, int rcuid);
    QDomElement findRCUMemRDElement(int bcuid, int rcuid);
    QDomElement findRCURchElement(int bcuid, int rcuid);
    QDomElement findRCUElement(int bcuid, int rcuid);
    QDomElement findElement(int bcuid, int rcuid, ElementType element, int id = 0);
    QDomElement findBFUInput(int bcuid, int rcuid, int id, Input_Type type);
    QDomElement findCUSTOMPEInput(int bcuid, int rcuid, int elementid, int inputid);
    QDomElement findSBOXInput(int bcuid, int rcuid, int id);
    QDomElement findBENESInput(int bcuid, int rcuid, int inputid);
    QDomElement findSBOXCnt(int bcuid, int rcuid, int sboxid, int cnt);


    bool isHadBcuInXml(int bcuid);
    bool isHadRcuInXml(int bcuid,int rcuid);
    bool isHadWriteMemInXml(int bcuid, int rcuid, int memputid);
    bool isHadWriteRchInXml(int bcuid, int rcuid, int memputid);
    bool isHadWriteLfsrInXml(int bcuid, int rcuid, int memputid);
    QDomElement findRCUMEMElement(int bcuid, int rcuid, int memputid);
    QDomElement findRCURCHElement(int bcuid, int rcuid, int id);
    QDomElement findRCULFSRElement(int bcuid, int rcuid, int id);
    QDomElement findRCUOUTElement(int bcuid, int rcuid, int outputid);
//    QDomElement findBCUOUTElement(int bcuid, int outputid);
    QDomElement findPortInfelctPos(QDomElement* domElement);
    QDomElement findPortPressedPos(QDomElement* domElement);

    bool isElementExist(int bcuid, int rcuid, ElementType element, int id);
    void removeElement(int bcuid, int rcuid, ElementType element, int id);
    void removeBcuElement(int bcuid);
    void removeRcuElement(int bcuid, int rcuid);
    void removeRcuMemElemtnt(int bcuid, int rcuid, int memputid);
    int getValue(QDomElement ele, AttributeID attr);

    bool initXmlToCode(CCode &code, const SProjectParameter *projectParameter);
    bool initXmlToScene(RcaGraphScene &rcaScene);   //版本与当前版本不同，则需要重新排版，返回true.
    bool initLogToScene(RcaGraphScene &rcaScene, const QString &fileName);

    void cleanRcaDomElement();
    QString getCurFile() const;

private:
    void initNeededMap();
    void readXmlToPort(ElementPort* port, QDomElement* portXml);
//    SXmlParameter *m_xmlParameter;

//signals:
//    void closeFinished(QString);


public slots:
    int getRsm(AttributeID attr, int defaultValue = 0);
    void setRsm(AttributeID attr,int value);
    void setInputPortToDom(QDomElement* eleInPort,ElementPort* port);
    void setBcuElementAttribute(ModuleBcu* bcuElement);
    void setRcuElementAttribute(ModuleRcu* rcuElement);
    void setArithUnitElementAttribute(BaseArithUnit* arithUnit);
    void setBfuElementAttribute(ElementBfu* bfuElement);
    void setCustomPEElementAttribute(ElementCustomPE* customPEElement);
    void setSboxElementAttribute(ElementSbox* sboxElement);
    void setBenesElementAttribute(ElementBenes* benesElement);
    void setWriteMemAttribute(int bcuIndex,int rcuIndex,ModuleWriteMem* writeMem);
    void setWriteRchAttribute(int bcuIndex,int rcuIndex,ModuleWriteRch* writeRch);
    void setWriteLfsrAttribute(int bcuIndex,int rcuIndex,ModuleWriteLfsr* writeLfsr);
    void setRchAttribute(int bcuIndex,int rcuIndex,ModuleWriteRch* writeRch);

    void setElementAttributeDouble(int bcuid, int rcuid, ElementType element, int elementid, AttributeID attrID, double value);
    void setElementAttribute(int bcuid, int rcuid, ElementType element, int elementid, AttributeID attrID, int value);
    void setElementAttributeStr(int bcuid, int rcuid, ElementType element, int elementid, AttributeID attrID, QString value);
    int getElementAttributeValue(int bcuid, int rcuid, ElementType element, int elementid, AttributeID attrID, int defaultValue = 0);
    double getElementAttributeValueDouble(int bcuid, int rcuid, ElementType element, int elementid, AttributeID attrID, int defaultValue = 0);
    QString getElementAttributeStr(int bcuid, int rcuid, ElementType element, int elemenid, AttributeID attrID, QString defaultValue = "");
    bool save();
    bool saveAs(const QString &fileName);

};

#endif // CXMLRESOLVER_H
