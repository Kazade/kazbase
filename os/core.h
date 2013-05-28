#include "../unicode.h"

namespace os {

void touch(const std::string& path);
void touch(const unicode& path);

bool delete_path(const unicode& path, bool recursive=false, bool fail_silently=false);
bool delete_path(const std::string& path, bool recursive=false, bool fail_silently=false);

void make_dirs(const unicode& path);

std::string temp_dir();

}

