#include "mysql.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mysql w;
    w.show();
    return a.exec();
}
