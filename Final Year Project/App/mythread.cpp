#include "mythread.h"
#include <QtCore>
#include <iostream>

MyThread::MyThread(const char * cmd) {
    int len = strlen(cmd);
    command = new char[len+1];
    int i;
    for (i=0; i<len; i++) {
        command[i] = cmd[i];
    }
    command[i]= '\0';
}

void MyThread::run() {
    system(command);
}

MyThread::~MyThread() {
    delete[] this->command;
}
