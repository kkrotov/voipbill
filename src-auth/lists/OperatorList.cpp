#include "OperatorList.h"

size_t OperatorList::item_size() {
    return sizeof (Operator);
}

string OperatorList::sql(BDb * db) {
    return "   select  id, name, code, source_rule_default_allowed, destination_rule_default_allowed " \
            "   from auth.operator " \
            "   order by id ";
}

inline void OperatorList::parse_item(BDbResult &row, void * obj) {
    pOperator item = (pOperator) obj;
    item->id = row.get_i(0);
    row.fill_cs(1, item->name, sizeof (item->name));
    item->code = row.get_i(2);
    item->source_rule_default_allowed = row.get_b(3);
    item->destination_rule_default_allowed = row.get_b(4);
}

inline int OperatorList::key(const void *obj) {
    return ((pOperator) obj)->id;
}

pOperator OperatorList::find(const int id) {
    return (pOperator) _find(id);
}


