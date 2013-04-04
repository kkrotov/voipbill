#pragma once

#include "CallsObjList.h"


class RuntimeCallsObjList : public CallsObjList {
protected:
    string sql(BDb * db);
    void parse_item(BDbResult &row, void * obj);
public:
	const int nrows = 50000;
    RuntimeCallsObjList();
    void refresh_current_id();
};
