#pragma once

#include "Task.h"
#include "../BDb.h"
#include "../UdpControlClient.h"

class TaskCheckStartTable : public Task
{
    Timer t;

    void run()
    {
        if (UdpControlClient::ready() == false) return;

        BDb db_rad(app.conf.db_rad);

        time_t last_t = 0;
        while(true){

            ssleep(10);

            t.start();

            try{

                map<string, string> slist;
                time_t curr_t = time(NULL);
                BDbResult res = db_rad.query("select acctsessionid, acctuniqueid from radacct_voip_start");
                while(res.next()){
                    slist[res.get_s(0)] = res.get_s(1);
                }

                if (slist.size() == 0) { last_t = curr_t; continue; }

                ssleep(1);

                vector<string> rlist;
                if (UdpControlClient::select(rlist) == false) continue;

                map<string,bool> rlist_map;
                for (vector<string>::iterator it = rlist.begin(); it != rlist.end(); ++it){
                    rlist_map[*it] = true;
                }

                for (map<string, string>::iterator it = slist.begin(); it != slist.end(); ++it){

                    if (rlist_map.find(it->first) != rlist_map.end()) continue;

                    bool deleted = false;
                    BDbResult res =
                            db_rad.query(
                                "select force_finish_call('"+it->first+"','"+lexical_cast<string>(curr_t - last_t)+"')");
                    if (res.next()) deleted = res.get_b(0);

                    if (deleted) Log::er("Force finish call " + it->second);

                }

                last_t = curr_t;


            }catch( DbException &e ){
                Log::er(e.what());
            }

            t.stop();

        }
    }


    void htmlfull(stringstream &html){
        this->html(html);

        html << "Time loop: <b>" << t.sloop() << "</b><br/>\n";
        html << "Time full loop: <b>" << t.sfull() << "</b><br/>\n";
        html << "loops: <b>" << t.count << "</b><br/>\n";
        html << "<br/>\n";
    }

public:
    TaskCheckStartTable() {
        id = "checkstarttable";
        name = "Check Start Table";
    }
};

