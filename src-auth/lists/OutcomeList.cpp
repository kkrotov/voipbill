#include "OutcomeList.h"

size_t OutcomeList::item_size() {
    return sizeof (Outcome);
}

string OutcomeList::sql(BDb * db) {
    return "   select  id, name, type_id, " \
            "       calling_station_id, called_station_id, " \
            "       route_case_id, release_reason_id, airp_id " \
            "   from auth.outcome " \
            "   order by id ";
}

inline void OutcomeList::parse_item(BDbResult &row, void * obj) {
    pOutcome item = (pOutcome) obj;
    item->id = row.get_i(0);
    row.fill_cs(1, item->name, sizeof (item->name));
    item->type_id = row.get_i(2);
    row.fill_cs(3, item->calling_station_id, sizeof (item->calling_station_id));
    row.fill_cs(4, item->called_station_id, sizeof (item->called_station_id));
    item->route_case_id = row.get_i(5);
    item->release_reason_id = row.get_i(6);
    item->airp_id = row.get_i(7);
}

inline int OutcomeList::key(const void *obj) {
    return ((pOutcome) obj)->id;
}

pOutcome OutcomeList::find(const int id) {
    return (pOutcome) _find(id);
}


