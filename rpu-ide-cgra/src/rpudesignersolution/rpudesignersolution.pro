TEMPLATE = subdirs

SUBDIRS += \
    rpu_simulator \
    singleapplication \
    hardwaredebugsolution \
    rpudesigner
#    rcpfirmwareupdate

rpudesigner.depends = rpu_simulator
rpudesigner.depends = singleapplication
#rpudesigner.depends = hardwaredebugsolution
