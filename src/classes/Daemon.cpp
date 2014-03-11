#include "Daemon.h"

#include "App.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

void Daemoin::setPidFile() {
    FILE* f = fopen(app().conf.pid_file.c_str(), "w+");
    if (f) {
        fprintf(f, "%u", getpid());
        fclose(f);
    }
}

#include <signal.h>
#include <execinfo.h>
#include <inttypes.h>

static void full_write(int fd, const char *buf, size_t len) {
    while (len > 0) {
        ssize_t ret = write(fd, buf, len);

        if ((ret == -1) && (errno != EINTR))
            break;

        buf += (size_t) ret;
        len -= (size_t) ret;
    }
}

void posix_death_signal(int signum) {
    static const char start[] = "BACKTRACE ------------\n";
    static const char end[] = "----------------------\n";

    void *bt[1024];
    int bt_size;
    char **bt_syms;
    int i;

    bt_size = backtrace(bt, 1024);
    bt_syms = backtrace_symbols(bt, bt_size);
    full_write(STDERR_FILENO, start, strlen(start));
    for (i = 1; i < bt_size; i++) {
        size_t len = strlen(bt_syms[i]);
        full_write(STDERR_FILENO, bt_syms[i], len);
        full_write(STDERR_FILENO, "\n", 1);
    }
    full_write(STDERR_FILENO, end, strlen(end));
    free(bt_syms);

    exit(3);
}

void Daemoin::initSignalHandler() {
    signal(SIGSEGV, posix_death_signal);
}
