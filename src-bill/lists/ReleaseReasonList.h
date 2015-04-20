#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/ReleaseReason.h"
#include "../classes/AppBill.h"

class ReleaseReasonList : public ObjList<ReleaseReason> {
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

    struct key_id {
        bool operator() (const ReleaseReason & left, int id) {
            return left.id < id;
        }
        bool operator() (int id, const ReleaseReason & right) {
            return id < right.id;
        }
    };

public:
    ReleaseReason * find(int id) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }
        return begin <  end ? &*begin : nullptr;
    }
};
