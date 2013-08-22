#include "ThreadLog.h"

void ThreadLog::run() {


    while (true) {

        ssleep(1);
    }
}

void ThreadLog::htmlfull(stringstream &html) {

    this->html(html);

}

ThreadLog::ThreadLog() {
    id = "log";
    name = "Log";
}


