#ifndef CPROPERTYBROWSER_H
#define CPROPERTYBROWSER_H

#include <QWidget>
#include "BaseItem.h"

#include <qtpropertybrowser.h>
#include <qtpropertymanager.h>
#include <qtvariantproperty.h>
#include <qteditorfactory.h>
class RcaGraphScene;

namespace Ui {
class CPropertyBrowser;
}

class CPropertyBrowser : public QWidget
{
    Q_OBJECT
    enum PropertyId {
        PropertyNone,
        PropertyBcu,
        PropertyRcu,
        PropertyBfu,
        PropertyCustomPE,
        PropertySbox,
        PropertyBenes,
        PropertyRImd,
        PropertyRMem,
        PropertyRRch,
        PropertyWMem,
        PropertyWRch,
        PropertyRLfsr,
        PropertyWLfsr};
public:
    explicit CPropertyBrowser(RcaGraphScene* scene, QWidget *parent = nullptr);
    ~CPropertyBrowser();

    void updateItemProperty(BaseItem *baseItem);
    void setEnable(bool enable);

private:
    Ui::CPropertyBrowser *ui;
    RcaGraphScene* m_curRcaScene = nullptr;
    BaseItem* m_curBaseItem = nullptr;
    QList<QString> m_resourceKeys;

    QtVariantPropertyManager *m_pVarManager;
    QtVariantEditorFactory *m_pVarFactory;
    QMap<QtProperty*,QString> m_property_dic;

    bool m_enable = true;

    void ItemPropertyView(PropertyId propertyId);

public slots:
    void onValueChanged(QtProperty *property, const QVariant &value);

private slots:
    void reloadCurItemAttr();
    bool validLoopChange(RcaGraphScene* m_curRcaScene,int iRcuIndex,int iNowPosFlag,int iChangeFlag,bool bLoop2=false);
};

#endif // CPROPERTYBROWSER_H
