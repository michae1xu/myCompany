#ifndef __XMLRESOLVER_H__
#define __XMLRESOLVER_H__
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "xmltype.h"
#include "tinyxml2.h"
using namespace std;
using namespace tinyxml2;
class XmlResolver
{
public:
    XmlResolver(const string &filename);
    ~XmlResolver();
    const string &getPath() const;
    bool isValid() const;
    map<int, string> funcMap;


private:
    int atoi1(const char *a);

private:
    bool valid;
//    map<int,string> inputTypeMap; //记录input类型的选择号和字符串的关系
//    map<int,string> bypassMap;
//    map<int,string> sboxModeMap;
//    map<int,string> auModMap;
//    map<int,string> memModeMap;

    string filename;
    string path;
    tinyxml2::XMLDocument *document;
    XMLElement *root;
    XMLElement *rca;
    XMLElement *rsm;

    string algrithmName;


public:
    XMLElement *findBCUElement(int bcuid);
    XMLElement *findRCUMemWTElement(int bcuid, int rcuid);
    XMLElement *findRCUMemRDElement(int bcuid, int rcuid);
    XMLElement *findRCUElement(int bcuid, int rcuid);
    XMLElement *findElement(int bcuid, int rcuid, ElementType element, int id = 0);
    XMLElement *findBFUInput(int bcuid, int rcuid, int id, Input_Type type);
    XMLElement *findSBOXInput(int bcuid, int rcuid, int id);
    XMLElement *findBENESInput(int bcuid, int rcuid, int inputid);
    XMLElement *findSBOXCnt(int bcuid, int rcuid, int sboxid, int cnt);
	//rch
	XMLElement *findRCURCHINPUTElememt(int bcuid, int rcuid,int id);
	XMLElement *findRCURCHlememt(int bcuid, int rcuid);
    //LSFR
    XMLElement *findRCULFSRINPUTElememt(int bcuid, int rcuid, int inputid);

    XMLElement *getRcaNode();

    XMLElement *findRCUMEMElement(int bcuid, int rcuid, int memputid);
    XMLElement *findBCUOUTElement(int bcuid, int rcuid, int outputid);
public:
    bool isElementExist(int bcuid, int rcuid, ElementType element, int id);
    void removeElement(int bcuid, int rcuid, ElementType element, int id);
    int getValue(XMLElement *ele, AttributeID attr);

private:
    void initNeededMap();
public:
    int getRsm(AttributeID attr, int defaultValue = 0);
    int getElementAttributeValue(int bcuid, int rcuid, ElementType element, int elementid, AttributeID attrID, int defaultValue = 0);
    const char *getElementAttributeStr(int bcuid, int rcuid, int elemenid, AttributeID attrID);
    void save();

};

#endif // XMLRESOLVER_H
