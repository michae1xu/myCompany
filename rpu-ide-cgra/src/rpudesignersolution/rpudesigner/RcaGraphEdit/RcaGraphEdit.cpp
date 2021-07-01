#include "RcaGraphEdit.h"
#include "RcaGraphDef.h"

#include <QDebug>

RcaGraphEdit::RcaGraphEdit() :
      rcaView(new RcaGraphView(nullptr))
{

}

RcaGraphEdit::~RcaGraphEdit()
{
    CHECK_AND_DELETE(rcaView);
}

void RcaGraphEdit::showRcaView()
{
    if(!rcaView) return;

    rcaView->show();
}

void RcaGraphEdit::hideRcaView()
{
    if(!rcaView) return;

    rcaView->hide();
}


RcaGraphView* RcaGraphEdit::getRcaView()
{
    return rcaView;
}



