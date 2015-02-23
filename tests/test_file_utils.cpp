
#include <UnitTest++.h>

#include <iostream>
#include "../kazbase/file_utils.h"
#include "../kazbase/os.h"

TEST(test_reading_unicode_file) {
    auto utf8 = os::path::join({os::path::dir_name(__FILE__), "utf8-file.txt"});
    auto utf16 = os::path::join({os::path::dir_name(__FILE__), "utf16-file.txt"});
    auto utf32 = os::path::join({os::path::dir_name(__FILE__), "utf32-file.txt"});

    std::string enc;

    auto data = file_utils::read(utf8, &enc);
    CHECK_EQUAL("utf-8", enc);
    data.encode();

    file_utils::read(utf16, &enc);
    CHECK_EQUAL("utf-16", enc);

    file_utils::read(utf32, &enc);
    CHECK_EQUAL("utf-32", enc);
}
