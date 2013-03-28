#include "OperatorList.h"

#include "../App.h"

size_t OperatorList::item_size(){	return sizeof(Operator);	}

string OperatorList::sql(BDb * db){
    string region = app.conf.str_region_id;
    return	"   select  id, default_pricelist_id, term_in_cost, term_out_cost, term_out_local_cost " \
            "   from billing.operator " \
            "   where region = " + region +
            "   order by id asc ";
}

void OperatorList::parse_item(BDbResult &row, void * obj){
    pOperator item = (pOperator)obj;
    item->id = row.get_i(0);
    item->pricelist_id = row.get_i(1);
    item->term_in_cost = row.get_d(2)*10000;
    item->term_out_cost = row.get_d(3)*10000;
    item->term_out_local_cost = row.get_d(4)*10000;
}

int OperatorList::key(void *obj){
    return ((pOperator)obj)->id;
}

pOperator OperatorList::find(const int id) {
    return (pOperator)_find(id);
}


