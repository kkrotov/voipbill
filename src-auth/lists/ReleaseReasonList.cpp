#include "ReleaseReasonList.h"

size_t ReleaseReasonList::item_size() {
    return sizeof (ReleaseReason);
}

string ReleaseReasonList::sql(BDb * db) {
    return "   select  id, name " \
            "   from release_reason " \
            "   order by id ";
}

inline void ReleaseReasonList::parse_item(BDbResult &row, void * obj) {
    pReleaseReason item = (pReleaseReason) obj;
    item->id = row.get_i(0);
    row.fill_cs(1, item->name, sizeof (item->name));
}

inline int ReleaseReasonList::key(const void *obj) {
    return ((pReleaseReason) obj)->id;
}

pReleaseReason ReleaseReasonList::find(const int id) {
    return (pReleaseReason) _find(id);
}


