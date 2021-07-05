#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__
class GModule;
class GMonitor;
class GVariable;
typedef unsigned long long U64;
typedef unsigned int U32;
typedef unsigned int U22;
typedef unsigned int U24;
typedef unsigned int U26;
typedef unsigned int U9;
typedef unsigned char U8;
typedef unsigned char U7;
typedef unsigned char U6;
typedef unsigned int U10;
typedef unsigned int U11;
typedef unsigned char U2;
typedef unsigned char U3;
typedef unsigned char U4;
typedef unsigned char U5;
typedef bool U1;
typedef GVariable* VAR;
typedef GVariable* INPUT;
typedef GVariable* OUTPUT;
enum EdgetType
{
    EDGET_POS,  //上升沿触发
    EDGET_NEG
};
typedef struct TrigTable
{
    GModule* module;
    int code;
    int bitIndex;
    int delayTime;
}TrigTable;

#define MONITORDLL "monitor"
#define VARDLL "variable"
#define GETINSTANCE "getInstance"


#define WHEN_L(code,c,delayTime,t) \
    if(!monitor)return;\
    if(t != 0){\
		if(code == c && delayTime == 0) {\
			monitor->addDelayTrigLevel(NULL,this,c,t);\
		}\
	}\
    if(code == c && delayTime == t)

#define WHEN_P(code,c,delayTime,t) \
    if(code == c && delayTime == 0)\
    {\
        if(monitor)monitor->addDelayTrigEdget(NULL,this,c,t,EDGET_POS);\
    }\
    if(code == c && delayTime == t)

#define WHEN_N(code,c,delayTime,t) \
    if(t != 0){\
    if(code == c && delayTime == 0)\
    {\
    if(monitor)monitor->addDelayTrigEdget(NULL,this,c,t,EDGET_NEG);\
    }}\
    if(code == c && delayTime == t)



#endif // TYPEDEF_H
