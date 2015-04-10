#pragma once

#include "BaseData.h"
#include "../lists/NetworkPrefixList.h"

class NetworkPrefixData : public BaseData {

    ObjList * create() {
        return new NetworkPrefixList();
    }

};