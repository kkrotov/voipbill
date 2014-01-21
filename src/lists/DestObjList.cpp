#include "DestObjList.h"
#include "../classes/App.h"

#include <boost/algorithm/string/join.hpp>

size_t DestObjList::item_size() {
    return sizeof (DestObj);
}

string DestObjList::sql(BDb * db) {
    string region_id_list;
    string city_prefix_list;

    BDbResult res = db->query(" select " \
                              "     substring(cast(region_id as varchar) from 2 for char_length(cast(region_id as varchar))-2), " \
                              "     replace(substring(cast(city_prefix as varchar) from 2 for char_length(cast(city_prefix as varchar))-2), ',', '|') " \
                              " from billing.instance_settings " \
                              " where id = " + app.conf.str_instance_id);
    if (res.next()) {
        region_id_list = res.get_s(0);
        city_prefix_list = res.get_s(1);
    }
    return "   select prefix, " \
                "            case when prefix ~ '^(" + city_prefix_list + ")' then " \
                "                -1 " \
                "            else " \
                "                case when region in (" + region_id_list + ") then " \
                "                    case when mob then "\
                "                        0 "\
                "                    else "\
                "                        1 "\
                "                    end " \
                "                else " \
                "                    dest " \
                "                end " \
                "            end, " \
                "    geo_id, geo_operator_id, mob" \
                "   from geo.prefix " \
                "   order by prefix asc ";
}

inline void DestObjList::parse_item(BDbResult &row, void * obj) {
    pDestObj item = (pDestObj) obj;
    strcpy(item->prefix, row.get(0));
    item->dest = row.get_i(1);
    item->geo_id = row.get_i(2);
    item->geo_operator_id = row.get_i(3);
    item->mob = row.get_b(4);
}

inline char * DestObjList::key(const void *obj) {
    return ( (pDestObj) obj)->prefix;
}

pDestObj DestObjList::find(const char * prefix) {
    return (pDestObj) _find(prefix);
}
