#include <UnitTest++.h>

#include "../kazbase/threadsafe/ring_buffer.h"

TEST(test_ring_buffer) {
    using namespace threadsafe;

    ring_buffer<int> buffer;

    CHECK(buffer.begin() == buffer.end());
    CHECK(buffer.empty());
    CHECK_EQUAL(0, buffer.size());

    buffer.push_back(1);
    buffer.push_back(2);

    CHECK_EQUAL(1, buffer.front());
    CHECK_EQUAL(2, buffer.back());

    ring_buffer<int>::iterator it = buffer.begin();

    int i = 0;
    for(; it != buffer.end(); ++it) {
        ++i;
    }

    CHECK_EQUAL(2, i);

    buffer.pop_back();

    CHECK_EQUAL(1, buffer.back());
    CHECK_EQUAL(1, buffer.front());

    CHECK_EQUAL(1, buffer.size());
    CHECK(!buffer.empty());

    buffer.erase(buffer.begin());

    CHECK(buffer.empty());
    CHECK_EQUAL(0, buffer.size());
    CHECK(buffer.begin() == buffer.end());

    buffer.push_back(5);
    buffer.push_back(10);

    it = buffer.begin();
    it++;

    buffer.erase(it);

    CHECK_EQUAL(5, buffer.front());
}
