#pragma once

#include "BaseData.h"
#include "../lists/ClientList.h"

class ClientData : public BaseData {

    ObjList * create() {
        return new ClientList();
    }

};