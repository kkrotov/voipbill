#pragma once

#include "BaseData.h"
#include "../lists/ServerList.h"

class ServerData : public BaseData {

    ObjList * create() {
        return new ServerList();
    }

};