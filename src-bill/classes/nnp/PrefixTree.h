# include <vector>
# include "PrefixNode.h"

class PrefixTree {

    PrefixTree ();

    unsigned int blockSize;
    unsigned int current;
    int master;
    std::vector<PrefixNode> block;

    int addRange (char* high, char* low, int i);
    int rangeUp   (char* high, int i);
    int rangeDown (char* low , int i);

    int addNode (PrefixTree& tree, int input, int output);
    int subNode (PrefixTree& tree, int input, int output);

    unsigned int alloc ();

    public:

    PrefixTree  (int storage_size);
    ~PrefixTree ();

    void addRange (std::pair<long long int, long long int>);
    void addTree  (PrefixTree& tree);
    void subTree  (PrefixTree& tree);

    void print (char* array, int pos, int i, std::vector<long long int>& storage);

    bool checkInclusion (char* array, int num);
};
