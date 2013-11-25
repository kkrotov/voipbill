#include "OperatorList.h"

#include "../classes/App.h"

size_t OperatorList::item_size() {
    return sizeof (Operator);
}

string OperatorList::sql(BDb * db) {
    string region = app.conf.str_region_id;
    return "   select  id, pricelist_id, term_in_cost, local_network_id, local_network_pricelist_id, client_7800_pricelist_id " \
            "   from billing.operator " \
            "   where region = " + region +
            "   order by id asc ";
}

void OperatorList::parse_item(BDbResult &row, void * obj) {
    pOperator item = (pOperator) obj;
    item->id = row.get_i(0);
    item->pricelist_id = row.get_i(1);
    item->term_in_cost = row.get_d(2)*10000;
    item->local_network_id = row.get_i(3);
    item->local_network_pricelist_id = row.get_i(4);
    item->client_7800_pricelist_id = row.get_i(5);
}

int OperatorList::key(const void *obj) {
    return ((pOperator) obj)->id;
}

pOperator OperatorList::find(const int id) {
    return (pOperator) _find(id);
}


