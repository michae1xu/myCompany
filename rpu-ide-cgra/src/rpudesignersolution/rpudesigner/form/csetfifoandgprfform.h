#ifndef CSETFIFOANDGPRFFORM_H
#define CSETFIFOANDGPRFFORM_H

#include <QDialog>
#include <QList>

namespace Ui {
class CSetFifoAndGprfForm;
}

typedef struct _tagMemorySet
{
    bool enable;
    int offset;
    uint *data;
    uint size;
} SMemorySet;

typedef struct _tagFifoSet
{
    bool enable;
    uint *data;
    uint size;
} SFifoSet;

class CSetFifoAndGprfForm : public QDialog
{
    Q_OBJECT

public:
    explicit CSetFifoAndGprfForm(QWidget *parent = 0);
    ~CSetFifoAndGprfForm();

    SMemorySet* getMemorySet();
    SFifoSet* getFifoSet();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::CSetFifoAndGprfForm *ui;

    SMemorySet m_memorySet[5];
    SFifoSet m_fifoSet[4];

    bool readData(uint *&data, uint &size, QString fileName);
};

#endif // CSETFIFOANDGPRFFORM_H
