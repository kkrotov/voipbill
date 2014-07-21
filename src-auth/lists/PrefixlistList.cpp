#include "PrefixlistList.h"

size_t PrefixlistList::item_size() {
    return sizeof (Prefixlist);
}

string PrefixlistList::sql(BDb * db) {
    return "   select  id, name " \
            "   from prefixlist " \
            "   order by id ";
}

inline void PrefixlistList::parse_item(BDbResult &row, void * obj) {
    pPrefixlist item = (pPrefixlist) obj;
    item->id = row.get_i(0);
    row.fill_cs(1, item->name, sizeof (item->name));
}

inline int PrefixlistList::key(const void *obj) {
    return ((pPrefixlist) obj)->id;
}

pPrefixlist PrefixlistList::find(const int id) {
    return (pPrefixlist) _find(id);
}


