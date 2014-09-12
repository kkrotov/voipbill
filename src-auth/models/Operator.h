#pragma once

#include "../../src/common.h"

struct Operator {
    int id;
    char name[51];
    int code;
    bool source_rule_default_allowed;
    bool destination_rule_default_allowed;

    string getFormattedCode() {
        string formattedCode = lexical_cast<string>(code);
        if (formattedCode.size() < 2) {
            formattedCode = "0" + formattedCode;
        }
        return formattedCode;
    }
};

typedef Operator * pOperator;