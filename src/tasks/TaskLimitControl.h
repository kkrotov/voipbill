#pragma once

#include "Task.h"
#include "../CalcFull.h"
#include "../CallsSaver.h"
#include "../KillCalls.h"
#include "../lists/CurrentCallsObjList.h"
#include "TaskCurrentCalls.h"


class TaskLimitControl : public Task
{
protected:
    Timer t;
    Timer t_calc;
    Timer t_kill;
    CalcFull calculator;

public:

    void wait()
    {
        while(app.init_sync_done == false ||
              app.init_load_data_done == false ||
              app.init_load_counters_done == false ||
              app.init_bill_runtime_started == false)
        {
            ssleep(1);
        }
    }

    void run()
    {
        BDb db_calls(app.conf.db_calls);
        calculator.setDb(&db_calls);

        while(true){
            ssleep(1);

            shared_ptr<CurrentCallsObjList> splist = TaskCurrentCalls::getList();
            CurrentCallsObjList * list = splist.get();

            t.start();
            try{

                if (list->loadtime + 60 >= time(NULL))
                {
                    t_calc.start();

                    calculator.calc_limit(list);

                    t_calc.stop();

                    t_kill.start();

                    KillCalls::kill(list);

                    t_kill.stop();

                }


            }catch( DbException &e ){
                Log::er(e.what());
            }
            t.stop();
        }
    }

    void htmlfull(stringstream &html){
        this->html(html);

        html << "Time calc: <b>" << t_calc.sloop() + "</b><br/>\n";
        html << "Time kill: <b>" << t_kill.sloop() + "</b><br/>\n";
        html << "Time loop: <b>" << t.sloop() + "</b><br/>\n";
        html << "Time full calc: <b>" << t_calc.sfull() + "</b><br/>\n";
        html << "Time full kill: <b>" << t_kill.sfull() + "</b><br/>\n";
        html << "Time full loop: <b>" << t.sfull() + "</b><br/>\n";
        html << "loops: <b>" << t.count << "</b><br/>\n";
        html << "<br/>\n";
        calculator.html(html);
    }
public:
    TaskLimitControl() {
        id = "limitcontrol";
        name = "Limit control";
    }
};

