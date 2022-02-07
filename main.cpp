#include <QTextCodec>
#include "exploitform.h"
#include <QApplication>
int main(int argc, char *argv[])
{    
    QApplication a(argc, argv);
    QCoreApplication::addLibraryPath("./");
    ExploitForm ef;
    ef.Initialize();
    ef.show();

    return a.exec();
}
