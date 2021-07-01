#ifndef TYPEDEF_H
#define TYPEDEF_H
enum ElementType
{
    ELEMENT_BFU,
    ELEMENT_CUSTOMPE,
    ELEMENT_SBOX,
    ELEMENT_BENES,
    ELEMENT_WIRE,
    ELEMENT_INIMD,
    ELEMENT_INFIFO,
    ELEMENT_INMEM,
    ELEMENT_INRCH,
    ELEMENT_OUTFIFO,
    ELEMENT_OUTMEM,
    ELEMENT_OUTRCH,
    ELEMENT_NULL
};
enum Input_Type
{
    INPUT_A,
    INPUT_B,
    INPUT_T
};


enum AttributeID
{
    //定位
    ID,
    BCUID,
    RCUID,
    BFUID,
    SBOXID,
    //位置
    Pos,
    PosX,
    PosY,
    Width,
    Height,
    //属性
    Attribute,
    BfuAttr,
    BfuFunc,    //au/lu/su
    BfuFuncExp,
    BfuBypass,
    BfuInputA,
    BfuInputB,
    BfuInputT,
    BfuAUMod,   //au的模
    BfuBypassExp,

    SboxAttr,
    SboxInput,
    sboxCNT,
    SboxCNT0,
    SboxCNT1,
    SboxCNT2,
    SboxCNT3,
    SboxMode,
    SboxSrc,
    BenesAttr,
    BenesInput0,
    BenesInput1,
    BenesInput2,
    BenesInput3,
    BenesSrc,
    RCUMEMAttr,
    RCUMEM0,
    RCUMEM1,
    RCUMEM2,
    RCUMEM3,
    BCUOUTPUTAttr,
    RCUOUTFIFO0,
    RCUOUTFIFO1,
    RCUOUTFIFO2,
    RCUOUTFIFO3,
    BCUROUTER,

    //index
    BfuInputA_Index,
    BfuInputB_Index,
    BfuInputT_Index,
    SboxInput_Index,
    BenesInput0_Index,
    BenesInput1_Index,
    BenesInput2_Index,
    BenesInput3_Index,
    RCUMEM0_Index,
    RCUMEM1_Index,
    RCUMEM2_Index,
    RCUMEM3_Index,
    RCUOUTFIFO0_Index,
    RCUOUTFIFO1_Index,
    RCUOUTFIFO2_Index,
    RCUOUTFIFO3_Index,


    //type
    BfuInputA_Type,
    BfuInputB_Type,
    BfuInputT_Type,
    SboxInput_Type,
    BenesInput0_Type,
    BenesInput1_Type,
    BenesInput2_Type,
    BenesInput3_Type,
    RCUMEM0_Type,
    RCUMEM1_Type,
    RCUMEM2_Type,
    RCUMEM3_Type,
    RCUOUTFIFO0_Type,
    RCUOUTFIFO1_Type,
    RCUOUTFIFO2_Type,
    RCUOUTFIFO3_Type,

    //height
    BfuInputA_Height,
    BfuInputB_Height,
    BfuInputT_Height,
    SboxInput_Height,
    BenesInput0_Height,
    BenesInput1_Height,
    BenesInput2_Height,
    BenesInput3_Height,
    RCUMEM0_Height,
    RCUMEM1_Height,
    RCUMEM2_Height,
    RCUMEM3_Height,
    RCUOUTFIFO0_Height,
    RCUOUTFIFO1_Height,
    RCUOUTFIFO2_Height,
    RCUOUTFIFO3_Height,
    //TH_MEM
    RCUMEMWT,
    RCUMEMWTMODE,
    RCUMEMWTADDR,
    RCUMEMWTMASK,
    RCUMEMWTOFFSET,
    RCUMEMWTTHREASHOLD,

    RCUMEMRD,
    RCUMEMRDMODE128_32,
    RCUMEMRDMODE,
    RCUMEMRDADDR1,
    RCUMEMRDADDR2,
    RCUMEMRDADDR3,
    RCUMEMRDADDR4,
    RCUMEMRDOFFSET,
    RCUMEMRDTHREASHOLD,

    //RSM
    ONLYRSM,
    RMODE,
    GAP,
    SOURCE,
    DEST,
    BURST,
    LOOP,
    REPEAT,
    BCUSEL
};

//func
#define FUNC_AU "au"
#define FUNC_LU "lu"
#define FUNC_SU "su"


//bypass
#define BYPASS_A "A"
#define BYPASS_B "B"
#define BYPASS_T "T"
#define BYPASS_NB "B`"

//
#define EMPTYCHOICE trUtf8("空")
#define CURBFUX trUtf8("当前行BFU输出X")
#define CURBFUY trUtf8("当前行BFU输出Y")
#define CURSBOX trUtf8("当前行SBOX输出")
#define CURBENES trUtf8("当前行BENES输出")
#define LASTBFUX trUtf8("上一行BFU输出X")
#define LASTBFUY trUtf8("上一行BFU输出Y")
#define LASTSBOX trUtf8("上一行SBOX输出")
#define LASTBENES trUtf8("上一行BENES输出")
#define INFIFO trUtf8("入口FIFO")
#define MEMOUT trUtf8("MEM输出")


#define BCUMAX 4
#define RCUMAX 10000
#define BFUMAX 8
#define SBOXMAX 4
#define BENESMAX 1
#define BENESINPUTNUM 4

//

#define TAGALGRITHM "ALGRITHM"
#define TAGRCUMEM "MEM_INPUT"
#define TAGRCURCH "RCH_INPUT"
#define TAGRCULFSR "LFSR_INPUT"
#define TAGOUTFIFO "OUT_FIFO"


#endif // TYPEDEF_H
