#include "PriceObjList.h"

size_t PriceObjList::item_size(){	return sizeof(PriceObj);	}
string PriceObjList::sql(BDb * db){
    string time = string_date(dt);
    return		"	select pricelist_id, ndef::varchar, cast(price*10000 as integer) from billing.defs " \
                "	where date_from <= '"+time+"' and (date_to is null or date_to >= '"+time+"') and deleted=false" \
                "	order by pricelist_id, ndef::varchar";
}
void PriceObjList::parse_item(BDbResult &row, void * obj){
    pPriceObj item = (pPriceObj)obj;
    item->pricelist_id = row.get_i(0);
    memcpy(item->prefix, row.get(1), 21);
    item->price = row.get_i(2);
}
int PriceObjList::key0(void *obj){
    return( (pPriceObj)obj)->pricelist_id;
}
char * PriceObjList::key(void *obj){
    return( (pPriceObj)obj)->prefix;
}

pPriceObj PriceObjList::find(int pricelist_id, char * prefix) {
    return (pPriceObj)_find(pricelist_id, prefix);
}

