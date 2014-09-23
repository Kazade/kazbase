#include <UnitTest++.h>

#include <iostream>
#include "../kazbase/json/json.h"

TEST(test_json) {

    json::JSON things;

    things.append_dict();
    things[0].set("key", 1.01);
    things[0].set("key2", "Banana");
    things[0].set("key3", true);
    things[0].set_null("key4");
    things[0].set("key5", 1);

    CHECK_EQUAL("[{key : 1.01, key2 : \"Banana\", key3 : true, key4 : null, key5 : 1}]", json::dumps(things));
}

TEST(test_json_update) {

    json::JSON dict1;
    json::JSON dict2;

    dict1.set("A", 1);
    dict1.set("B", 2);
    dict2.set("C", 3);
    dict2.insert_array("D").append_value().set(4);

    dict1.update(dict2);

    CHECK_EQUAL("{A : 1, B : 2, C : 3, D : [4]}", json::dumps(dict1));

}

