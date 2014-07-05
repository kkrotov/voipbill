#include "PricelistList.h"

#include "../classes/AppBill.h"

size_t PricelistList::item_size() {
    return sizeof (Pricelist);
}

string PricelistList::sql(BDb * db) {
    return "   select  id, region, operator_id, tariffication_by_minutes, tariffication_full_first_minute " \
            "   from billing.pricelist " \
            "   order by id asc ";
}

inline void PricelistList::parse_item(BDbResult &row, void * obj) {
    pPricelist item = (pPricelist) obj;
    item->id = row.get_i(0);
    item->region = row.get_i(1);
    item->operator_id = row.get_i(2);
    item->tariffication_by_minutes = row.get_b(3);
    item->tariffication_full_first_minute = row.get_b(4);
}

inline int PricelistList::key(const void *obj) {
    return ((pPricelist) obj)->id;
}

pPricelist PricelistList::find(const int id) {
    return (pPricelist) _find(id);
}


