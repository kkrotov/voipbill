#include "CurrentCallsObjList.h"
#include "../App.h"

size_t CurrentCallsObjList::item_size(){	return sizeof(CallObj);	}
string CurrentCallsObjList::sql(BDb * db){
    return	"	select acctsessionid, connecttime, direction, callingstationid, calledstationid, prefix_oper, in_oper, out_oper " \
            "	from radacct_voip_start " \
            "   order by acctsessionid";
}
void CurrentCallsObjList::parse_item(BDbResult &row, void * obj){
    pCallObj item = (pCallObj)obj;
    strncpy((char*)item->id, row.get(0), 20);
    item->id_num = row.get_ll(0);
    strncpy((char*)item->time, row.get(1), 25);
    item->make_dt();
    item->out = (strcmp(row.get(2), "out") == 0);
    if (item->out){
        strcpy((char*)&item->usage, row.get(3));
        strcpy((char*)&item->phone, row.get(4));
        item->region = row.get_i(6);
    }else{
        strcpy((char*)&item->phone, row.get(3));
        strcpy((char*)&item->usage, row.get(4));
        item->region = row.get_i(7);
    }
    item->operator_id = row.get_i(5);
    if (item->region == 0) item->region = app.conf.region_id;
    item->kill_call_reason = 0;
}
long long int CurrentCallsObjList::key(void *obj){
    return ((pCallObj)obj)->id_num;
}

pCallObj CurrentCallsObjList::get(size_t i){
    return (pCallObj)_get(i);
}
pCallObj CurrentCallsObjList::find(long long int value) {
    return (pCallObj)_find(value);
}

