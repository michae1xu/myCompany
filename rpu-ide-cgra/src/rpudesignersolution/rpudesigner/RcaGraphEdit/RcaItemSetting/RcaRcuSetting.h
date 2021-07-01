#ifndef RCARCUSETTING_H
#define RCARCUSETTING_H

#include <QDialog>
#include <QAbstractButton>
#include <QComboBox>

#include "RcaGraphScene.h"

namespace Ui {
class RcaRcuSetting;
}

class RcaRcuSetting : public QDialog
{
    Q_OBJECT

public:
    explicit RcaRcuSetting(QWidget *parent = 0);
    ~RcaRcuSetting();
    static RcaRcuSetting* instance(QWidget *parent = 0) {
        if ( !rcuSetingInstance ) {
            rcuSetingInstance = new RcaRcuSetting(parent);
        }
        return rcuSetingInstance;
    }
//    void setRcaScene(RcaGraphScene* scene);
    void showRcu(ModuleRcu* rcu,QPoint pos);
    BaseItem* getItem() { return moduleRcu;}

signals:
    void rcuIndexChanged();

private slots:
    void handleRcuBeDeleted(BaseItem* deletedItem);

    void on_buttonBox_clicked(QAbstractButton *button);

    void on_comboBox_read_mode_currentIndexChanged(int index);

    void on_comboBox_write_mode_currentIndexChanged(int index);

private:
    Ui::RcaRcuSetting *ui;
    static RcaRcuSetting* rcuSetingInstance;
//    RcaGraphScene* rcaScene;
    ModuleRcu* moduleRcu;
};

#endif // RCARCUSETTING_H
