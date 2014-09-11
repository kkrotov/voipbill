#include "TrunkGroupList.h"

size_t TrunkGroupList::item_size() {
    return sizeof (TrunkGroup);
}

string TrunkGroupList::sql(BDb * db) {
    return "   select  id, name, trunk_numbers " \
            "   from auth.trunk_group " \
            "   order by id ";
}

inline void TrunkGroupList::parse_item(BDbResult &row, void * obj) {
    pTrunkGroup item = (pTrunkGroup) obj;
    item->id = row.get_i(0);
    row.fill_cs(1, item->name, sizeof (item->name));
    row.fill_cs(2, item->trunk_numbers, sizeof (item->trunk_numbers));
}

inline int TrunkGroupList::key(const void *obj) {
    return ((pTrunkGroup) obj)->id;
}

pTrunkGroup TrunkGroupList::find(const int id) {
    return (pTrunkGroup) _find(id);
}


