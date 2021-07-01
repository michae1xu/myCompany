#qtHaveModule(printsupport): QT += printsupport
QT += svg

include(./qtpropertybrowser/qtpropertybrowser.pri)

HEADERS += \
    $$PWD/CPropertyBrowser.h \
    $$PWD/DebugModifyData.h \
    $$PWD/ModuleReadImd.h \
    $$PWD/ModuleReadLfsr.h \
    $$PWD/ModuleReadRch.h \
    $$PWD/ModuleWriteLfsr.h \
    $$PWD/ModuleWriteRch.h \
    $$PWD/RcaGraphEdit.h \
    $$PWD/RcaGraphView.h \
    $$PWD/RcaGraphScene.h \
    $$PWD/BasePort.h \
    $$PWD/ElementPort.h \
    $$PWD/BaseItem.h \
    $$PWD/BaseArithUnit.h \
    $$PWD/ElementBfu.h \
    $$PWD/RcaGraphDef.h \
    $$PWD/ModuleRcu.h \
    $$PWD/ModuleBcu.h \
    $$PWD/ElementSbox.h \
    $$PWD/ElementBenes.h \
    $$PWD/ElementWire.h \
    $$PWD/RcaScaleView.h \
    $$PWD/ModuleFifo.h \
    $$PWD/ModuleReadMem.h \
    $$PWD/ModuleWriteMem.h \
#    $$PWD/RcaItemSetting/RcaBcuSetting.h \
#    $$PWD/RcaItemSetting/RcaRcuSetting.h \
#    $$PWD/RcaItemSetting/RcaBfuSetting.h \
#    $$PWD/RcaItemSetting/RcaSboxSetting.h \
#    $$PWD/RcaItemSetting/RcaBenesSetting.h \
#    $$PWD/RcaItemSetting/RcaRWMemSetting.h \
#    $$PWD/RcaItemSetting/RcaSetting.h \
    $$PWD/ItemUndoCommands.h \
    $$PWD/RcaGridItem.h \
    $$PWD/ItemCopyPaste.h \
    $$PWD/citemattributesetting.h \
    $$PWD/elementcustompe.h \
    $$PWD/resourceselectdialog.h

SOURCES += \
    $$PWD/CPropertyBrowser.cpp \
    $$PWD/DebugModifyData.cpp \
    $$PWD/ModuleReadImd.cpp \
    $$PWD/ModuleReadLfsr.cpp \
    $$PWD/ModuleReadRch.cpp \
    $$PWD/ModuleWriteLfsr.cpp \
    $$PWD/ModuleWriteRch.cpp \
    $$PWD/RcaGraphEdit.cpp \
    $$PWD/RcaGraphView.cpp \
    $$PWD/RcaGraphScene.cpp \
    $$PWD/BasePort.cpp \
    $$PWD/ElementPort.cpp \
    $$PWD/BaseItem.cpp \
    $$PWD/BaseArithUnit.cpp \
    $$PWD/ElementBfu.cpp \
    $$PWD/ModuleRcu.cpp \
    $$PWD/ModuleBcu.cpp \
    $$PWD/ElementSbox.cpp \
    $$PWD/ElementBenes.cpp \
    $$PWD/ElementWire.cpp \
    $$PWD/RcaScaleView.cpp \
    $$PWD/ModuleFifo.cpp \
    $$PWD/ModuleReadMem.cpp \
    $$PWD/ModuleWriteMem.cpp \
#    $$PWD/RcaItemSetting/RcaBcuSetting.cpp \
#    $$PWD/RcaItemSetting/RcaRcuSetting.cpp \
#    $$PWD/RcaItemSetting/RcaBfuSetting.cpp \
#    $$PWD/RcaItemSetting/RcaSboxSetting.cpp \
#    $$PWD/RcaItemSetting/RcaBenesSetting.cpp \
#    $$PWD/RcaItemSetting/RcaRWMemSetting.cpp \
#    $$PWD/RcaItemSetting/RcaSetting.cpp \
    $$PWD/ItemUndoCommands.cpp \
    $$PWD/RcaGridItem.cpp \
    $$PWD/ItemCopyPaste.cpp \
    $$PWD/citemattributesetting.cpp \
    $$PWD/elementcustompe.cpp \
    $$PWD/resourceselectdialog.cpp

FORMS += \
#    $$PWD/RcaItemSetting/RcaBcuSetting.ui \
#    $$PWD/RcaItemSetting/RcaRcuSetting.ui \
#    $$PWD/RcaItemSetting/RcaBfuSetting.ui \
#    $$PWD/RcaItemSetting/RcaSboxSetting.ui \
#    $$PWD/RcaItemSetting/RcaBenesSetting.ui \
#    $$PWD/RcaItemSetting/RcaRWMemSetting.ui
    $$PWD/CPropertyBrowser.ui \
    $$PWD/DebugModifyData.ui \
    $$PWD/citemattributesetting.ui \
    $$PWD/resourceselectdialog.ui
