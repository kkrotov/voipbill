#include "OperatorRuleList.h"

size_t OperatorRuleList::item_size() {
    return sizeof (OperatorRule);
}

string OperatorRuleList::sql(BDb * db) {
    return "   select  operator_id, outgoing, \"order\", " \
            "       trunk_group_id, prefixlist_id " \
            "   from auth.operator_rule " \
            "   order by operator_id, outgoing, \"order\" ";
}

inline void OperatorRuleList::parse_item(BDbResult &row, void * obj) {
    pOperatorRule item = (pOperatorRule) obj;
    item->operator_id = row.get_i(0);
    item->outgoing = row.get_b(1);
    item->order = row.get_i(2);
    item->trunk_group_id = row.get_i(3);
    item->prefixlist_id = row.get_i(4);
    item->key = (item->operator_id << 10) + (item->outgoing ? item->order + 500 : item->order);
}

inline int OperatorRuleList::key(const void *obj) {
    return ((pOperatorRule) obj)->key;
}

pOperatorRule OperatorRuleList::find(const int operator_id, const bool outgoing, const int order) {
    int key = (operator_id << 10) + (outgoing ? order + 500 : order);
    return (pOperatorRule) _find(key);
}


