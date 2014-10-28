#include "Daemon.h"

#include "App.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

extern App & app();

void Daemon::setPidFile(string pid_file) {
    FILE* f = fopen(pid_file.c_str(), "w+");
    if (f) {
        fprintf(f, "%u", getpid());
        fclose(f);
    }
}

#include <signal.h>
#include <execinfo.h>
#include <inttypes.h>

void Daemon::full_write(int fd, const char *buf, size_t len) {
    while (len > 0) {
        ssize_t ret = write(fd, buf, len);

        if ((ret == -1) && (errno != EINTR))
            break;

        buf += (size_t) ret;
        len -= (size_t) ret;
    }
}

void Daemon::posix_death_signal(int signum) {
    printf("\nDEATH\n");
    
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

void Daemon::termination_handler(int signum) {
    switch (signum) {
        case SIGSEGV:
            posix_death_signal(signum);
            break;
        case SIGTERM:
        case SIGHUP:
        case SIGINT:
        case SIGQUIT:
        case SIGABRT:
            app().setStatus(AppStatus::APP_STOPPED);
    }
}

void set_action(int signum, __sighandler_t handler) {
    struct sigaction new_action = {0};
    new_action.sa_handler = handler;
    sigaction(signum, &new_action, NULL);
}

void Daemon::initSignalHandler() {
    int catch_signals[] = {SIGSEGV, SIGINT, SIGHUP, SIGTERM, SIGQUIT, SIGABRT};
    for (auto signal : catch_signals) {
        set_action(signal, termination_handler);
    }
}
