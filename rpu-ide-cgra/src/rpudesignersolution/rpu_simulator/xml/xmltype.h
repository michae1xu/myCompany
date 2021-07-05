#ifndef __XMLTYPE_H__
#define __XMLTYPE_H__

enum ElementType
{
	ELEMENT_BFU,
	ELEMENT_SBOX,
	ELEMENT_BENES,
	ELEMENT_WIRE,
	ELEMENT_INFIFO,
	ELEMENT_INMEM,
	ELEMENT_OUTFIFO,
	ELEMENT_OUTMEM,
	ELEMENT_RCH,
    ELEMENT_LFSR,
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
    BCUOUTFIFO0,
    BCUOUTFIFO1,
    BCUOUTFIFO2,
    BCUOUTFIFO3,
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
    BCUOUTFIFO0_Index,
    BCUOUTFIFO1_Index,
    BCUOUTFIFO2_Index,
    BCUOUTFIFO3_Index,
	RCURCH_Index,


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
    BCUOUTFIFO0_Type,
    BCUOUTFIFO1_Type,
    BCUOUTFIFO2_Type,
    BCUOUTFIFO3_Type,
	RCURCH_Type,
	RCURCH_Mode,
	RCURCH_DEST0,
	RCURCH_DEST1,
	RCURCH_DEST2,
	RCURCH_DEST3,

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
    BCUOUTFIFO0_Height,
    BCUOUTFIFO1_Height,
    BCUOUTFIFO2_Height,
    BCUOUTFIFO3_Height,
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
    RMODE0,
    RMODE1,
    GAP,
    SOURCE,
    DEST,
    BURST,
    LOOP,
    REPEAT,
    BCUSEL,

    //LSFR
    LFSR,
    LFSR_INDEX

};

//func
#define FUNC_AU "au"
#define FUNC_LU "lu"
#define FUNC_SU "su"


//bypass
#define BYPASS_A "A"
#define BYPASS_B "B"
#define BYPASS_T "T"
#define BYPASS_b "b"

//
//#define EMPTYCHOICE trUtf8("空")
//#define CURBFUX trUtf8("当前行BFU输出X")
//#define CURBFUY trUtf8("当前行BFU输出Y")
//#define CURSBOX trUtf8("当前行SBOX输出")
//#define CURBENES trUtf8("当前行BENES输出")
//#define LASTBFUX trUtf8("上一行BFU输出X")
//#define LASTBFUY trUtf8("上一行BFU输出Y")
//#define LASTSBOX trUtf8("上一行SBOX输出")
//#define LASTBENES trUtf8("上一行BENES输出")
//#define INFIFO trUtf8("入口FIFO")
//#define MEMOUT trUtf8("MEM输出")
#define EMPTYCHOICE "空"
#define CURBFUX "当前行BFU输出X"
#define CURBFUY "当前行BFU输出Y"
#define CURSBOX "当前行SBOX输出"
#define CURBENES "当前行BENES输出"
#define LASTBFUX "上一行BFU输出X"
#define LASTBFUY "上一行BFU输出Y"
#define LASTSBOX "上一行SBOX输出"
#define LASTBENES "上一行BENES输出"
#define INFIFO "入口FIFO"
#define MEMOUT "MEM输出"


#define BCUMAX 4
#define RCUMAX 256
#define BFUMAX 8
#define SBOXMAX 4
#define BENESMAX 1
#define BENESINPUTNUM 4

//

#define TAGALGRITHM "ALGRITHM"
#define TAGRCUMEM "MEM_INPUT"
#define TAGOUTFIFO "OUT_FIFO"


#endif // XMLTYPE_H
