#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/NetworkPrefixObj.h"
#include "../classes/AppBill.h"

class NetworkPrefixObjList : public ObjListByIntPrefix<NetworkPrefixObj> {
protected:

    string sql(BDb * db) {
        string time = string_date(dt);
        string server_id = app().conf.str_instance_id;
        return "	select operator_id, prefix, network_type_id from billing.network_prefix " \
            "	where instance_id = '" + server_id + "' and date_from <= '" + time + "' and (date_to is null or date_to >= '" + time + "') and deleted=false" \
            "	order by operator_id, prefix";
    }

    inline void parse_item(BDbResult &row, NetworkPrefixObj * item) {
        item->operator_id = row.get_i(0);
        memcpy(item->prefix, row.get(1), 21);
        item->network_type_id = row.get_i(2);
    }

    inline int key0(NetworkPrefixObj *item) {
        return item->operator_id;
    }

    inline char * key(NetworkPrefixObj *item) {
        return item->prefix;
    }

public:
    NetworkPrefixObj * find(const int operator_id, const char * prefix) {
        return (NetworkPrefixObj *) _find(operator_id, prefix);
    }
};
