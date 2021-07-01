#ifndef RCAGRAPHDEF_H
#define RCAGRAPHDEF_H

#include "common/debug.h"
#include <QPen>

namespace rca_space {

#ifndef LK_NO_DEBUG_OUTPUT   //rpudesigner.pro中注释后可开启/关闭日志
#define QL_DEBUG qDebug() << QString("%1 > %2() > %3 >").arg(__FILE__).arg(__FUNCTION__).arg(__LINE__).toLocal8Bit().data()
#else
#define QL_DEBUG noDebug()
#endif

const int BFU_INPUTCOUNT        (3);
const int CUSTOMPE_INPUTCOUNT   (4);
const int SBOX_INPUTCOUNT       (1);
const int BENES_INPUTCOUNT      (4);

const int BFU_OUTPUTCOUNT       (2);
const int CUSTOMPE_OUTPUTCOUNT  (4);
const int SBOX_OUTPUTCOUNT      (4);
const int BENES_OUTPUTCOUNT     (4);

const int WINDTH_ONE_PORT       (30);
const int HEIGHT_ARITH_UNIT     (60);

#define ARITH_UNIT_OBJECT_NAME "ArithUnit"

const int MIN_STEP_COUNT    (5);
const int MAX_STEP_COUNT    (100);

const int BCU_MAX_COUNT(1);     //一张图里bcu的max count
const int RCU_MAX_COUNT(10000);  //一个bcu里rcu 的 max count
const int RCU_FIRST_COUNT(0);
const int RCU_LAST_COUNT(RCU_MAX_COUNT-1);
const int BFU_MAX_COUNT(8);     //一个rcu里bfu 的 max count
const int SBOX_MAX_COUNT(4);    //一个rcu里sbox 的 max count
const int BENES_MAX_COUNT(1);   //一个rcu里benes 的 max count
const int WRITE_MEM_MAX_COUNT(4);   //一个rcu里write mem 的 max count
const int READ_MEM_MAX_COUNT(4);   //一个rcu里read mem 最多为0~3编号，4个不同的port，如果port相同，可以。
const int READ_IMD_MAX_COUNT(4);   //一个rcu里read imd 最多为0~3编号，4个不同的port，如果port相同，可以。
const int READ_RCH_MAX_COUNT(16);   //所有Bcu里read rch 最多为0~16编号，16个不同的port，如果port相同，可以。
const int READ_LFSR_MAX_COUNT(16);   //所有Bcu里read lfsr 最多为0~16编号，16个不同的port，如果port相同，可以。
const int READ_RCH_MAX_COUNT_ONE_GROUP(16);   //一个group里read rch 最多为0~3编号，4个不同的port，如果port相同，可以。
const int WRITE_RCH_MAX_COUNT(4);   //一个Bcu里write rch 的 max count
const int WRITE_LFSR_MAX_COUNT(1);      //一个Bcu里write lfsr 的 max count
const int ALL_BCU_WRITE_RCH_COUNT(16);   //所有Bcu里write rch 的 count
const int OUT_FIFO_SIGNAL_RCU_INDEX(0);   //BCU里面的outfifo信号输入只能是最后一行rcu， fix：CGRA RCU只有一行 NOTE 可能引起bug
enum ModuleBcuIndex {
//    ModuleBcuIndexNone = -1,
    ModuleBcuIndex0 = 0,
    ModuleBcuIndex1,
    ModuleBcuIndex2,
    ModuleBcuIndex3,
};

enum ArithUnitId{
    BFU = 0,
    CUSTOMPE,
    SBOX,
    BENES,
    ArithUnit_END
};

enum ModuleUnitId{
    Module_None = 0,
    Arith_Unit,
    Module_Rcu,
    Module_Bcu,
    Module_Bca,
    Module_END
};

enum ElementUnitId {
    UNIT_NONE,
    BCU_ID,
    RCU_ID,
    BFU_ID,
    SBOX_ID,
    BENES_ID,
    RMEM_ID,
    WMEM_ID,
    PORT_ID,
    RIMD_ID,
    RRCH_ID,
    WRCH_ID,
    RLFSR_ID,
    WLFSR_ID,
    CUSTOMPE_ID,
};

enum BfuFuncIndex {
    Func_Au_Index,
    Func_Su_Index,
    Func_Lu_Index,
    Func_Mu_Index,
};

enum SelectedItemsAttr {
    SelectedAttrNone            = 0x00000000,
    isEnabledAlign              = 0x00000001,
    isEnabledEquidistance       = 0x00000002,
    isEnabledDel                = 0x00000004,
    isEnabledCopy               = 0x00000008,
    isEnabledCut                = 0x00000010,
    isEnabledPaste              = 0x00000020,
    isEnabledUndo               = 0x00000040,
    isEnabledRedo               = 0x00000080,
};

const QMap<int,QString> mapBfuFunc ={
    {Func_Au_Index,"au"},
    {Func_Su_Index,"su"},
    {Func_Lu_Index,"lu"},
    {Func_Mu_Index,"mu"},
};

const QMap<int,QString> mapBfuAuMod ={
    {0,"2^32"},
    {1,"2^16"},
    {2,"2^8"},
    {3,"2^31-1"},
};

const QMap<int,QString> mapBfuBypass ={
    {0,"A"},
    {1,"B"},
    {2,"T"},
    {3,"B`"},
};

enum InputPortType {
    InputPort_NULL = 0,
    InputPort_CurrentBfuX,
    InputPort_CurrentBfuY,
    InputPort_CurrentSbox,
    InputPort_CurrentBenes,
    InputPort_LastBfuX,
    InputPort_LastBfuY,
    InputPort_LastSbox,
    InputPort_LastBenes,
    InputPort_InFifo,
    InputPort_Mem,
    InputPort_Imd,
    InputPort_Rch,
    InputPort_Lfsr,
    InputPort_CurrentCustomPE,
    InputPort_LastCustomPE,
};

const QMap<int,QString> mapInputPortType ={
    {InputPort_NULL,"空"},
    {InputPort_CurrentBfuX,"当前行BFU输出X"},
    {InputPort_CurrentBfuY,"当前行BFU输出Y"},
    {InputPort_CurrentCustomPE,"当前行CUSTOMPE输出"},
    {InputPort_CurrentSbox,"当前行SBOX输出"},
    {InputPort_CurrentBenes,"当前行BENES输出"},
    {InputPort_LastBfuX,"上一行BFU输出X"},
    {InputPort_LastBfuY,"上一行BFU输出Y"},
    {InputPort_LastSbox,"上一行SBOX输出"},
    {InputPort_LastBenes,"上一行BENES输出"},
    {InputPort_InFifo,"入口FIFO"},
    {InputPort_Mem,"MEM输出"},
};

const QMap<int,QString> mapModuleUnitName ={
    {Module_Rcu,"RCU"},
    {Module_Bcu,"BCU"},
    {Module_Bca,"BCA"},
};

const QMap<int,QString> mapArithUnitName ={
    {BFU,"BFU"},
    {CUSTOMPE,"CBFU"},
    {SBOX,"SBOX"},
    {BENES,"BENES"},
};

const QMap<int,int> mapArithUnitInPort ={
    {BFU,BFU_INPUTCOUNT},
    {SBOX,SBOX_INPUTCOUNT},
    {BENES,BENES_INPUTCOUNT}
};

const QMap<int,int> mapArithUnitOutPort ={
    {BFU,BFU_OUTPUTCOUNT},
    {SBOX,SBOX_OUTPUTCOUNT},
    {BENES,BENES_OUTPUTCOUNT}
};

const QMap<int,QString> mapSboxMode ={
    {0,"4bit-4bit拼接"},
    {1,"6bit-4bit拼接"},
    {2,"8bit-8bit拼接"},
    {3,"8bit-8bit异或"},
    {4,"8bit-32bit"},
    {5,"9bit-9bit一路"},
};

enum SboxGroupValue {
    SboxGroupValue_0 = 0,
    SboxGroupValue_1,
    SboxGroupValue_2,
    SboxGroupValue_3,
};

const QString XML_Attr_Id       ("id");

const QString XML_XAtParent     ("XAtParent");
const QString XML_YAtParent     ("YAtParent");
const QString XML_X             ("Left");
const QString XML_Y             ("Top");
const QString XML_Width         ("Width");
const QString XML_Height        ("Height");


const QString XML_BCU_DATA_SRC      ("router");
const QString XML_BCU_DATA_SORT     ("sort");
const QString XML_BCU_DATA_BURST    ("burst");
const QString XML_BCU_DATA_GAP      ("gap");
const QString XML_BCU_DATA_INFIFOSRC      ("infifosrc");
const QString XML_BCU_DATA_OUTFIFOSRC     ("outfifosrc");
const QString XML_BCU_DATA_LFSRGROUP      ("lfsrgroup");
const QString XML_BCU_OUT_FIFO      ("OUT_FIFO");
const QString XML_BCU_RIMD_MODE128_32   ("mode128_32");
const QString XML_BCU_MEM_SRC           ("MEM_SRC");
const QString XML_BCU_IMD_SRC           ("IMD_SRC");
const QString XML_BCU_INFIFO_SRC        ("INFIFO_SRC");
const QString XML_BCU_LFSR_DEPTH        ("LFSR_DEPTH");
const QString XML_BCU_LFSR_WIDTH        ("LFSR_WIDTH");
const QString XML_BCU_LFSR_ADDRESSMODE  ("LFSR_ADDRESSMODE");

const QString XML_PORT_INFECT_POS   ("PORT_INFLECTION_POS");
const QString XML_PORT_PRESSED_POS  ("PORT_PRESSED_POS");

const QString XML_INFECT_POS   ("inflection_pos");
const QString XML_PRESSED_POS  ("pressed_pos");

const QString XML_RCU_LOOP              ("LOOP");
const QString XML_RCU_LOOP_START_END    ("LOOP_start_end");
const QString XML_RCU_LOOP_TIMES        ("LOOP_times");
const QString XML_RCU_LOOP_START_END2   ("LOOP_start_end2");
const QString XML_RCU_LOOP_TIMES2       ("LOOP_times2");
const QString XML_RCU_WMEM_NAME         ("MEM_WRITE");
const QString XML_RCU_WMEM_OFFSET       ("offset");
const QString XML_RCU_WMEM_MASK         ("mask");
const QString XML_RCU_WMEM_MODE         ("mode");
const QString XML_RCU_WMEM_THREASHOLD   ("threashold");
const QString XML_RCU_WMEM_ADDR         ("addr");

const QString XML_RCU_RMEM_NAME         ("MEM_READ");
const QString XML_RCU_RMEM_OFFSET       ("offset");
const QString XML_RCU_RMEM_MODE         ("mode");
const QString XML_RCU_RMEM_MODE128_32   ("mode128_32");
const QString XML_RCU_RMEM_THREASHOLD   ("threashold");
const QString XML_RCU_RMEM_ADDR1        ("addr1");
const QString XML_RCU_RMEM_ADDR2        ("addr2");
const QString XML_RCU_RMEM_ADDR3        ("addr3");
const QString XML_RCU_RMEM_ADDR4        ("addr4");

const QString XML_RCU_RCH_NAME          ("RCH");
const QString XML_RCU_RCH_MODE          ("mode");
const QString XML_RCU_RCH_ADDR1         ("addr1");
const QString XML_RCU_RCH_ADDR2         ("addr2");
const QString XML_RCU_RCH_ADDR3         ("addr3");
const QString XML_RCU_RCH_ADDR4         ("addr4");

const QString XML_RCU_MEM_INPUT_NAME    ("MEM_INPUT");
const QString XML_RCU_RCH_INPUT_NAME    ("RCH_INPUT");
const QString XML_RCU_LFSR_INPUT_NAME   ("LFSR_INPUT");

const QString XML_Bfu_Func      ("func");
const QString XML_Bfu_Func_Exp  ("exp");
const QString XML_Bfu_Au_Mod    ("mod");
const QString XML_Bfu_Au_Carry  ("carry");
const QString XML_Bfu_Mu_Mod    ("muMod");
const QString XML_Bfu_Bypass    ("Bypass");
const QString XML_Bfu_Bypass_Exp("BypassExp");
const QString XML_Bfu_InputA    ("INPUTA");
const QString XML_Bfu_InputB    ("INPUTB");
const QString XML_Bfu_InputT    ("INPUTT");

const QString XML_CustomPE_Func      ("func");
const QString XML_CustomPE_Func_Exp  ("exp");
const QString XML_CustomPE_Au_Mod    ("mod");
//const QString XML_CustomPE_Au_Carry  ("carry");
const QString XML_CustomPE_Mu_Mod    ("muMod");
const QString XML_CustomPE_Bypass    ("Bypass");
const QString XML_CustomPE_Bypass_Exp("BypassExp");
const QString XML_CustomPE_Input     ("INPUT");
const QString XML_CustomPE_InputNum  ("inputNum");
const QString XML_CustomPE_OutputNum ("outputNum");

const QString XML_Sbox_Src      ("src");
const QString XML_Sbox_Mode     ("mode");
const QString XML_Sbox_Group    ("group");
const QString XML_Sbox_ByteSel  ("byteSel");
const QString XML_Sbox_IncreaseMode     ("increasemode");
const QString XML_Sbox_Input    ("INPUT");
const QString XML_Sbox_Cnt      ("CNT");
const QString XML_Sbox_Cnt_Value("value");

const QString XML_Benes_Src     ("src");
const QString XML_Benes_Input   ("INPUT");

const QString XML_InPort_Type   ("Type");//bfu-x 1;bfu-y 2;sbox 3;benes 4
const QString XML_InPort_Index  ("Index");//bfu pe_index 0-7;sbox pe_port_index  0-11;benes pe_port_index  0-3
const QString XML_InPort_Address  ("Address");

const QString XML_InPort_Middle_Y   ("MiddlePosY");


const qreal DEFAULT_PEN_WIDTH      (1);
const qreal DEFAULT_PEN_WIDTH2     (2);

const int    ARITH_TYPE_TEXT_PIXELSIZE(20);
const int    ARITH_TEXT_PIXELSIZE(14);
const QPen   ARITH_TEXT_PEN(QColor(Qt::black),DEFAULT_PEN_WIDTH,Qt::SolidLine);
const QPen   ARITH_BOUNDING_PEN(QColor(Qt::black),DEFAULT_PEN_WIDTH,Qt::SolidLine);
const QBrush ARITH_BRUSH_FILLIN(Qt::transparent,Qt::SolidPattern);
const QBrush ARITH_TEXT_NORMAL_BRUSH_FILLIN(Qt::transparent,Qt::SolidPattern);
const QBrush ARITH_TEXT_SELECTED_BRUSH_FILLIN(QColor(255,255,0,80),Qt::SolidPattern);
//const QBrush ARITH_TEXT_SELECTED_BRUSH_FILLIN(QColor(255,255,0),Qt::SolidPattern);

const QPen   ARITH_FOCUSED_BOUNDING_PEN(QColor(Qt::green),DEFAULT_PEN_WIDTH,Qt::SolidLine);
const QBrush ARITH_FOCUSED_BRUSH_FILLIN(QColor(255,255,0,80),Qt::Dense1Pattern);
//const QBrush ARITH_FOCUSED_BRUSH_FILLIN(QColor(255,255,0),Qt::Dense1Pattern);

const QPen   ARITH_COLLIDED_BOUNDING_PEN(QColor(Qt::green),DEFAULT_PEN_WIDTH,Qt::SolidLine);
const QBrush ARITH_COLLIDED_BRUSH_FILLIN(QColor(0,255,0,80),Qt::Dense3Pattern);
//const QBrush ARITH_COLLIDED_BRUSH_FILLIN(QColor(0,255,0),Qt::Dense3Pattern);

const QRectF ARITHUNIT_PORT_DEFAULT_RECT    (0,0,14,18);
const qreal  ARITHUNIT_TEXT_RECT_WIDTH      (20);
const QRectF BFU_ELEMENT_DEFAULT_RECT       (0,0,qMax(BFU_INPUTCOUNT,BFU_OUTPUTCOUNT)*WINDTH_ONE_PORT,HEIGHT_ARITH_UNIT);
const QRectF CUSTOMPE_ELEMENT_DEFAULT_RECT  (0,0,qMax(BENES_INPUTCOUNT,BENES_OUTPUTCOUNT)*WINDTH_ONE_PORT,HEIGHT_ARITH_UNIT);
const QRectF SBOX_ELEMENT_DEFAULT_RECT      (0,0,qMax(SBOX_INPUTCOUNT,SBOX_OUTPUTCOUNT)*WINDTH_ONE_PORT,HEIGHT_ARITH_UNIT);

const qreal  SBOX_ELEMENT_MARK_SPACE        (2);
const qreal  SBOX_ELEMENT_MARK_SIDE_L       (10);

const qreal  SBOX_ELEMENT_MARK_1_LEFT       ((SBOX_ELEMENT_DEFAULT_RECT.width()-ARITHUNIT_TEXT_RECT_WIDTH-3*SBOX_ELEMENT_MARK_SIDE_L
                                             -2*SBOX_ELEMENT_MARK_SPACE)/2+SBOX_ELEMENT_DEFAULT_RECT.left()+ARITHUNIT_TEXT_RECT_WIDTH);
const qreal  SBOX_ELEMENT_MARK_1_TOP        ((SBOX_ELEMENT_DEFAULT_RECT.height()-2*SBOX_ELEMENT_MARK_SIDE_L-SBOX_ELEMENT_MARK_SPACE)/2
                                             +SBOX_ELEMENT_DEFAULT_RECT.top());
const int SBOX_MARK_RECT_COUNT              (6);
const QRectF SBOX_ELEMENT_MARK_RECT[SBOX_MARK_RECT_COUNT]
                                            {{QRectF(SBOX_ELEMENT_MARK_1_LEFT,SBOX_ELEMENT_MARK_1_TOP,
                                                     SBOX_ELEMENT_MARK_SIDE_L,SBOX_ELEMENT_MARK_SIDE_L)},
                                             {QRectF(SBOX_ELEMENT_MARK_1_LEFT + SBOX_ELEMENT_MARK_SPACE + SBOX_ELEMENT_MARK_SIDE_L,
                                                     SBOX_ELEMENT_MARK_1_TOP,
                                                     SBOX_ELEMENT_MARK_SIDE_L,
                                                     SBOX_ELEMENT_MARK_SIDE_L)},
                                             {QRectF(SBOX_ELEMENT_MARK_1_LEFT + 2*SBOX_ELEMENT_MARK_SPACE + 2*SBOX_ELEMENT_MARK_SIDE_L,
                                                     SBOX_ELEMENT_MARK_1_TOP,
                                                     SBOX_ELEMENT_MARK_SIDE_L,
                                                     SBOX_ELEMENT_MARK_SIDE_L)},
                                             {QRectF(SBOX_ELEMENT_MARK_RECT[0].left(),
                                                     SBOX_ELEMENT_MARK_1_TOP + SBOX_ELEMENT_MARK_SPACE + SBOX_ELEMENT_MARK_SIDE_L,
                                                     SBOX_ELEMENT_MARK_SIDE_L,
                                                     SBOX_ELEMENT_MARK_SIDE_L)},
                                             {QRectF(SBOX_ELEMENT_MARK_RECT[1].left(),
                                                     SBOX_ELEMENT_MARK_1_TOP + SBOX_ELEMENT_MARK_SPACE + SBOX_ELEMENT_MARK_SIDE_L,
                                                     SBOX_ELEMENT_MARK_SIDE_L,
                                                     SBOX_ELEMENT_MARK_SIDE_L)},
                                             {QRectF(SBOX_ELEMENT_MARK_RECT[2].left(),
                                                     SBOX_ELEMENT_MARK_1_TOP + SBOX_ELEMENT_MARK_SPACE + SBOX_ELEMENT_MARK_SIDE_L,
                                                     SBOX_ELEMENT_MARK_SIDE_L,
                                                     SBOX_ELEMENT_MARK_SIDE_L)},
                                             };

const QRectF BENES_ELEMENT_DEFAULT_RECT     (0,0,qMax(BENES_INPUTCOUNT,BENES_OUTPUTCOUNT)*WINDTH_ONE_PORT,HEIGHT_ARITH_UNIT);

const qreal  BENES_ELEMENT_MARK_SPACE_Y     (10);
const qreal  BENES_ELEMENT_MARK_SPACE_X     (20);
const QRectF BENES_ELEMENT_MARK_RECT        (QRectF(BENES_ELEMENT_DEFAULT_RECT.left()+ARITHUNIT_TEXT_RECT_WIDTH+BENES_ELEMENT_MARK_SPACE_X,
                                                    BENES_ELEMENT_DEFAULT_RECT.top()+ARITHUNIT_PORT_DEFAULT_RECT.height()+BENES_ELEMENT_MARK_SPACE_Y,
                                                    BENES_ELEMENT_DEFAULT_RECT.width()-ARITHUNIT_TEXT_RECT_WIDTH-2*BENES_ELEMENT_MARK_SPACE_X,
                                                    BENES_ELEMENT_DEFAULT_RECT.height()-2*ARITHUNIT_PORT_DEFAULT_RECT.height()-2*BENES_ELEMENT_MARK_SPACE_Y));
const int RCU_INPUT_RECT_HEIGHT             (30);
const int RCU_OUTPORT_RECT_HEIGHT           (30);

const int BCU_INPORT_SPACE_COUNT            (5);
const int BCU_OUTPORT_SPACE_COUNT           (5);
const int BCU_INPORT_RECT_HEIGHT            (30);
const int BCU_OUTPORT_RECT_HEIGHT           (30);
const qreal BCU_RIGHT_MARK_WIDTH            (30);
const qreal BCU_TEXT_RECT_WIDTH             (BCU_RIGHT_MARK_WIDTH);

const QRectF RCU_MODULE_DEFAULT_RECT        (0,0,1100,170);
const qreal  RCU_MODULE_MIN_WIDTH           (500);
//const qreal  RCU_MODULE_MIN_HEIGHT          (100);
const qreal  RCU_MODULE_MIN_HEIGHT          (170);
const QRectF BCU_MODULE_DEFAULT_RECT        (0,0,1200,2650);
const qreal  BCU_MODULE_MIN_WIDTH           (600);
const qreal  BCU_MODULE_MIN_HEIGHT          (300);

const qreal ARITHUNIT_IN_SPACE              (2);// space interior in arithmeic unit bounding.for bfu,sbox,benes
//const qreal ENABLED_RECT_IN_SPACE           (20);// space interior in module bounding. for example: bcu,rcu
const qreal ENABLED_RECT_IN_SPACE           (50);// space interior in module bounding. for example: bcu,rcu
const qreal ENABLED_BCU_RECT_IN_SPACE       (30);
const qreal ENABLED_RCU_RECT_IN_SPACE       (25);

const int    RCU_TEXT_PIXELSIZE             (18);
const qreal  RCU_TEXT_RECT_WIDTH            (24);
const QPen   RCU_TEXT_PEN                   (QColor(Qt::black),DEFAULT_PEN_WIDTH,Qt::SolidLine);
const QPen   RCU_BOUNDING_PEN               (QColor(Qt::black),DEFAULT_PEN_WIDTH,Qt::DashLine);
const QBrush RCU_PAINT_BRUSH_FILLIN         (Qt::transparent,Qt::SolidPattern);
const QBrush RCU_TEXT_NORMAL_BRUSH_FILLIN   (QColor(200,190,230),Qt::SolidPattern);
const QBrush RCU_TEXT_SELECTED_BRUSH_FILLIN (QColor(255,255,0,80),Qt::SolidPattern);
//const QBrush RCU_TEXT_SELECTED_BRUSH_FILLIN (QColor(255,255,0),Qt::SolidPattern);

const QPen   RCU_FOCUSED_BOUNDING_PEN       (QColor(Qt::green),DEFAULT_PEN_WIDTH,Qt::SolidLine);
const QBrush RCU_FOCUSED_BRUSH_FILLIN       (QColor(255,255,0,80),Qt::Dense1Pattern);
//const QBrush RCU_FOCUSED_BRUSH_FILLIN       (QColor(255,255,0),Qt::Dense1Pattern);

const QPen   RCU_COLLIDED_BOUNDING_PEN      (QColor(Qt::green),DEFAULT_PEN_WIDTH,Qt::SolidLine);
const QBrush RCU_COLLIDED_BRUSH_FILLIN      (QColor(0,255,0,80),Qt::Dense3Pattern);
//const QBrush RCU_COLLIDED_BRUSH_FILLIN      (QColor(0,255,0),Qt::Dense3Pattern);

const int    BCU_TEXT_PIXELSIZE             (22);
const QPen   BCU_TEXT_PEN                   (QColor(Qt::black),DEFAULT_PEN_WIDTH,Qt::SolidLine);
const QPen   BCU_BOUNDING_PEN               (QColor(Qt::black),DEFAULT_PEN_WIDTH,Qt::DashLine);
const QBrush BCU_BRUSH_FILLIN               (Qt::white,Qt::SolidPattern);
const QBrush BCU_TEXT_NORMAL_BRUSH_FILLIN   (QColor(150,210,250),Qt::SolidPattern);
const QBrush BCU_INFIFO_BRUSH_FILLIN        (QColor(255,180,180,120),Qt::Dense4Pattern);
const QBrush BCU_OUTFIFO_BRUSH_FILLIN       (QColor(0,0,255,120),Qt::Dense4Pattern);
//const QBrush BCU_TEXT_SELECTED_BRUSH_FILLIN (QColor(255,255,0,80),Qt::SolidPattern);
const QBrush BCU_TEXT_SELECTED_BRUSH_FILLIN (QColor(255,255,0),Qt::SolidPattern);

const QPen   BCU_FOCUSED_BOUNDING_PEN(QColor(Qt::green),DEFAULT_PEN_WIDTH,Qt::SolidLine);
//const QBrush BCU_FOCUSED_BRUSH_FILLIN(QColor(255,255,0,80),Qt::Dense1Pattern);
const QBrush BCU_FOCUSED_BRUSH_FILLIN(QColor(255,255,0),Qt::Dense1Pattern);

const QPen   BCU_COLLIDED_BOUNDING_PEN(QColor(Qt::green),DEFAULT_PEN_WIDTH,Qt::SolidLine);
const QBrush BCU_COLLIDED_BRUSH_FILLIN(QColor(0,255,0,80),Qt::Dense3Pattern);
//const QBrush BCU_COLLIDED_BRUSH_FILLIN(QColor(0,255,0),Qt::Dense3Pattern);

const QString RCU_IFIFO_NAME("InFifo");
const QString RCU_OFIFO_NAME("OutFifo");

//const QString BCU_IFIFO_NAME("InFifo");
//const QString BCU_OFIFO_NAME("OutFifo");

//鼠标在item边框时状态。
enum HoverPosAtItem{
    PosNone         = (0<<0),
    PosTop          = (1<<1),
    PosBottom       = (1<<2),
    PosLeft         = (1<<3),
    PosRight        = (1<<4),
    PosHorLine      = (1<<5),
    PosVerLine      = (1<<6),
    PosTopLeft      = (PosTop|PosLeft),
    PosTopRight     = (PosTop|PosRight),
    PosBottomLeft   = (PosBottom|PosLeft),
    PosBottomRight  = (PosBottom|PosRight),

};
//align direction
enum ItemsAlignPosAttr{
    ItemsAlignNone         = (0<<0),
    ItemsAlignTop          = (1<<1),
    ItemsAlignBottom       = (1<<2),
    ItemsAlignLeft         = (1<<3),
    ItemsAlignRight        = (1<<4),
};

const qreal DEFAULT_COLLIDE_RANGE(3);   //鼠标在此范围内，则选中item边框
const qreal MIN_MODULE_WIDTH(40);
const qreal MIN_MODELE_HEIGHT(40);


#define CHECK_AND_DELETE(name) \
    if(name) \
    { \
        delete name; \
        name = NULL; \
    }
enum RcaSceneStates
{
    Painting_wire,
    Normal,
    Add_Wire,
    Add_Bcu,
    Add_Rcu,
    Add_Bfu,
    Add_Sbox,
    Add_Benes,
    Add_Imd,
    Add_Rch,
    Add_Mem,
    Add_Lfsr,
    Add_CustomPE,
};

enum HoverPortState
{
    None_Hover = 0,
    Can_Hover,
    Cannot_Hover,
};






const qreal WIRE_DEFAULT_PEN_WIDTH      (1);
const qreal WIRE_DARK_PEN1_WIDTH        (2);
const qreal WIRE_ADJUEST_WIDTH          (3);   //鼠标在此范围内，则可以调整wire

//const QPen   WIRE_UNSELECTED_PEN(QColor(0,0,0),WIRE_DEFAULT_PEN1_WIDTH,Qt::SolidLine);
//const QPen   WIRE_SELECTED_PEN(QColor(100,100,255),WIRE_DEFAULT_PEN1_WIDTH,Qt::SolidLine);
const QBrush WIRE_UNSELECTED_BRUSH(QColor(50,50,50),Qt::SolidPattern);
const QBrush WIRE_SELECTED_BRUSH(QColor(100,100,255),Qt::SolidPattern);

//const QColor WIRE_DRAG_ENTER_COLOR          (QColor(0,0,255,80));
const QColor WIRE_DRAG_ENTER_COLOR          (QColor(0,0,255));
const QColor WIRE_DRAG_LEAVE_COLOR          (QColor(255,255,255));

const int PORT_DEFAULT_TEXT_HEIGHT                  (10);
const QBrush PORT_NORMAL_BRUSH_FILLIN       (Qt::lightGray,Qt::Dense4Pattern);
const QBrush PORT_CONNECTED_BRUSH_FILLIN    (QColor(160,160,255),Qt::SolidPattern);

const int    MODULE_MEM_TEXT_PIXELSIZE(10);
const QPen   MODULE_MEM_TEXT_PEN(QColor(Qt::black),DEFAULT_PEN_WIDTH,Qt::SolidLine);
const QPen   MODULE_MEM_BOUNDING_PEN(QColor(Qt::green),DEFAULT_PEN_WIDTH,Qt::SolidLine);
//const QBrush MODULE_MEM_FOCUSED_BRUSH_FILLIN(QColor(255,255,0,80),Qt::SolidPattern);
const QBrush MODULE_MEM_FOCUSED_BRUSH_FILLIN(QColor(255,255,0),Qt::SolidPattern);

const QRectF MODULE_READ_MEM_DEFAULT_RECT           (0,0,22,24);
const QRectF MODULE_READ_MEM_DEFAULT_PAINT_RECT     (0,10,22,14);
const QPen   MODULE_READ_MEM_PEN                    (QColor(Qt::black),DEFAULT_PEN_WIDTH,Qt::SolidLine);
const QPen   MODULE_READ_MEM_LINE_PEN               (QColor(Qt::black),DEFAULT_PEN_WIDTH2,Qt::SolidLine);
//const QBrush MODULE_READ_MEM_BRUSH                  (QColor(255,180,180,120),Qt::Dense1Pattern);
const QBrush MODULE_READ_MEM_BRUSH                  (QColor(0,162,232),Qt::Dense1Pattern);

const QRectF MODULE_WRITE_MEM_DEFAULT_RECT          (0,0,22,24);
const QRectF MODULE_WRITE_MEM_DEFAULT_PAINT_RECT    (0,0,22,14);
const QPen   MODULE_WRITE_MEM_PEN                   (QColor(Qt::black),DEFAULT_PEN_WIDTH,Qt::SolidLine);
const QPen   MODULE_WRITE_MEM_LINE_PEN              (QColor(Qt::black),DEFAULT_PEN_WIDTH2,Qt::SolidLine);
//const QBrush MODULE_WRITE_MEM_BRUSH                 (QColor(0,0,255,120),Qt::Dense1Pattern);
const QBrush MODULE_WRITE_MEM_BRUSH                 (QColor(0,0,255),Qt::Dense1Pattern);

const qreal  MAP_MEM_DEFAULT_WIDTH                  (MODULE_READ_MEM_DEFAULT_PAINT_RECT.width());
const qreal  MAP_MEM_DEFAULT_HEIGHT                 (7);

const int    MODULE_IMD_TEXT_PIXELSIZE(10);
const QPen   MODULE_IMD_TEXT_PEN(QColor(Qt::black),DEFAULT_PEN_WIDTH,Qt::SolidLine);
const QPen   MODULE_IMD_BOUNDING_PEN(QColor(Qt::green),DEFAULT_PEN_WIDTH,Qt::SolidLine);
//const QBrush MODULE_IMD_FOCUSED_BRUSH_FILLIN(QColor(255,255,0,80),Qt::SolidPattern);
const QBrush MODULE_IMD_FOCUSED_BRUSH_FILLIN(QColor(255,255,0),Qt::SolidPattern);


const QRectF MODULE_READ_IMD_DEFAULT_RECT           (0,0,22,24);
const QRectF MODULE_READ_IMD_DEFAULT_PAINT_RECT     (0,10,22,14);
const QPen   MODULE_READ_IMD_PEN                    (QColor(Qt::black),DEFAULT_PEN_WIDTH,Qt::SolidLine);
const QPen   MODULE_READ_IMD_LINE_PEN               (QColor(Qt::black),DEFAULT_PEN_WIDTH2,Qt::SolidLine);
//const QBrush MODULE_READ_IMD_BRUSH                  (QColor(255,180,180,120),Qt::Dense1Pattern);
const QBrush MODULE_READ_IMD_BRUSH                  (QColor(181,230,29),Qt::Dense1Pattern);
const qreal  MAP_IMD_DEFAULT_WIDTH                  (MODULE_READ_IMD_DEFAULT_PAINT_RECT.width());
const qreal  MAP_IMD_DEFAULT_HEIGHT                 (7);

const int    MODULE_RCH_TEXT_PIXELSIZE(10);
const QPen   MODULE_RCH_TEXT_PEN(QColor(Qt::black),DEFAULT_PEN_WIDTH,Qt::SolidLine);
const QPen   MODULE_RCH_BOUNDING_PEN(QColor(Qt::green),DEFAULT_PEN_WIDTH,Qt::SolidLine);
//const QBrush MODULE_RCH_FOCUSED_BRUSH_FILLIN(QColor(255,255,0,80),Qt::SolidPattern);
const QBrush MODULE_RCH_FOCUSED_BRUSH_FILLIN(QColor(255,255,0),Qt::SolidPattern);


const QRectF MODULE_READ_RCH_DEFAULT_RECT           (0,0,22,24);
const QRectF MODULE_READ_RCH_DEFAULT_PAINT_RECT     (0,10,22,14);
const QPen   MODULE_READ_RCH_PEN                    (QColor(Qt::black),DEFAULT_PEN_WIDTH,Qt::SolidLine);
const QPen   MODULE_READ_RCH_LINE_PEN               (QColor(Qt::black),DEFAULT_PEN_WIDTH2,Qt::SolidLine);
//const QBrush MODULE_READ_RCH_BRUSH                  (QColor(255,180,180,120),Qt::Dense1Pattern);
const QBrush MODULE_READ_RCH_BRUSH                  (QColor(239,228,176),Qt::Dense1Pattern);
const QBrush MODULE_READ_LFSR_BRUSH                 (QColor(215,101,119),Qt::Dense1Pattern);

const QRectF MODULE_WRITE_RCH_DEFAULT_RECT           (0,0,22,24);
const QRectF MODULE_WRITE_RCH_DEFAULT_PAINT_RECT     (0,0,22,14);
const QPen   MODULE_WRITE_RCH_PEN                    (QColor(Qt::black),DEFAULT_PEN_WIDTH,Qt::SolidLine);
const QPen   MODULE_WRITE_RCH_LINE_PEN               (QColor(Qt::black),DEFAULT_PEN_WIDTH2,Qt::SolidLine);
//const QBrush MODULE_WRITE_RCH_BRUSH                  (QColor(255,180,180,120),Qt::Dense1Pattern);
const QBrush MODULE_WRITE_RCH_BRUSH                  (QColor(255,201,14),Qt::Dense1Pattern);
const QBrush MODULE_WRITE_LFSR_BRUSH                 (QColor(215,21,85),Qt::Dense1Pattern);


const qreal  MAP_RCH_DEFAULT_WIDTH                  (MODULE_READ_RCH_DEFAULT_PAINT_RECT.width());
const qreal  MAP_RCH_DEFAULT_HEIGHT                 (7);

enum WireDirection
{
    DirectionError = 0, //线不符合要求
    DirectionVer,       //竖线
    DirectionHor,       //横线
};

enum PortFromWhereAttr {
    PORT_FROM_None = 0,
    PORT_FROM_BFU_A,
    PORT_FROM_BFU_B,
    PORT_FROM_BFU_T,
    PORT_FROM_BFU_X,
    PORT_FROM_BFU_Y,
    PORT_FROM_CUSTOMPE_IN,
    PORT_FROM_CUSTOMPE_OUT,
    PORT_FROM_SBOX_IN,
    PORT_FROM_SBOX_OUT,
    PORT_FROM_BENES_IN,
    PORT_FROM_BENES_OUT,
    PORT_FROM_WMEM,
    PORT_FROM_RMEM,
    PORT_FROM_RIMD,
    PORT_FROM_RRCH,
    PORT_FROM_WRCH,
    PORT_FROM_INFIFO,
    PORT_FORM_OUTFIFO,
    PORT_FORM_RLFSR,
    PORT_FORM_WLFSR,
};

const qreal BCU_AT_SCENE_POSX(100);
const qreal BCU_AT_SCENE_POSY(100);
const qreal BCU_AT_SCENE_YSPACE(100);
const qreal BCU_AT_SCENE_XSPACE(100);
enum EBcuComposing {
    BcuVCom = 0,
    BcuVCom2 = 1,
    BcuHCom = 2,
    BcuEnd
};

const QPointF BCU_POS_AT_SCENE[BCU_MAX_COUNT]{
    {BCU_AT_SCENE_POSX,BCU_AT_SCENE_POSY},
//    {BCU_AT_SCENE_POSX,BCU_AT_SCENE_POSY + 1*(BCU_AT_SCENE_YSPACE + BCU_MODULE_DEFAULT_RECT.height())},
//    {BCU_AT_SCENE_POSX,BCU_AT_SCENE_POSY + 2*(BCU_AT_SCENE_YSPACE + BCU_MODULE_DEFAULT_RECT.height())},
//    {BCU_AT_SCENE_POSX,BCU_AT_SCENE_POSY + 3*(BCU_AT_SCENE_YSPACE + BCU_MODULE_DEFAULT_RECT.height())},
};


const qreal RCU_AT_BCU_POSX(BCU_TEXT_RECT_WIDTH + ENABLED_BCU_RECT_IN_SPACE);
const qreal RCU_AT_BCU_POSY(ENABLED_BCU_RECT_IN_SPACE);
const qreal RCU_AT_BCU_YSPACE(50);

const QPointF RCU_POS_AT_BCU[RCU_MAX_COUNT]{
    {RCU_AT_BCU_POSX,RCU_AT_BCU_POSY},
    {RCU_AT_BCU_POSX,RCU_AT_BCU_POSY + 1*(RCU_AT_BCU_YSPACE + RCU_MODULE_DEFAULT_RECT.height())},
    {RCU_AT_BCU_POSX,RCU_AT_BCU_POSY + 2*(RCU_AT_BCU_YSPACE + RCU_MODULE_DEFAULT_RECT.height())},
    {RCU_AT_BCU_POSX,RCU_AT_BCU_POSY + 3*(RCU_AT_BCU_YSPACE + RCU_MODULE_DEFAULT_RECT.height())},
};

const qreal ARITHUNIT_AT_RCU_POSX(RCU_TEXT_RECT_WIDTH + ENABLED_RCU_RECT_IN_SPACE);
const qreal ARITHUNIT_AT_RCU_POSY(RCU_INPUT_RECT_HEIGHT + ENABLED_RCU_RECT_IN_SPACE);
const qreal ARITHUNIT_AT_RCU_XSPACE(20);

const QPointF ARITHUNIT_POS_AT_RCU[BFU_MAX_COUNT + SBOX_MAX_COUNT + BENES_MAX_COUNT]{
    //BFU
    {ARITHUNIT_AT_RCU_POSX,ARITHUNIT_AT_RCU_POSY},
    {ARITHUNIT_POS_AT_RCU[0].x() + ARITHUNIT_AT_RCU_XSPACE + BFU_ELEMENT_DEFAULT_RECT.width(),ARITHUNIT_AT_RCU_POSY},
    {ARITHUNIT_POS_AT_RCU[1].x() + ARITHUNIT_AT_RCU_XSPACE + BFU_ELEMENT_DEFAULT_RECT.width(),ARITHUNIT_AT_RCU_POSY},
    {ARITHUNIT_POS_AT_RCU[2].x() + ARITHUNIT_AT_RCU_XSPACE + BFU_ELEMENT_DEFAULT_RECT.width(),ARITHUNIT_AT_RCU_POSY},
    {ARITHUNIT_POS_AT_RCU[3].x() + ARITHUNIT_AT_RCU_XSPACE + BFU_ELEMENT_DEFAULT_RECT.width(),ARITHUNIT_AT_RCU_POSY},
    {ARITHUNIT_POS_AT_RCU[4].x() + ARITHUNIT_AT_RCU_XSPACE + BFU_ELEMENT_DEFAULT_RECT.width(),ARITHUNIT_AT_RCU_POSY},
    {ARITHUNIT_POS_AT_RCU[5].x() + ARITHUNIT_AT_RCU_XSPACE + BFU_ELEMENT_DEFAULT_RECT.width(),ARITHUNIT_AT_RCU_POSY},
    {ARITHUNIT_POS_AT_RCU[6].x() + ARITHUNIT_AT_RCU_XSPACE + BFU_ELEMENT_DEFAULT_RECT.width(),ARITHUNIT_AT_RCU_POSY},
    //SBOX
    {ARITHUNIT_POS_AT_RCU[7].x() + ARITHUNIT_AT_RCU_XSPACE + BFU_ELEMENT_DEFAULT_RECT.width(),ARITHUNIT_AT_RCU_POSY},
    {ARITHUNIT_POS_AT_RCU[8].x() + ARITHUNIT_AT_RCU_XSPACE + SBOX_ELEMENT_DEFAULT_RECT.width(),ARITHUNIT_AT_RCU_POSY},
    {ARITHUNIT_POS_AT_RCU[9].x() + ARITHUNIT_AT_RCU_XSPACE + SBOX_ELEMENT_DEFAULT_RECT.width(),ARITHUNIT_AT_RCU_POSY},
    {ARITHUNIT_POS_AT_RCU[10].x() + ARITHUNIT_AT_RCU_XSPACE + SBOX_ELEMENT_DEFAULT_RECT.width(),ARITHUNIT_AT_RCU_POSY},
    //BENES
    {ARITHUNIT_POS_AT_RCU[11].x() + ARITHUNIT_AT_RCU_XSPACE + SBOX_ELEMENT_DEFAULT_RECT.width(),ARITHUNIT_AT_RCU_POSY},
};

//const QPointF ARITHUNIT_POS_AT_RCU[BFU_MAX_COUNT + SBOX_MAX_COUNT + BENES_MAX_COUNT]{
//    {ARITHUNIT_AT_RCU_POSX,ARITHUNIT_AT_RCU_POSY},
//    {ARITHUNIT_AT_RCU_POSX + 1*(ARITHUNIT_AT_RCU_XSPACE + BFU_ELEMENT_DEFAULT_RECT.width()),ARITHUNIT_AT_RCU_POSY},
//    {ARITHUNIT_AT_RCU_POSX + 2*(ARITHUNIT_AT_RCU_XSPACE + BFU_ELEMENT_DEFAULT_RECT.width()),ARITHUNIT_AT_RCU_POSY},
//    {ARITHUNIT_AT_RCU_POSX + 3*(ARITHUNIT_AT_RCU_XSPACE + BFU_ELEMENT_DEFAULT_RECT.width()),ARITHUNIT_AT_RCU_POSY},
//    {ARITHUNIT_AT_RCU_POSX + 4*(ARITHUNIT_AT_RCU_XSPACE + BFU_ELEMENT_DEFAULT_RECT.width()),ARITHUNIT_AT_RCU_POSY},
//    {ARITHUNIT_AT_RCU_POSX + 5*(ARITHUNIT_AT_RCU_XSPACE + BFU_ELEMENT_DEFAULT_RECT.width()),ARITHUNIT_AT_RCU_POSY},
//    {ARITHUNIT_AT_RCU_POSX + 6*(ARITHUNIT_AT_RCU_XSPACE + BFU_ELEMENT_DEFAULT_RECT.width()),ARITHUNIT_AT_RCU_POSY},
//    {ARITHUNIT_AT_RCU_POSX + 7*(ARITHUNIT_AT_RCU_XSPACE + BFU_ELEMENT_DEFAULT_RECT.width()),ARITHUNIT_AT_RCU_POSY},
//};

}

#endif // RCAGRAPHDEF_H
