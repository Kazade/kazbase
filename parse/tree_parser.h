#ifndef TREE_PARSER_H_INCLUDED
#define TREE_PARSER_H_INCLUDED

#include <string>
#include <vector>
#include <utility>

#include "kazbase/regex/regex.h"

namespace parse {

struct Node {
    std::string full_contents;
    std::string inner_contents;
    std::vector<Node> child_nodes;
};

class TreeParser {
public:
    TreeParser(const std::string& start_tag_regex,
               const std::string& end_tag_regex);

    Node parse(const std::string& text);

private:
    regex::Match find_open_tag(const std::string& text, unsigned int start=0);
    regex::Match find_close_tag(const std::string& text, unsigned int start=0);
    regex::Match find_next_tag(const std::string& text, unsigned int start=0);
    regex::Match find_matching_close_tag(const std::string& text, unsigned int start_tag_position);

    std::string start_regex_;
    std::string end_regex_;

    void parse_node(Node& node);
};

}

#endif // TREE_PARSER_H_INCLUDED
