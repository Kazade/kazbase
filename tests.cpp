
#include <unittest++/UnitTest++.h>

#include "kazbase/base/composite_id.h"
#include "kazbase/os/path.h"
#include "kazbase/os/core.h"
#include "kazbase/parse/tree_parser.h"
#include "kazbase/logging/logging.h"

SUITE(path_tests) {

TEST(test_split) {
    std::string orig = "/usr/bin";

    std::pair<std::string, std::string> parts = os::path::split(orig);
    CHECK_EQUAL("/usr", parts.first);
    CHECK_EQUAL("bin", parts.second);

    orig = "/usr/local/lib";
    parts = os::path::split(orig);
    CHECK_EQUAL("/usr/local", parts.first);
    CHECK_EQUAL("lib", parts.second);
}

TEST(test_join) {
    std::string dir = "/usr";
    std::string file = "test.txt";

    CHECK_EQUAL("/usr/test.txt", os::path::join(dir, file));
}

TEST(test_makedirs) {
    std::string path_to_make = os::path::join(os::temp_dir(), "my_dir");
    if(os::path::exists(path_to_make)) {
        //Recursively delete whatever is there
        os::delete_path(path_to_make, true);
    }

    CHECK(!os::path::exists(path_to_make));
    path_to_make = os::path::join(path_to_make, "my_other");

    os::make_dirs(path_to_make);
    CHECK(os::path::exists(path_to_make));
}

TEST(test_expand_user) {
    std::string p = "~/.config";
    CHECK_EQUAL("/home/lukeb/.config", os::path::expand_user(p));
}

}

SUITE(tree_parser) {

TEST(test_basic_usage) {
    std::string content = "<<<1234|1234>|1234>|1234>";

    parse::TreeParser parser("<", ">");

    parse::Node res = parser.parse(content);

    CHECK_EQUAL(1, res.child_nodes.size());
    CHECK_EQUAL(1, res.child_nodes[0].child_nodes.size());
    CHECK_EQUAL("<1234|1234>", res.child_nodes[0].child_nodes[0].full_contents);
    CHECK_EQUAL("<<1234|1234>|1234>", res.child_nodes[0].full_contents);
    CHECK_EQUAL("<<<1234|1234>|1234>|1234>", res.full_contents);
}

}

SUITE(composite_id) {

TEST(test_from_string) {
    std::string s = "[[[[twitter][1234]]][12345]]";

    typedef base::CompositeID<base::CompositeID<std::string, int>, int> IDType;

    IDType test = IDType::from_string(s);

    CHECK_EQUAL(test.second(), 12345);
    CHECK_EQUAL(test.first().second(), 1234);
    CHECK_EQUAL(test.first().first(), "twitter");
}

}


int main() {
    logging::get_logger("/")->add_handler(logging::Handler::ptr(new logging::StdIOHandler));
    return UnitTest::RunAllTests();
}

