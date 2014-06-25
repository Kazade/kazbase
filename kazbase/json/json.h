#ifndef JSON_H_INCLUDED
#define JSON_H_INCLUDED

#include <stdexcept>
#include <string>
#include <vector>
#include <cassert>
#include <set>
#include <map>
#include <memory>

#include "../unicode.h"

namespace json {

/*
    Syntax:

    JSON data = json::loads(some_json_string);
    data["value"][0];
    data["value2"]["value3"]

    JSON data;
    data.insert_value("key").set("value");
    data.insert_dict("key2").insert_value("key").set("value");
    data.insert_array("key3").append_value().set("value");

    std::string value = data["key"];
    value = data["key2"]["key"];
    value = data["key3"][0];
*/

enum NodeType {
    NODE_TYPE_VALUE,
    NODE_TYPE_ARRAY,
    NODE_TYPE_DICT
};

enum ValueType {
    VALUE_TYPE_NO_VALUE = 0,
    VALUE_TYPE_STRING,
    VALUE_TYPE_NULL,
    VALUE_TYPE_BOOL,
    VALUE_TYPE_NUMBER
};

class KeyError : public std::logic_error {
public:
    KeyError(const std::string& what):
        std::logic_error(what){}
};

class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& what):
        std::runtime_error(what) {}
};

class Node {
public:
    typedef std::shared_ptr<Node> ptr;

private:
    std::map<unicode, Node::ptr> dict_;
    std::vector<Node::ptr> array_;
    unicode value_;
    bool value_bool_;
    int value_number_;

    NodeType type_;
    ValueType value_type_;

    Node(NodeType type, Node* parent):
        type_(type),
        value_type_(VALUE_TYPE_NO_VALUE),
        parent_(parent) {}

    Node* parent_;

public:
    Node(NodeType type):
        type_(type),
        value_type_(VALUE_TYPE_NO_VALUE),
        parent_(nullptr) {}

    Node():
        type_(NODE_TYPE_DICT),
        value_type_(VALUE_TYPE_NO_VALUE),
        parent_(nullptr) {}


    unicode value() const; //Deprecated

    unicode get() const {
        assert(type_ == NODE_TYPE_VALUE && value_type_ == VALUE_TYPE_STRING);
        return value_;
    }

    bool get_bool() const {
        assert(type_ == NODE_TYPE_VALUE && value_type_ == VALUE_TYPE_BOOL);
        return value_bool_;
    }

    int get_number() const {
        assert(type_ == NODE_TYPE_VALUE && value_type_ == VALUE_TYPE_NUMBER);
        return value_number_;
    }

    bool is_null() const {
        return type_ == NODE_TYPE_VALUE && value_type_ == VALUE_TYPE_NULL;
    }

    Node& array_value(uint64_t index) const;
    Node& dict_value(const unicode& key) const;
    NodeType type() const;

    void set(const unicode& value);
    void set_null() {
        value_type_ = VALUE_TYPE_NULL;
    }

    void set_bool(bool value) {
        value_type_ = VALUE_TYPE_BOOL;
        value_bool_ = value;
    }

    void set_number(int number) {
        value_type_ = VALUE_TYPE_NUMBER;
        value_number_ = number;
    }

    Node& append_dict();
    Node& append_array();
    Node& append_value();

    Node& insert_dict(const unicode& key);
    Node& insert_array(const unicode& key);
    Node& insert_value(const unicode& key);

    bool has_key(const unicode& key) const;
    std::set<unicode> keys() const;

    Node& operator[](const int64_t index) {
        return array_value(index);
    }

    Node& operator[](const std::string& key) {
        return dict_value(key);
    }

    operator unicode() const {
        return value();
    }

    uint64_t length() const {
        switch(type_) {
            case NODE_TYPE_ARRAY: return array_.size();
            case NODE_TYPE_DICT: return dict_.size();
            default:
                throw std::logic_error("It doesn't make sense to call length on a value node. Did you mean value().length()?");
        }
    }

    Node* parent() const { return parent_; }

    uint32_t child_count() const { return array_.size(); }
    Node& child(uint32_t i) {
        assert(type_ == NODE_TYPE_ARRAY);
        return *(array_.at(i));
    }

    void dump_to(unicode &s) const;
};

typedef Node JSON;

JSON loads(const unicode& json_string);
unicode dumps(const JSON& json);

}

#endif // JSON_H_INCLUDED
