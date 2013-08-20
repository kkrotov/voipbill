#pragma once

#include "CalcBase.h"

class CalcFull : public CalcBase {
protected:
    void calc_item(pCallObj call);

public:
    void html(std::stringstream &html);
};

