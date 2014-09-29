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
    KeyError(const unicode& what):
        std::logic_error(what.encode()){}
};

class ParseError : public std::runtime_error {
public:
    ParseError(const unicode& what):
        std::runtime_error(what.encode()) {}
};

class Node {
public:
    typedef std::shared_ptr<Node> ptr;

private:
    std::map<unicode, Node::ptr> dict_;
    std::vector<Node::ptr> array_;
    unicode value_;
    bool value_bool_;
    float value_number_;

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

    float get_number() const {
        assert(type_ == NODE_TYPE_VALUE && value_type_ == VALUE_TYPE_NUMBER);
        return value_number_;
    }

    bool is_null() const {
        return type_ == NODE_TYPE_VALUE && value_type_ == VALUE_TYPE_NULL;
    }

    Node& array_value(uint64_t index) const;
    Node& dict_value(const unicode& key) const;
    NodeType type() const;

    void set(const char* value) {
        set(unicode(value));
    }

    void set(const unicode& value);
    void set_null() {
        value_type_ = VALUE_TYPE_NULL;
    }

    void set(bool value) {
        value_type_ = VALUE_TYPE_BOOL;
        value_bool_ = value;
    }

    void set(int number) {
        set(float(number));
    }

    void set(float number) {
        value_type_ = VALUE_TYPE_NUMBER;
        value_number_ = number;
    }

    Node& set(const unicode& key, const char* value) {
        return set(key, unicode(value));
    }

    Node& set(const unicode& key, const unicode& value) {
       assert(type_ == NODE_TYPE_DICT);
       Node& node = insert_value(key);
       node.set(value);
       return node;
    }

    Node& set(const unicode& key, const double& value) {
        assert(type_ == NODE_TYPE_DICT);
        Node& node = insert_value(key);
        node.set((float)value);
        return node;
    }

    Node& set(const unicode& key, const int& value) {
        assert(type_ == NODE_TYPE_DICT);
        Node& node = insert_value(key);
        node.set((int)value);
        return node;
    }

    Node& set(const unicode& key, const float& value) {
        assert(type_ == NODE_TYPE_DICT);
        Node& node = insert_value(key);
        node.set((float)value);
        return node;
    }

    Node& set(const unicode& key, const bool& value) {
        assert(type_ == NODE_TYPE_DICT);
        Node& node = insert_value(key);
        node.set(value);
        return node;
    }

    Node& set_null(const unicode& key) {
        assert(type_ == NODE_TYPE_DICT);
        Node& node = insert_value(key);
        node.set_null();
        return node;
    }

    void operator=(const float& rhs) {
        set(rhs);
    }

    void operator=(const unicode& rhs) {
        set(rhs);
    }

    Node& append_dict();
    Node& append_array();
    Node& append_value();

    Node& insert_dict(const unicode& key);
    Node& insert_array(const unicode& key);
    Node& insert_value(const unicode& key);

    bool has_key(const unicode& key) const;
    std::set<unicode> keys() const;


    Node& operator[](const int index) const {
        return array_value(index);
    }

    Node& operator[](const int64_t index) const {
        return array_value(index);
    }

    Node& operator[](const char* str) const {
        return dict_value(unicode(str));
    }

    Node& operator[](const unicode& key) const {
        return dict_value(key);
    }

    operator float() const {
        return get_number();
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

    void update(const Node& other);
};

typedef Node JSON;

JSON loads(const unicode& json_string);
unicode dumps(const JSON& json);

}

#endif // JSON_H_INCLUDED
