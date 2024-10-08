#include <iostream>
#include <qapplication.h>
#include "App.h"

int main(int argc, char *argv[]) {

    QApplication a(argc, argv);

    App app;

    app.show();

    a.exec();

}
