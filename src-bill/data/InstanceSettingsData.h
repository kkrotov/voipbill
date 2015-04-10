#pragma once

#include "BaseData.h"
#include "../lists/InstanceSettingsList.h"

class InstanceSettingsData : public BaseData {

    ObjList * create() {
        return new InstanceSettingsList();
    }

};