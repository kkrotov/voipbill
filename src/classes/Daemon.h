#pragma once

#include <string>
using namespace std;

class Daemon {
public:
    static void setPidFile(string pid_file);
    static void initSignalHandler();
protected:
    static void full_write(int fd, const char *buf, size_t len);
    static void posix_death_signal(int signum);
    static void termination_handler(int signum);
};
