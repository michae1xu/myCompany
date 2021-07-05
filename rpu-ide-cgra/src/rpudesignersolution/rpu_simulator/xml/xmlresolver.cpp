#include "xmlresolver.h"
#include "./strhandler.h"

XmlResolver::XmlResolver(const string &filename):
    valid(false),
    filename(filename),
    root(NULL),rca(NULL),rsm(NULL)
{
    initNeededMap();
    document = new XMLDocument();
    fstream _f;
    _f.open(filename,ios_base::in);
    if(_f.is_open())
    {
        _f.close();
        document->LoadFile(filename.c_str());
        root = document->RootElement();
        if(root)rca = root->FirstChildElement("RCA");
        if(rca)rsm = rca->NextSiblingElement("RSM");
        path = StrHandler::relate_path(filename) + "/";
        valid = true;
    }
    else
    {
        valid = false;
    }
}

bool XmlResolver::isValid() const
{
    return valid;
}

int XmlResolver::atoi1(const char *a)
{
    if(!a) return 0;
    else return atoi(a);
}
const string &XmlResolver::getPath() const
{
    return path;
}
XmlResolver::~XmlResolver()
{
//    document->Clear();
    delete document;
}

XMLElement* XmlResolver::findBCUElement(int bcuid)
{
    if(bcuid >= BCUMAX || !rca)return NULL;
    XMLElement *ele = rca->FirstChildElement("BCU");
    while(ele && atoi1(ele->Attribute("id")) != bcuid)
    {
        ele = ele->NextSiblingElement("BCU");
    }
    return ele;
}
XMLElement* XmlResolver::findRCUMemWTElement(int bcuid, int rcuid)
{
    XMLElement* rcu = findRCUElement(bcuid,rcuid);
    if(bcuid >= BCUMAX || rcuid >= RCUMAX || !rcu)return NULL;
    XMLElement *ele = rcu->FirstChildElement("MEM_WRITE");
    return ele;
}
XMLElement* XmlResolver::findRCUMemRDElement(int bcuid, int rcuid)
{
    XMLElement* rcu = findRCUElement(bcuid,rcuid);
    if(bcuid >= BCUMAX || rcuid >= RCUMAX || !rcu)return NULL;
    XMLElement *ele = rcu->FirstChildElement("MEM_READ");
    return ele;
}




XMLElement* XmlResolver::findRCUElement(int bcuid, int rcuid)
{
    XMLElement* bcu = findBCUElement(bcuid);
    if(bcuid >= BCUMAX || rcuid >= RCUMAX || !bcu)return NULL;
    XMLElement *ele = bcu->FirstChildElement("RCU");
    while(ele && atoi1(ele->Attribute("id")) != rcuid)
    {
        ele = ele->NextSiblingElement("RCU");
    }
    return ele;
}
XMLElement* XmlResolver::findBFUInput(int bcuid, int rcuid, int id, Input_Type type)
{
    XMLElement* bfu = findElement(bcuid,rcuid,ELEMENT_BFU,id);
    if(bcuid >= BCUMAX || rcuid >= RCUMAX || id >= BFUMAX || !bfu)return NULL;
    string tagname = (type==INPUT_A)?"INPUTA":(type==INPUT_B)?"INPUTB":"INPUTT";
    XMLElement *ele = bfu->FirstChildElement(tagname.c_str());
    return ele;
}
XMLElement* XmlResolver::findSBOXInput(int bcuid, int rcuid, int id)
{
    XMLElement* sbox = findElement(bcuid,rcuid,ELEMENT_SBOX,id);
    if(bcuid >= BCUMAX || rcuid >= RCUMAX || id >= SBOXMAX || !sbox)return NULL;
    XMLElement *ele = sbox->FirstChildElement("INPUT");
    return ele;
}
XMLElement* XmlResolver::findBENESInput(int bcuid, int rcuid, int inputid)
{
    XMLElement* benes = findElement(bcuid,rcuid,ELEMENT_BENES);
    if(bcuid >= BCUMAX || rcuid >= RCUMAX || inputid >= BENESINPUTNUM || !benes)return NULL;
    XMLElement* ele = benes->FirstChildElement("INPUT");
    while(ele && atoi1(ele->Attribute("id")) != inputid)
    {
        ele = ele->NextSiblingElement("INPUT");
    }
    return ele;
}


XMLElement *XmlResolver::findRCURCHINPUTElememt(int bcuid, int rcuid, int inputid)
{
	XMLElement* rcu = findRCUElement(bcuid, rcuid);
	if (bcuid >= BCUMAX || rcuid >= RCUMAX || !rcu)return NULL;
	XMLElement *ele = rcu->FirstChildElement("RCH_INPUT");

	while (ele && atoi1(ele->Attribute("id")) != inputid)
	{
		ele = ele->NextSiblingElement("RCH_INPUT");
	}
	return ele;
}


XMLElement *XmlResolver::findRCULFSRINPUTElememt(int bcuid, int rcuid, int inputid)
{
    XMLElement* rcu = findRCUElement(bcuid, rcuid);
    if (bcuid >= BCUMAX || rcuid >= RCUMAX || !rcu)return NULL;
    XMLElement *ele = rcu->FirstChildElement("LFSR_INPUT");

    while (ele && atoi1(ele->Attribute("id")) != inputid)
    {
        ele = ele->NextSiblingElement("LFSR_INPUT");
    }
    return ele;
}




XMLElement *XmlResolver::findRCURCHlememt(int bcuid, int rcuid)
{
	XMLElement* rcu = findRCUElement(bcuid, rcuid);
	if (bcuid >= BCUMAX || rcuid >= RCUMAX || !rcu)return NULL;
	XMLElement *ele = rcu->FirstChildElement("RCH");

	//while (ele && atoi1(ele->Attribute("id")) != inputid)
	//{
	//	ele = ele->NextSiblingElement("RCH_INPUT");
	//}
	return ele;
}




XMLElement* XmlResolver::findSBOXCnt(int bcuid, int rcuid, int sboxid, int cnt)
{
    XMLElement* sbox = findElement(bcuid,rcuid,ELEMENT_SBOX,sboxid);
    if(bcuid >= BCUMAX || rcuid >= RCUMAX || sboxid >= SBOXMAX || !sbox)return NULL;
    XMLElement *ele = sbox->FirstChildElement("CNT");
    while(ele && atoi1(ele->Attribute("id")) != cnt)
    {
        ele = ele->NextSiblingElement("CNT");
    }
    return ele;
}

XMLElement* XmlResolver::findElement(int bcuid, int rcuid, ElementType element, int id)
{
    XMLElement* rcu = findRCUElement(bcuid,rcuid);
    int max = (element==ELEMENT_BFU)?BFUMAX:((element==ELEMENT_SBOX)?SBOXMAX:BENESMAX);
    if(bcuid >= BCUMAX || rcuid >= RCUMAX || id >= max || !rcu)return NULL;

    string name = element==ELEMENT_BFU?"BFU":
                   element==ELEMENT_SBOX?"SBOX":
                   element==ELEMENT_BENES?"BENES":"";
    if(name.empty())
    {
        return NULL;
    }
    XMLElement *ele = rcu->FirstChildElement(name.c_str());
    while(ele && atoi1(ele->Attribute("id")) != id)
    {
        ele = ele->NextSiblingElement(name.c_str());
    }
    return ele;
}

XMLElement* XmlResolver::findRCUMEMElement(int bcuid, int rcuid, int memputid)
{
    XMLElement* rcu = findRCUElement(bcuid,rcuid);
    if(bcuid >= BCUMAX || rcuid >= RCUMAX || !rcu)return NULL;
    XMLElement *ele = rcu->FirstChildElement(TAGRCUMEM);
    while(ele && atoi1(ele->Attribute("id")) != memputid)
    {
        ele = ele->NextSiblingElement(TAGRCUMEM);
    }
    return ele;
}
XMLElement* XmlResolver::findBCUOUTElement(int bcuid,int rcuid,int outputid)
{
	XMLElement* rcu = findRCUElement(bcuid, rcuid);
	if (bcuid >= BCUMAX || rcuid >= RCUMAX || !rcu)return NULL;
	XMLElement *ele = rcu->FirstChildElement(TAGOUTFIFO);
	while (ele && atoi1(ele->Attribute("id")) != outputid)
	{
		ele = ele->NextSiblingElement(TAGOUTFIFO);
	}
	return ele;
}

bool XmlResolver::isElementExist(int bcuid, int rcuid, ElementType element, int id)
{
    return (bool)findElement(bcuid,rcuid,element,id);
}

void XmlResolver::initNeededMap()
{
//    //inputtype
//    inputTypeMap[0] = EMPTYCHOICE;
//    inputTypeMap[1] = CURBFUX;
//    inputTypeMap[2] = CURBFUY;
//    inputTypeMap[3] = CURSBOX;
//    inputTypeMap[4] = CURBENES;
//    inputTypeMap[5] = LASTBFUX;
//    inputTypeMap[6] = LASTBFUY;
//    inputTypeMap[7] = LASTSBOX;
//    inputTypeMap[8] = LASTBENES;
//    inputTypeMap[9] = INFIFO;
//    inputTypeMap[10] = MEMOUT;
//    //bypass
//    bypassMap[0] = BYPASS_A;
//    bypassMap[1] = BYPASS_B;
//    bypassMap[2] = BYPASS_T;
//    bypassMap[3] = BYPASS_b;
    //func
    funcMap[0] = FUNC_SU;
    funcMap[1] = FUNC_LU;
    funcMap[2] = FUNC_AU;
//    //sbox
//    sboxModeMap[0] = "4bit-4bit拼接";
//    sboxModeMap[1] = "6bit-6bit拼接";
//    sboxModeMap[2] = "8bit-8bit拼接";
//    sboxModeMap[3] = "8bit-8bit异或";
//    sboxModeMap[4] = "8bit-32bit";
//    sboxModeMap[5] = "9bit-9bit一路";
//    //bfu_au
//    auModMap[0] = "2^32";
//    auModMap[1] = "2^16";
//    auModMap[2] = "2^8";
//    auModMap[3] = "2^32-1";
//    //mem
//    memModeMap[0] = "固定模式";
//    memModeMap[1] = "累加模式";
//    memModeMap[2] = "阈值累加";
//    memModeMap[3] = "累加&阈值清零";
}

int XmlResolver::getRsm(AttributeID attr, int defaultValue)
{
    switch(attr)
    {
#define GETATTR(attr,str) \
    case attr:\
    {\
        return atoi(rsm->Attribute(str));\
    }
    GETATTR(ONLYRSM,"OnlyRsm")
    GETATTR(RMODE0,"rmode0")
    GETATTR(RMODE1,"rmode1")
    GETATTR(GAP,"gap")
    GETATTR(SOURCE,"source")
    GETATTR(DEST,"dest")
    GETATTR(BURST,"burst")
    GETATTR(LOOP,"loop")
    GETATTR(REPEAT,"repeat")
    GETATTR(BCUSEL,"bcusel")
    default:return defaultValue;
    }
}

XMLElement *XmlResolver::getRcaNode()
{
    return rca;
}

#define _CHCK_NULL_STR(x) if(!(x))return "";
const char *XmlResolver::getElementAttributeStr(int bcuid, int rcuid, int elementid, AttributeID attrID)
{
    switch(attrID)
    {
    case BfuFuncExp:
    {
        _CHCK_NULL_STR(findElement(bcuid,rcuid,ELEMENT_BFU,elementid));
        const char *ret = findElement(bcuid,rcuid,ELEMENT_BFU,elementid)->Attribute("exp");
        if(ret == 0x0)return "";
        return ret;
    }
    case BenesSrc:
    {
        _CHCK_NULL_STR(findElement(bcuid,rcuid,ELEMENT_BENES,elementid));
        const char *ret = findElement(bcuid,rcuid,ELEMENT_BENES,elementid)->Attribute("src");
		if (ret == 0x0)
		{
			return "";
		}
        return ret;
    }
    case SboxSrc:
    {
        _CHCK_NULL_STR(findElement(bcuid,rcuid,ELEMENT_SBOX,elementid));
        const char *ret = findElement(bcuid,rcuid,ELEMENT_SBOX,elementid)->Attribute("src");
		if (ret == 0x0)
		{
			return "";
		}
        return ret;
    }
    case BfuBypassExp:
    {
        _CHCK_NULL_STR(findElement(bcuid,rcuid,ELEMENT_BFU,elementid));
        const char *ret = findElement(bcuid,rcuid,ELEMENT_BFU,elementid)->Attribute("BypassExp");
        if(ret == 0x0)return "";
        return ret;
    }
    default:{cerr << "$warning: cannot find this attrid!" << endl;return "";}
    }
}
#define _CHCK_NULL(x) if(!(x))return 0;
int XmlResolver::getElementAttributeValue(int bcuid, int rcuid, ElementType element, int elementid, AttributeID attrID, int defaultValue)
{
    switch(attrID)
    {
    case PosX:
    {
        _CHCK_NULL(findElement(bcuid,rcuid,element,elementid))
        return atoi1(findElement(bcuid,rcuid,element,elementid)->Attribute("X"));
    }
    case PosY:
    {
        _CHCK_NULL(findElement(bcuid,rcuid,element,elementid))
        return atoi1(findElement(bcuid,rcuid,element,elementid)->Attribute("Y"));
    }
    case BfuFunc:
    {
        _CHCK_NULL(findElement(bcuid,rcuid,ELEMENT_BFU,elementid))
        const char* func = findElement(bcuid,rcuid,ELEMENT_BFU,elementid)->Attribute("func");
        if(func == 0x0)return 0;
        else
        {
            const string f = string(func);
            for(unsigned int i = 0; i<funcMap.size(); i++)
            {
                if(f == funcMap[i])return i;
            }
            return 0;
        }
    }
    case BfuAUMod:
    {
        _CHCK_NULL(findElement(bcuid,rcuid,element,elementid))
        return atoi1(findElement(bcuid,rcuid,element,elementid)->Attribute("mod"));
    }
	case BfuBypass:
	{
        _CHCK_NULL(findElement(bcuid,rcuid,element,elementid))
        return atoi1(findElement(bcuid,rcuid,element,elementid)->Attribute("Bypass"));
	}
    case BfuInputA_Type:
    {
        _CHCK_NULL(findBFUInput(bcuid,rcuid,elementid,INPUT_A))
        return atoi1(findBFUInput(bcuid,rcuid,elementid,INPUT_A)->Attribute("Type"));
    }
    case BfuInputA_Height:
    {
        _CHCK_NULL(findBFUInput(bcuid,rcuid,elementid,INPUT_A))
        return atoi1(findBFUInput(bcuid,rcuid,elementid,INPUT_A)->Attribute("Height"));
    }
    case BfuInputB_Type:
    {
        _CHCK_NULL(findBFUInput(bcuid,rcuid,elementid,INPUT_B))
        return atoi1(findBFUInput(bcuid,rcuid,elementid,INPUT_B)->Attribute("Type"));
    }
    case BfuInputB_Height:
    {
        _CHCK_NULL(findBFUInput(bcuid,rcuid,elementid,INPUT_B))
        return atoi1(findBFUInput(bcuid,rcuid,elementid,INPUT_B)->Attribute("Height"));
    }
    case BfuInputT_Type:
    {
        _CHCK_NULL(findBFUInput(bcuid,rcuid,elementid,INPUT_T))
        return atoi1(findBFUInput(bcuid,rcuid,elementid,INPUT_T)->Attribute("Type"));
    }
    case BfuInputT_Height:
    {
        _CHCK_NULL(findBFUInput(bcuid,rcuid,elementid,INPUT_T))
        return atoi1(findBFUInput(bcuid,rcuid,elementid,INPUT_T)->Attribute("Height"));
    }
    case SboxInput_Type:
    {
        _CHCK_NULL(findSBOXInput(bcuid,rcuid,elementid))
        return atoi1(findSBOXInput(bcuid,rcuid,elementid)->Attribute("Type"));
    }
    case SboxInput_Height:
    {
        _CHCK_NULL(findSBOXInput(bcuid,rcuid,elementid))
        return atoi1(findSBOXInput(bcuid,rcuid,elementid)->Attribute("Height"));
    }
    case BenesInput0_Type:
    {
        _CHCK_NULL(findBENESInput(bcuid,rcuid,0))
        return atoi1(findBENESInput(bcuid,rcuid,0)->Attribute("Type"));
    }
    case BenesInput0_Height:
    {
        _CHCK_NULL(findBENESInput(bcuid,rcuid,0))
        return atoi1(findBENESInput(bcuid,rcuid,0)->Attribute("Height"));
    }
    case BenesInput1_Height:
    {
        _CHCK_NULL(findBENESInput(bcuid,rcuid,1))
        return atoi1(findBENESInput(bcuid,rcuid,1)->Attribute("Height"));
    }
    case BenesInput2_Height:
    {
        _CHCK_NULL(findBENESInput(bcuid,rcuid,2))
        return atoi1(findBENESInput(bcuid,rcuid,2)->Attribute("Height"));
    }
    case BenesInput3_Height:
    {
        _CHCK_NULL(findBENESInput(bcuid,rcuid,3))
        return atoi1(findBENESInput(bcuid,rcuid,3)->Attribute("Height"));
    }
    case BenesInput1_Type:
    {
        _CHCK_NULL(findBENESInput(bcuid,rcuid,1))
        return atoi1(findBENESInput(bcuid,rcuid,1)->Attribute("Type"));
    }
    case BenesInput2_Type:
    {
        _CHCK_NULL(findBENESInput(bcuid,rcuid,2))
        return atoi1(findBENESInput(bcuid,rcuid,2)->Attribute("Type"));
    }
    case BenesInput3_Type:
    {
        _CHCK_NULL(findBENESInput(bcuid,rcuid,3))
        return atoi1(findBENESInput(bcuid,rcuid,3)->Attribute("Type"));
    }
    case RCUMEM0_Type:
    {
        _CHCK_NULL(findRCUMEMElement(bcuid,rcuid,0))
        return atoi1(findRCUMEMElement(bcuid,rcuid,0)->Attribute("Type"));
    }
    case RCUMEM0_Height:
    {
        _CHCK_NULL(findRCUMEMElement(bcuid,rcuid,0))
        return atoi1(findRCUMEMElement(bcuid,rcuid,0)->Attribute("Height"));
    }
    case RCUMEM1_Height:
    {
        _CHCK_NULL(findRCUMEMElement(bcuid,rcuid,1))
        return atoi1(findRCUMEMElement(bcuid,rcuid,1)->Attribute("Height"));
    }
    case RCUMEM2_Height:
    {
        _CHCK_NULL(findRCUMEMElement(bcuid,rcuid,2))
        return atoi1(findRCUMEMElement(bcuid,rcuid,2)->Attribute("Height"));
    }
    case RCUMEM3_Height:
    {
        _CHCK_NULL(findRCUMEMElement(bcuid,rcuid,3))
        return atoi1(findRCUMEMElement(bcuid,rcuid,3)->Attribute("Height"));
    }
    case RCUMEM1_Type:
    {
        _CHCK_NULL(findRCUMEMElement(bcuid,rcuid,1))
        return atoi1(findRCUMEMElement(bcuid,rcuid,1)->Attribute("Type"));
    }
    case RCUMEM2_Type:
    {
        _CHCK_NULL(findRCUMEMElement(bcuid,rcuid,2))
        return atoi1(findRCUMEMElement(bcuid,rcuid,2)->Attribute("Type"));
    }
    case RCUMEM3_Type:
    {
        _CHCK_NULL(findRCUMEMElement(bcuid,rcuid,3))
        return atoi1(findRCUMEMElement(bcuid,rcuid,3)->Attribute("Type"));
    }
    case BCUOUTFIFO0_Type:
    {
        _CHCK_NULL(findBCUOUTElement(bcuid, rcuid,0))
        return atoi1(findBCUOUTElement(bcuid, rcuid, 0)->Attribute("Type"));
    }
    case BCUOUTFIFO0_Height:
    {
        _CHCK_NULL(findBCUOUTElement(bcuid, rcuid, 0))
        return atoi1(findBCUOUTElement(bcuid, rcuid, 0)->Attribute("Height"));
    }
    case BCUOUTFIFO1_Height:
    {
        _CHCK_NULL(findBCUOUTElement(bcuid, rcuid, 1))
        return atoi1(findBCUOUTElement(bcuid, rcuid, 1)->Attribute("Height"));
    }
    case BCUOUTFIFO2_Height:
    {
        _CHCK_NULL(findBCUOUTElement(bcuid, rcuid, 2))
        return atoi1(findBCUOUTElement(bcuid, rcuid, 2)->Attribute("Height"));
    }
    case BCUOUTFIFO3_Height:
    {
        _CHCK_NULL(findBCUOUTElement(bcuid, rcuid, 3))
        return atoi1(findBCUOUTElement(bcuid, rcuid, 3)->Attribute("Height"));
    }
    case BCUOUTFIFO1_Type:
    {
        _CHCK_NULL(findBCUOUTElement(bcuid, rcuid, 1))
        return atoi1(findBCUOUTElement(bcuid, rcuid, 1)->Attribute("Type"));
    }
    case BCUOUTFIFO2_Type:
    {
        _CHCK_NULL(findBCUOUTElement(bcuid, rcuid, 2))
        return atoi1(findBCUOUTElement(bcuid, rcuid, 2)->Attribute("Type"));
    }
    case BCUOUTFIFO3_Type:
    {
        _CHCK_NULL(findBCUOUTElement(bcuid, rcuid, 3))
        return atoi1(findBCUOUTElement(bcuid, rcuid, 3)->Attribute("Type"));
    }
    case BfuInputA_Index:
    {
        _CHCK_NULL(findBFUInput(bcuid,rcuid,elementid,INPUT_A))
        return atoi1(findBFUInput(bcuid,rcuid,elementid,INPUT_A)->Attribute("Index"));
    }
    case BfuInputB_Index:
    {
        _CHCK_NULL(findBFUInput(bcuid,rcuid,elementid,INPUT_B))
        return atoi1(findBFUInput(bcuid,rcuid,elementid,INPUT_B)->Attribute("Index"));
    }
    case BfuInputT_Index:
    {
        _CHCK_NULL(findBFUInput(bcuid,rcuid,elementid,INPUT_T))
        return atoi1(findBFUInput(bcuid,rcuid,elementid,INPUT_T)->Attribute("Index"));
    }
    case SboxInput_Index:
    {
        _CHCK_NULL(findSBOXInput(bcuid,rcuid,elementid))
        return atoi1(findSBOXInput(bcuid,rcuid,elementid)->Attribute("Index"));
    }
    case SboxCNT0:
    {
        _CHCK_NULL(findSBOXCnt(bcuid,rcuid,elementid,0))
        return atoi1(findSBOXCnt(bcuid,rcuid,elementid,0)->Attribute("value"));
    }
    case SboxCNT1:
    {
        _CHCK_NULL(findSBOXCnt(bcuid,rcuid,elementid,1))
        return atoi1(findSBOXCnt(bcuid,rcuid,elementid,1)->Attribute("value"));
    }
    case SboxCNT2:
    {
        _CHCK_NULL(findSBOXCnt(bcuid,rcuid,elementid,2))
        return atoi1(findSBOXCnt(bcuid,rcuid,elementid,2)->Attribute("value"));
    }
    case SboxCNT3:
    {
        _CHCK_NULL(findSBOXCnt(bcuid,rcuid,elementid,3))
        return atoi1(findSBOXCnt(bcuid,rcuid,elementid,3)->Attribute("value"));
    }
    case SboxMode:
    {
        _CHCK_NULL(findElement(bcuid,rcuid,element,elementid))
        return atoi1(findElement(bcuid,rcuid,element,elementid)->Attribute("mode"));
    }
    case BenesInput0_Index:
    {
        _CHCK_NULL(findBENESInput(bcuid,rcuid,0))
        return atoi1(findBENESInput(bcuid,rcuid,0)->Attribute("Index"));
    }
    case BenesInput1_Index:
    {
        _CHCK_NULL(findBENESInput(bcuid,rcuid,1))
        return atoi1(findBENESInput(bcuid,rcuid,1)->Attribute("Index"));
    }
    case BenesInput2_Index:
    {
        _CHCK_NULL(findBENESInput(bcuid,rcuid,2))
        return atoi1(findBENESInput(bcuid,rcuid,2)->Attribute("Index"));
    }
    case BenesInput3_Index:
    {
        _CHCK_NULL(findBENESInput(bcuid,rcuid,3))
        return atoi1(findBENESInput(bcuid,rcuid,3)->Attribute("Index"));
    }
    case RCUMEM0_Index:
    {
        _CHCK_NULL(findRCUMEMElement(bcuid,rcuid,0))
        return atoi1(findRCUMEMElement(bcuid,rcuid,0)->Attribute("Index"));
    }
    case RCUMEM1_Index:
    {
        _CHCK_NULL(findRCUMEMElement(bcuid,rcuid,1))
        return atoi1(findRCUMEMElement(bcuid,rcuid,1)->Attribute("Index"));
    }
    case RCUMEM2_Index:
    {
        _CHCK_NULL(findRCUMEMElement(bcuid,rcuid,2))
        return atoi1(findRCUMEMElement(bcuid,rcuid,2)->Attribute("Index"));
    }
    case RCUMEM3_Index:
    {
        _CHCK_NULL(findRCUMEMElement(bcuid,rcuid,3))
        return atoi1(findRCUMEMElement(bcuid,rcuid,3)->Attribute("Index"));
    }
    case BCUOUTFIFO0_Index:
    {
        _CHCK_NULL(findBCUOUTElement(bcuid, rcuid, 0))
        return atoi1(findBCUOUTElement(bcuid, rcuid, 0)->Attribute("Index"));
    }
    case BCUOUTFIFO1_Index:
    {
        _CHCK_NULL(findBCUOUTElement(bcuid, rcuid, 1))
        return atoi1(findBCUOUTElement(bcuid, rcuid, 1)->Attribute("Index"));
    }
    case BCUOUTFIFO2_Index:
    {
        _CHCK_NULL(findBCUOUTElement(bcuid, rcuid, 2))
        return atoi1(findBCUOUTElement(bcuid, rcuid, 2)->Attribute("Index"));
    }
    case BCUOUTFIFO3_Index:
    {
        _CHCK_NULL(findBCUOUTElement(bcuid, rcuid, 3))
        return atoi1(findBCUOUTElement(bcuid, rcuid, 3)->Attribute("Index"));
    }
    case BCUROUTER:
    {
        _CHCK_NULL(findBCUElement(bcuid))
        return atoi1(findBCUElement(bcuid)->Attribute("router"));
    }
    case RCUMEMWTMODE:
    {
        _CHCK_NULL(findRCUMemWTElement(bcuid,rcuid))
        return atoi1(findRCUMemWTElement(bcuid,rcuid)->Attribute("mode"));
    }
    case RCUMEMWTADDR:
    {
        _CHCK_NULL(findRCUMemWTElement(bcuid,rcuid))
        return atoi1(findRCUMemWTElement(bcuid,rcuid)->Attribute("addr"));
    }
    case RCUMEMWTMASK:
    {
        _CHCK_NULL(findRCUMemWTElement(bcuid,rcuid))
        return atoi1(findRCUMemWTElement(bcuid,rcuid)->Attribute("mask"));
    }
    case RCUMEMWTOFFSET:
    {
        _CHCK_NULL(findRCUMemWTElement(bcuid,rcuid))
        return atoi1(findRCUMemWTElement(bcuid,rcuid)->Attribute("offset"));
    }
    case RCUMEMWTTHREASHOLD:
    {
        _CHCK_NULL(findRCUMemWTElement(bcuid,rcuid))
        return atoi1(findRCUMemWTElement(bcuid,rcuid)->Attribute("threashold"));
    }
    case RCUMEMRDMODE128_32:
    {
        _CHCK_NULL(findRCUMemRDElement(bcuid,rcuid))
        return atoi1(findRCUMemRDElement(bcuid,rcuid)->Attribute("mode128_32"));
    }
    case RCUMEMRDMODE:
    {
        _CHCK_NULL(findRCUMemRDElement(bcuid,rcuid))
        return atoi1(findRCUMemRDElement(bcuid,rcuid)->Attribute("mode"));
    }
    case RCUMEMRDADDR1:
    {
        _CHCK_NULL(findRCUMemRDElement(bcuid,rcuid))
        return atoi1(findRCUMemRDElement(bcuid,rcuid)->Attribute("addr1"));
    }
    case RCUMEMRDADDR2:
    {
        _CHCK_NULL(findRCUMemRDElement(bcuid,rcuid))
        return atoi1(findRCUMemRDElement(bcuid,rcuid)->Attribute("addr2"));
    }
    case RCUMEMRDADDR3:
    {
        _CHCK_NULL(findRCUMemRDElement(bcuid,rcuid))
        return atoi1(findRCUMemRDElement(bcuid,rcuid)->Attribute("addr3"));
    }
    case RCUMEMRDADDR4:
    {
        _CHCK_NULL(findRCUMemRDElement(bcuid,rcuid))
        return atoi1(findRCUMemRDElement(bcuid,rcuid)->Attribute("addr4"));
    }
    case RCUMEMRDOFFSET:
    {
        _CHCK_NULL(findRCUMemRDElement(bcuid,rcuid))
        return atoi1(findRCUMemRDElement(bcuid,rcuid)->Attribute("offset"));
    }
    case RCUMEMRDTHREASHOLD:
    {
        _CHCK_NULL(findRCUMemRDElement(bcuid,rcuid))
        return atoi1(findRCUMemRDElement(bcuid,rcuid)->Attribute("threashold"));
    }
	case RCURCH_Type:
		_CHCK_NULL(findRCURCHINPUTElememt(bcuid,rcuid,elementid));
		return atoi1(findRCURCHINPUTElememt(bcuid, rcuid, elementid)->Attribute("Type"));

	case RCURCH_Index:
		_CHCK_NULL(findRCURCHINPUTElememt(bcuid, rcuid, elementid));
		return atoi1(findRCURCHINPUTElememt(bcuid, rcuid, elementid)->Attribute("Index"));

	case RCURCH_Mode:
		_CHCK_NULL(findRCURCHlememt(bcuid, rcuid));
		return atoi1(findRCURCHlememt(bcuid, rcuid)->Attribute("mode"));
	case RCURCH_DEST0:
		_CHCK_NULL(findRCURCHINPUTElememt(bcuid, rcuid, defaultValue));
		return atoi1(findRCURCHINPUTElememt(bcuid, rcuid, defaultValue)->Attribute("Address"));
	case RCURCH_DEST1:
		_CHCK_NULL(findRCURCHINPUTElememt(bcuid, rcuid, defaultValue));
		return atoi1(findRCURCHINPUTElememt(bcuid, rcuid, defaultValue)->Attribute("Address"));
	case RCURCH_DEST2:
		_CHCK_NULL(findRCURCHINPUTElememt(bcuid, rcuid, defaultValue));
		return atoi1(findRCURCHINPUTElememt(bcuid, rcuid, defaultValue)->Attribute("Address"));
	case RCURCH_DEST3:
		_CHCK_NULL(findRCURCHINPUTElememt(bcuid, rcuid, defaultValue));
		return atoi1(findRCURCHINPUTElememt(bcuid, rcuid, defaultValue)->Attribute("Address"));
    case LFSR:
        _CHCK_NULL(findRCULFSRINPUTElememt(bcuid, rcuid, elementid));
        return atoi1(findRCULFSRINPUTElememt(bcuid, rcuid, elementid)->Attribute("Type"));
    case LFSR_INDEX:
        _CHCK_NULL(findRCULFSRINPUTElememt(bcuid, rcuid, elementid));
        return atoi1(findRCULFSRINPUTElememt(bcuid, rcuid, elementid)->Attribute("Index"));

    default:{cerr << "$warning: cannot find this attrid!" << endl;return defaultValue;}
    }
}


