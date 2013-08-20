#include "AsteriskNumberObjList.h"

size_t AsteriskNumberObjList::item_size() {
    return sizeof (AsteriskNumberObj);
}

string AsteriskNumberObjList::sql(BDb * db) {
    return " select " \
            "   case strpos(callerid,'74959505680*') " \
            "       when 1 then " \
            "           substring(callerid from 13) " \
            "       else " \
            "           callerid " \
            "   end as usr_num, " \
            "   max('85.94.32.'||substring(ippbx from 4)) as srv_ip, " \
            "   max(host(ipaddr)) as usr_ip " \
            " from sip_users " \
            " where ipaddr between '89.235.181.0' and '89.235.181.255' " \
            " group by usr_num " \
            " order by usr_num ";
}

void AsteriskNumberObjList::parse_item(BDbResult &row, void * obj) {
    pAsteriskNumberObj item = (pAsteriskNumberObj) obj;
    item->usr_num = row.get_ll(0);
    strcpy(item->srv_ip, row.get(1));
    strcpy(item->usr_ip, row.get(2));
}

long long int AsteriskNumberObjList::key(void *obj) {
    return ((pAsteriskNumberObj) obj)->usr_num;
}

pAsteriskNumberObj AsteriskNumberObjList::find(long long int value) {
    return (pAsteriskNumberObj) _find(value);
}


