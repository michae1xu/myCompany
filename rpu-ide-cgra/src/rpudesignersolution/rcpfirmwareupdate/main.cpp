#include "cfirmwareupdatedialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CFirmwareUpdateDialog dlg;
    dlg.show();
    return a.exec();
}
