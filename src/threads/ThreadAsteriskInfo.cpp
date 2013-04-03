/*
#include "ThreadAsteriskInfo.h"

shared_ptr<AsteriskNumberObjList> ThreadAsteriskInfo::list(new AsteriskNumberObjList());
boost::detail::spinlock ThreadAsteriskInfo::lock;

shared_ptr<AsteriskNumberObjList> ThreadAsteriskInfo::getList()
{
    lock.lock();
    shared_ptr<AsteriskNumberObjList> plist = list;
    lock.unlock();
    return plist;
}

void ThreadAsteriskInfo::run()
{
    if (app.conf.db_ast == "") return;

    BDb db_ast(app.conf.db_ast);

    while(true){

        this->t.start();

        shared_ptr<AsteriskNumberObjList> l(new AsteriskNumberObjList());

        try{

            l->load(&db_ast, 0);

            lock.lock();
            ThreadAsteriskInfo::list = l;
            lock.unlock();

        }catch( DbException &e ){
            Log::er(e.what());
        }

        this->t.stop();

        ssleep(30);
    }
}

void ThreadAsteriskInfo::htmlfull(stringstream &html){
    this->html(html);

    html << "Time loop: <b>" << this->t.sloop() << "</b><br/>\n";
    html << "Time full loop: <b>" << this->t.sfull() << "</b><br/>\n";
    html << "loops: <b>" << t.count << "</b><br/>\n";
    html << "<br/>\n";

    html << "Count yota numbers: <b>" << ThreadAsteriskInfo::list->count << "</b><br/>\n";
}

ThreadAsteriskInfo::ThreadAsteriskInfo() {
    id = "asterisk";
    name = "Asterisk Info";
}
*/
