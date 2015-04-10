#pragma once

#include "BaseData.h"
#include "../lists/TrunkRuleList.h"

class TrunkRuleData : public BaseData {

    ObjList * create() {
        return new TrunkRuleList();
    }

};