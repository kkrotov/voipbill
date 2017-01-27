# include "PrefixNode.h"

namespace
{
    constexpr int ALL = -1,
                  NO  = -2,
                  ERROR = -3;
}

PrefixNode :: PrefixNode ()
    :
    count (0)
{
    child[0] =
    child[1] =
    child[2] =
    child[3] =
    child[4] =
    child[5] =
    child[6] =
    child[7] =
    child[8] =
    child[9] = NO;
}

int  PrefixNode :: get (unsigned char i) {
    if ( i > 9 )
        return ERROR;
    return child[i];
}

void PrefixNode :: set (unsigned char i, int what) {
    if ( i > 9 )
        return ;

    if ( what >= 0 ) {
        if ( child[i] != ALL )
            child[i] = what;
    }
    else if ( what == ALL ) {
        if ( child[i] != ALL )
            count++;
        child[i] = ALL;
    }
    else if ( what == NO ) {
        if ( child[i] == ALL )
            count--;
        child[i] = NO;
    }
}

unsigned char  PrefixNode :: getCount () {
    return count;
}

void PrefixNode :: setAll () {
    child[0] =
    child[1] =
    child[2] =
    child[3] =
    child[4] =
    child[5] =
    child[6] =
    child[7] =
    child[8] =
    child[9] = ALL;

    count = 10;
}
