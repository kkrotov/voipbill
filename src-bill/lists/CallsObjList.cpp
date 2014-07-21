#include "CallsObjList.h"
#include <boost/lexical_cast.hpp>

CallsObjList::CallsObjList() {
    last_id = 0;
    new_last_id = 0;
}

size_t CallsObjList::item_size() {
    return sizeof (CallObj);
}

bool CallsObjList::loaddata(BDb *db) {
    ObjList::load(db, 0);
    if (count > 0) {
        pCallObj item = (pCallObj) _get(count - 1);
        new_last_id = boost::lexical_cast<long long int>(item->id);
        return true;
    }
    return false;
}

bool CallsObjList::next() {
    if (count > 0) {
        last_id = new_last_id;
        new_last_id = 0;
        return true;
    } else {
        return false;
    }
}

pCallObj CallsObjList::get(size_t i) {
    return (pCallObj) _get(i);
}
