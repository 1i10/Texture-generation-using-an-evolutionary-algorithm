#include "generationtextureinterface.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GenerationTextureInterface w;
    w.show();
    return a.exec();
}
