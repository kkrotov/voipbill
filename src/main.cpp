#include "App.h"

#include "tasks/TaskBlacklist.h"
#include "tasks/TaskBillRuntime.h"
#include "tasks/TaskLimitControl.h"
#include "tasks/TaskLoader.h"
#include "tasks/TaskSync.h"
#include "tasks/TaskSaveCounters.h"
#include "tasks/TaskCheckStartTable.h"
#include "tasks/TaskAsteriskInfo.h"
#include "tasks/TaskCurrentCalls.h"
#include "tasks/TaskYotaQoS.h"
#include "tasks/TaskWeb.h"

#include <boost/thread.hpp>
#include <boost/interprocess/sync/posix/mutex.hpp>

void SetPidFile()
{
    FILE* f = fopen(app.conf.pid_file.c_str(), "w+");
    if (f)
    {
        fprintf(f, "%u", getpid());
        fclose(f);
    }
}


App app;

int main_test(int argc, char* argv[]);

#include <execinfo.h>
#include <inttypes.h>

static void full_write(int fd, const char *buf, size_t len)
{
    while (len > 0)
    {
        ssize_t ret = write(fd, buf, len);

        if ((ret == -1) && (errno != EINTR))
            break;

        buf += (size_t) ret;
        len -= (size_t) ret;
    }
}


void posix_death_signal(int signum)
{
    static const char start[] = "BACKTRACE ------------\n";
    static const char end[] = "----------------------\n";

    void *bt[1024];
    int bt_size;
    char **bt_syms;
    int i;

    bt_size = backtrace(bt, 1024);
    bt_syms = backtrace_symbols(bt, bt_size);
    full_write(STDERR_FILENO, start, strlen(start));
    for (i = 1; i < bt_size; i++)
    {
        size_t len = strlen(bt_syms[i]);
        full_write(STDERR_FILENO, bt_syms[i], len);
        full_write(STDERR_FILENO, "\n", 1);
    }
    full_write(STDERR_FILENO, end, strlen(end));
    free(bt_syms);

    exit(3);
}

#include "BlackList.h"

int main(int argc, char* argv[]) {

    signal(SIGSEGV, posix_death_signal);

    srand(time(0));

    if (!app.init(argc, argv))
        return 1;

    if (app.conf.test_mode)
        return main_test(argc, argv);


    SetPidFile();


    TaskWeb web;

    boost::thread web_thread(web);


    TaskSync * task_sync = new TaskSync();
    TaskLoader * task_loader = new TaskLoader();
    TaskSaveCounters * task_savecounters = new TaskSaveCounters();
    TaskBlacklist * task_blacklist = new TaskBlacklist();
    TaskLimitControl * task_limitcontrol = new TaskLimitControl();
    TaskBillRuntime * task_billruntime = new TaskBillRuntime();
    TaskCheckStartTable * task_checkstarttable = new TaskCheckStartTable();
    TaskAsteriskInfo * task_asteriskinfo = new TaskAsteriskInfo();
    TaskCurrentCalls * task_currentcalls = new TaskCurrentCalls();
    TaskYotaQoS * task_yotaqos = new TaskYotaQoS();


    task_sync->start();
    task_loader->start();
    task_billruntime->start();
    task_limitcontrol->start();
    task_blacklist->start();
    task_savecounters->start();
    task_asteriskinfo->start();
    task_currentcalls->start();
    task_checkstarttable->start();
    task_yotaqos->start();


    while(true)
    {
        sleep(3600);
    }
    return 0;

}

