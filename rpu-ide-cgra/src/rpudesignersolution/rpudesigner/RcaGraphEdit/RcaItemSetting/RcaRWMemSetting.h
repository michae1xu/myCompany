#ifndef RCARWMEMSETTING_H
#define RCARWMEMSETTING_H

#include <QDialog>
#include <QAbstractButton>
#include <QComboBox>

#include "RcaGraphScene.h"

namespace Ui {
class RcaRWMemSetting;
}

class RcaRWMemSetting : public QDialog
{
    Q_OBJECT

public:
    explicit RcaRWMemSetting(QWidget *parent = 0);
    ~RcaRWMemSetting();
    static RcaRWMemSetting* instance(QWidget *parent = 0) {
        if ( !rwMemSetingInstance ) {
            rwMemSetingInstance = new RcaRWMemSetting(parent);
        }
        return rwMemSetingInstance;
    }
//    void setRcaScene(RcaGraphScene* scene);
    void showRWMem(BasePort* mem,QPoint pos);
    BaseItem* getItem() { return rwMem;}
private slots:
    void handleRWMemBeDeleted(BaseItem* deletedItem);

    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::RcaRWMemSetting *ui;
    static RcaRWMemSetting* rwMemSetingInstance;
//    RcaGraphScene* rcaScene;
    BasePort* rwMem;
};

#endif // RCARWMEMSETTING_H
