class PrefixNode {

    int   child[10];
    unsigned char count;

    public:

    PrefixNode ();

    int  get (unsigned char i);
    void set (unsigned char i, int what);

    unsigned char getCount ();

    void setAll ();
};
