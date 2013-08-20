/*
#include "ThreadYotaQoS.h"

#include "ThreadCurrentCalls.h"
#include "ThreadAsteriskInfo.h"

#include <execinfo.h>
#include <inttypes.h>

#include <boost/regex.hpp>

ThreadYotaQoS::ThreadYotaQoS() {
    id = "yotaqos";
    name = "Yota QoS";
}

bool apply_qos(string srv_ip, string usr_ip, string &session_id, string &error)
{
    string data =  "{ \"endUserId\": \"$ENDUSER_ID\", " \
                    "  \"qosFeatureId\": \"RtpAudio\", " \
                    "  \"qosFeatureProps\": { " \
                    "     \"duration\": $DURATION, " \
                    "     \"maxUBw\": 120, " \
                    "     \"mediaProps\": [ { " \
                    "           \"srvIp\": \"$SRV_IP\", " \
                    "           \"usrIp\": \"$USR_IP\" } ], " \
                    "     \"maxDBw\": 120 } }";
    replace_all(data, "\"", "\\\"");
    replace_all(data, "$ENDUSER_ID", "89.235.180.0");
    replace_all(data, "$SRV_IP", srv_ip);
    replace_all(data, "$USR_IP", usr_ip);
    replace_all(data, "$DURATION", lexical_cast<string>(3600));


    string program = "curl";
    program += " -v";
    program += " https://api.yotatop.ru/rest/1/qos/sessions";
    program += " --data";
    program += " \""+data+"\"";
    program += " --header";
    program += " \"Content-Type: application/json\"";
    program += " --header";
    program += " \"Authorization: Basic OWJmOWNkMzU3ZTA3NThiMDE0NDc1NjA1NjdhMTMxN2E6YzA1ODVhYWQ2NzFjZjM0Yw==\"";
    program += " 2>&1";

    FILE *file;
    if(!(file = popen(program.c_str(), "r")))
    {
        error = "exec curl";
        return false;
    }

    char buff[4000];
    size_t res = fread(buff, 1, sizeof(buff)-1, file);
    buff[res] = 0;

    if (pclose(file) != 0)
    {
        error = "bad curl result";
        return false;
    }

    string str(buff);
    char sep[3];
    sep[0] = 10;
    sep[1] = 10;
    sep[2] = 0;
    size_t body_pos = str.find(sep);
    string str_body;
    if (body_pos != str.npos)
        str_body = str.substr(body_pos+2);

    if (str.find(" 201 Created") == string::npos)
    {
        error = str_body;
        return false;
    }

    boost::regex exp(".*\"sfId\":\"([\\w\\d\\.-]+)\".*");
    boost::smatch mres;

    if (boost::regex_match(str_body, mres, exp) == false)
    {
        error = "session_id not matched";
        return false;
    }

    session_id = mres[1];
    return true;
}

bool remove_qos(string session_id, string &error)
{
    string program = "curl";
    program += " -v";
    program += " -X";
    program += " DELETE";
    program += " \"https://api.yotatop.ru/rest/1/qos/sessions/"+session_id+"\"";
    program += " --header";
    program += " \"Authorization: Basic OWJmOWNkMzU3ZTA3NThiMDE0NDc1NjA1NjdhMTMxN2E6YzA1ODVhYWQ2NzFjZjM0Yw==\"";
    program += " 2>&1";

    FILE *file;
    if(!(file = popen(program.c_str(), "r")))
    {
        error = "exec curl";
        return false;
    }

    char buff[4000];
    size_t res = fread(buff, 1, sizeof(buff)-1, file);
    buff[res] = 0;

    if (pclose(file) != 0)
    {
        error = "bad curl result";
        return false;
    }

    string str(buff);
    if (str.find(" 204 No Content") == string::npos)
    {
        error = "bad result code";
        return false;
    }

    return true;
}


void ThreadYotaQoS::run()
{
    if (app.conf.db_ast == "") return;

    deleted_sessions_count = 0;

    while(true){
        ssleep(1);

        t.start();
        try{

            shared_ptr<CurrentCallsObjList> calls = ThreadCurrentCalls::getList();
            shared_ptr<AsteriskNumberObjList> ast_list = ThreadAsteriskInfo::getList();


            sessions_count = sessions.size();

            map<long long int, pAsteriskNumberObj> new_sessions;
            map<long long int, string> old_sessions;

            {
                // if have new calls
                for(int i=0; i<calls->count;i++){
                    pCallObj call = calls->get(i);
                    pAsteriskNumberObj ast = ast_list->find(atoll(call->usage));

                    if (ast == 0) continue;


                    if (sessions.find(call->id_num) != sessions.end()) continue;

                    new_sessions[call->id_num] = ast;
                }
                new_sessions_count = new_sessions.size();
            }


            {
                // if have removed calls
                map<long long int, string>::iterator i = sessions.begin();
                while (i != sessions.end()) {
                    if (calls->find(i->first) == 0)
                    {
                        old_sessions[i->first] = i->second;
                        sessions.erase(i++);
                    }else
                        ++i;
                }
                old_sessions_count = new_sessions.size();
            }


            t_api.start();


            {
                map<long long int, pAsteriskNumberObj>::iterator i = new_sessions.begin();
                while (i != new_sessions.end()) {
                    pAsteriskNumberObj v = i->second;
                    string session_id;
                    string error;

                    if (apply_qos(v->srv_ip, v->usr_ip, session_id, error))
                    {
                        Log::info("Add QoS: " + session_id + " " + string(v->usr_ip) + " " + string(v->srv_ip) + " " + lexical_cast<string>(v->usr_num));
                        sessions[i->first] = session_id;
                        new_sessions.erase(i++);
                    }else{
                        Log::error("Not Add QoS:" + string(v->usr_ip) + " / " + error);
                        ++i;
                    }
                }
            }

            {
                map<long long int, string>::iterator i = old_sessions.begin();
                while (i != old_sessions.end()) {
                    string session_id = i->second;
                    string error;
                    if (remove_qos(session_id, error))
                    {
                        Log::info("Rem QoS: " + session_id);
                    }else{
                        Log::error("Not Rem QoS: " + session_id + " / " + error);
                    }

                    if (true)
                        old_sessions.erase(i++);
                    else
                        ++i;

                    deleted_sessions_count++;
                }
            }


            t_api.stop();


        }catch( Exception &e ){
            e.addTrace("ThreadYotaQoS::run");
            Log::exception(e);
        }
        t.stop();
    }
}

void ThreadYotaQoS::htmlfull(stringstream &html){
    this->html(html);

    shared_ptr<CurrentCallsObjList> l1 = ThreadCurrentCalls::getList();
    shared_ptr<AsteriskNumberObjList> l2 = ThreadAsteriskInfo::getList();

    html << "Time api: <b>" << t_api.sloop() << "</b><br/>\n";
    html << "Time loop: <b>" << t.sloop() << "</b><br/>\n";
    html << "Time full api: <b>" << t_api.sfull() << "</b><br/>\n";
    html << "Time full loop: <b>" << t.sfull() << "</b><br/>\n";
    html << "loops: <b>" << t.count << "</b><br/>\n";
    html << "<br/>\n";
    html << "All calls: <b>" << l1->count << "</b><br/>\n";
    html << "Asterisk Yota numbers: <b>" << l2->count << "</b><br/>\n";
    html << "<br/>\n";
    html << "Sessions: <b>" << sessions_count << "</b><br/>\n";
    html << "Establishing sessions: <b>" << new_sessions_count << "</b><br/>\n";
    html << "Terminationg sessions: <b>" << old_sessions_count << "</b><br/>\n";
    html << "Deleted sessions: <b>" << deleted_sessions_count << "</b><br/>\n";
}
 */
