#include "AirpList.h"

size_t AirpList::item_size() {
    return sizeof (Airp);
}

string AirpList::sql(BDb * db) {
    return "   select  id, name " \
            "   from auth.airp " \
            "   order by id ";
}

inline void AirpList::parse_item(BDbResult &row, void * obj) {
    pAirp item = (pAirp) obj;
    item->id = row.get_i(0);
    row.fill_cs(1, item->name, sizeof (item->name));
}

inline int AirpList::key(const void *obj) {
    return ((pAirp) obj)->id;
}

pAirp AirpList::find(const int id) {
    return (pAirp) _find(id);
}


