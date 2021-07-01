#ifndef RCASBOXSETTING_H
#define RCASBOXSETTING_H

class QListWidgetItem;

#include <QDialog>
#include <QAbstractButton>
#include <QComboBox>

#include "RcaGraphScene.h"

namespace Ui {
class RcaSboxSetting;
}

class RcaSboxSetting : public QDialog
{
    Q_OBJECT

public:
    explicit RcaSboxSetting(QWidget *parent = 0);
    ~RcaSboxSetting();
    static RcaSboxSetting* instance(QWidget *parent = 0) {
        if ( !sboxSetingInstance ) {
            sboxSetingInstance = new RcaSboxSetting(parent);
        }
        return sboxSetingInstance;
    }
//    void setRcaScene(RcaGraphScene* scene);
    void resetResourceMap(RcaGraphScene* scene);
    void showSbox(ElementSbox* sbox,QPoint pos);
    BaseItem* getItem() { return elementSbox;}
private slots:
    void handleSboxBeDeleted(BaseItem* deletedItem);

    void on_comboBox_input0Type_currentIndexChanged(int index);

    void on_buttonBox_clicked(QAbstractButton *button);

    void on_pushButton_explorer_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    void setComboBox(QComboBox* box,InputPortType type);
    void setComboBoxBfuX(QComboBox* box);
    void setComboBoxBfuY(QComboBox* box);
    void setComboBoxSbox(QComboBox* box);
    void setComboBoxBenes(QComboBox* box);
    void setComboBoxMem(QComboBox* box);
    void setComboBoxFifo(QComboBox* box);
private:
    Ui::RcaSboxSetting *ui;
    static RcaSboxSetting* sboxSetingInstance;
    RcaGraphScene* rcaScene;
    ElementSbox* elementSbox;
    bool isTrue;
};

#endif // RCASBOXSETTING_H
