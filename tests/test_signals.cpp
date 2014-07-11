
#include <UnitTest++.h>

#include "../kazbase/signals.h"

TEST(test_signals) {


    struct Tester {
        int counter = 0;

        void one() {
            counter++;
        }

        int two(int i) {
            return ++counter;
        }
    };

    Tester tester;

    sig::signal<void ()> signal1;
    sig::signal<int (int)> signal2;

    sig::connection conn1 = signal1.connect(std::bind(&Tester::one, &tester));
    sig::connection conn2 = signal2.connect(std::bind(&Tester::two, &tester, std::placeholders::_1));

    CHECK(conn1.is_connected());
    CHECK(conn2.is_connected());

    signal1();

    CHECK_EQUAL(1, tester.counter);

    signal2(5);

    CHECK_EQUAL(2, tester.counter);

    conn1.disconnect();

    signal1();

    CHECK_EQUAL(2, tester.counter);

    conn2.disconnect();

    signal2(5);

    CHECK_EQUAL(2, tester.counter);
}

