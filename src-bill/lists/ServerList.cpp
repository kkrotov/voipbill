#include "ServerList.h"

int ServerList::getServerIdByIP(struct in_addr l_addr) {
    auto begin = this->data.begin();
    auto end = this->data.end();
    for (auto i = begin; i < end; i++) {

        if(i->nas_ip_address.s_addr == l_addr.s_addr) return i->id;
    }
    return 0;
}
