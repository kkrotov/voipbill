#pragma once

#include "BaseData.h"
#include "../lists/PrefixlistList.h"

class PrefixlistData : public BaseData {

    ObjList * create() {
        return new PrefixlistList();
    }

};