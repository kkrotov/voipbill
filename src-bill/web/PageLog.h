#pragma once

#include "BasePage.h"
#include "../classes/AppBill.h"

using namespace std;

class PageLog : public BasePage {
public:
    bool canHandle(std::string &path) {
        return path == "/log";
    }
    void render(std::stringstream &html, map<string, string> &parameters) {
        renderHeader("log",html);


        list<pLogMessage> history = app().logger.getHistory();

        html << "Queue length: <b>" << app().logger.getQueueLength() << "</b><br/>\n";
        html << "History length: <b>" << history.size() << "</b><br/>\n";
        html << "<br/>\n";

        html << "<table width=100% border=0 cellspacing=5 cellpadding=5>\n";
        for (auto it = history.begin(); it != history.end(); ++it) {
            
            pLogMessage message = *it;
            html << "<tr>\n";
            html << "<td style='text-align: left;vertical-align:top'>" << message->countInGroup << "</td>\n";
            html << "<td nowrap style='text-align: left;vertical-align:top'>" << string_time(message->timeInGroup) << "</td>\n";
            html << "<td style='word-break:break-all; text-align: left'>" << message->message << "</td>\n";
            html << "</tr>\n";
        }
        html << "</table>\n";
    }
};