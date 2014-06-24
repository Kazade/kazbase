
#include <UnitTest++.h>

#include "../kazbase/unicode.h"

TEST(test_unicode_formatting) {

    unicode test1 = _u("{0} is formatted. So is {1}").format(0, 1);
    CHECK_EQUAL(unicode("0 is formatted. So is 1"), test1);

    unicode test2 = _u("{0:.2f} is formatted. So is {1:.2f}").format(0.011, 1.051);
    CHECK_EQUAL(unicode("0.01 is formatted. So is 1.05"), test2);
}
