#pragma once

#include "../Repository.h"
#include "../../classes/CalcException.h"

class NNPResolver {
    Repository *repository;
    stringstream *trace;

    long long int getCountryCodeByID(int country_code);

public:
    NNPResolver(Repository *repository);

    void setTrace(stringstream *trace);

    void clearTrace();

    bool matchNumberNNPDestination(long long int numberPrefix, int nnp_destination_id);


    void findAllNumberRange(vector<int>&nnp_number_rangeIds,long long int numberPrefix);

    bool isExistNumberPrefixInNNPNumberRange(long long int numberPrefix, NNPNumberRange *nnpNumberRange);
};
