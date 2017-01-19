#pragma once

#include "BasePage.h"

class PageTask : public BasePage {
public:
    bool canHandle(std::string &path) {
        return path == "/task";
    }
    void render(std::stringstream &html, map<string, string> &parameters) {
        string task_id;
        if (parameters.find("id") != parameters.end())
            task_id = parameters["id"];
        Thread * thread = 0;

        app().threads.forAllThreads([&](Thread* th) {
            if (th->id == task_id) {
                thread = th;
                return false;
            }
            return true;
        });

        if (thread == 0) {
            return; // false
        }

        renderHeader("task",html);

        thread->htmlfull(html);

        return; // true
    }
};