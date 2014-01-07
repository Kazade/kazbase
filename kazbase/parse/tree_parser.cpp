#if 0 //DISABLE

#include <iostream>
#include "tree_parser.h"
#include "../logging.h"

namespace parse {

regex::Match TreeParser::find_open_tag(const std::string& text, unsigned int start) {
    return regex::find(start_regex_, text, start);
}

regex::Match TreeParser::find_close_tag(const std::string& text, unsigned int start) {
    return regex::find(end_regex_, text, start);
}

regex::Match TreeParser::find_next_tag(const std::string& text, unsigned int start) {
    if(start >= text.size()) {
        return regex::Match();
    }

    regex::Match next_open = find_open_tag(text, start);
    regex::Match next_close = find_close_tag(text, start);

    if(next_open == regex::Match() && next_close == regex::Match()) {
        L_DEBUG("Didn't find any tags");
        return regex::Match();
    }

    if(next_close == regex::Match() || next_open.start_pos < next_close.start_pos) {
        return next_open;
    }

    return next_close;
}

regex::Match TreeParser::find_matching_close_tag(const std::string& text, unsigned int start_tag_position) {

    int i = 1;
    regex::Match next = find_next_tag(text, start_tag_position + 1);
    while(next != regex::Match()) {
        if(find_close_tag(next.groups[0], 0) != regex::Match()) { //FIXME:This is wrong!!!!
            L_DEBUG("Found closing tag");
            --i; //Closing tag
        } else {
            L_DEBUG("Found opening tag");
            ++i; //Another opening tag
        }

        if(i == 0) { //We just closed the opening tag
            return next;
        }
        next = find_next_tag(text, next.end_pos);
    }

    return regex::Match();
}

TreeParser::TreeParser(const std::string& start_tag_regex,
                       const std::string& end_tag_regex):
start_regex_(start_tag_regex),
end_regex_(end_tag_regex) {

}

void TreeParser::parse_node(Node& node) {

    std::string text = node.inner_contents;

    L_DEBUG("Parsing: " + text);
    regex::Match start_tag = find_open_tag(text, 0);

    if(start_tag == regex::Match()) {
        L_DEBUG("No start tag found, returning");
        //No more nested tags
        return;
    }


    regex::Match closing_tag;
    while(start_tag != regex::Match()) {
        closing_tag = find_matching_close_tag(text, start_tag.start_pos);

        if(closing_tag == regex::Match()) {
            L_DEBUG("Couldn't find closing tag");
            break;
        }

        std::string tag_content = text.substr(start_tag.start_pos, closing_tag.end_pos);

        Node n;
        n.full_contents = tag_content;
        n.inner_contents = tag_content.substr(1, tag_content.size() - 2);
        node.child_nodes.push_back(n);

        start_tag = find_open_tag(text, closing_tag.end_pos);
    }

    for(unsigned int i = 0; i < node.child_nodes.size(); ++i) {
        parse_node(node.child_nodes[i]);
    }
}

/*
void TreeParser::do_parse(const std::string& text, Node& parent) {
    L_DEBUG("Parsing: " + text);
    regex::Match start_tag = find_open_tag(text, 0);
    if(start_tag == regex::Match()) {
        L_DEBUG("No start tag found, returning");
        //No more nested tags
        return;
    }

    regex::Match closing_tag;
    while(start_tag != regex::Match()) {
        closing_tag = find_matching_close_tag(text, start_tag.start_pos);

        if(closing_tag == regex::Match()) {
            L_DEBUG("Couldn't find closing tag");
        }

        std::string tag_content = text.substr(start_tag.start_pos, closing_tag.end_pos);
        parent.full_contents = tag_content;

        L_DEBUG("Processing nested");
        Node n;
        std::string to_parse = text.substr(start_tag.end_pos, closing_tag.start_pos - 1);
        do_parse(to_parse, n);
        parent.child_nodes.push_back(n);

        L_DEBUG("Starting next iteration from point: " + boost::lexical_cast<std::string>(closing_tag.end_pos));
        start_tag = find_open_tag(text, closing_tag.end_pos - 1);
    }
}*/

Node TreeParser::parse(const std::string& text) {
    Node root;
    root.full_contents = text;
    root.inner_contents = text.substr(1, text.size() - 2);
    L_DEBUG(root.full_contents);
    L_DEBUG(root.inner_contents);

    parse_node(root);
    return root;
}


}


/*
    Need to be able to parse this:

    <<<something|1234>|1234>|<something|1234>>

    def parse_id(str):
        return (key, key)

    def find_open_tag(str, start_pos = 0):
        return str.find("<", start_pos)

    def find_close_tag(str, start_pos = 0):
        return str.find(">", start_pos)

    def find_next_tag(str, start_pos):
        string::pos end = find_close_tag(start_pos)
        string::pos start = find_open_tag(start_pos)
        if(end < start):
            return pair<end, true>
        return pair<start, false>

    def find_matching_close_tag(str, pos):
        eof = false
        next = find_next_tag(str, pos, eof):
        int i = 1
        while(!eof) {
            if next.second: i--;
            else: i++;

            if i == 0:
                return next.first

            next = find_next_tag(str, next.first, eof)
        }


*/

#endif //DISABLE
