#include "UsageRawList.h"

size_t UsageRawList::item_size(){	return sizeof(UsageRaw);	}

string UsageRawList::sql(BDb * db){
    return	"   select  id, client_id, phone_num, actual_from, actual_to, region " \
            "   from billing.usage " \
            "   order by id asc ";
}

void UsageRawList::parse_item(BDbResult &row, void * obj){
    pUsageRaw item = (pUsageRaw)obj;
    item->id = row.get_i(0);
    item->client_id = row.get_i(1);
    item->phone_num = row.get_ll(2);
    item->actual_from = 0;
    item->actual_to = 0;
    item->region = row.get_i(5);
}

int UsageRawList::key(void *obj){
    return ((pUsageRaw)obj)->id;
}

pUsageRaw UsageRawList::find(const int id) {
    return (pUsageRaw)_find(id);
}

