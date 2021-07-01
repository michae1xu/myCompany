#ifndef RCAGRAPHEDIT_H
#define RCAGRAPHEDIT_H

#include "RcaGraphView.h"
#include "RcaGraphScene.h"
#include "RcaScaleView.h"

#include <QObject>

class RcaGraphEdit : public QObject
{
    Q_OBJECT
public:
    RcaGraphEdit();
    ~RcaGraphEdit();
    void showRcaView();
    void hideRcaView();
    RcaGraphView* getRcaView();
private slots:
public slots:

private:
    RcaGraphView *rcaView;
};

#endif // RCAGRAPHEDIT_H
