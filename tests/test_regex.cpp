#include <UnitTest++/UnitTest++.h>

#include "../kazbase/regex.h"

TEST(test_regex_match) {
    auto re = regex::compile(R"(Your number is <b>(\d+)</b>)");
    auto match = re.match("Your number is <b>123</b>  fdjsk");

    CHECK(match); //Match should evaluate to true
    CHECK_EQUAL(2, match.groups().size()); //Should be two groups (the full match, and the number)
    CHECK_EQUAL(_u("Your number is <b>123</b>"), match.group());
    CHECK_EQUAL(_u("123"), match.group(1));
    CHECK_EQUAL(0, match.start());
    CHECK_EQUAL(25, match.end());
    CHECK_EQUAL(0, match.span().first);
    CHECK_EQUAL(25, match.span().second);

    //Check that the start position works
    match = re.match("xxx Your number is <b>123</b>", 4);
    CHECK(match);
    CHECK_EQUAL(_u("123"), match.group(1));
    CHECK_EQUAL(4, match.start());
    CHECK_EQUAL(29, match.end());
    CHECK_EQUAL(4, match.span().first);
    CHECK_EQUAL(29, match.span().second);
}
