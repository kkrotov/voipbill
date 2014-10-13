#include "TrunkList.h"

size_t TrunkList::item_size() {
    return sizeof (Trunk);
}

string TrunkList::sql(BDb * db) {
    return "   select number, name, route_table_id " \
            "   from auth.trunk " \
            "   order by number ";
}

inline void TrunkList::parse_item(BDbResult &row, void * obj) {
    pTrunk item = (pTrunk) obj;
    item->number = row.get_i(0);
    row.fill_cs(1, item->name, sizeof (item->name));
    item->route_table_id = row.get_i(2);
}

inline int TrunkList::key(const void *obj) {
    return ((pTrunk) obj)->number;
}

pTrunk TrunkList::find(const int number) {
    return (pTrunk) _find(number);
}


