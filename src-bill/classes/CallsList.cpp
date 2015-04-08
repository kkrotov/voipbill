#include "CallsList.h"


CallsList::CallsList(CdrObjList *cdr_list) {
    count = cdr_list->count;

    size_t datasize = count * sizeof(CallObj);

    data = malloc(datasize);
    memset(data, 0, datasize);

    size_t i = 0;
    while (i < count) {
        pCdrObj cdr = cdr_list->get(i);
        pCallObj call = this->get(i);
        //call->ini
    }
}

pCallObj CallsList::get(size_t i) {
    return (pCallObj)((char *) data + i * sizeof(CallObj));
}