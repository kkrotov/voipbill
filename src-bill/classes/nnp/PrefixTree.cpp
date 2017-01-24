# include <iostream>
# include <stdio.h>

# include "PrefixTree.h"

namespace {
    constexpr int ALL = -1,
                  NO  = -2;
}

unsigned int PrefixTree :: alloc () {
    if ( current == block.size() ) {
        block.resize (block.size() + blockSize);
    }
    int value = current;
    current++;
    return value;
}

PrefixTree :: PrefixTree(int storage_size)
    :
    blockSize (storage_size),
    current   (0),
    master    (0)
{
    block.resize (blockSize);
    master = alloc();
}

void PrefixTree :: addTree  (PrefixTree& tree) {
    addNode (tree, 0, 0);
    return ;
}

int PrefixTree :: addNode (PrefixTree& tree, int input, int output)
{
    for (int i = 0; i < 10; i++) {
        if ( block.at(output).get(i) == ALL )
            continue;
        if ( tree.block.at(input).get(i) == ALL )
            block.at(output).set (i, ALL);
        else if ( tree.block.at(input).get(i) != NO ) {
            if ( block.at(output).get(i) == NO )
                block.at(output).set (i, alloc());
            if ( addNode (tree, tree.block.at(input).get(i), block.at(output).get(i)) == 10 )
                block.at(output).set (i, ALL);
        }
    }
    return block.at(output).getCount ();
}

void PrefixTree :: subTree (PrefixTree& tree) {
    subNode (tree, 0, 0);
}

int PrefixTree :: subNode (PrefixTree& tree, int input, int output) {
    for (int i = 0; i < 10; i++) {
        if ( block.at(output).get(i) == NO )
            continue;
        if ( tree.block.at(input).get(i) == ALL )
            block.at(output).set (i, NO);
        else if ( tree.block.at(input).get(i) != NO ) {
            if ( block.at(output).get(i) == ALL ) {
                block.at(output).set (i, NO);
                block.at(output).set (i, alloc());
                block.at ( block.at(output).get(i) ).setAll();
            }
            if ( subNode (tree, tree.block.at(input).get(i), block.at(output).get(i)) == 0 )
                block.at(output).set (i, NO);
        }
    }
    return block.at (output).getCount();
}

void PrefixTree :: addRange (std::pair<long long int, long long int> pair) {
    char low [32];
    char high[32];
    sprintf (low,  "%lld", pair.first);
    sprintf (high, "%lld", pair.second);

    addRange (high, low, master);

    return ;
}

bool PrefixTree :: checkInclusion (char* array, int i) {
    int index = array[0] - '0';
    if ( block.at(i).get(index) == NO )
        return false;
    else if ( block.at(i).get(index) == ALL )
        return true;
    if ( !*(array+1) ) {
        return false;
    }
    return checkInclusion (array+1, block.at(i).get(index));
}

int PrefixTree :: addRange (char* high, char* low, int i) {
    if ( *high == *low ) {
        int num = *high - '0';
        if ( block.at(i).get(num) == ALL )
            return block.at(i).getCount();
        else if ( block.at(i).get(num) == NO ) {
            if ( *(high + 1) == 0 ) {
                block.at(i).set(num, ALL);
            }
            else {
                block.at(i).set(num, alloc());
                if ( addRange (high + 1, low + 1, block.at(i).get(num)) == 10 )
                    block.at(i).set(num, ALL);
                return block.at(i).getCount();
            }
        }
        else {
            if ( *(high + 1) == 0 ) {
                block.at(i).set(num, ALL);
            }
            else {
                if ( addRange (high + 1, low + 1, block.at(i).get(num)) == 10 )
                    block.at(i).set(num, ALL);
                return block.at(i).getCount();
            }
        }
    }
    else {
        for (int j = *low + 1; j <= *high - 1; j++) {
            block.at(i).set(j-'0', ALL);
        }
        if ( *(high + 1) == 0 ) {
            block.at(i).set(*high - '0', ALL);
            block.at(i).set(*low  - '0', ALL);
        }
        else {
            int res = 0;
            if ( block.at(i).get(*high - '0') == NO )
                block.at (i).set(*high - '0', alloc());
            if ( block.at(i).get(*high - '0') >= 0 )
                res = rangeDown (high + 1, block.at (i).get(*high - '0'));
            if ( res == 10 )
                block.at(i).set(*high-'0', ALL);


            res = 0;
            if ( block.at(i).get(*low - '0') == NO )
                block.at (i).set(*low - '0', alloc());
            if ( block.at(i).get(*low - '0') >= 0 )
                res = rangeUp (low + 1, block.at (i).get(*low - '0'));
            if ( res == 10 )
                block.at(i).set(*low-'0', ALL);
        }
    }
    return block.at(i).getCount();
}

int PrefixTree :: rangeUp   (char* high, int i) {
    if ( *(high + 1) == 0 ) {
        block.at(i).set(*high - '0', ALL);
    }
    else {
        if ( block.at(i).get(*high - '0') == NO )
            block.at (i).set(*high - '0', alloc());
        if ( block.at(i).get(*high - '0') >= 0 )
            if ( rangeUp (high + 1, block.at(i).get(*high - '0')) == 10 )
                block.at(i).set(*high - '0', ALL);
    }
    for (int j = *high + 1; j <= '9'; j++) {
        block.at(i).set(j - '0', ALL);
    }
    return block.at(i).getCount();
}

int PrefixTree :: rangeDown (char* low , int i) {
    if ( *(low + 1) == 0 ) {
        block.at(i).set(*low - '0', ALL);
    }
    else {
        if ( block.at(i).get(*low - '0') == NO )
            block.at(i).set(*low - '0', alloc());
        if ( block.at(i).get(*low - '0') >= 0 )
            if ( rangeDown (low + 1, block.at(i).get(*low - '0')) == 10 )
                block.at(i).set(*low - '0', ALL);
    }
    for (int j = *low - 1; j >= '0'; j--) {
        block.at(i).set(j - '0', ALL);
    }
    return block.at(i).getCount();
}

void PrefixTree :: print (char* array, int pos, int i, std::vector<long long int>& storage) {
    for (int j = 0; j < 10; j++) {
        int value = block.at(i).get(j);
        if ( value == NO )
            continue;
        if ( value == ALL ) {
            *(array + pos) = '0' + j;
            *(array + pos + 1) = 0;
            long long int num;
            sscanf (array, "%lld", &num);
            storage.push_back(num);
        }
        if ( value >= 0 ) {
            *(array + pos) = '0' + j;
            print (array, pos + 1, value, storage);
        }
    }
}

PrefixTree :: ~PrefixTree () {
}
