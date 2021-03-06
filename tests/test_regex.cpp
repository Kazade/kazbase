#include <UnitTest++.h>

#include "../kazbase/regex.h"
#include "../kazbase/os.h"
#include "../kazbase/file_utils.h"

TEST(test_regex_match) {
    auto re = regex::Regex(R"(Your number is <b>(\d+)</b>)");
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

    re = regex::Regex(R"([^"\\]*(?:(?:\\.|"(?!""))[^"\\]*)*""")");
    match = re.match("this is some text that ends with \"\"\"");
    CHECK(match);
    match = re.match("this is some text that doesn't");
    CHECK(!match);
}

TEST(test_regex_unicode) {
    auto utf8 = os::path::join({os::path::dir_name(__FILE__), "utf8-file.txt"});

    std::string enc;

    auto data = file_utils::read(utf8, &enc);
    CHECK_EQUAL("utf-8", enc);

    regex::Regex re("test");

    auto matches = re.search(data);
}
