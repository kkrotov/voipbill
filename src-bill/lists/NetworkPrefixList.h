#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/NetworkPrefix.h"
#include "../classes/AppBill.h"

class NetworkPrefixList : public ObjListByIntPrefix {
protected:
    size_t item_size() {
        return sizeof (NetworkPrefix);
    }

    string sql(BDb * db) {
        string time = string_date(dt);
        string server_id = app().conf.str_instance_id;
        return "	select operator_id, prefix, network_type_id from billing.network_prefix " \
            "	where instance_id = '" + server_id + "' and date_from <= '" + time + "' and (date_to is null or date_to >= '" + time + "') and deleted=false" \
            "	order by operator_id, prefix";
    }

    inline void parse_item(BDbResult &row, void * obj) {
        NetworkPrefix * item = (NetworkPrefix *) obj;
        item->operator_id = row.get_i(0);
        memcpy(item->prefix, row.get(1), 21);
        item->network_type_id = row.get_i(2);
    }

    inline int key0(const void *obj) {
        return ( (NetworkPrefix *) obj)->operator_id;
    }

    inline char * key(const void *obj) {
        return ( (NetworkPrefix *) obj)->prefix;
    }

public:
    NetworkPrefix * find(const int operator_id, const char * prefix) {
        return (NetworkPrefix *) _find(operator_id, prefix);
    }
};
