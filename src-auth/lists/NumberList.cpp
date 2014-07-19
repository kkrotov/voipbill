#include "NumberList.h"

size_t NumberList::item_size() {
    return sizeof (Number);
}

string NumberList::sql(BDb * db) {
    return "   select  id, name, prefixlist_ids " \
            "   from number " \
            "   order by id ";
}

inline void NumberList::parse_item(BDbResult &row, void * obj) {
    pNumber item = (pNumber) obj;
    item->id = row.get_i(0);
    row.fill_cs(1, item->name, sizeof (item->name));
    row.fill_cs(2, item->prefixlist_ids, sizeof (item->prefixlist_ids));
}

inline int NumberList::key(const void *obj) {
    return ((pNumber) obj)->id;
}

pNumber NumberList::find(const int id) {
    return (pNumber) _find(id);
}


