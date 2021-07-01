#include "elementcustompe.h"
#include "RcaGraphDef.h"

#include <QGraphicsScene>
#include <QDebug>
#include <QDebug>

using namespace rca_space;

ElementCustomPE::ElementCustomPE(int bcu,int rcu,int index, QGraphicsItem* parent)
    : BaseArithUnit(CUSTOMPE,bcu,rcu,index,parent)
    , funcIndex(Func_Au_Index)
    , funcExp(QString())
    , funcAuModIndex(0)
    , funcMuModIndex(0)
    , bypassIndex(0)
    , bypassExp(QString())
{
    unitId = CUSTOMPE_ID;
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setAcceptHoverEvents(true);

    auto i = 0;
    for(i = 0; i < m_inputNum; ++i)
    {
        m_inputPortVector.emplace_back(new ElementPort(InputDirection, this, PORT_FROM_CUSTOMPE_IN));
    }
    for(i = 0; i < m_outputNum; ++i)
    {
        m_outputPortVector.emplace_back(new ElementPort(OutputDirection, this, PORT_FROM_CUSTOMPE_OUT));
    }

    realBoundingRect = CUSTOMPE_ELEMENT_DEFAULT_RECT;
    textRect = QRectF(realBoundingRect.left(),realBoundingRect.top(),
                      ARITHUNIT_TEXT_RECT_WIDTH,realBoundingRect.height());

    paintRect = QRectF(textRect.right(),realBoundingRect.top(),
                       realBoundingRect.width() - ARITHUNIT_TEXT_RECT_WIDTH,realBoundingRect.height());

    paintPath.addRect(realBoundingRect);
    collidedItems.clear();
    for(i = 0; i < m_inputNum; ++i)
    {
        connect(m_inputPortVector.at(i),SIGNAL(destroyed(QObject*)),this,SLOT(handlePortDeleted(QObject*)));
        connect(m_inputPortVector.at(i),SIGNAL(addReadMemSuccessed(ModuleReadMem*)),this,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));
    }
    for(i = 0; i < m_outputNum; ++i)
    {
        connect(m_outputPortVector.at(i),SIGNAL(destroyed(QObject*)),this,SLOT(handlePortDeleted(QObject*)));
        connect(m_outputPortVector.at(i),SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),this,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));
    }
}

ElementCustomPE::ElementCustomPE(const ElementCustomPE &copyCustomPE, QGraphicsItem *parent)
    : BaseArithUnit(CUSTOMPE,copyCustomPE.indexBcu,copyCustomPE.indexRcu,copyCustomPE.indexInRcu,parent)
    , funcIndex(Func_Au_Index)
    , funcExp(QString())
    , funcAuModIndex(0)
    , funcMuModIndex(0)
    , bypassIndex(0)
    , bypassExp(QString())
{
    unitId = CUSTOMPE_ID;
    funcIndex = copyCustomPE.funcIndex;
    m_inputNum = copyCustomPE.m_inputNum;
    m_outputNum = copyCustomPE.m_outputNum;
    funcExp = copyCustomPE.funcExp;
    funcAuModIndex = copyCustomPE.funcAuModIndex;
    bypassIndex = copyCustomPE.bypassIndex;
    bypassExp = copyCustomPE.bypassExp;

    auto i = 0;
    for(i = 0; i < m_inputNum; ++i)
    {
        m_inputPortVector.emplace_back(new ElementPort(InputDirection, this, PORT_FROM_CUSTOMPE_IN));
    }
    for(i = 0; i < m_outputNum; ++i)
    {
        m_outputPortVector.emplace_back(new ElementPort(OutputDirection, this, PORT_FROM_CUSTOMPE_OUT));
    }

    for(i = 0; i < m_inputNum; ++i)
    {
        m_inputPortVector.at(i)->copyOtherPort(*copyCustomPE.m_inputPortVector.at(i));
    }
    for(i = 0; i < m_outputNum; ++i)
    {
        m_outputPortVector.at(i)->copyOtherPort(*copyCustomPE.m_outputPortVector.at(i));
    }

    this->setPos(copyCustomPE.pos());
}

ElementCustomPE::~ElementCustomPE()
{
    for (int i = 0; i < m_inputPortVector.size(); ++i) {
        CHECK_AND_DELETE(m_inputPortVector.at(i));
    }
    for (int i = 0; i < m_outputPortVector.size(); ++i) {
        CHECK_AND_DELETE(m_outputPortVector.at(i));
    }
    QL_DEBUG<<"~ElementCustomPE()";
}

void ElementCustomPE::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setRenderHints(QPainter::Antialiasing|
                           QPainter::TextAntialiasing);

    painter->setBrush(ARITH_BRUSH_FILLIN);
    painter->setPen(ARITH_BOUNDING_PEN);
    painter->fillRect(paintRect,ARITH_BRUSH_FILLIN);
    painter->drawRect(paintRect);

    qreal posX = paintRect.left();
    qreal posY = paintRect.top() + DEFAULT_PEN_WIDTH;
    qreal inPortSpace = paintRect.width();

    auto i = 0;
    for(i = 0; i < m_inputNum; ++i)
    {
        inPortSpace = inPortSpace - m_inputPortVector.at(i)->getPaintRect().width();
    }
    inPortSpace = inPortSpace/(m_inputNum + 1);

    qreal outPortSpace = paintRect.width();
    for(i = 0; i < m_outputNum; ++i)
    {
        outPortSpace = outPortSpace - m_outputPortVector.at(i)->getPaintRect().width();
    }
    outPortSpace = outPortSpace/(m_outputNum + 1);

    posX += inPortSpace;
    for(i = 0; i < m_inputNum; ++i)
    {
        m_inputPortVector.at(i)->setPos(posX,posY);
        posX += m_inputPortVector.at(i)->getPaintRect().width() + inPortSpace;
    }

    posX = paintRect.left();
    posY = paintRect.bottom() - m_outputPortVector.at(0)->getPaintRect().height() - DEFAULT_PEN_WIDTH;
    posX += outPortSpace;
    for(i = 0; i < m_outputNum; ++i)
    {
        m_outputPortVector.at(i)->setPos(posX,posY);
        posX += m_outputPortVector.at(i)->getPaintRect().width() + outPortSpace;
    }

    if(isSelected())
    {
        painter->setBrush(ARITH_TEXT_SELECTED_BRUSH_FILLIN);
        painter->setPen(ARITH_BOUNDING_PEN);
        painter->fillRect(textRect,ARITH_TEXT_SELECTED_BRUSH_FILLIN);
        painter->drawRect(textRect);
    }
    else
    {
        painter->setBrush(ARITH_TEXT_NORMAL_BRUSH_FILLIN);
        painter->setPen(ARITH_BOUNDING_PEN);
        painter->fillRect(textRect,ARITH_TEXT_NORMAL_BRUSH_FILLIN);
        painter->drawRect(textRect);
    }

    if(checkCollidedItems(Qt::IntersectsItemBoundingRect))
    {
        painter->setBrush(ARITH_COLLIDED_BRUSH_FILLIN);
        painter->setPen(ARITH_COLLIDED_BOUNDING_PEN);
        painter->fillRect(realBoundingRect,ARITH_COLLIDED_BRUSH_FILLIN);
        painter->drawRect(realBoundingRect);
    }

    QFont font;
    font.setPixelSize(ARITH_TEXT_PIXELSIZE);
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(ARITH_TEXT_PEN);

    painter->rotate(90);
    painter->drawText(textRect.top() + 2,
                      (- (textRect.width() - font.pixelSize())/2 - textRect.left()),
                      arithUnitName);
}

QPainterPath ElementCustomPE::shape() const
{
    return paintPath;
}

void ElementCustomPE::changeInputPort(int portNum)
{
    int iSize = m_inputPortVector.size();
    if(portNum > iSize)
    {
        for(int i = iSize; i < portNum; ++i)
        {
            m_inputPortVector.emplace_back(new ElementPort(InputDirection, this, PORT_FROM_CUSTOMPE_IN));
        }
    }
    else if(portNum < iSize)
    {
        std::vector<ElementPort*>::iterator result = std::find(
                    m_inputPortVector.begin(),m_inputPortVector.end(),m_inputPortVector.at(portNum));
        if(result != m_inputPortVector.end())
        {
            for(int i = portNum; i < iSize; ++i)
            {
                delete m_inputPortVector.at(i);
                m_inputPortVector.at(i) = NULL;
            }
            m_inputPortVector.erase(result,m_inputPortVector.end());
        }
    }
}

void ElementCustomPE::changeOutputPort(int portNum)
{
    int iSize = m_outputPortVector.size();
    if(portNum > iSize)
    {
        for(int i = iSize; i < portNum; ++i)
        {
            m_outputPortVector.emplace_back(new ElementPort(OutputDirection, this, PORT_FROM_CUSTOMPE_OUT));
        }
    }
    else if(portNum < iSize)
    {
        std::vector<ElementPort*>::iterator result = std::find(
                    m_outputPortVector.begin(),m_outputPortVector.end(),m_outputPortVector.at(portNum));
        if(result != m_outputPortVector.end())
        {
            for(int i = portNum; i < iSize; ++i)
            {
                delete m_outputPortVector.at(i);
                m_outputPortVector.at(i) = NULL;
            }
            m_outputPortVector.erase(result,m_outputPortVector.end());
        }
    }
}

void ElementCustomPE::handlePortDeleted(QObject *delPort)
{
    for (int i = 0; i < inputNum(); ++i) {
        if(m_inputPortVector.at(i) == delPort)
            m_inputPortVector.at(i) = NULL;
    }

    for (int i = 0; i < outputNum(); ++i) {
        if(m_outputPortVector.at(i) == delPort)
            m_outputPortVector.at(i) = NULL;
    }
}

int ElementCustomPE::outputNum() const
{
    return m_outputNum;
}

void ElementCustomPE::setOutputNum(int outputNum)
{
    m_outputNum = outputNum;
}

int ElementCustomPE::inputNum() const
{
    return m_inputNum;
}

void ElementCustomPE::setInputNum(int inputNum)
{
    m_inputNum = inputNum;
}

void ElementCustomPE::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
//    QL_DEBUG<<objectName();
    BaseItem::mousePressEvent(event);
}

void ElementCustomPE::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    BaseItem::mouseReleaseEvent(event);
}

