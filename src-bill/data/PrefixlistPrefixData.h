#pragma once

#include "BaseData.h"
#include "../lists/PrefixlistPrefixList.h"

class PrefixlistPrefixData : public BaseData {

    ObjList * create() {
        return new PrefixlistPrefixList();
    }

};