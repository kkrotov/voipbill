#include "Test.h"

#include "../classes/App.h"
#include "../lists/ClientCounter.h"
#include "../lists/FminCounter.h"
#include "../classes/Loader.h"


TEST(Loader,ClientCounter) {

    ClientCounter l;
    l.get(1000).sum = 1;
    l.get(1001).sum = 1;
    l.get(1001).sum++;

    ClientCounter l2;
    l2.get(1000).sum = 1;
    l2.get(1002).sum = 1;

    l.append(&l2);

    EXPECT_EQ(3, (int)l.counter.size());
    EXPECT_EQ(2, l.get(1000).sum);
    EXPECT_EQ(2, l.get(1001).sum);
    EXPECT_EQ(1, l.get(1002).sum);

    EXPECT_EQ(2, (int)l2.counter.size());
    EXPECT_EQ(1, l2.get(1000).sum);
    EXPECT_EQ(1, l2.get(1002).sum);
}


TEST(Loader,FminCounter) {

    FminCounter l;
    l.set(0,1000, 1);
    l.add(0,1000, 3);
    l.add(0, 1001, 4);
    l.get(0, 1001)++;

    FminCounter l2;
    l2.set(0, 1000, 1);
    l2.set(0, 1002, 1);
    l2.set(1000, 1003, 1);

    l.append(&l2);

    EXPECT_EQ(4, (int)l.counter.size());
    EXPECT_EQ(5, l.get(0, 1000));
    EXPECT_EQ(5, l.get(0, 1001));
    EXPECT_EQ(1, l.get(0, 1002));
    EXPECT_EQ(1, l.get(1000, 1003));

    EXPECT_EQ(3, (int)l2.counter.size());
    EXPECT_EQ(1, l2.get(0, 1000));
    EXPECT_EQ(1, l2.get(0, 1002));
    EXPECT_EQ(1, l2.get(1000, 1003));
}


TEST(Loader,Loader) {

    Loader<FminCounter> k1;

    FminCounter *l1 = new FminCounter();
    l1->set(0, 1000, 1);
    l1->set(0, 1001, 1);
    k1.addlist(0, l1);

    FminCounter *l2 = new FminCounter();
    l2->set(0, 1000, 1);
    l2->set(0, 1002, 1);
    k1.addlist(0, l2);


    FminCounter *l3 = new FminCounter();
    l3->set(0, 1000, 1);
    l3->set(0, 1003, 1);
    k1.addlist(1000, l3);

    Loader<FminCounter> k2;

    FminCounter *l4 = new FminCounter();
    l4->set(0, 1000, 1);
    l4->set(0, 1004, 1);
    k2.addlist(0, l4);

    FminCounter *l5 = new FminCounter();
    l5->set(0, 1000, 1);
    l5->set(0, 1004, 1);
    k2.addlist(2000, l5);

    k1.append(k2);


    shared_ptr<FminCounter> ll;


    ll = k1.get(0);
    EXPECT_EQ(2, ll->get(0, 1000));
    EXPECT_EQ(1, ll->get(0, 1002));
    EXPECT_EQ(1, ll->get(0, 1004));

    ll = k1.get(1000);
    EXPECT_EQ(1, ll->get(0, 1000));
    EXPECT_EQ(1, ll->get(0, 1003));

    ll = k1.get(2000);
    EXPECT_EQ(1, ll->get(0, 1000));
    EXPECT_EQ(1, ll->get(0, 1004));

    EXPECT_FALSE(k2.get(0) == 0);
    EXPECT_TRUE(k2.get(1234) == 0);

    EXPECT_EQ(3, (int)k1.datamap.size());
    EXPECT_EQ(2, (int)k2.datamap.size());

}


