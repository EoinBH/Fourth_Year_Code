#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <QtCore>

class MyThread : public QThread {
    public:
        MyThread(const char* cmd);
        void run();
        char* command;
        ~MyThread();
};

#endif // MYTHREAD_H
