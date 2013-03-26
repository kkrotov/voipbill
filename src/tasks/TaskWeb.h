#pragma once

#include <map>
#include <string>
using namespace std;

class TaskWeb
{
public:

    void operator()();

    static void handlerHeader(stringstream &html);

    static void handlerHome(stringstream &html);

    static void handlerConfig(stringstream &html);

    static bool handlerTask(stringstream &html, map<string,string> &parameters);

    static void handlerCounters(stringstream &html);

    static void handlerClient(stringstream &html, map<string,string> &parameters);

};
