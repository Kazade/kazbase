#ifndef TAGGABLE_OBJECT_H_INCLUDED
#define TAGGABLE_OBJECT_H_INCLUDED

#include "../list_utils.h"
#include "../string.h"

namespace base {

typedef std::string Tag;
typedef std::set<Tag> TagList;

class TaggableObject {
public:
    virtual ~TaggableObject() {}

    bool has_tag(const Tag& tag) const {
        return container::contains(tags_, str::lower(str::strip(tag)));
    }

    bool has_any_tags(const TagList& tags) const {
        for(Tag t: tags) {
            if(has_tag(t)) {
                return true;
            }
        }
        return false;
    }

    bool has_all_tags(const TagList& tags) const {
        for(Tag t: tags) {
            if(!has_tag(t)) {
                return false;
            }
        }
        return true;
    }

    void tag(const Tag& tag) {
        tags_.insert(str::lower(str::strip(tag)));
    }

    void tag(const TagList& tags) {
        for(TagList::const_iterator it = tags.begin(); it != tags.end(); ++it) {
            tag(*it);
        }
    }

    void untag(const Tag& tag) {
        tags_.erase(tag);
    }

    void clear_tags() {
        tags_.clear();
    }

    TagList get_tags() const { return tags_; }

private:
    TagList tags_;
};

}

#endif // TAGGABLE_OBJECT_H_INCLUDED
