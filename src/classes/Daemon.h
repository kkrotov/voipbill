#pragma once

#include <string>
using namespace std;

class Daemoin {
public:
    static void setPidFile(string pid_file);
    static void initSignalHandler();
};
