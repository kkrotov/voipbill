#include "NetworkPrefixObjList.h"

#include "../classes/App.h"

size_t NetworkPrefixObjList::item_size() {
    return sizeof (PriceObj);
}

string NetworkPrefixObjList::sql(BDb * db) {
    string time = string_date(dt);
    string instance_id = app.conf.str_instance_id;
    return "	select operator_id, prefix, network_type_id from billing.network_prefix " \
            "	where instance_id = '" + instance_id + "' and date_from <= '" + time + "' and (date_to is null or date_to >= '" + time + "') and deleted=false" \
            "	order by operator_id, prefix";
}

inline void NetworkPrefixObjList::parse_item(BDbResult &row, void * obj) {
    pNetworkPrefixObj item = (pNetworkPrefixObj) obj;
    item->operator_id = row.get_i(0);
    memcpy(item->prefix, row.get(1), 21);
    item->network_type_id = row.get_i(2);
}

inline int NetworkPrefixObjList::key0(const void *obj) {
    return ( (pNetworkPrefixObj) obj)->operator_id;
}

inline char * NetworkPrefixObjList::key(const void *obj) {
    return ( (pNetworkPrefixObj) obj)->prefix;
}

pNetworkPrefixObj NetworkPrefixObjList::find(const int operator_id, const char * prefix) {
    return (pNetworkPrefixObj) _find(operator_id, prefix);
}

