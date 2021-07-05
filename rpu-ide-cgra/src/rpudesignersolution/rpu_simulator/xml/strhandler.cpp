#include "strhandler.h"
#include <iostream>
#include <memory.h>
#include <deque>
StrHandler::StrHandler()
{

}

/**
 * @brief StrHandler::file_name
 * @param p
 * @return
 * "dir/file.txt" --> "file"
 */
string StrHandler::file_name(const string &p)
{
    string _t = relate_file(p);
    _t = _t.substr(0, _t.find('.'));
    return _t;
}
/**
 * @brief StrHandler::relate_file
 * @param p
 * @return
 * "dir/file.txt" --> "file.txt"
 */
string StrHandler::relate_file(const string &p)
{
    string::size_type t1 =  p.find_last_of("/");
    string::size_type t2 = p.find_last_of("\\");
    if(t1 == string::npos && t2 == string::npos) return string(p);
    else return (t1+1)>(t2+1)?p.substr(t1+1,p.size()-t1-1):p.substr(t2+1,p.size()-t2-1);
}
/**
 * @brief StrHandler::relate_path
 * @param p
 * @return
 * "dir/file.txt" --> "dir"
 */
string StrHandler::relate_path(const string &p)
{
    string::size_type t1 =  p.find_last_of("/");
    string::size_type t2 = p.find_last_of("\\");
    if(t1 == string::npos && t2 == string::npos) return ".";
    else return (t1+1)>(t2+1)?p.substr(0,t1):p.substr(0,t2);
}

string StrHandler::real_path(const string &r, const string &p)
{
#ifdef WIN32
        if(p.find(":") != string::npos) return p;
#else
        if(p.empty() || p[0] == '/') return p;
#endif
        return r + "/" + p;
}
string StrHandler::data_str(int data, int w, int radix)
{
    string ret = intToStr(data,radix);
    int count = w - ret.size();
    for(int i = 0; i < count; i++)
    {
        ret = "0"+ret;
    }
    return ret;
}
string StrHandler::monitor_str(ElementType eletype, int bcu, int rcu, int bfu, int sbox, int benes, int infifo, int outfifo, int input, int output)
{
    //mes
    string ret("");
    switch(eletype)
    {
    case ELEMENT_BFU:
        ret += "BFU  (" + intToStr(bcu) + "," + intToStr(rcu) + "," +
                intToStr(bfu) + ") ";
        break;
    case ELEMENT_SBOX:
        ret += "SBOX (" + intToStr(bcu) + "," + intToStr(rcu) + "," +
                intToStr(sbox) + ") ";
        break;
    case ELEMENT_BENES:
        ret += "BENES(" + intToStr(bcu) + "," + intToStr(rcu) + "," +
                intToStr(benes) + ") ";
        break;
    case ELEMENT_INFIFO:
        ret += "INFIFO  " + intToStr(infifo);
        break;
    case ELEMENT_OUTFIFO:
        ret += "OUTFIFO " + intToStr(outfifo);
        break;
    default:return ret;
    }
    if(input >= 0) ret += "input: " + intToStr(input);
    else ret += "output: " + intToStr(output);
    return ret;
}

U32 StrHandler::getUIntFromStr(const string &str, U32 start, U32 end)
{
    U32 ret = 0;
    for(unsigned i = start; i <= end && i < str.size(); i++)
    {
        if(str[str.size() - i - 1] == '1') ret |= (1 << (i - start));
    }
    return ret;
}

string StrHandler::getStrByUInt(const vector<U32> &data, unsigned start, unsigned end)
{
    string ret("");
    for(unsigned i = start; i <= end && i < data.size(); i++)
    {
        string tmp("");
        for(int j = 31; j >= 0; j--)
        {
            if(data[i] & (1 << j))
            {
                tmp.push_back('1');
            }
            else tmp.push_back('0');
        }
        ret = tmp + ret;
    }
    return ret;
}

string StrHandler::getStrByStrAndUInt(const string &str, const vector<U32> &data, unsigned start, unsigned end)
{
    return getStrByUInt(data,start,end) + str;
}

string StrHandler::getStrFromStr(const string &str, unsigned start, unsigned end)
{
    if(start >= str.size() || start > end) return "";
    return str.substr((int)(str.size()-end-1)<0?0:(str.size()-end-1) ,end-start+1);
}
bool StrHandler::startWithAll(const string &str, const string &s)
{
    if(str.find(s.c_str()) == 0) return true;
    return false;
}
bool StrHandler::isNumber(const string &str)
{
    string tmp = str;
    int radix = 10;
    if(startWithAll(tmp,"0x"))
    {
        tmp.erase(0,2);
        radix = 16;
    }
    else if(startWithAll(tmp,"0b"))
    {
        tmp.erase(0,2);
        radix = 2;
    }
    for(unsigned i = 0; i < tmp.size(); i++)
    {
        if(radix <= 10 && (tmp[i] < '0' || (tmp[i] > '0'+radix))) return false;
        else
        {
            bool en = isalnum(tmp[i]) && !(tmp[i] > 'F' && tmp[i] <= 'Z') && !(tmp[i] > 'f' && tmp[i] <= 'z');
            if(!en) return false;
        }
    }
    return true;
}
U32 StrHandler::strToUInt(const string &str)
{
    int ret = 0;
    int radix = 10;
    string tmp = str;
    if(startWithAll(tmp,"0x"))
    {
        tmp.erase(0,2);
        radix = 16;
    }
    else if(startWithAll(tmp,"0b"))
    {
        tmp.erase(0,2);
        radix = 2;
    }
    switch(radix)
    {
    case 16:
    {
        for(unsigned i = 0; i < tmp.size(); i++)
        {
            char _c = tmp[i];
            int _n = 0;
            if(_c >= '0' && _c <= 'F')
            {
                _n = _c - '0';
            }
            else if(_c >= 'a' && _c <= 'f')
            {
                _n = _c - 'a' + 10;
            }
            else return 0;
            ret = (ret << 4) + _n;
        }
        break;
    }
    case 2:
    {
        for(unsigned i = 0; i < tmp.size(); i++)
        {
            char _c = tmp[i];
            int _n = 0;
            if(_c >= '0' && _c <= '1')
            {
                _n = _c - '0';
            }
            else return 0;
            ret = (ret << 1) + _n;
        }
        break;
    }
    case 10:
    {
        for(unsigned i = 0; i < tmp.size(); i++)
        {
            char _c = tmp[i];
            int _n = 0;
            if(_c >= '0' && _c <= '9')
            {
                _n = _c - '0';
            }
            else return 0;
            ret = (ret * 10) + _n;
        }
        break;
    }
    default:return 0;
    }
    return ret;
}
string StrHandler::trimAll(const string &str)
{
    string ret("");
    for(unsigned i = 0; i < str.size(); i++)
    {
        char _c = str[i];
        if(_c != ' ') ret.push_back(_c);
        else
        {
            char _b = ' ';
            if(i > 0) _b = str[i-1];
            char _d = ' ';
            if(i < str.size()-1) _d = str[i+1];
            if((isalnum(_b) || _b == '.' || _b == '_') &&
                    (isalnum(_d) || _d == '.' || _d == '_'))
                ret.push_back(_c);
        }
    }
    return ret;
}
int StrHandler::strToInt(const string &str)
{
    return (int)strToUInt(str);
}
vector<string> StrHandler::split(const string &str, char sp)
{
    string tmp = replaceAll(str,'\t',' ');
    vector<string> ret;
    while(tmp.size())
    {
        if(tmp.find_first_of(sp) == string::npos)
        {
            tmp = tmp.substr(0,tmp.find_last_not_of(' ')+1);
            ret.push_back(tmp);
            return ret;
        }

        string _t = tmp.substr(0,tmp.find_first_of(sp));
        tmp.erase(0,tmp.find_first_of(sp)+1);
        tmp.erase(0,tmp.find_first_not_of(' '));
        //去掉最后的空格
        _t = _t.substr(0,_t.find_last_not_of(' ')+1);
        ret.push_back(_t);
    }
    return ret;
}
string StrHandler::replaceAll(const string &str, char b, char c)
{
    string ret("");
    for(unsigned i = 0; i < str.size(); i++)
    {
        char _b = str[i];
        if(_b == b)
        {
            ret.push_back(c);
        }
        else
        {
            ret.push_back(_b);
        }
    }
    return ret;
}

string StrHandler::genBinStr(int length, char d)
{
    char *_ret = new char[length+1];
    memset(_ret,d,length*sizeof(char));
    _ret[length] = '\0';
    return string(_ret);
}
string StrHandler::intToStr(U32 value, int radix, int minl)
{
    deque<char> str;
    while(value)
    {
        int _v = value%radix;
        str.push_front(_v < 10 ? ('0' + _v) : ('a' + _v - 10));
        value /= radix;
    }
    string ret;
    while(!str.empty() || (minl > 0))
    {

        if(minl > (int)str.size()) {
            ret.push_back('0');
        }
        else {
            if(str.empty()) break;
            ret.push_back(str.front());
            str.pop_front();
        }
        minl --;
    }
    return ret;
}


