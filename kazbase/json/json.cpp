#include <cassert>

#include "../list_utils.h"
#include "../logging.h"
#include "json.h"

namespace json {

unicode Node::value() const {
    assert(type_ == NODE_TYPE_VALUE);
    return value_;
}

Node& Node::array_value(uint64_t index) const {
    assert(type_ == NODE_TYPE_ARRAY);
    return *array_.at(index);
}

Node& Node::dict_value(const unicode& key) const {
    assert(type_ == NODE_TYPE_DICT);        
    std::map<unicode, Node::ptr>::const_iterator it = dict_.find(key);
    if(it == dict_.end()) {
        throw KeyError(_u("Invalid key: '{0}'").format(key).encode());
    }

    return *(*it).second;
}

NodeType Node::type() const {
    return type_;
}

void Node::set(const unicode& value) {
    assert(type_ == NODE_TYPE_VALUE);
    value_ = value;
    value_type_ = VALUE_TYPE_STRING;
}

Node& Node::append_dict() {
    if(type_ == NODE_TYPE_DICT && !dict_.empty()) {
        throw ValueError("Attempted to append to a dictionary value");
    } else if(type_ == NODE_TYPE_VALUE) {
        throw ValueError("Attempted to append to a value node");
    } else {
        type_ = NODE_TYPE_ARRAY;
    }

    Node::ptr new_node(new Node(NODE_TYPE_DICT, this));
    array_.push_back(new_node);
    return *new_node;
}

Node& Node::append_array() {
    if(type_ == NODE_TYPE_DICT && !dict_.empty()) {
        throw ValueError("Attempted to append to a dictionary value");
    } else if(type_ == NODE_TYPE_VALUE) {
        throw ValueError("Attempted to append to a value node");
    } else {
        type_ = NODE_TYPE_ARRAY;
    }

    Node::ptr new_node(new Node(NODE_TYPE_ARRAY, this));
    array_.push_back(new_node);
    return *new_node;
}

Node& Node::append_value() {
    if(type_ == NODE_TYPE_DICT && !dict_.empty()) {
        throw ValueError("Attempted to append to a dictionary value");
    } else if(type_ == NODE_TYPE_VALUE) {
        throw ValueError("Attempted to append to a value node");
    } else {
        type_ = NODE_TYPE_ARRAY;
    }

    Node::ptr new_node(new Node(NODE_TYPE_VALUE, this));
    array_.push_back(new_node);
    return *new_node;
}

Node& Node::insert_dict(const unicode& key) {
    assert(type_ == NODE_TYPE_DICT);
    Node::ptr new_node(new Node(NODE_TYPE_DICT, this));
    dict_[key] = new_node;
    return *new_node;
}

Node& Node::insert_array(const unicode& key) {
    assert(type_ == NODE_TYPE_DICT);
    Node::ptr new_node(new Node(NODE_TYPE_ARRAY, this));
    dict_[key] = new_node;
    return *new_node;
}

Node& Node::insert_value(const unicode& key) {
    assert(type_ == NODE_TYPE_DICT);
    Node::ptr new_node(new Node(NODE_TYPE_VALUE, this));
    dict_[key] = new_node;
    return *new_node;
}

bool Node::has_key(const unicode& key) const {
    assert(type_ == NODE_TYPE_DICT);
    return dict_.find(key) != dict_.end();
}

std::set<unicode> Node::keys() const {
    assert(type_ == NODE_TYPE_DICT);
    return container::keys(dict_);
}

wchar_t find_first_token(const unicode& json_string) {

    std::vector<wchar_t> tokens = { '{','}',',',':','[',']'};

    for(wchar_t c: tokens) {
        if(json_string.contains(c)) {
            return c;
        }
    }

    return '\0';
}

unicode unescape(const unicode& buffer) {
    return buffer.replace("\\/", "/");
}

uint32_t decode_unicode_value(const std::string& chars) {
   uint32_t unicode = 0;
   for(int i = 0; i < 4; ++i) {
      char c = chars[i];
      unicode *= 16;
      if ( c >= '0'  &&  c <= '9' )
         unicode += c - '0';
      else if ( c >= 'a'  &&  c <= 'f' )
         unicode += c - 'a' + 10;
      else if ( c >= 'A'  &&  c <= 'F' )
         unicode += c - 'A' + 10;
      else
         throw ParseError("Bad unicode escape sequence in string: hexadecimal digit expected: " + chars);
   }
   return unicode;
}

unicode codepoint_to_utf8(unsigned int cp) {
   std::string result;

   // based on description from http://en.wikipedia.org/wiki/UTF-8

   if (cp <= 0x7f)
   {
      result.resize(1);
      result[0] = static_cast<char>(cp);
   }
   else if (cp <= 0x7FF)
   {
      result.resize(2);
      result[1] = static_cast<char>(0x80 | (0x3f & cp));
      result[0] = static_cast<char>(0xC0 | (0x1f & (cp >> 6)));
   }
   else if (cp <= 0xFFFF)
   {
      result.resize(3);
      result[2] = static_cast<char>(0x80 | (0x3f & cp));
      result[1] = 0x80 | static_cast<char>((0x3f & (cp >> 6)));
      result[0] = 0xE0 | static_cast<char>((0xf & (cp >> 12)));
   }
   else if (cp <= 0x10FFFF)
   {
      result.resize(4);
      result[3] = static_cast<char>(0x80 | (0x3f & cp));
      result[2] = static_cast<char>(0x80 | (0x3f & (cp >> 6)));
      result[1] = static_cast<char>(0x80 | (0x3f & (cp >> 12)));
      result[0] = static_cast<char>(0xF0 | (0x7 & (cp >> 18)));
   }

   return unicode(result);
}

void Node::dump_to(unicode& s) const {
    std::stringstream stream;
    if(type_ == NODE_TYPE_DICT) {
        stream << "{";

        uint32_t i = 0, size = dict_.size();
        for(auto p: dict_) {
            unicode new_string;
            p.second->dump_to(new_string);
            stream << p.first + " : " + new_string;

            if(++i != size) {
                stream << ", ";
            }
        }

        stream << "}";
    } else if (type_ == NODE_TYPE_ARRAY) {
        stream << "[";

        std::vector<unicode> outputs;
        for(Node::ptr p: array_) {
            unicode new_string;
            p->dump_to(new_string);
            outputs.push_back(new_string);
        }
        stream << _u(", ").join(outputs);
        stream << "]";
    } else {
        if(value_type_ == VALUE_TYPE_NULL) {
            stream << "null";
        } else if (value_type_ == VALUE_TYPE_BOOL) {
            if(value_bool_) {
                stream << "true";
            } else {
                stream << "false";
            }
        } else if (value_type_ == VALUE_TYPE_NUMBER) {
            stream << value_number_;
        } else {
            stream << "\"" << value_ << "\"";
        }
    }

    s += stream.str();
}

unicode dumps(const JSON &json) {
    unicode result;
    json.dump_to(result);
    return result;
}

void set_value(Node& node, const unicode& value, bool buffer_is_string) {
    if(buffer_is_string) {
        node.set(value);
    } else {
        if(value == "true" || value == "false") {
            node.set(value == "true");
            return;
        }

        if(value == "null") {
            node.set_null();
            return;
        }

        try {
            node.set(value.to_int());
        } catch(std::invalid_argument& e) {
            throw ParseError(_u("Unknown value type: {0}").format(value).encode());
        }
    }
}

JSON loads(const unicode& json_string) {
    JSON result;

    char tok = find_first_token(json_string);
    if(tok == '{') {
        result = JSON(NODE_TYPE_DICT);
    } else if(tok =='[') {
        result = JSON(NODE_TYPE_ARRAY);
    } else {
        L_DEBUG(json_string);
        throw ParseError("Not a valid JSON string");
    }

    std::vector<char> whitespace = { '\t', ' ', '\n', '\r' };

    Node* current_node = &result;

    char last_token = '\0';
    int current_line = 0;
    int current_char = 0;

    bool inside_string = false;
    bool buffer_is_string = false;

    unicode last_key = "";
    unicode buffer = "";

    for(uint64_t i = 0; i < json_string.length(); ++i) {
        char c = json_string[i];

        current_char++;

        if(c == '\n') {
            current_line++;
            current_char = 0;
        }
        if(container::contains(whitespace, c) && !inside_string) continue; //Ignore whitespace
        switch(c) {
            case '{':
                if(inside_string){
                    buffer.push_back(c);
                    continue;
                }

                if(last_token != ':' &&
                   last_token != '[' &&
                   last_token != ',' &&
                   last_token != '\0') {

                    throw ParseError(_u("Hit invalid '{' token on line {0} char: {1}").format(current_line, current_char));
                }

                if(last_token != '\0') {
                    //If we've hit an { and the last token was a : then we had something like
                    // "mykey" : { ...
                    // so we can now create the dictionary node and make it current
                    if(current_node->type() == NODE_TYPE_ARRAY) {
                        current_node = &current_node->append_dict();
                    } else if(current_node->type() == NODE_TYPE_DICT) {
                        current_node = &current_node->insert_dict(last_key);
                    } else {
                        throw ParseError(_u("Hit invalid '{' token on line {0}").format(current_line));
                    }

                    buffer = "";
                    buffer_is_string = false;
                }

                last_token = '{';
                continue;
            break;
            case '}':
                if(inside_string){
                    buffer.push_back(c);
                    continue;
                }

                if(last_token != ':' && last_token != ']' && last_token != '}' && last_token != '{') {
                    throw ParseError(_u("Hit invalid '}' token on line {0}").format(current_line));
                }

                if(!buffer.empty()) {
                    if(last_token == ':') {
                        set_value(current_node->insert_value(last_key), unescape(buffer), buffer_is_string);
                    } else {
                        set_value(*current_node, unescape(buffer), buffer_is_string);
                    }
                }
                buffer = "";
                buffer_is_string = false;

                //Move out of the current node
                current_node = current_node->parent();
                last_token = '}';
                continue;
            break;
            case '[':
                if(inside_string){
                    buffer.push_back(c);
                    continue;
                }

                if(last_token != ':' && last_token != '\0' && last_token != '[' && last_token != ',') {
                    throw ParseError(_u("Hit invalid '[' token on line {0}").format(current_line));
                }

                if(last_token != '\0') {
                    if(current_node->type() == NODE_TYPE_ARRAY) {
                        current_node = &current_node->append_array();
                    } else if (current_node->type() == NODE_TYPE_DICT) {
                        current_node = &current_node->insert_array(last_key);
                    } else {
                        throw ParseError("Something went badly wrong");
                    }
                }

                last_token = '[';
            break;
            case ']':
                if(inside_string){
                    buffer.push_back(c);
                    continue;
                }

                if(!buffer.empty()) {
                    set_value(current_node->append_value(), unescape(buffer), buffer_is_string);
                    buffer = "";
                    buffer_is_string = false;
                }

                current_node = current_node->parent();
                last_token = ']';

            break;
            case ',':
                if(inside_string){
                    buffer.push_back(c);
                    continue;
                }

                if(!buffer.empty()) {
                    if(current_node->type() == NODE_TYPE_DICT) {
                        set_value(current_node->insert_value(last_key), unescape(buffer), buffer_is_string);
                    }
                    else if(current_node->type() == NODE_TYPE_ARRAY) {
                        set_value(current_node->append_value(), unescape(buffer), buffer_is_string);
                    } else {
                        set_value(*current_node, unescape(buffer), buffer_is_string);
                    }
                    buffer = "";
                    buffer_is_string = false;
                }
                last_token = ',';
                continue;
            break;
            case ':':
                if(inside_string){
                    buffer.push_back(c);
                    continue;
                }

                last_key = unescape(buffer);
                buffer = "";
                buffer_is_string = false;
                last_token = ':';
                continue;
            break;
            case '\\': {
                c = json_string[++i]; //Get the next character
                switch(c) {
                    case '"':
                    case '/':
                    case '\\':
                        buffer.push_back(c);
                    break;
                    case 'b':
                        buffer.push_back('\b');
                    break;
                    case 'f':
                        buffer.push_back('\f');
                    break;
                    case 'n':
                        buffer.push_back('\n');
                    break;
                    case 'r':
                        buffer.push_back('\r');
                    break;
                    case 't':
                        buffer.push_back('\t');
                    break;
                    case 'u': {
                        std::string value;
                        for(int j = 0; j < 4; ++j) {
                            value.push_back(json_string[++i]);
                        }
                        uint32_t unicode = decode_unicode_value(value);
                        if (unicode >= 0xD800 && unicode <= 0xDBFF) {
                            ++i; //Skip '\'
                            ++i; //Skip 'u'

                            std::string value2;
                            for(int j = 0; j < 4; ++j) {
                                value2.push_back(json_string[++i]);
                            }
                            uint32_t surrogate_pair = decode_unicode_value(value2);
                            unicode = 0x10000 + ((unicode & 0x3FF) << 10) + (surrogate_pair & 0x3FF);
                        }
                        buffer += codepoint_to_utf8(unicode);
                    }
                }
            }
            break;
            case '"': {
                inside_string = !inside_string;
                if(!inside_string) {
                    buffer_is_string = true;
                }
            }
            break;
            default:
                buffer.push_back(c);
        }
    }

    return result;
}

void Node::update(const Node& other) {
    if(other.type() == NODE_TYPE_ARRAY) {
        for(int i = 0; i < other.length(); ++i) {
            const Node& n = other[i];
            if(n.type() == NODE_TYPE_ARRAY) {
                this->append_array().update(n);
            } else if(n.type() == NODE_TYPE_DICT) {
                this->append_dict().update(n);
            } else if(n.type() == NODE_TYPE_VALUE) {
                this->append_value().update(n);
            }
        }
    } else if(other.type() == NODE_TYPE_DICT) {
        for(auto key: other.keys()) {
            const Node& n = other[key];
            if(n.type() == NODE_TYPE_ARRAY) {
                this->insert_array(key).update(n);
            } else if(n.type() == NODE_TYPE_DICT) {
                this->insert_dict(key).update(n);
            } else {
                this->insert_value(key).update(n);
            }
        }
    } else {
        if(other.value_type_ == VALUE_TYPE_STRING) {
            this->set(other.get());
        } else if(other.value_type_ == VALUE_TYPE_NUMBER) {
            this->set(other.get_number());
        } else if(other.value_type_ == VALUE_TYPE_BOOL) {
            this->set(other.get_bool());
        } else {
            this->set_null();
        }

    }
}

}
