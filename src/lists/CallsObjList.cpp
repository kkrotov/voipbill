#include "CallsObjList.h"
#include <boost/lexical_cast.hpp>

size_t CallsObjList::item_size(){	return sizeof(CallObj);	}
void CallsObjList::init(time_t from, time_t to, int rows_count){
    d_from = from;	d_to = to;	nrows = rows_count;
    last_id = 0;	new_last_id = 0;
}
bool CallsObjList::loaddata(BDb *db){
    ObjList::load(db, 0);
    if (count > 0){
        pCallObj item = (pCallObj)_get(count-1);
        new_last_id = boost::lexical_cast<long long int>(item->id);
        return true;
    }
    return false;
}
bool CallsObjList::next(){
    if (count > 0){
        last_id = new_last_id;
        new_last_id = 0;
        return true;
    }else{
        return false;
    }
}
pCallObj CallsObjList::get(size_t i){
    return (pCallObj)_get(i);
}
