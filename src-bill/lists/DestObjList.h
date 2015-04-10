#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/obj.h"
#include "../classes/AppBill.h"
#include "../../src/classes/DbException.h"

class DestObjList : public ObjListByPrefix {
protected:
    size_t item_size() {
        return sizeof (DestObj);
    }

    string sql(BDb * db) {
        BDbResult res = db->query(" select " \
                              "     substring(cast(region_id as varchar) from 2 for char_length(cast(region_id as varchar))-2), " \
                              "     city_geo_id " \
                              " from billing.instance_settings " \
                              " where id = " + app().conf.str_instance_id);
        if (res.next()) {
            string region_id_list = res.get_s(0);
            string city_geo_id = res.get_s(1);

            return "   select prefix, " \
                    "            case when geo_id = " + city_geo_id + " and not mob then " \
                    "                -1 " \
                    "            else " \
                    "                case when region in (" + region_id_list + ") then " \
                    "                    0 " \
                    "                else " \
                    "                    dest " \
                    "                end " \
                    "            end, " \
                    "    geo_id, operator_id, mob" \
                    "   from geo.prefix " \
                    "   order by prefix asc ";
        } else {
            throw DbException("instance_settings not found", "DestObjList::sql");
        }
    }

    inline void parse_item(BDbResult &row, void * obj) {
        pDestObj item = (pDestObj) obj;
        strcpy(item->prefix, row.get(0));
        item->dest = row.get_i(1);
        item->geo_id = row.get_i(2);
        item->geo_operator_id = row.get_i(3);
        item->mob = row.get_b(4);
    }

    inline char * key(const void *obj) {
        return ( (pDestObj) obj)->prefix;
    }


public:

    pDestObj find(const char * prefix) {
        return (pDestObj) _find(prefix);
    }
};
