#ifndef __STRHANDLER_H__
#define __STRHANDLER_H__

#include <string>
#include <vector>
#include "../xml/xmltype.h"
#include "./typedef.h"
using namespace std;

class StrHandler
{
public:
    StrHandler();
    ~StrHandler(){}

public:
    static string file_name(const string &p);
    static string relate_file(const string &p);
    static string relate_path(const string &p);
    static string real_path(const string &r, const string &p);
    static string data_str(int data, int w, int radix = 16);
    static string monitor_str(ElementType eletype, int bcu, int rcu, int bfu, int sbox, int benes, int infifo, int outfifo, int input, int output);
    static U32 getUIntFromStr(const string &str, U32 start, U32 end);
    static string getStrByUInt(const vector<U32> &data, unsigned start, unsigned end);
    static string getStrByStrAndUInt(const string &str, const vector<U32> &data, unsigned start, unsigned end);
    static string getStrFromStr(const string &str, unsigned start, unsigned end);
    static bool startWithAll(const string &str, const string &s);
    static bool isNumber(const string &str);
    static U32 strToUInt(const string &str);
    static int strToInt(const string &str);
    static string trimAll(const string &str);
    static string intToStr(U32 value, int radix = 10, int minl = 1);
    static vector<string> split(const string &str, char sp);
    static string replaceAll(const string &str, char b, char c);
    static string genBinStr(int length, char d);
private:

};

#endif // CMDSCANFER_H
