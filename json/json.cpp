#include <cassert>

#include "kazbase/list_utils.h"
#include "kazbase/string.h"
#include "kazbase/logging/logging.h"
#include "json.h"

namespace json {

std::string Node::value() const {
    assert(type_ == NODE_TYPE_VALUE);
    return value_;
}

Node& Node::array_value(uint64_t index) const {
    assert(type_ == NODE_TYPE_ARRAY);
    return *array_.at(index);
}

Node& Node::dict_value(const std::string& key) const {
    assert(type_ == NODE_TYPE_DICT);
    std::map<std::string, Node::ptr>::const_iterator it = dict_.find(key);
    if(it == dict_.end()) {
        std::string keys = str::join(container::keys(dict_), ", ");
        L_DEBUG(keys);
        throw KeyError("Invalid key: '" + key + "'");
    }

    return *(*it).second;
}

NodeType Node::type() const {
    return type_;
}

void Node::set(const std::string& value) {
    assert(type_ == NODE_TYPE_VALUE);
    value_ = value;
    value_type_ = VALUE_TYPE_STRING;
}

Node& Node::append_dict() {
    assert(type_ == NODE_TYPE_ARRAY);
    Node::ptr new_node(new Node(NODE_TYPE_DICT, this));
    array_.push_back(new_node);
    return *new_node;
}

Node& Node::append_array() {
    assert(type_ == NODE_TYPE_ARRAY);
    Node::ptr new_node(new Node(NODE_TYPE_ARRAY, this));
    array_.push_back(new_node);
    return *new_node;
}

Node& Node::append_value(const std::string& value) {
    assert(type_ == NODE_TYPE_ARRAY);
    Node::ptr new_node(new Node(NODE_TYPE_VALUE, this));
    new_node->set(value);
    array_.push_back(new_node);
    return *new_node;
}

Node& Node::insert_dict(const std::string& key) {
    assert(type_ == NODE_TYPE_DICT);
    Node::ptr new_node(new Node(NODE_TYPE_DICT, this));
    dict_[key] = new_node;
    return *new_node;
}

Node& Node::insert_array(const std::string& key) {
    assert(type_ == NODE_TYPE_DICT);
    Node::ptr new_node(new Node(NODE_TYPE_ARRAY, this));
    dict_[key] = new_node;
    return *new_node;
}

Node& Node::insert_value(const std::string& key, const std::string& value) {
    assert(type_ == NODE_TYPE_DICT);
    Node::ptr new_node(new Node(NODE_TYPE_VALUE, this));
    new_node->set(value);

    dict_[key] = new_node;
    return *new_node;
}

bool Node::has_key(const std::string& key) const {
    assert(type_ == NODE_TYPE_DICT);
    return dict_.find(key) != dict_.end();
}

std::set<std::string> Node::keys() const {
    assert(type_ == NODE_TYPE_DICT);
    return container::keys(dict_);
}

char find_first_token(const std::string& json_string) {

    std::vector<char> tokens = { '{','}',',',':','[',']'};
    for(char c: json_string) {
        if(container::contains(tokens, c)) {
            return c;
        }
    }

    return '\0';
}

std::string unescape(const std::string& buffer) {
    return str::replace(buffer, "\\/", "/");
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

std::string codepoint_to_utf8(unsigned int cp) {
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

   return result;
}

void Node::dump_to(std::string& s, int indent) const {
    std::stringstream stream;
    if(type_ == NODE_TYPE_DICT) {
        stream << std::setw(indent * 4) << "{" << std::endl;

        for(std::pair<std::string, Node::ptr> p: dict_) {
            std::string new_string;
            p.second->dump_to(new_string, indent + 1);
            stream << str::lpad(p.first + " : " + new_string, (indent + 1) * 4) << std::endl;
        }

        stream << std::setw(indent * 4) << "}" << std::endl;
    } else if (type_ == NODE_TYPE_ARRAY) {
        stream << "[" << std::endl;

        std::vector<std::string> outputs;
        for(Node::ptr p: array_) {
            std::string new_string;
            p->dump_to(new_string, indent + 1);
            outputs.push_back(str::lpad(new_string, (indent + 1) * 4));
        }
        stream << str::join(outputs, ",\n") << std::endl;
        stream << str::lpad("]", indent * 4) << std::endl;
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

    s.append(stream.str());
}

std::string dumps(const JSON &json) {
    std::string result;
    json.dump_to(result);
    return result;
}

JSON loads(const std::string& json_string) {
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

    std::string last_key = "";
    std::string buffer = "";

    for(uint64_t i = 0; i < json_string.size(); ++i) {
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
                    throw ParseError("Hit invalid '{' token on line " + boost::lexical_cast<std::string>(current_line) + " char: " + boost::lexical_cast<std::string>(current_char));
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
                        throw ParseError("Hit invalid '{' token on line " + boost::lexical_cast<std::string>(current_line));
                    }

                    buffer = "";
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
                    throw ParseError("Hit invalid '}' token on line " + boost::lexical_cast<std::string>(current_line));
                }

                if(!buffer.empty()) {
                    if(last_token == ':') {
                        current_node->insert_value(last_key, unescape(buffer));
                    } else {
                        current_node->set(unescape(buffer));
                    }
                }
                buffer = "";

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
                    throw ParseError("Hit invalid '[' token on line " + boost::lexical_cast<std::string>(current_line));
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
                    current_node->append_value(unescape(buffer));
                    buffer = "";
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
                        current_node->insert_value(last_key, unescape(buffer));
                    }
                    else if(current_node->type() == NODE_TYPE_ARRAY) {
                        current_node->append_value(unescape(buffer));
                    } else {
                        current_node->set(unescape(buffer));
                    }
                    buffer = "";
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
            }
            break;
            default:
                buffer.push_back(c);
        }
    }

    return result;
}

}
