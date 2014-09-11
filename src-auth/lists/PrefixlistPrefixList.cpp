#include "PrefixlistPrefixList.h"

size_t PrefixlistPrefixList::item_size() {
    return sizeof (PrefixlistPrefix);
}

string PrefixlistPrefixList::sql(BDb * db) {
    return "   select  prefixlist_id, prefix " \
            "   from auth.prefixlist_prefix " \
            "   order by prefixlist_id, prefix ";
}

inline void PrefixlistPrefixList::parse_item(BDbResult &row, void * obj) {
    pPrefixlistPrefix item = (pPrefixlistPrefix) obj;
    item->prefixlist_id = row.get_i(0);
    row.fill_cs(1, item->prefix, sizeof (item->prefix));
}

inline int PrefixlistPrefixList::key0(const void *obj) {
    return ( (pPrefixlistPrefix) obj)->prefixlist_id;
}

inline char * PrefixlistPrefixList::key(const void *obj) {
    return ( (pPrefixlistPrefix) obj)->prefix;
}

pPrefixlistPrefix PrefixlistPrefixList::find(const int prefixlist_id, const char * prefix) {
    return (pPrefixlistPrefix) _find(prefixlist_id, prefix);
}

pPrefixlistPrefix PrefixlistPrefixList::find(const int prefixlist_id, const long long int nPrefix) {
    char prefix[20];
    sprintf(prefix, "%d", nPrefix);
    return (pPrefixlistPrefix) _find(prefixlist_id, prefix);
}
