TEMPLATE = aux

VERSION = 6.2.6
DATETIME = 20180322114308
INSTALLER = ../../release/rpudesigner_win_x86_v$${VERSION}_$${DATETIME}

INPUT = $$PWD/config/config.xml $$PWD/packages
example.input = INPUT
example.output = $$INSTALLER
win32{
example.commands = C:/Qt/QtIFW-3.0.1/bin/binarycreator -c $$PWD/config/config.xml -p $$PWD/packages ${QMAKE_FILE_OUT}
}
unix{
example.commands = /root/Qt/QtIFW-3.0.1/bin/binarycreator -c $$PWD/config/config.xml -p $$PWD/packages ${QMAKE_FILE_OUT}
}
example.CONFIG += target_predeps no_link combine

QMAKE_EXTRA_COMPILERS += example

