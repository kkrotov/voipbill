#include "PriceObjList.h"

size_t PriceObjList::item_size() {
    return sizeof (PriceObj);
}

string PriceObjList::sql(BDb * db) {
    string time = string_date(dt);
    return "	select pricelist_id, ndef::varchar, cast(price*10000 as integer) from billing.defs " \
            "	where date_from <= '" + time + "' and (date_to is null or date_to >= '" + time + "') and deleted=false" \
            "	order by pricelist_id, ndef::varchar";
}

inline void PriceObjList::parse_item(BDbResult &row, void * obj) {
    pPriceObj item = (pPriceObj) obj;
    item->pricelist_id = row.get_i(0);
    memcpy(item->prefix, row.get(1), 21);
    item->price = row.get_i(2);
}

inline int PriceObjList::key0(const void *obj) {
    return ( (pPriceObj) obj)->pricelist_id;
}

inline char * PriceObjList::key(const void *obj) {
    return ( (pPriceObj) obj)->prefix;
}

pPriceObj PriceObjList::find(const int pricelist_id, const char * prefix) {
    return (pPriceObj) _find(pricelist_id, prefix);
}

pPriceObj PriceObjList::find(const int pricelist_id, const long long int nPrefix) {
    char prefix[20];
    sprintf(prefix, "%d", nPrefix);
    return (pPriceObj) _find(pricelist_id, prefix);
}

