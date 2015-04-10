#include "CallsList.h"


CallsList::CallsList(CdrObjList *cdr_list) {
    count = cdr_list->count;

    size_t datasize = count * 2 * sizeof(CallObj);

    data = malloc(datasize);
    memset(data, 0, datasize);

    size_t i = 0;
    while (i < count) {
        pCdrObj cdr = cdr_list->get(i);

        get(i)->initOrigByCdr(cdr);

        i++;

        get(i)->initTermByCdr(cdr);

        i++;
    }
}

pCallObj CallsList::get(size_t i) {
    return (pCallObj)((char *) data + i * sizeof(CallObj));
}