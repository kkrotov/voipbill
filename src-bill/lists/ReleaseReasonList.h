#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/ReleaseReason.h"
#include "../classes/AppBill.h"

class ReleaseReasonList : public ObjListByInt<ReleaseReason> {
protected:

    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        return "   select id, name " \
            "   from auth.release_reason " \
            "   where server_id = " + server_id +
            "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, ReleaseReason * item) {
        item->id = row.get_i(0);
        row.fill_cs(1, item->name, sizeof(item->name));
    }

    inline int key(ReleaseReason *item) {
        return item->id;
    }
public:
    ReleaseReason * find(const int id) {
        return (ReleaseReason *) _find(id);
    }
};
