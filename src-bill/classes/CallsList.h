#pragma once

#include "../models/obj.h"
#include "../lists/CdrObjList.h"

class CallsList {
protected:
    void * data;
public:
    int count;
    CallsList(CdrObjList *cdr_list);
    pCallObj get(size_t i);
};
