#pragma once

struct PrefixlistPrefix {
    int prefixlist_id;
    char prefix[21];
};

typedef PrefixlistPrefix * pPrefixlistPrefix;