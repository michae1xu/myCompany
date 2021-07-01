#include "basepe.h"
#include <map>
#include <queue>
#include <stack>
#include <QFile>
#include <QDir>

using namespace std;

namespace {
class Math
{
public:
    static U32 calByLbc(U32 A, U32 B, LBCType lbc)
    {
        switch(lbc)
        {
        case LBC_A:
            return A;
        case LBC_NA:
            return ~A;
        case LBC_AXORB:
            return (A ^ B);
        case LBC_AANDB:
            return (A & B);
        case LBC_AORB:
            return (A | B);
        case LBC_NAXORB:
            return (~A ^ B);
        case LBC_NAANDB:
            return (~A & B);
        case LBC_NAORB:
            return (~A | B);
        default:return 0;
        }
    }
    static U32 cal(U32 t0, U32 t1, char exp)
    {
        if(exp == ' '){return 0;}
#define _CALEXP(c,ret) \
        else if(exp == c)return ret;
        _CALEXP('+',(t0+t1))
//        CALEXP('-',(t0-t1))
        _CALEXP('*',(t0*t1))
//        CALEXP('/',(t0/t1))
        _CALEXP('|',(t0|t1))
        _CALEXP('^',(t0^t1))
        _CALEXP('&',(t0&t1))
        else return 0;
    }
    static U32 shift(U32 value, SHIFT_Type type, U32 shift)
    {
        switch(type)
        {
        case SHIFT_ARITHR:
        {
            for(unsigned i = 0; i<shift; i++)
            {
                bool temp = (bool)(value & 0x80000000);
                value >>= 1;
                value |= (temp?0x80000000:0);
            }
            return value;
        }
        case SHIFT_LOGICR:
        {
            for(unsigned i = 0; i<shift; i++)
            {
                value >>= 1;
                value &= 0x7fffffff;
            }
            return value;
        }
        case SHIFT_LAL:
        {
            for(unsigned i = 0; i<shift; i++)
            {
                value <<= 1;
                value &= 0xfffffffe;
            }
            return value;
        }
        case SHIFT_LOOPL:
        {
            for(unsigned i = 0; i<shift; i++)
            {
                bool temp = (bool)(value & 0x80000000);
                value <<= 1;
                value |= temp & 0x1;
            }
            return value;
        }
        default:return 0;
        }
    }
};

bool transFormulaToPostfix(const string &exp, string &postFixExp)
{
    if(exp.empty())return false;
    static map<char,int> priority_map;
    if(priority_map.empty())
    {
        priority_map['+'] = 1;
//        priority_map['-'] = 1;
//        priority_map['*'] = 2;
//        priority_map['/'] = 2;
        priority_map['|'] = 3;
        priority_map['&'] = 4;
        priority_map['^'] = 5;
        priority_map['~'] = 6;
        priority_map['!'] = 6;
        priority_map[')'] = 0;
        priority_map['('] = 7;
    }
    static map<string,string> expMap;
    if(expMap.count(exp))
    {
        postFixExp = expMap[exp];
        return true;
    }
    postFixExp.clear();
    stack<char> temp_stack;
    bool shouldBeExp = false;   //是否应该是符号了（表达式中不允许两个A/B/T紧挨着）
    for(unsigned int i = 0; i<exp.size(); i++)
    {
        char c = exp.at(i);
        if(c == 'A' || c == 'B' || c == 'T')
        {
            if(shouldBeExp)return false;
            postFixExp.push_back(c);
            shouldBeExp = true;
        }
        else if(priority_map.count(c))
        {
            if(temp_stack.empty())
            {
                temp_stack.push(c);
            }
            else if(c == ')')
            {
                while(!temp_stack.empty() && temp_stack.top() != '(')
                {
                    postFixExp.push_back(temp_stack.top());
                    temp_stack.pop();
                }
                if(!temp_stack.empty() && temp_stack.top() == '(')temp_stack.pop();    //'('
            }
            else if(c == '(')
            {
                temp_stack.push(c);
            }
            else
            {
                if(priority_map[temp_stack.top()] < priority_map[c] || temp_stack.top() == '(')
                {
                    temp_stack.push(c);
                }
                else
                {
                    while(!temp_stack.empty() && priority_map[temp_stack.top()] >= priority_map[c] && temp_stack.top() != '(')
                    {
                        postFixExp.push_back(temp_stack.top());
                        temp_stack.pop();
                    }
                    temp_stack.push(c);
                }
            }
            shouldBeExp = false;
        }
        else return false;
    }
    while(!temp_stack.empty())
    {
        postFixExp.push_back(temp_stack.top());
        temp_stack.pop();
    }
    expMap[exp] = postFixExp;
    return true;
}

U32 mod31_1(U64 v)
{
    U32 s = 0x7fffffff;
//    if(v < s) return (U32)v;
//    else if(v == s) return 0;
//    else return (v&s) + 1;
    while(v >= s){
        v -= s;
    }
    return (U32)v;
}

U32 calculateAuattr(U32 inputA, U32 inputB, U32 inputT, const AUAttr &attr)
{
    U32 mux0_out = attr.mux[0] == 1?inputA:(inputA^inputT);
    U64 add_out = (U64)mux0_out + (U64)inputB + (attr.carry ? inputT : 0);
    U32 mode_out_32 = add_out&0xffffffff;
    U32 mode_out_16[2];
    mode_out_16[0] = ((mux0_out&0xffff) + (inputB&0xffff)) & 0xffff;
    mode_out_16[1] = ((mux0_out&0xffff0000) + (inputB&0xffff0000)) & 0xffff0000;
    U32 mode_out_16_final = mode_out_16[0] | mode_out_16[1];
    U32 mode_out_8[4];
    mode_out_8[0] = ((mux0_out&0xff) +(inputB&0xff)) & 0xff;
    mode_out_8[1] = ((mux0_out&0xff00) + (inputB&0xff00)) & 0xff00;
    mode_out_8[2] = ((mux0_out&0xff0000) + (inputB&0xff0000)) & 0xff0000;
    mode_out_8[3] = ((mux0_out&0xff000000) + (inputB&0xff000000)) & 0xff000000;
    U32 mode_out_8_final = mode_out_8[0] | mode_out_8[1] | mode_out_8[2] | mode_out_8[3];
    U32 mode_out_31_1 = mod31_1(add_out);

    U32 mode_out = attr.mod == 0?mode_out_32:
                   attr.mod == 1?mode_out_16_final:
                   attr.mod == 2?mode_out_8_final:mode_out_31_1;
    U32 lbc_out = Math::calByLbc(mode_out,inputT,attr.lbc);
    U32 mux1_out = attr.mux[1] == 1?lbc_out:mode_out;
    return mux1_out;
}

U32 calculatPostFix(U32 inputA, U32 inputB, U32 inputT, const string &postFixExp)
{
    stack<U32> temp_satck;
    for(unsigned int i = 0; i<postFixExp.size(); i++)
    {
        char c = postFixExp.at(i);
        if(c == 'A')
        {
            temp_satck.push(inputA);
        }
        else if(c == 'B')
        {
            temp_satck.push(inputB);
        }
        else if(c == 'T')
        {
            temp_satck.push(inputT);
        }
        else if(c == '~' || c == '!')
        {
            if(temp_satck.size())
            {
                U32 temp = temp_satck.top();
                temp_satck.pop();
                temp_satck.push((~temp));
            }
        }
        else
        {
            if(temp_satck.size() >= 2)
            {
                U32 temp0 = temp_satck.top();
                temp_satck.pop();
                U32 temp1 = temp_satck.top();
                temp_satck.pop();
                temp_satck.push(Math::cal(temp0,temp1,c));
            }
        }
    }
    return temp_satck.top();
}



U32 calculateLuattr(U32 inputA, U32 inputB, U32 inputT, const LUAttr &luattr)
{
    U32 temp_0 = Math::calByLbc(inputA,inputB,luattr.lbc[0]);
    U32 temp_1 = Math::calByLbc(inputT,inputA,luattr.lbc[1]);
    U32 temp_2 = Math::calByLbc(inputB,inputT,luattr.lbc[2]);
    U32 temp_3 = Math::calByLbc(temp_0,temp_1,luattr.lbc[3]);
    U32 temp_4 = Math::calByLbc(temp_1,temp_2,luattr.lbc[4]);
    U32 temp_5 = Math::calByLbc(temp_3,temp_4,luattr.lbc[5]);
    return temp_5;
}


inline SHIFT_Type getShiftMode(const char c)
{
    return (c == 'l') ? SHIFT_LAL   :
           (c == 'L') ? SHIFT_LOOPL :
           (c == 'R') ? SHIFT_ARITHR:
           (c == 'r') ? SHIFT_LOGICR : SHIFT_Type(0);
}

bool getLbcParaByLbcexp(const string &lbc_exp, SUAttr &attr, bool AorB)
{
    string str = AorB ? "A":"B";
    int lbcIndex = AorB?0:1;
    if(lbc_exp == str) attr.lbc[lbcIndex] = LBC_A;
    else if(lbc_exp == "~"+str) attr.lbc[lbcIndex] = LBC_NA;
    else if((lbc_exp == str+"^T" || lbc_exp == "T^"+str))
        attr.lbc[lbcIndex] = LBC_AXORB;
    else if((lbc_exp == str+"&T" || lbc_exp == "T&"+str))
        attr.lbc[lbcIndex] = LBC_AANDB;
    else if((lbc_exp == str+"|T" || lbc_exp == "T|"+str))
        attr.lbc[lbcIndex] = LBC_AORB;
    else if((lbc_exp == str+"~^T" || lbc_exp == "T^"+str+"~") || lbc_exp == "~"+str+"^T" || lbc_exp == "T^~"+str)
        attr.lbc[lbcIndex] = LBC_NAXORB;
    else if((lbc_exp == str+"~&T" || lbc_exp == "T&"+str+"~") || lbc_exp == "~"+str+"&T" || lbc_exp == "T&~"+str)
        attr.lbc[lbcIndex] = LBC_NAANDB;
    else if((lbc_exp == str+"~|T" || lbc_exp == "T|"+str+"~") || lbc_exp == "~"+str+"|T" || lbc_exp == "T|~"+str)
        attr.lbc[lbcIndex] = LBC_NAORB;
    else return false;  //不是这些表达式中的一个则不合法
    return true;
}

bool getLbc2ParaByLbcexp(const string &lbc_exp, SUAttr &attr)
{
    string str = "A";
    int lbcIndex = 2;
    if(lbc_exp == str) attr.lbc[lbcIndex] = LBC_A;
    else if(lbc_exp == "~"+str) attr.lbc[lbcIndex] = LBC_NA;
    else if((lbc_exp == str+"^T" || lbc_exp == "T^"+str))
        attr.lbc[lbcIndex] = LBC_AXORB;
    else if((lbc_exp == str+"&T" || lbc_exp == "T&"+str))
        attr.lbc[lbcIndex] = LBC_AANDB;
    else if((lbc_exp == str+"|T" || lbc_exp == "T|"+str))
        attr.lbc[lbcIndex] = LBC_AORB;
    else if((lbc_exp == str+"~^T" || lbc_exp == "T^"+str+"~") || lbc_exp == "~"+str+"^T" || lbc_exp == "T^~"+str)
        attr.lbc[lbcIndex] = LBC_NAXORB;
    else if((lbc_exp == str+"~&T" || lbc_exp == "T&"+str+"~") || lbc_exp == "~"+str+"&T" || lbc_exp == "T&~"+str)
        attr.lbc[lbcIndex] = LBC_NAANDB;
    else if((lbc_exp == str+"~|T" || lbc_exp == "T|"+str+"~") || lbc_exp == "~"+str+"|T" || lbc_exp == "T|~"+str)
        attr.lbc[lbcIndex] = LBC_NAORB;
    else return false;  //不是这些表达式中的一个则不合法
    return true;
}

U32 calculateBySuAttr(U32 inputA, U32 inputB, U32 inputT, const SUAttr &attr)
{
    U32 lbc_out[2];
    lbc_out[0] = Math::calByLbc(inputA,inputT,attr.lbc[0]);
    lbc_out[1] = Math::calByLbc(inputB,inputT,attr.lbc[1]);
    U32 shift_out[2];
    shift_out[0] = Math::shift(lbc_out[0],attr.shift_mode[0],attr.shift[0]);
    shift_out[1] = Math::shift(lbc_out[1],attr.shift_mode[1],attr.shift[1]);
    U32 mux_out[2];
    mux_out[0] = attr.mux[0] == 1 ? shift_out[0]:(shift_out[0] ^ shift_out[1]);
    mux_out[1] = attr.mux[1] == 1 ? mux_out[0]:(mux_out[0] ^ inputA);
    return mux_out[1];
}

bool analyseExpByTest(const string &exp, SUAttr &attr, bool bypass)
{
    string postFixExp = "";
    if(!transFormulaToPostfix(exp,postFixExp)) return false;
    for(int lbc0 = 0; lbc0<8; lbc0++)
    {
        for(int mux0 = 1; mux0 >= 0; mux0 --)
        {
            for(int mux1 = 1; mux1 >= 0; mux1 --)
            {
                for(int lbc1 = (bypass?attr.lbc[1]:0); lbc1 < (bypass?(attr.lbc[1]+1):8); lbc1++)
                {
                    SUAttr temp_attr;
                    temp_attr.lbc[0] = (LBCType)lbc0;
                    temp_attr.lbc[1] = (LBCType)lbc1;
                    temp_attr.lbc[2] = (LBCType)0;
                    temp_attr.shift[0] = 0;
                    temp_attr.shift[1] = attr.shift[1];
                    temp_attr.shift_mode[0] = SHIFT_Type(0);
                    temp_attr.shift_mode[1] = attr.shift_mode[1];
                    temp_attr.mux[0] = mux0;
                    temp_attr.mux[1] = mux1;
                    //
                    bool en = true;
label:
                    if(!en)continue;
                    //试验
                    for(int a = 0; a<2; a++)
                    {
                        for(int b = 0; b<2; b++)
                        {
                            for(int t = 0; t<2; t++)
                            {
                                if(calculateBySuAttr(a,b,t,temp_attr) != calculatPostFix(a,b,t,postFixExp))
                                {
                                    en = false;
                                    goto label;
                                }
                            }
                        }
                    }
                    if(en)
                    {
                        attr = temp_attr;
                        return true;
                    }
                }
            }
        }
    }
    return false;

}

bool getLbcParaByMux_Mux(string &mux_mux_exp, SUAttr &attr, bool AorB)
{
    string str = AorB?"A":"B";
    string rep = AorB?"a":"b";
    int lbcIndex = AorB?0:1;
    string::size_type pos = -1;
    if(((pos = mux_mux_exp.find("~"+str+"^T")) != string::npos) || ((pos = mux_mux_exp.find("T^~"+str)) != string::npos))
    {
        mux_mux_exp.replace(pos,4,rep);
        attr.lbc[lbcIndex] = LBC_NAXORB;
    }
    else if(((pos = mux_mux_exp.find("~"+str+"&T")) != string::npos) || ((pos = mux_mux_exp.find("T&~"+str)) != string::npos))
    {
        mux_mux_exp.replace(pos,4,rep);
        attr.lbc[lbcIndex] = LBC_NAANDB;
    }
    else if(((pos = mux_mux_exp.find("~"+str+"|T")) != string::npos) || ((pos = mux_mux_exp.find("T|~"+str)) != string::npos))
    {
        mux_mux_exp.replace(pos,4,rep);
        attr.lbc[lbcIndex] = LBC_NAORB;
    }
    else if(((pos = mux_mux_exp.find(str+"^T")) != string::npos) || ((pos = mux_mux_exp.find("T^"+str)) != string::npos))
    {
        mux_mux_exp.replace(pos,3,rep);
        attr.lbc[lbcIndex] = LBC_AXORB;
    }
    else if(((pos = mux_mux_exp.find(str+"&T")) != string::npos) || ((pos = mux_mux_exp.find("T&"+str)) != string::npos))
    {
        mux_mux_exp.replace(pos,3,rep);
        attr.lbc[lbcIndex] = LBC_AANDB;
    }
    else if(((pos = mux_mux_exp.find(str+"|T")) != string::npos) || ((pos = mux_mux_exp.find("T|"+str)) != string::npos))
    {
        mux_mux_exp.replace(pos,3,rep);
        attr.lbc[lbcIndex] = LBC_AORB;
    }

    else if((pos = mux_mux_exp.find("~"+str)) != string::npos)
    {
        mux_mux_exp.replace(pos,2,rep);
        attr.lbc[lbcIndex] = LBC_NA;
    }
    else if((pos = mux_mux_exp.find(str)) != string::npos)
    {
        mux_mux_exp.replace(pos,1,rep);
        attr.lbc[lbcIndex] = LBC_A;
    }
    else return false;
    return true;
}

bool getMuxPara(const string &mux_mux_exp, SUAttr &attr)
{
    string mux_exp = "";
    for(unsigned int i = 0; i<mux_mux_exp.size(); i++)
    {
        if(mux_mux_exp[i] != '(' && mux_mux_exp[i] != ')')mux_exp.push_back(mux_mux_exp[i]);
    }

    if(mux_exp == "a")
    {
        attr.mux[0] = 1;
        attr.mux[1] = 1;
    }
    else if(mux_exp == "a^b" || mux_exp == "b^a")
    {
        attr.mux[0] = 0;
        attr.mux[1] = 1;
    }
    else if (mux_exp == "a^A" || mux_exp == "A^a")
    {
        attr.mux[0] = 1;
        attr.mux[1] = 0;
    }
    else if(mux_exp == "a^b^A" || mux_exp == "a^A^b" || mux_exp == "b^a^A" || mux_exp == "b^A^a" ||
            mux_exp == "A^a^b" || mux_exp == "A^b^a")
    {
        attr.mux[0] = 0;
        attr.mux[1] = 0;
    }
    else return false;
    return true;
}
}

bool aea(const string &exp, AUAttr &attr)
{
    if(exp.empty()) {
        attr = {{1,0}, 0, 0, LBC_A};
        return true;
    }

    string postFixExp;
    bool exp_en = transFormulaToPostfix(exp,postFixExp);  //转换为后缀表达式
    if(!exp_en)
        return false;
    static map<string,AUAttr> postAttrMap;
    if(postAttrMap.count(postFixExp))
    {
        attr = postAttrMap[postFixExp];
//        cout << attr.lbc[0] << attr.lbc[1] << attr.lbc[2] << attr.lbc[3] << attr.lbc[4] << attr.lbc[5] << endl;
        return true;
    }
    AUAttr temp_attr;
    for(int m0 = 1; m0>=0; m0--)
    {
//        for(int a = 0; a<2; a++)
//        {
            for(int mode = 0; mode<4; mode++)
            {
                for(int lbc = 0; lbc<8; lbc++)
                {
                    for(int m1 = 1; m1>=0; m1--)
                    {
                        for(int carry = 0; carry < 1; ++carry)
                        {
                            temp_attr.mux[0] = (LBCType)m0;
    //                        temp_attr.add = (LBCType)a;
                            temp_attr.carry = carry;
                            temp_attr.mod = (LBCType)mode;
                            temp_attr.lbc = (LBCType)lbc;
                            temp_attr.mux[1] = (LBCType)m1;
                            //
                            bool en = true;
    label:
                            if(!en)continue;
                            //试验
                            for(int a = 0; a<2; a++)
                            {
                                for(int b = 0; b<2; b++)
                                {
                                    for(int t = 0; t<2; t++)
                                    {
                                        if(calculateAuattr(a,b,t,temp_attr) != calculatPostFix(a,b,t,postFixExp))
                                        {
                                            en = false;
                                            goto label;
                                        }
                                    }
                                }
                            }
                            if(en)
                            {
                                attr = temp_attr;
    //                                cout << attr.lbc[0] << attr.lbc[1] << attr.lbc[2] << attr.lbc[3] << attr.lbc[4] << attr.lbc[5] << endl;
                                postAttrMap[postFixExp] = attr;

                                return true;
                            }
                        }

                    }
                }
            }
//        }
    }
    return false;   //没有找到合适的配置
}

bool ael(const string &exp, LUAttr &attr)
{
    if(exp.empty())
    {
        attr = {LBC_A, LBC_A, LBC_A, LBC_A, LBC_A, LBC_A};
        return true; //如果exp是空的也返回真，以致不会报表达式的错
    }

    //将B/T单独拿出来配置
    if(exp == "B" || exp == "T") return false;

    string postFixExp;
    bool exp_en = transFormulaToPostfix(exp,postFixExp);  //转换为后缀表达式
    if(!exp_en)return false;
    static map<string,LUAttr> postAttrMap;
    if(postAttrMap.count(postFixExp))
    {
        attr = postAttrMap[postFixExp];
//        cout << attr.lbc[0] << attr.lbc[1] << attr.lbc[2] << attr.lbc[3] << attr.lbc[4] << attr.lbc[5] << endl;
        return true;
    }
    LUAttr temp_attr;
    for(int i5 = 0; i5<8; i5++)
    {
        for(int i4 = 0; i4<8; i4++)
        {
            for(int i3 = 0; i3<8; i3++)
            {
                for(int i2 = 0; i2<8; i2++)
                {
                    for(int i1 = 0; i1<8; i1++)
                    {
                        for(int i0 = 0; i0<8; i0++)
                        {
                            temp_attr.lbc[0] = (LBCType)i0;
                            temp_attr.lbc[1] = (LBCType)i1;
                            temp_attr.lbc[2] = (LBCType)i2;
                            temp_attr.lbc[3] = (LBCType)i3;
                            temp_attr.lbc[4] = (LBCType)i4;
                            temp_attr.lbc[5] = (LBCType)i5;
                            //
                            bool en = true;
label:
                            if(!en)continue;
                            //试验
                            for(int a = 0; a<2; a++)
                            {
                                for(int b = 0; b<2; b++)
                                {
                                    for(int t = 0; t<2; t++)
                                    {
                                        if(calculateLuattr(a,b,t,temp_attr) != calculatPostFix(a,b,t,postFixExp))
                                        {
                                            en = false;
                                            goto label;
                                        }
                                    }
                                }
                            }
                            if(en)
                            {
                                attr = temp_attr;
//                                cout << attr.lbc[0] << attr.lbc[1] << attr.lbc[2] << attr.lbc[3] << attr.lbc[4] << attr.lbc[5] << endl;
                                postAttrMap[postFixExp] = attr;

                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;   //没有找到合适的配置
}

bool aes(const string &exp, SUAttr &attr)
{
    if(exp.empty())
    {
        attr = {{LBC_A, LBC_A, LBC_A}, {SHIFT_LOGICR, SHIFT_LOGICR}, {0, 0 }, {1, 1}};
        return true; //如果exp是空的也返回真，以致不会报表达式的错
    }
    //检验是否曾经转换过
    static map<string, SUAttr> expAttrMap;
    if(expAttrMap.count(exp))
    {
        attr = expAttrMap[exp];
        return true;
    }

    //----------------第一步:转换成没有空格的表达式，并且将<< = l,  <<< = L,  >> = r, >>> = R ----------------
    queue<unsigned char> temp_exp;
    for(unsigned int i = 0; i<exp.size(); i++)
    {
        if(exp[i] == ' ')continue;    //不要空格
        else if(exp[i] == '<')
        {
            i ++;
            if(i < exp.size() && exp[i] == '<')
            {
                i ++;
                if(i < exp.size() && exp[i] == '<')
                {
                    temp_exp.push('L');
                }
                else
                {
                    temp_exp.push('l');
                    i --;
                }
            }
            else return false;
        }
        else if(exp[i] == '>')
        {
            i ++;
            if(i < exp.size() && exp[i] == '>')
            {
                i ++;
                if(i < exp.size() && exp[i] == '>')
                {
                    temp_exp.push('R');
                }
                else
                {
                    temp_exp.push('r');
                    i --;
                }
            }
            else return false;
        }
        else if(isdigit(exp[i]))
        {
            U32 temp_value = (U32)(exp[i]-'0');
            i ++;
            while(i < exp.size() && isdigit(exp[i]))
            {
                temp_value = (10*temp_value) + (U32)(exp[i]-'0');
                i ++;
            }
            i --;
            temp_value &= 0x1f; //最多5位,即范围0-31，所以用ascii码的前32个直接代替
            //
            temp_exp.push((unsigned char)temp_value);
        }
        else
        {
            temp_exp.push(exp[i]);
        }
    }
    //----------------------------------第二步:解析表达式---------------------------------
    deque<unsigned char> result_deque;  //最终计算的队列
    stack<unsigned char> shift_stacks[2];   //两个移位操作的栈
    while(temp_exp.size())
    {
        unsigned char c = temp_exp.front();
        if(c != 'l' && c != 'L' && c != 'r' && c != 'R')    //如果不是移位操作符，则放入deque
        {
            if(c < 32)return false; //如果直接出现立即数：非法
            result_deque.push_back(c);
            temp_exp.pop();
        }
        else    //遇到移位操作
        {
            stack<unsigned char> *temp_st = &shift_stacks[0];
            if(temp_st->size())temp_st = &shift_stacks[1];
            if(temp_st->size())return false;    //如果用完了两个移位栈还有移位操作：非法
            //将和该移位操作有关的所有内容放入一个移位栈

            //1.将移位符号放入deque
            result_deque.push_back(c);
            temp_exp.pop();

            //2.将移位符后的立即数放入栈
            if(temp_exp.front() >= 32)return false; //如果超过范围或不是符号：非法
            temp_st->push(temp_exp.front());
            temp_exp.pop();

            //3.将deque中与该移位符有关的所有内容放入栈
            temp_st->push(result_deque.back()); //移位符号
            result_deque.pop_back();

            if(result_deque.size() <= 0)return false;   //如果移位符号前没有移位主角：非法

            int close = 0;  //是否形成封闭表达式，如果遇到')'必须有'('才算封闭,close=0代表封闭， 一旦封闭即可停止压栈
            char exp_t = 't';   //该移位是哪一个移位（含有A的则为'a'，含有B的则为'b'， 同时含有A和B或者同时没有的为非法）
            unsigned char c = result_deque.back();
            if(c == ')')close ++;
            else if(c == 'A'){if(exp_t == 't')exp_t = 'a';else return false;}
            else if(c == 'B'){if(exp_t == 't')exp_t = 'b';else return false;}
            temp_st->push(c); //移位符号前的内容
            result_deque.pop_back();

            while(close > 0)
            {
//                if(close < 0)return false;  //在未出现')'之前就出现了'('：非法
                unsigned char c = result_deque.back();

                if(c == ')')close ++;
                if(c == '(')close --;
                else if(c == 'A'){if(exp_t == 't')exp_t = 'a';else return false;}
                else if(c == 'B'){if(exp_t == 't')exp_t = 'b';else return false;}
                temp_st->push(c); //移位符号前的内容
                result_deque.pop_back();
            }

            if(exp_t == 't')return false;   //同时没有A和B
            result_deque.push_back(exp_t);  //形成 A ^ a ^ b的形式，A代表旁路的A,a代表含有A的移位，b代表含有B的移位
        }
    }

    //------------------第三步：检验三个表达式栈的合法性 并化为string--------------------------
    string mux_exp;
    string lbc_exp[2];
    string shift_exp[2];
    char exp_type[2] = {'c','c'};   //'a' 'b'

    //移位后的表达式检查
    map<char,int> mux_map;
    while(result_deque.size())
    {
        unsigned char c = result_deque.front();

        if(c != 'A' && c != 'a' && c != 'b' && c != '^')return false;//有其他运算符：非法

        mux_map[c] ++;
        if(c != '^' && mux_map[c] >= 2)return false; //a或b出现两次以上：非法

        mux_exp.push_back(c);
        result_deque.pop_front();
    }
    //移位表达式检查
    for(int i = 0; i<2; i++)
    {
        while(shift_stacks[i].size())
        {
            unsigned char c = shift_stacks[i].top();
            if(c != 'A' && c != 'B' && c != 'T' && c != '&' && c != '(' && c != ')' && c != 'l' && c != 'L' &&
               c != '|' && c != '~' && c != '^' && c != '!' && c >= 32 && c != 'r' && c != 'R')return false;//除此之外都是非法字符

            if(c == '(' || c == ')')
            {
                shift_stacks[i].pop();
                continue;   //不存括号
            }
            else if(c == 'l' || c == 'L' || c == 'r' || c == 'R' || c < 32)
            {
                shift_exp[i].push_back(c);
            }
            else if(c == 'A')
            {
                exp_type[i] = 'a';
                lbc_exp[i].push_back(c);
            }
            else if(c == 'B')
            {
                exp_type[i] = 'b';
                lbc_exp[i].push_back(c);
            }
            else if(c == '~' || c == '!')
            {
                lbc_exp[i].push_back('~');
            }
            else
            {
                lbc_exp[i].push_back(c);
            }
            shift_stacks[i].pop();
        }
    }

    //-------------------第四步：根据几个栈的内容设置参数--------------------
//    cout << mux_exp << lbc_exp[0] << lbc_exp[1] << shift_exp[0] << shift_exp[1];
    for(int i = 0; i<2; i++)
    {
        int index = (exp_type[i] == 'a')?0:1;
        //lbc
        if(exp_type[i] == 'a')
        {
            if(!lbc_exp[i].compare("A"))attr.lbc[index] = (LBCType)0;
            else if(!lbc_exp[i].compare("~A"))attr.lbc[index] = (LBCType)1;
            else if(!lbc_exp[i].compare("A^T") || !lbc_exp[i].compare("T^A"))attr.lbc[index] = (LBCType)2;
            else if(!lbc_exp[i].compare("A&T") || !lbc_exp[i].compare("T&A"))attr.lbc[index] = (LBCType)3;
            else if(!lbc_exp[i].compare("A|T") || !lbc_exp[i].compare("T|A"))attr.lbc[index] = (LBCType)4;
            else if(!lbc_exp[i].compare("~A^T") || !lbc_exp[i].compare("T^~A"))attr.lbc[index] = (LBCType)5;
            else if(!lbc_exp[i].compare("~A&T") || !lbc_exp[i].compare("T&~A"))attr.lbc[index] = (LBCType)6;
            else if(!lbc_exp[i].compare("~A|T") || !lbc_exp[i].compare("T|~A"))attr.lbc[index] = (LBCType)7;
        }
        else if(exp_type[i] == 'b')
        {
            if(!lbc_exp[i].compare("B"))attr.lbc[index] = (LBCType)0;
            else if(!lbc_exp[i].compare("~B"))attr.lbc[index] = (LBCType)1;
            else if(!lbc_exp[i].compare("B^T") || !lbc_exp[i].compare("T^B"))attr.lbc[index] = (LBCType)2;
            else if(!lbc_exp[i].compare("B&T") || !lbc_exp[i].compare("T&B"))attr.lbc[index] = (LBCType)3;
            else if(!lbc_exp[i].compare("B|T") || !lbc_exp[i].compare("T|B"))attr.lbc[index] = (LBCType)4;
            else if(!lbc_exp[i].compare("~B^T") || !lbc_exp[i].compare("T^~B"))attr.lbc[index] = (LBCType)5;
            else if(!lbc_exp[i].compare("~B&T") || !lbc_exp[i].compare("T&~B"))attr.lbc[index] = (LBCType)6;
            else if(!lbc_exp[i].compare("~B|T") || !lbc_exp[i].compare("T|~B"))attr.lbc[index] = (LBCType)7;
        }
        //shift
        for(unsigned int j = 0; j<shift_exp[i].size(); j++)
        {
            char c = shift_exp[i].at(j);
            if(c == 'r')attr.shift_mode[index] = SHIFT_LOGICR;
            else if(c == 'R')attr.shift_mode[index] = SHIFT_ARITHR;
            else if(c == 'l')attr.shift_mode[index] = SHIFT_LAL;
            else if(c == 'L')attr.shift_mode[index] = SHIFT_LOOPL;
            else if(c < 32)attr.shift[index] = (U32)c;
        }
    }
    //mux
    if(mux_map['^'] == 0)
    {
        if(mux_exp == "a")
        {
            attr.mux[0] = 1;
            attr.mux[1] = 1;
        }
        else return false;
    }
    else if(mux_map['^'] == 1)
    {
        if(mux_exp == "a^b" || mux_exp == "b^a")
        {
            attr.mux[0] = 0;
            attr.mux[1] = 1;
        }
        else if (mux_exp == "a^A" || mux_exp == "A^a")
        {
            attr.mux[0] = 1;
            attr.mux[1] = 0;
        }
        else return false;
    }
    else if(mux_map['^'] == 2)
    {
        attr.mux[0] = 0;
        attr.mux[1] = 0;
    }  

//    cout << "ok" << endl;
    //保存到静态map中
    expAttrMap[exp] = attr;
//    for(int i = 0; i<2; i++)
//    {
//        cout << attr.lbc[i] << endl;
//        cout << attr.shift_mode[i] << endl;
//        cout << attr.shift[i] << endl;
//        cout << attr.mux[i] << endl;
//    }
    return true;
}

bool aes(const string &exp, const string &bypassExp, SUAttr &attr)
{
    if(exp.empty()) {
        attr = {{LBC_A, LBC_A, LBC_A}, {SHIFT_LOGICR, SHIFT_LOGICR}, {0, 0 }, {1, 1}};
        return true; //如果exp是空的也返回真，以致不会报表达式的错
    }
    //----------------第一步:转换成没有空格的表达式，并且将<< = l,  <<< = L,  >> = r, >>> = R ----------------
    //-------exp--------
    string temp_exp;
    for(unsigned int i = 0; i<exp.size(); i++)
    {
        if(exp[i] == ' ')continue;    //不要空格
        else if(exp[i] == '<')
        {
            i ++;
            if(i < exp.size() && exp[i] == '<')
            {
                i ++;
                if(i < exp.size() && exp[i] == '<')
                {
                    temp_exp.push_back('L');
                }
                else
                {
                    temp_exp.push_back('l');
                    i --;
                }
            }
            else return false;
        }
        else if(exp[i] == '>')
        {
            i ++;
            if(i < exp.size() && exp[i] == '>')
            {
                i ++;
                if(i < exp.size() && exp[i] == '>')
                {
                    temp_exp.push_back('R');
                }
                else
                {
                    temp_exp.push_back('r');
                    i --;
                }
            }
            else return false;
        }
        else if(isdigit(exp[i]))
        {
            U32 temp_value = (U32)(exp[i]-'0');
            i ++;
            while(i < exp.size() && isdigit(exp[i]))
            {
                temp_value = (10*temp_value) + (U32)(exp[i]-'0');
                i ++;
            }
            i --;
            temp_value &= 0x1f; //最多5位,即范围0-31，所以用ascii码的前32个直接代替
            //
            temp_exp.push_back((unsigned char)temp_value);
        }
        else
        {
            temp_exp.push_back(exp[i]);
        }
    }
    //-------bypassExp-----------
    string temp_bypassExp;
    for(unsigned int i = 0; i<bypassExp.size(); i++)
    {
        if(bypassExp[i] == ' ')continue;    //不要空格
        else if(bypassExp[i] == '<')
        {
            i ++;
            if(i < bypassExp.size() && bypassExp[i] == '<')
            {
                i ++;
                if(i < bypassExp.size() && bypassExp[i] == '<')
                {
                    temp_bypassExp.push_back('L');
                }
                else
                {
                    temp_bypassExp.push_back('l');
                    i --;
                }
            }
            else return false;
        }
        else if(bypassExp[i] == '>')
        {
            i ++;
            if(i < bypassExp.size() && bypassExp[i] == '>')
            {
                i ++;
                if(i < bypassExp.size() && bypassExp[i] == '>')
                {
                    temp_bypassExp.push_back('R');
                }
                else
                {
                    temp_bypassExp.push_back('r');
                    i --;
                }
            }
            else return false;
        }
        else if(isdigit(bypassExp[i]))
        {
            U32 temp_value = (U32)(bypassExp[i]-'0');
            i ++;
            while(i < bypassExp.size() && isdigit(bypassExp[i]))
            {
                temp_value = (10*temp_value) + (U32)(bypassExp[i]-'0');
                i ++;
            }
            i --;
            temp_value &= 0x1f; //最多5位,即范围0-31，所以用ascii码的前32个直接代替
            //
            temp_bypassExp.push_back((unsigned char)temp_value);
        }
        else
        {
            temp_bypassExp.push_back(bypassExp[i]);
        }
    }
    //---------------------------------第二步:检验bypassExp合法性,并解析bypass-------------------------------
    SUAttr bypass_attr = attr;  //bypass部分使用的attr
    bool bypass_shift = false;  //bypass部分是否有移位
    string bypass_lbc_exp = "";
    int bypass_shift_pos = temp_bypassExp.size();
    if(!temp_bypassExp.empty())
    {
        for(unsigned int i = 0; i<temp_bypassExp.size(); i++)
        {
            char temp_bc = temp_bypassExp[i];
            if(temp_bc == 'l' || temp_bc == 'L' || temp_bc == 'R' || temp_bc == 'r')
            {
                bypass_attr.shift_mode[1] = getShiftMode(temp_bc);
                //判断后面有没有数字
                int j = i+1;
                if((int)temp_bypassExp.size() <= j || temp_bypassExp[j] >= 32) return false;    //移位操作符后面没有数字,不合法
                bypass_attr.shift[1] = temp_bypassExp[j];

                j = i+2;
                if((int)temp_bypassExp.size() > j) return false; //移位操作数之后还有内容，不合法
                //判断前面的表达式
                j = i-1;
                if(j < 0) return false;    //前面没有表达式 不合法
                bypass_shift_pos = i;
                bypass_shift = true;
                break;
            }
        }
        //读取前面的表达式
        for(int j = 0; j<bypass_shift_pos; j++)
        {
            char bc = temp_bypassExp[j];
            if(bc != ')' && bc != '(') bypass_lbc_exp.push_back(bc);
        }
        if(!getLbcParaByLbcexp(bypass_lbc_exp,bypass_attr,false))return false;
    }
    //目前bypassExp已经合法
    SUAttr exp_attr = bypass_attr;
    //-----------------------------------第三步:根据bypass表达式检验x表达式合法性并解析--------------------------------------
    //1.判断exp中的移位操作数
    int shift_count = 0;
    int shift_pos[2];
    for(unsigned int i = 0; i<temp_exp.size(); i++)
    {
        if(temp_exp[i] == 'l' || temp_exp[i] == 'L' || temp_exp[i] == 'r' || temp_exp[i] == 'R')
        {
            if(shift_count < 2) shift_pos[shift_count] = i;
            else return false;  //移位操作数超过2个 不合法
            shift_count++;
        }
    }
    if(shift_count == 0)
    {
        if(bypass_shift && temp_exp.find('B') != string::npos) return false;    //bypass有shift而exp的B相关操作没有shift，不合法
        //根据bypass的参数来穷举
        if(analyseExpByTest(exp,exp_attr,!temp_bypassExp.empty()))
        {
            attr = exp_attr;
            return true;
        }
        else return false;
    }
    else if(shift_count == 1)
    {
        //将shift_exp读出来
        if(shift_pos[0]+1 >= (int)temp_exp.size() || temp_exp[shift_pos[0]+1] > 32 || shift_pos[0]-1 < 0) return false; //移位符号后面没有数字，不合法
        string exp_lbc_exp;
        bool brak = false;
        int shift_start_pos = -1;   //该移位操作开始的位置
        for(int j = shift_pos[0]-1; j>=0; j--)
        {
            if(temp_exp[j] == ')') brak = true;
            else if(temp_exp[j] == '(') {shift_start_pos = j;break;}
            else
            {
                exp_lbc_exp.push_back(temp_exp[j]);
                if(!brak){shift_start_pos = j;break;}
            }
        }
        //先把含有移位的部分去掉
        string mux_mux_exp = "";
        string exp_lbc_exp_sig = "a";
        if(exp_lbc_exp.find("A") != string::npos && exp_lbc_exp.find("B") == string::npos) exp_lbc_exp_sig = "a";
        else if(exp_lbc_exp.find("A") == string::npos && exp_lbc_exp.find("B") != string::npos) exp_lbc_exp_sig = "b";
        else return false;
        mux_mux_exp = temp_exp;
        mux_mux_exp.replace(shift_start_pos,shift_pos[0]+2-shift_start_pos,exp_lbc_exp_sig);

        //设置lbc2的值
        if(mux_mux_exp.substr(mux_mux_exp.length()-1,1) == "A")
        {
            string _temp_exp = mux_mux_exp;
            _temp_exp = "T" + _temp_exp.substr(mux_mux_exp.length()-2,2);
            getLbc2ParaByLbcexp(_temp_exp,exp_attr);
        }

        //判断该移位是B的还是A的
        if(exp_lbc_exp_sig == "a")
        {
            //A部分参数
            if(!getLbcParaByLbcexp(exp_lbc_exp,exp_attr,true))return false;
            exp_attr.shift_mode[0] = getShiftMode(temp_exp[shift_pos[0]]);
            exp_attr.shift[0] = temp_exp[shift_pos[0]+1];

            //bypass部分
            if(mux_mux_exp.find("B") != string::npos && bypass_shift) return false;
            if(bypass_shift && mux_mux_exp.find("B") == string::npos)    //没有B,exp和bypass互不干扰
            {
                if(!mux_mux_exp.compare("a^A") || !mux_mux_exp.compare("A^a"))
                {
                    exp_attr.mux[0] = 1;
                    exp_attr.mux[1] = 0;
                }
                else if(!mux_mux_exp.compare("a"))
                {
                    exp_attr.mux[0] = 1;
                    exp_attr.mux[1] = 1;
                }
                else return false;
                attr = exp_attr;
                return true;
            }
            else    //外面含有B， bypass没有移位
            {
                //获取mux_mux_exp中含有B相关的部分
                if(mux_mux_exp.find("B") != string::npos)
                {
                    if(!getLbcParaByMux_Mux(mux_mux_exp,exp_attr,false)) return false;
                    if(exp_attr.lbc[1] != bypass_attr.lbc[1] && !temp_bypassExp.empty()) return false;
                }
                if(!getMuxPara(mux_mux_exp,exp_attr)) return false;
                attr = exp_attr;
                return true;
            }
        }
        else    //该移位是B部分
        {

            if(!bypass_shift && !temp_bypassExp.empty()) return false;
            if(!temp_bypassExp.empty()) //bypass存在则比较两个lbc
            {
                if(!getLbcParaByLbcexp(exp_lbc_exp,exp_attr,false)) return false;
                if(exp_attr.lbc[1] != bypass_attr.lbc[1]) return false;
                if(temp_exp[shift_pos[0]] != temp_bypassExp[bypass_shift_pos] ||
                        temp_exp[shift_pos[0]+1] != temp_bypassExp[bypass_shift_pos+1]) return false;

            }
            else    //bypass不存在
            {
                //B部分参数
                if(!getLbcParaByLbcexp(exp_lbc_exp,exp_attr,false))return false;
                exp_attr.shift_mode[1] = getShiftMode(temp_exp[shift_pos[0]]);
                exp_attr.shift[1] = temp_exp[shift_pos[0]+1];
            }
            //判断是否有A
            if(mux_mux_exp.find("A") != string::npos)
            {
                //去掉A部分
                if(!getLbcParaByMux_Mux(mux_mux_exp,exp_attr,true)) return false;
                if(!getMuxPara(mux_mux_exp,exp_attr)) return false;
                attr = exp_attr;
                return true;
            }
            else return false;      //B算式不能单独存在，必须有A
        }

    }
    else if(shift_count == 2)
    {
        if(!bypass_shift && !temp_bypassExp.empty()) return false;
        string exp_lbc_exps[2];
        int shift_start_poses[2];
        string result_mux_mux_exp = temp_exp;
        string exp_lbc_exp_sigs[2];
        int aIndex,bIndex;  //表示哪一个是A表达式的，哪一个是B表达式的
        //拆分
        for(int i = 0; i<2; i++)
        {
            if(shift_pos[i]+1 >= (int)temp_exp.size() || temp_exp[shift_pos[i]+1] > 32 || shift_pos[i]-1 < 0) return false; //移位符号后面没有数字，不合法
            bool brak = false;
            shift_start_poses[i] = -1;   //该移位操作开始的位置
            for(int j = shift_pos[i]-1; j>=0; j--)
            {
                if(temp_exp[j] == ')') brak = true;
                else if(temp_exp[j] == '(') {shift_start_poses[i] = j;break;}
                else
                {
                    exp_lbc_exps[i].push_back(temp_exp[j]);
                    if(!brak){shift_start_poses[i] = j;break;}
                }
            }
        }
        for(int i = 0; i<2; i++)
        {
            if(exp_lbc_exps[i].find("A") != string::npos && exp_lbc_exps[i].find("B") == string::npos) exp_lbc_exp_sigs[i] = "a";
            else if(exp_lbc_exps[i].find("A") == string::npos && exp_lbc_exps[i].find("B") != string::npos) exp_lbc_exp_sigs[i] = "b";
            else return false;
        }
        if(exp_lbc_exp_sigs[0] == "a" && exp_lbc_exp_sigs[1] == "b") {aIndex = 0;bIndex = 1;}
        else if(exp_lbc_exp_sigs[0] == "b" && exp_lbc_exp_sigs[1] == "a") {aIndex = 1;bIndex = 0;}
        else return false;
        if(shift_pos[0] > shift_pos[1])
        {
            result_mux_mux_exp.replace(shift_start_poses[0],shift_pos[0]+2-shift_start_poses[0],exp_lbc_exp_sigs[0]);
            result_mux_mux_exp.replace(shift_start_poses[1],shift_pos[1]+2-shift_start_poses[1],exp_lbc_exp_sigs[1]);
        }
        else
        {
            result_mux_mux_exp.replace(shift_start_poses[1],shift_pos[1]+2-shift_start_poses[1],exp_lbc_exp_sigs[1]);
            result_mux_mux_exp.replace(shift_start_poses[0],shift_pos[0]+2-shift_start_poses[0],exp_lbc_exp_sigs[0]);
        }

        //设置lbc2的值
        if(result_mux_mux_exp.substr(result_mux_mux_exp.length()-1,1) == "A" ||
                result_mux_mux_exp.substr(0,1) == "A")
        {
            string _temp_exp = result_mux_mux_exp;
//            _temp_exp = "T" + _temp_exp.substr(result_mux_mux_exp.length()-2,2);
            _temp_exp = "T^A";
            getLbc2ParaByLbcexp(_temp_exp,exp_attr);
        }

        //判断bypass 以确定B部分参数
        if(!temp_bypassExp.empty())
        {

            if(!getLbcParaByLbcexp(exp_lbc_exps[bIndex],exp_attr,false)) return false;
            if(exp_attr.lbc[1] != bypass_attr.lbc[1]) return false;
            if(temp_exp[shift_pos[bIndex]] != temp_bypassExp[bypass_shift_pos] ||
                    temp_exp[shift_pos[bIndex]+1] != temp_bypassExp[bypass_shift_pos+1]) return false;

        }
        else
        {
            if(!getLbcParaByLbcexp(exp_lbc_exps[bIndex],exp_attr,false))return false;
            exp_attr.shift_mode[1] = getShiftMode(temp_exp[shift_pos[bIndex]]);
            exp_attr.shift[1] = temp_exp[shift_pos[bIndex]+1];
        }
        //A部分参数
        if(!getLbcParaByLbcexp(exp_lbc_exps[aIndex],exp_attr,true))return false;
        exp_attr.shift_mode[0] = getShiftMode(temp_exp[shift_pos[aIndex]]);
        exp_attr.shift[0] = temp_exp[shift_pos[aIndex]+1];
        //mux参数
        if(!getMuxPara(result_mux_mux_exp,exp_attr)) return false;
        attr = exp_attr;
        return true;
    }
    else return false;
}

//bool BASEPE_EXPORT (*aea)(const std::string &, AUAttr &) = _aea;
//bool BASEPE_EXPORT (*ael)(const std::string &, LUAttr &) = _ael;
//bool BASEPE_EXPORT (*aes)(const std::string &, SUAttr &) = _aes;
//bool BASEPE_EXPORT (*aes_y)(const std::string &, const std::string &, SUAttr &) = _aes_y;

bool b2f(const QList<QBitArray> &list, const QString &fileName, QString &error)
{
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly)) {
        error = QString(u8"文件打开失败，%1").arg(file.errorString());
        return false;
    }

    foreach(const auto& array, list) {
        file.write(array.bits(), array.count()/8 + (array.count()%8>0));
    }
    return true;
}

bool bp2f(const QList<QPair<QString, QBitArray> > &list, const QString &fileName, QString &error)
{
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly)) {
        error = QString(u8"文件打开失败，%1").arg(file.errorString());
        return false;
    }

    foreach(const auto& array, list) {
        file.write(array.second.bits(), array.second.count()/8 + (array.second.count()%8>0));
    }
    return true;
}

BasePe::BasePe() :
    m_index({0, 0, 0}),
    m_lastError(QStringLiteral("NO ERROR"))
{
}

BasePe::~BasePe()
{

}

bool BasePe::operator ==(const BasePe &right) const {
    return m_index == right.m_index;
}

bool BasePe::operator <(const BasePe &right) const {
    return m_index < right.m_index;
}

void BasePe::setLastError(const QString &lastError)
{
    m_lastError = lastError;
}

QString BasePe::getLastError() const
{
    return m_lastError;
}

void BasePe::setIndex(int bcu, int rcu, int pe)
{
    m_index = { bcu, rcu, pe };
}

void BasePe::setIndex(const SIndex &index)
{
    m_index = index;
}

BasePe::SIndex BasePe::getIndex() const {
    return m_index;
}

BasePe::InputSourceType BasePe::getInputSource()
{
    return m_inputSource;
}

void BasePe::i2b(QBitArray &bitArray, int value, int len, int startPos, bool reverse) {
    const auto& _temp = QBitArray::fromBits((const char*)(&value), 32);
    for(int i = 0; i < len; ++i)
    {
        if(_temp.at(i))
        {
            if(reverse)
                bitArray.setBit(startPos + i);
            else
                bitArray.setBit(startPos + len - i - 1);
        }
    }
}

//二进制转16进制
QString BinToHex(const QString strBin)
{
    QString strout;

    if(strBin=="1111") strout="f";
    else if(strBin=="1110") strout="e";
    else if(strBin=="1101") strout="d";
    else if(strBin=="1100") strout="c";
    else if(strBin=="1011") strout="b";
    else if(strBin=="1010") strout="a";
    else if(strBin=="1001") strout="9";
    else if(strBin=="1000") strout="8";
    else if(strBin=="0111") strout="7";
    else if(strBin=="0110") strout="6";
    else if(strBin=="0101") strout="5";
    else if(strBin=="0100") strout="4";
    else if(strBin=="0011") strout="3";
    else if(strBin=="0010") strout="2";
    else if(strBin=="0001") strout="1";
    else if(strBin=="0000") strout="0";

    return strout;
}

//传入4 size的QBitArray，生成16进制字符串
QString bitToHexStr(QBitArray &bitArray)
{
    QString str = "";
    int iNeed = bitArray.size()%4;
    if(iNeed != 0)
        bitArray.resize(bitArray.size()+iNeed);

    for (int i = 0; i < bitArray.size(); ) {
        QString strBin = "";
        for(int j = 0; j < 4; j++)
        {
            if(bitArray[i+j] == 0)
                strBin += "0";
            else
                strBin += "1";
        }
        str += strBin;
        i = i+4;
    }
    return str;
}
